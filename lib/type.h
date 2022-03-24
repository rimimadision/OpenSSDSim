#ifndef TYPE_H
#define TYPE_H

#include "../emu/emu_config.h"

#define KB (1 << 10)
#define MB (1 << 20)
#define GB (1 << 30)

#ifndef EMU
#include "xil_types.h"
#else
#define u64 unsigned long long 
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
#define char8 char
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#define FAIL (1)
#define SUCCESS (0)

#ifndef NULL
#define NULL (0)
#endif

#endif
