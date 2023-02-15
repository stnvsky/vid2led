#ifndef VID2LED_DISPLAY_H
#define VID2LED_DISPLAY_H

#include "vid2led.h"

#define FRAMERATE_TO_USEC(x)  (1000000/x)

int display_init(vid2led_t *vid2led_object);
int display_deinit(vid2led_t *vid2led_object);
int display_frame(matrix_t *frame);

void enable_interrupts(void);
void disable_interrupts(void);

//misc
int msleep(long msec);

#endif // VID2LED_DISPLAY_H
