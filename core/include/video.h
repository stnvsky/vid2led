#ifndef LINUX_VIDEO_H
#define LINUX_VIDEO_H

#include <stdio.h>
#include "buffer.h"

typedef enum {
    VID_TYPE_DUMMY,
    VID_TYPE_AVI
} vid_type;

typedef struct video_stream_s {
    vid_type type;
    int (*get_frame)(matrix_t *);
    int frames;
    int framerate;
    FILE *fd;
} video_stream_t;

int init_video(video_stream_t* vid, const char *path, vid_type type);

#endif //LINUX_VIDEO_H
