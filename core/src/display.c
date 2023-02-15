#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>

#include "display.h"
#include "debug_msg.h"

#define CLIENT_SOCK_FILE "client.sock"
#define SERVER_SOCK_FILE "../../emulator/cmake-build-debug/.socket"

static int fd;
static vid2led_t *display_vid2led_object;
static sigset_t set;

extern matrix_t video_frame;

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

static int socket_init() {
    struct sockaddr_un addr;
    int ok = 1;

    if ((fd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        ok = 0;
    }

    if (ok) {
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, CLIENT_SOCK_FILE);
        unlink(CLIENT_SOCK_FILE);
        if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("bind");
            ok = 0;
        }
    }

    if (ok) {
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, SERVER_SOCK_FILE);

        printf("Connecting to the emulator...\n");
        while (1) {
            if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) != -1) {
                perror("connect");
                break;
            }
            msleep(1000);
        }
    }

    return 0;
}

static void display_interrupt_handler(int signo)
{
    (void)signo;
    video_stream_t *vid = &display_vid2led_object->video_stream;

    if (vid->frames_displayed < vid->frames) {
        if (!rgb_buffer_read(&display_vid2led_object->rgb_buffer, &video_frame)) {
            display_frame(&video_frame);
            vid->frames_displayed++;
        } else {
            DEBUG_MSG("Display stutters");
        }
    }
}

static void set_timer_interrupt(uint32_t usec_period) {
    struct itimerspec tim_spec;
    struct sigaction act;
    timer_t t_id;

    uint32_t seconds = (usec_period - (usec_period % 1000000))/1000000;
    uint32_t nanoseconds = (usec_period % 1000000) * 1000;

    sigemptyset( &set );
    sigaddset( &set, SIGALRM );

    act.sa_flags = 0;
    act.sa_mask = set;
    act.sa_handler = &display_interrupt_handler;

    sigaction( SIGALRM, &act, NULL );

    if (timer_create(CLOCK_MONOTONIC, NULL, &t_id))
        perror("timer_create");

    tim_spec.it_interval.tv_sec = seconds;
    tim_spec.it_interval.tv_nsec = nanoseconds;
    tim_spec.it_value.tv_sec = seconds;
    tim_spec.it_value.tv_nsec = nanoseconds;

    if (timer_settime(t_id, 0, &tim_spec, NULL))
        perror("timer_settime");
}

void enable_interrupts(void) {
    if (sigprocmask(SIG_UNBLOCK, &set, NULL) == -1)
        perror("sigprocmask");
}

void disable_interrupts(void) {
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    if (sigprocmask(SIG_SETMASK, &set, NULL) == -1) {
        perror("sigprocmask");
    }
}

int display_init(vid2led_t *vid2led_object) {
    display_vid2led_object = vid2led_object;

    socket_init();
    if (vid2led_object->video_stream.framerate != 0) {
        set_timer_interrupt(FRAMERATE_TO_USEC(vid2led_object->video_stream.framerate));
    }

    return 0;
}

int display_frame(matrix_t *frame) {
    if (send(fd, frame, sizeof(matrix_t), 0) == -1) {
        perror("send");
        return -1;
    }

    return 0;
}

int display_deinit(vid2led_t *vid2led_object) {
    (void)vid2led_object;

    if (fd >= 0) {
        close(fd);
    }
    unlink (CLIENT_SOCK_FILE);

    return 0;
}