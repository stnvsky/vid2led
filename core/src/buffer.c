#include <stdlib.h>
#include "buffer.h"
#include "debug_msg.h"

rgb_buffer_t buffer = {0};

#if (STATIC_BUFFER != 0)
matrix_t matrix_array[STATIC_BUFFER_LENGTH] = {0};

rgb_buffer_t *init_rgb_buffer(uint32_t buf_len) {
    rgb_buffer_t *buf = &buffer;

    if (buf_len > STATIC_BUFFER_LENGTH) {
        DEBUG_MSG("Requested buffer length is greater then maximum buffer size");
        return NULL;
    }

    buf->matrix_array = matrix_array;
    buf->len = buf_len;
    buf->written = 0;

    return buf;
}
#else
rgb_buffer_t *init_rgb_buffer(uint32_t buf_len) {
    rgb_buffer_t *buf = &buffer;

    buf->matrix_array = (matrix_t*)malloc(buf_len * sizeof(matrix_t));
    buf->len = buf_len;
    buf->written = 0;

    return buf;
}
#endif // (STATIC_BUFFER != 0)

int read_rgb_buffer(rgb_buffer_t* buf) {
    (void)buf;
    return 0;
}

int write_rgb_buffer(rgb_buffer_t* buf) {
    (void)buf;
    return 0;
}