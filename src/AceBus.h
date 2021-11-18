#ifndef ACEBUS_H
#define ACEBUS_H

#include "Arduino.h"
#include "AceBusError.h"
#include "aceframe.h"

#define HIGH_PRIORITY     (0x00)
#define MEDIUM_PRIORITY   (0x04)
#define LOW_PRIORIY       (0x07)

typedef void (*AceBus_rxCallback)(uint8_t *data, uint8_t length);

class AceBus {
public:
  AceBus(HardwareSerial &serial, unsigned long baud, uint8_t interruptPin,
         AceBus_rxCallback callback);
  void begin();
  char update();
  char write(uint8_t *data, uint8_t length, uint8_t priority);

private:
  HardwareSerial &serialPort;
  uint8_t rxInterruptPin;
  AceBus_rxCallback rxCallback;
  unsigned long baudRate;
  unsigned int bitPeriodMicros;
  unsigned int interFrameMicros;
  aceframe_t txFrame;
  uint8_t txIndex;
  unsigned long txHoldOff;
  aceframe_t rxFrame;
  uint8_t rxIndex;
  static void externalInterrupt(void);
  static volatile unsigned long rxActiveMicros;
};

#endif //  ACEBUS_H
