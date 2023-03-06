#ifndef VID2LED_BUFFER_H
#define VID2LED_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

typedef struct rgb_scalar_s {
    uint8_t B;
    uint8_t G;
    uint8_t R;
} rgb_scalar_t;

typedef rgb_scalar_t matrix_t[VID2LEN_FRAME_ROWS][VID2LEN_FRAME_COLS];

typedef struct rgb_buffer_s {
    uint16_t written;
    uint16_t len;
    uint16_t cursor_read;
    uint16_t cursor_write;
    matrix_t *matrix_array;
} rgb_buffer_t;

int rgb_buffer_init(rgb_buffer_t *buf, uint32_t buf_len) ;
int rgb_buffer_read(rgb_buffer_t *buf, matrix_t *frame);
int rgb_buffer_write(rgb_buffer_t *buf, matrix_t *frame);

bool rgb_buffer_is_full(rgb_buffer_t *buf);

#endif // VID2LED_BUFFER_H
