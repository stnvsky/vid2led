#ifndef VID2LED_VIDEO_H
#define VID2LED_VIDEO_H

#include <stdio.h>
#include "buffer.h"

typedef enum {
    VID_TYPE_DUMMY,
    VID_TYPE_AVI
} vid_format;

typedef struct video_stream_s {
    vid_format type;
    int (*get_frame)(matrix_t *);
    int framerate;
    int frames;
    int frames_buffered;
    int frames_displayed;
    FILE *fd;
} video_stream_t;

int init_video(video_stream_t* vid, const char *path, vid_format type);
int deinit_video(video_stream_t* vid);

#endif // VID2LED_VIDEO_H
