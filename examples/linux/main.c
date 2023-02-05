#include "buffer.h"
#include "video.h"
#include "debug_msg.h"


rgb_buffer_t rgb_buffer = {0};
video_stream_t video_stream = {0};
matrix_t video_frame = {0};

int main() {
    rgb_buffer_t *buf = &rgb_buffer;
    video_stream_t *vid = &video_stream;
    matrix_t *frame = &video_frame;
    int retval;

    retval = init_rgb_buffer(buf, STATIC_BUFFER_LENGTH);
    if (!buf) {
        DEBUG_MSG("Could not initialize RGB matrix buffer");
        return -1;
    }

    (void)retval;

    retval = init_video(vid, "./video.avi", VID_TYPE_DUMMY);
    (void)retval;

    do {
        vid->get_frame(frame);
        retval = write_rgb_buffer(buf, frame);
    } while (!retval);

    do {
        write_rgb_buffer(buf, frame);
        retval = read_rgb_buffer(buf, frame);
    } while (!retval);



    return 0;
}
