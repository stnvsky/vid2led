#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "debug_msg.h"


#if (STATIC_BUFFER != 0)
matrix_t matrix_array[STATIC_BUFFER_LENGTH] = {0};

int init_rgb_buffer(rgb_buffer_t *buf, uint32_t buf_len) {
    if (buf == NULL) {
        return -1;
    }
    if (buf_len > STATIC_BUFFER_LENGTH) {
        DEBUG_MSG("Requested buffer length is greater then maximum buffer size");
        return -1;
    }

    buf->matrix_array = matrix_array;
    buf->len = buf_len;
    buf->written = 0;

    return 0;
}
#else
int init_rgb_buffer(rgb_buffer_t *buf, uint32_t buf_len) {
    if (buf == NULL) {
        return -1;
    }

    buf->matrix_array = (matrix_t*)malloc(buf_len * sizeof(matrix_t));
    buf->len = buf_len;
    buf->written = 0;
    buf->cursor_read = 0;
    buf->cursor_write = 0;

    return 0;
}
#endif // (STATIC_BUFFER != 0)

int read_rgb_buffer(rgb_buffer_t* buf, matrix_t *frame) {
    if (buf->written) {
        memcpy(frame[0], buf->matrix_array + buf->cursor_read, sizeof(matrix_t));
        buf->cursor_read++;
        buf->cursor_read = buf->cursor_read % buf->len;
        buf->written--;
    } else {
        return 1;
    }

    return 0;
}

int write_rgb_buffer(rgb_buffer_t* buf, matrix_t *frame) {
    if (buf->written < buf->len) {
        buf->cursor_write++;
        buf->cursor_write = buf->cursor_write %  buf->len;
        memcpy(buf->matrix_array + buf->cursor_write, frame[0], sizeof(matrix_t));
        buf->written++;
    } else {
        return 1;
    }

    return 0;
}
