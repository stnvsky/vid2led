#ifndef VID2LED_H
#define VID2LED_H

#include <stdio.h>
#include "buffer.h"
#include "video.h"
#include "config.h"

typedef struct vid2led_s {
    uint32_t buffer_length;
    vid_format video_format;
    const char* video_path;
    rgb_buffer_t rgb_buffer;
    video_stream_t video_stream;
} vid2led_t;

#include "display.h"

int vid2led_init(vid2led_t *vid2led_object);
int vid2led_deinit(vid2led_t *vid2led_object);
int vid2led_service(vid2led_t *vid2led_object);

#endif // VID2LED_H
