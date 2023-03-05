#include "vid2led.h"

int main() {
    vid2led_t vid2led;

    vid2led.buffer_length = VID2LEN_STATIC_BUFFER_LENGTH;
    vid2led.video_format = VID_TYPE_AVI;
    vid2led.video_path = "./test.avi";

    vid2led_init(&vid2led);

    while (!vid2led_service(&vid2led)) {
        msleep(100);
    }

    vid2led_deinit(&vid2led);
    return 0;
}
