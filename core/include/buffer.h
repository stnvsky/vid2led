#ifndef VID2LED_BUFFER_H
#define VID2LED_BUFFER_H

#include <stdint.h>
#include "config.h"

typedef struct rgb_scalar_s {
    uint8_t R;
    uint8_t G;
    uint8_t B;
} rgb_scalar_t;

typedef rgb_scalar_t matrix_t[ROWS][COLS];

typedef struct rgb_buffer_s {
    matrix_t *matrix_array;
    uint32_t written;
    uint32_t len;
} rgb_buffer_t;

rgb_buffer_t *init_rgb_buffer(uint32_t buf_len);
int read_rgb_buffer(rgb_buffer_t* buf);
int write_rgb_buffer(rgb_buffer_t* buf);

#endif //VID2LED_BUFFER_H
