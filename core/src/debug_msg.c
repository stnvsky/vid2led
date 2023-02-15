#include <stdio.h>
#include <stdarg.h>

#include "debug_msg.h"
#include "config.h"

/* This function needs to be implemented for each platform separately.
 * Length of the message is calculated with the terminating new line, but without
 * terminating zero, however a zero character is added after last new line character
 * in every message, so using functions like printf() is also safe here.
 *
 * [TODO] to be placed in separate directory
 */
static void platform_specific_print_function(const char *msg, int msg_len) {
    (void)msg_len;
    printf("%s", msg);
}

void debug_msg(const char* func, const char *format, ...)
{
    va_list list;
    int written;
    char buf[VID2LEN_DEBUG_MSG_MAX_LENGTH+2];
    ssize_t buf_size = sizeof(buf);

    if ((written = snprintf(buf, buf_size, "%s - ", func)) < 0) {
        return;
    }
    if (written > buf_size) {
        written = (int)buf_size;
    }

    va_start(list, format);
    if ((written += vsnprintf(buf + written,buf_size - written, format, list)) < 0) {
        return;
    }
    va_end(list);

    /* Add new line at the end of the message */
    if (written > buf_size-2) {
        written = (int)buf_size-2;
    }
    buf[written] = '\n';
    buf[written+1] = 0;

    platform_specific_print_function(buf, written+1);
}
