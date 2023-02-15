#ifndef VID2LED_DEBUG_MSG_H
#define VID2LED_DEBUG_MSG_H

#include "config.h"

void debug_msg(const char* func, const char *format, ...);

#if (VID2LEN_DEBUG_MESSAGES != 0)
#define DEBUG_MSG(fmt, args...) debug_msg(__func__, fmt, ##args)
#else
#define DEBUG_MSG(fmt, ...) (void)0;
#endif

#endif // VID2LED_DEBUG_MSG_H
