#ifndef TINBUSERROR_H
#define TINBUSERROR_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
  TinBus_kOK = 0,
  TinBus_kWriteBusy = 1,
  TinBus_kWriteComplete = 2,
  TinBus_kWriteCollision = 3,
  TinBus_kWriteTimeout = 4,
  TinBus_kReadCRCError = 5,
  TinBus_kReadOverunError = 6,
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINBUSERROR_H
