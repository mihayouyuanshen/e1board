#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "util_logger_g.h"
#include "common_g.h"

void LOG_fatal(const schar *fmt, ...)
{
#ifdef LOG_FATAL_OUT
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
#endif
}
void LOG_error(const schar *fmt, ...)
{
#ifdef LOG_ERROR_OUT
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
#endif
}
void LOG_info(const schar *fmt, ...)
{
#ifdef LOG_INFO_OUT
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
#endif
}
void LOG_warn(const schar *fmt, ...)
{
#ifdef LOG_WARN_OUT
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
#endif
}

void LOG_debug(const schar *fmt, ...)
{
#ifdef LOG_DEBUG_OUT
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
#endif
}

void LOG_entry(char *methodName)
{
#ifdef METHOD_ENTRY_LOG
	LOG_debug("->Entry method: [%s]", methodName);
#endif
}

void LOG_leave(char *methodName)
{
#ifdef METHOD_ENTRY_LOG
	LOG_debug("<-Leave method: [%s]", methodName);
#endif
}