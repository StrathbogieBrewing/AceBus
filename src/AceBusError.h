#ifndef AceBusERROR_H
#define AceBusERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#define AceBus_kOK (0)
#define AceBus_kWriteBusy (1)
#define AceBus_kWriteComplete (2)
#define AceBus_kWriteCollision (3)
#define AceBus_kWriteTimeout (4)
#define AceBus_kWriteOverrun (5)
#define AceBus_kReadCRCError (6)
#define AceBus_kReadOverrun (7)


#ifdef __cplusplus
} // extern "C"
#endif

#endif // AceBusERROR_H
