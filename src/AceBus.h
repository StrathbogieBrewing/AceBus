#ifndef ACEBUS_H
#define ACEBUS_H

#include "Arduino.h"
#include "tinframe.h"

#define AceBus_kBaud (1200UL)
#define AceBus_kBitPeriodMicros (1000000UL / AceBus_kBaud)
#define AceBus_kInterFrameMicros (AceBus_kBitPeriodMicros * 15)

typedef void (*AceBus_rxCallback)(tinframe_t *frame);

enum {
  AceBus_kOK = 0,
  AceBus_kWriteBusy,
  AceBus_kWriteCollision,
  AceBus_kWriteTimeout,
  AceBus_kWriteComplete,
  AceBus_kReadNoData,
  AceBus_kReadCRCError,
  AceBus_kReadSequenceError,
  AceBus_kReadOverunError,
  AceBus_kReadDataReady,
  AceBus_kReadComplete,
};

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
