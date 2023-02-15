#include <stdlib.h>

#include "vid2led.h"
#include "debug_msg.h"
#include "display.h"

matrix_t video_frame = {0};

int vid2led_init(vid2led_t *vid2led_obj) {
    if (rgb_buffer_init(&vid2led_obj->rgb_buffer, vid2led_obj->buffer_length)) {
        DEBUG_MSG("Could not initialize RGB matrix buffer");
        return -1;
    }

    if (init_video(&vid2led_obj->video_stream,
                   vid2led_obj->video_path, vid2led_obj->video_format)) {
        DEBUG_MSG("Could not initialize RGB matrix buffer");
        return -1;
    }

    if (display_init(vid2led_obj)) {
        DEBUG_MSG("Could not initialize display");
        return -1;
    }

    return 0;
}

int vid2led_deinit(vid2led_t *vid2led_obj) {
    display_deinit(vid2led_obj);
#if (VID2LEN_USE_STATIC_BUFFER == 0)
    free(vid2led_obj->rgb_buffer.matrix_array);
#endif
    return 0;
}

int vid2led_service(vid2led_t *vid2led_obj) {
    rgb_buffer_t *buf = &vid2led_obj->rgb_buffer;
    video_stream_t *vid = &vid2led_obj->video_stream;
    int ret;

    if (vid->frames_displayed >= vid->frames) {
        return -1;
    }

    if ((vid->frames_buffered < vid->frames)) {
        disable_interrupts();
        do {
            if (!rgb_buffer_is_full(buf)) {
                vid->get_frame(&video_frame);
                vid->frames_buffered++;
                ret = rgb_buffer_write(buf, &video_frame);
            } else {
                break;
            }
        } while (!ret);
        enable_interrupts();
    }

    return 0;
}