#ifndef ACEBUSERROR_H
#define ACEBUSERROR_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
  AceBus_kOK = 0,
  AceBus_kWriteBusy,
  AceBus_kWriteCollision,
  AceBus_kWriteTimeout,
  AceBus_kReadCRCError,
  AceBus_kReadOverunError,
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ACEBUSERROR_H
