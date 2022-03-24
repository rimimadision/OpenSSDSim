#ifndef D_PRINT_H
#define D_PRINT_H
#include"../emu/emu_config.h"

#ifndef EMU
#include"xil_printf.h"
#else
#include "../emu/emu_log.h"
#endif

#define DEBUG_TEST1
#ifdef DEBUG_TEST
#define DEBUG_PRINTF xil_printf
#else
#define DEBUG_PRINTF //
#endif

//void debug_printf(const char8 *ctrl1, ...);
void debug_printf(const char8* fmt, ...);
#include"../emu/emu_log.h"
#include<stdio.h>
#include<stdarg.h>
#ifdef DEBUG_TEST 
#define DEBUG_PRINTF debug_printf
#else
#define DEBUG_TEST
#endif

#endif 
