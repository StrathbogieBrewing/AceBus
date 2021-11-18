#ifndef ACEFRAME_H
#define ACEFRAME_H

#ifdef __cplusplus
extern "C" {
#endif

#define aceframe_kOK (0)
#define aceframe_kCRCError (1)
#define aceframe_kFrameError (2)

extern unsigned char aceframe_priority[];

#define aceframe_kMaxDataBytes (16)
typedef struct {
  unsigned char priority;
  unsigned char dataLength;
  unsigned char data[aceframe_kMaxDataBytes];
  unsigned char crc;
} aceframe_t;
#define aceframe_kFrameSize (sizeof(aceframe_t))
#define aceframe_kFrameOverhead (aceframe_kFrameSize - aceframe_kMaxDataBytes)

void aceframe_prepareFrame(aceframe_t *frame);
char aceframe_checkFrame(const aceframe_t *frame);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ACEFRAME_H
