#include "AceBus.h"

#define kRXDone (aceframe_kFrameSize + 1)
#define kTXRequest (aceframe_kFrameSize + 1)
#define kTXIdle (aceframe_kFrameSize + 2)

AceBus::AceBus(HardwareSerial &serial, unsigned long baud,
               uint8_t interruptPin, AceBus_rxCallback callback)
    : serialPort{serial}, baudRate{baud}, rxInterruptPin{interruptPin},
      rxCallback{callback} {}

volatile unsigned long AceBus::rxActiveMicros = 0;

void AceBus::externalInterrupt(void) { rxActiveMicros = micros(); }

void AceBus::begin() {
  serialPort.begin(baudRate);
  bitPeriodMicros = 1000000UL / baudRate;
  interFrameMicros = bitPeriodMicros * 15L;
  pinMode(rxInterruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(rxInterruptPin), externalInterrupt,
                  CHANGE);
  rxIndex = kRXDone;
  txIndex = kTXIdle;
}

char AceBus::update() {
  noInterrupts();
  unsigned long lastActivity = micros() - rxActiveMicros;
  interrupts();
  if (lastActivity > interFrameMicros) {
    rxIndex = 0;                          // reset rx buffer
    while (serialPort.available() > 0) {
      serialPort.read();                  // and purge rx fifo
    }
    if (txIndex == kTXRequest) {
      if ((lastActivity > txHoldOff) &&
          (digitalRead(rxInterruptPin) == HIGH)) {
        txIndex = 0;
        uint8_t txData = ((char *)&txFrame)[txIndex];
        serialPort.write(txData);         // start tx
        noInterrupts();
        rxActiveMicros = micros();        // immediately update rxActiveMicros
        interrupts();
        return AceBus_kWriteBusy;
      }
    }
    if (lastActivity > (interFrameMicros * 2)){
      if (txIndex != kTXIdle) {
        txIndex = kTXIdle;
        return AceBus_kWriteTimeout;
      }
    }
  }
  if (txIndex < aceframe_kFrameSize) {
    uint8_t txData = ((char *)&txFrame)[txIndex];
    if (serialPort.available() > 0) {
      uint8_t rxData = serialPort.read();
      if (rxData == txData) {
        ++txIndex;
        if (txIndex < txFrame.dataLength + aceframe_kFrameOverhead) {
          txData = ((char *)&txFrame)[txIndex];
          serialPort.write(txData);
          return AceBus_kWriteBusy;
        } else {
          txIndex = kTXIdle;
          return AceBus_kWriteComplete;
        }
      } else {
        txIndex = kTXIdle;
        return AceBus_kWriteCollision;
      }
    }
    return AceBus_kWriteBusy;
  }

  if (serialPort.available() > 0) {
    uint8_t rxData = serialPort.read();
    if (rxIndex < aceframe_kFrameSize) {
      ((char *)&rxFrame)[rxIndex++] = rxData;
    } else {
      return AceBus_kReadOverrun;
    }
    if (rxIndex == (rxFrame.dataLength + aceframe_kFrameOverhead)) {
      rxIndex = kRXDone;
      if (aceframe_checkFrame(&rxFrame) == aceframe_kOK) {
        rxCallback(rxFrame.data, rxFrame.dataLength);
        return AceBus_kOK;
      } else {
        return AceBus_kReadCRCError;
      }
    }
  }
  return AceBus_kOK;
}

char AceBus::write(uint8_t *data, uint8_t length, uint8_t priority) {
  if (txIndex != kTXIdle) {
    return AceBus_kWriteBusy;
  }
  priority &= 0x07;
  txFrame.priority = aceframe_priority[priority];
  txHoldOff = interFrameMicros + bitPeriodMicros * (uint32_t)priority;
  txFrame.dataLength = length;
  if(length > aceframe_kMaxDataBytes){
    return AceBus_kWriteOverrun;
  }
  memcpy(txFrame.data, data, length);
  aceframe_prepareFrame(&txFrame);
  txIndex = kTXRequest;
  return AceBus_kOK;
}
