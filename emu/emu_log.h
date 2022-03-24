#ifndef EMU_LOG
#define EMU_LOG

#include "emu_config.h"
#ifdef EMU
#include <stdarg.h>
#include "../lib/type.h"
#include <stdio.h>
#include <pthread.h>
extern pthread_mutex_t log_mu; 
typedef enum LOG_TYPE
{
	LOG,
	ERR,
	WARN,
	XIL,
	DEBUG,
} LOG_TYPE;

void EMU_log_println(LOG_TYPE T, const char fmt[], ...);
static inline void dprint(const char fmt[], ...)
{
	pthread_mutex_lock(&log_mu);
	va_list args = NULL;
	va_start(args, fmt);
	printf("[DEBUG]");
	vprintf(fmt, args);
	va_end(args);
	pthread_mutex_unlock(&log_mu);
}
static inline void xil_printf(const char fmt[], ...)
{
	pthread_mutex_lock(&log_mu);
	va_list args = NULL;
	va_start(args, fmt);
	printf("[XIL]");
	vprintf(fmt, args);
	va_end(args);
	pthread_mutex_unlock(&log_mu);
}
#endif

#endif