#ifndef ACEBUS_H
#define ACEBUS_H

#include "Arduino.h"
#include "AceBusError.h"
#include "tinframe.h"

#define AceBus_kBaud (1200UL)
#define AceBus_kBitPeriodMicros (1000000UL / AceBus_kBaud)
#define AceBus_kInterFrameMicros (AceBus_kBitPeriodMicros * 15)

typedef void (*AceBus_rxCallback)(tinframe_t *frame);

class AceBus {
public:
  AceBus(HardwareSerial &serial, unsigned char interruptPin,
         AceBus_rxCallback callback);
  void begin();
  int update();
  int write(tinframe_t *frame);

private:
  HardwareSerial &serialPort;
  unsigned char rxInterruptPin;
  AceBus_rxCallback rxCallback;

  tinframe_t txFrame;
  unsigned char txIndex;
  tinframe_t rxFrame;
  unsigned char rxIndex;

  static void externalInterrupt(void);
  static volatile unsigned long rxActiveMicros;
};

#endif //  ACEBUS_H
