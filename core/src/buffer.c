#include <string.h>
#include <stdlib.h>

#include "buffer.h"
#include "debug_msg.h"

#if (VID2LEN_USE_STATIC_BUFFER != 0)
matrix_t matrix_array[VID2LEN_STATIC_BUFFER_LENGTH] = {0};

int rgb_buffer_init(rgb_buffer_t *buf, uint32_t buf_len) {
    if (buf == NULL) {
        return -1;
    }
    if (buf_len > VID2LEN_STATIC_BUFFER_LENGTH) {
        DEBUG_MSG("Requested buffer length is greater then maximum buffer size");
        return -1;
    }

    buf->written = 0;
    buf->len = buf_len;
    buf->cursor_read = 0;
    buf->cursor_write = 0;
    buf->matrix_array = matrix_array;

    return 0;
}
#else
int rgb_buffer_init(rgb_buffer_t *buf, uint32_t buf_len) {
    if (buf == NULL) {
        return -1;
    }

    buf->len = buf_len;
    buf->written = 0;
    buf->cursor_read = 0;
    buf->cursor_write = 0;
    buf->matrix_array = (matrix_t*)malloc(buf_len * sizeof(matrix_t));

    return 0;
}
#endif // (VID2LEN_USE_STATIC_BUFFER != 0)

int rgb_buffer_read(rgb_buffer_t* buf, matrix_t *frame) {
    if (buf->written) {
        memcpy(frame[0], buf->matrix_array + buf->cursor_read, sizeof(matrix_t));
        buf->cursor_read++;
        buf->cursor_read = buf->cursor_read % buf->len;
        buf->written--;
    } else {
        return -1;
    }

    return 0;
}

int rgb_buffer_write(rgb_buffer_t* buf, matrix_t *frame) {
    if (buf->written < buf->len) {
        memcpy(buf->matrix_array + buf->cursor_write, frame[0], sizeof(matrix_t));
        buf->cursor_write++;
        buf->cursor_write = buf->cursor_write % buf->len;
        buf->written++;
    } else {
        return -1;
    }

    return 0;
}

bool rgb_buffer_is_full(rgb_buffer_t *buf) {
    if (buf->written == buf->len) {
        return true;
    } else {
        return false;
    }
}
