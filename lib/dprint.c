#include"dprint.h"

#include<stdio.h>
#include<stdarg.h>

#include "type.h"

void debug_printf(const char8 *fmt, ...)
{
    #ifdef DEBUG_PRINTF
    va_list args;
    va_start(args, fmt);
    #ifdef EMU
    EMU_log_println(DEBUG, fmt, args);
    #else
    xil_printf(fmt, args);
    #endif
    va_end(args);
    #endif
}
