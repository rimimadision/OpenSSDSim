#ifndef EMU_IO_H
#define EMU_IO_H

#include "../lib/type.h"

static inline void Xil_Out32(u32 addr, u32 value)
{
    (*(u32*)addr) = value;
}

static inline u32 Xil_In32(u32 addr)
{
    return *((u32 *)addr);
}
#endif
