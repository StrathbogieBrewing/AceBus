#ifndef ACEBUSERROR_H
#define ACEBUSERROR_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
  AceBus_kOK = 0,
  AceBus_kWriteBusy = 1,
  AceBus_kWriteComplete = 2,
  AceBus_kWriteCollision = 3,
  AceBus_kWriteTimeout = 4,
  AceBus_kReadCRCError = 5,
  AceBus_kReadOverunError = 6,
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ACEBUSERROR_H
