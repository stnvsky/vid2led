#ifndef LINUX_DRIVER_H
#define LINUX_DRIVER_H

#include "buffer.h"
#include "video.h"

int display_init(rgb_buffer_t *buf, video_stream_t *vid_stream);
int display_deinit(void);
int display_frame(matrix_t *frame);

//misc
int msleep(long msec);
void set_timer_interrupt(void);
void enable_interrupts(void);
void disable_interrupts(void);

#endif //LINUX_DRIVER_H
