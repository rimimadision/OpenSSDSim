#include"emu_config.h"
#include"emu_log.h"
#include <stdio.h>
#include<stdarg.h>
#include<pthread.h>

pthread_mutex_t log_mu = PTHREAD_MUTEX_INITIALIZER;

void EMU_log_println(LOG_TYPE T, const char fmt[], ...)
{
#ifdef EMU
	pthread_mutex_lock(&log_mu);
	va_list args = NULL;
	va_start(args, fmt);
	switch (T)
	{
	case LOG:
		printf("[EMU LOG]");
		break;
	case WARN:
		printf("[EMU WARN]");
		break;
	case ERR:
		printf("[EMU ERR]");
		break;
	case DEBUG:
		printf("[EMU DEBUG]");
		break;
	default:
		printf("Log not identified\n");
		return;
	}
	
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
	pthread_mutex_unlock(&log_mu);
#endif
}