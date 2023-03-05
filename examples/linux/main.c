#include "vid2led.h"
#include "picojpeg.h"

#include <stdlib.h>
#include <stdio.h>
#include "avi.h"

int main() {
    vid2led_t vid2led;

    vid2led.buffer_length = VID2LEN_STATIC_BUFFER_LENGTH;
    vid2led.video_format = VID_TYPE_DUMMY;
    vid2led.video_path = "./video.avi";

    FILE* fp = fopen(FILE_NAME, "rb");
    if (!fp) {
        return -1;
    }

    init_file_avi(fp);

    for (int k = 0; k < 5; k++) {
        get_frame_avi(fp);
    }

    fclose(fp);

    vid2led_init(&vid2led);

    while (!vid2led_service(&vid2led)) {
        msleep(100);
    }

    vid2led_deinit(&vid2led);
    return 0;
}
