#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "video.h"
#include "debug_msg.h"

#include "avi.h"

static int get_frame_dummy(matrix_t *frame) {
    int i, j;

    for (i = 0; i < VID2LEN_FRAME_ROWS; i++) {
        for (j = 0; j < VID2LEN_FRAME_COLS; j++) {
            frame[0][i][j].R = rand() % 256;
            frame[0][i][j].G = rand() % 256;
            frame[0][i][j].B = rand() % 256;
        }
    }

    return 0;
}

static void init_dummy(video_stream_t *vid) {
    srand(time(NULL));

    vid->type = VID_TYPE_DUMMY;
    vid->get_frame = get_frame_dummy;
    vid->framerate = 24;
    vid->frames = 120;
    vid->frames_buffered = 0;
    vid->frames_displayed = 0;
    vid->fd = NULL;
}

static int init_avi(video_stream_t *vid) {
    if (init_file_avi(vid->fd)) {
        DEBUG_MSG("Could not init AVI file");
        return -1;
    }

    vid->type = VID_TYPE_AVI;
    vid->get_frame = get_frame_avi;
    vid->framerate = 1; // [TODO]
    vid->frames = 5; // [TODO]
    vid->frames_buffered = 0;
    vid->frames_displayed = 0;

    return 0;
}

int init_video(video_stream_t* vid, const char *path, vid_format type) {
    if (vid == NULL) {
        return -1;
    }

    if (type != VID_TYPE_DUMMY) {
        vid->fd = fopen(path, "rb");
        if (!vid->fd) {
            DEBUG_MSG("Could not open video file");
            return -1;
        }
    }

    switch (type) {
        case VID_TYPE_DUMMY:
            (void)path;
            init_dummy(vid);
            break;
        case VID_TYPE_AVI:
            if (init_avi(vid)) {
                return -1;
            }
            break;
        default:
            break;
    }

    return 0;
}

int deinit_video(video_stream_t* vid) {
    switch (vid->type) {
        case VID_TYPE_DUMMY:
            break;
        case VID_TYPE_AVI:
            fclose(vid->fd);
            break;
        default:
            break;
    }
}