#include "buffer.h"
#include "video.h"
#include "debug_msg.h"
#include "display.h"

rgb_buffer_t rgb_buffer = {0};
video_stream_t video_stream = {0};
extern matrix_t video_frame;

int main() {
    int ret = 0;

    if (init_rgb_buffer(&rgb_buffer, STATIC_BUFFER_LENGTH)) {
        DEBUG_MSG("Could not initialize RGB matrix buffer");
        return -1;
    }

    if (init_video(&video_stream, "./video.avi", VID_TYPE_DUMMY)) {
        DEBUG_MSG("Could not initialize RGB matrix buffer");
        return -1;
    }

    if (display_init(&rgb_buffer, &video_stream)) {
        DEBUG_MSG("Could not initialize display");
        return -1;
    }

    set_timer_interrupt();

    int i = 0;

    while(1) {
        disable_interrupts();
        do {
            video_stream.get_frame(&video_frame);
            ret = write_rgb_buffer(&rgb_buffer, &video_frame);
            i++;
        } while (!ret);
        printf("i = %d\n", i);
        i = 0;
        enable_interrupts();
        msleep(200);
    }

    display_deinit();

    return 0;
}
