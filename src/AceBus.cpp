#include "AceBus.h"

#define kRXDataReady (tinframe_kFrameSize + 1)
#define kRXDone (tinframe_kFrameSize + 2)

#define kTXRequest (tinframe_kFrameSize + 1)
#define kTXIdle (tinframe_kFrameSize + 2)

AceBus::AceBus(HardwareSerial &serial, unsigned char interruptPin, AceBus_rxCallback callback)
    : serialPort{serial}, rxInterruptPin{interruptPin}, rxCallback{callback} {}

volatile unsigned long AceBus::rxActiveMicros = 0;

void AceBus::externalInterrupt(void) { rxActiveMicros = micros(); }

void AceBus::begin() {
  serialPort.begin(AceBus_kBaud);
  pinMode(rxInterruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(rxInterruptPin), externalInterrupt,
                  CHANGE);
  rxIndex = kRXDone;
  txIndex = kTXIdle;
}

int AceBus::update() {
  noInterrupts();
  unsigned long lastActivity = micros() - rxActiveMicros;
  interrupts();
  if (lastActivity > AceBus_kInterFrameMicros) {
    rxIndex = 0;
    while (serialPort.available() > 0) {
      serialPort.read();
    }
    unsigned int txPriority = AceBus_kInterFrameMicros;
    unsigned char firstDataByte = txFrame.data[0];  // priority based on byte 0
    while(firstDataByte){
      txPriority += AceBus_kBitPeriodMicros;
      firstDataByte <<= 1;
    }
    if ((lastActivity > txPriority) && (digitalRead(rxInterruptPin) == HIGH)){
      if (txIndex == kTXRequest) {
        txIndex = 0;
        unsigned char txData = ((char *)&txFrame)[txIndex];
        serialPort.write(txData);
        noInterrupts();
        rxActiveMicros = micros(); // immediately update rxActiveMicros
        interrupts();
        return AceBus_kWriteBusy;
      } else {
        if(txIndex != kTXIdle){
          txIndex = kTXIdle;
          return AceBus_kWriteTimeout;
        }
        txIndex = kTXIdle;
      }
    }
  }
  if (txIndex < tinframe_kFrameSize) {
    unsigned char txData = ((char *)&txFrame)[txIndex];
    if (serialPort.available() > 0) {
      unsigned char rxData = serialPort.read();
      if (rxData == txData) {
        ++txIndex;
        if (txIndex < tinframe_kFrameSize) {
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
    unsigned char rxData = serialPort.read();
    if (rxIndex < tinframe_kFrameSize) {
      ((char *)&rxFrame)[rxIndex++] = rxData;
    } else {
      return AceBus_kReadOverunError;
    }
    if (rxIndex == tinframe_kFrameSize) {
      rxIndex = kRXDone;
      if (tinframe_checkFrame(&rxFrame) == tinframe_kOK) {
        rxCallback(&rxFrame);
        return AceBus_kOK;
      } else {
        return AceBus_kReadCRCError;
      }
    }
  }
  return AceBus_kOK;
}

int AceBus::write(tinframe_t *frame) {
  if (txIndex != kTXIdle) {
    return AceBus_kWriteBusy;
  }
  tinframe_prepareFrame(frame);
  memcpy(&txFrame, frame, tinframe_kFrameSize);
  txIndex = kTXRequest;
  return AceBus_kOK;
}
