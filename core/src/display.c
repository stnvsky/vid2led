#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>

#include "display.h"

#define CLIENT_SOCK_FILE "client.sock"
#define SERVER_SOCK_FILE "/home/szymon/workspace/vid2led/emulator/cmake-build-debug/.socket"

/* msleep(): Sleep for the requested number of milliseconds. */

int fd;
rgb_buffer_t *display_rgb_buffer;
matrix_t video_frame = {0};

timer_t t_id;
struct sigaction act;
sigset_t set;
struct itimerspec tim_spec;

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

static void handler( int signo )
{
    (void)signo;
    static int i = 0;
    read_rgb_buffer(display_rgb_buffer, &video_frame);
    display_frame(&video_frame);
    printf("%d. frame displayed\n", i++);
}

void set_timer_interrupt() {
    sigemptyset( &set );
    sigaddset( &set, SIGALRM );

    act.sa_flags = 0;
    act.sa_mask = set;
    act.sa_handler = &handler;

    sigaction( SIGALRM, &act, NULL );

    if (timer_create(CLOCK_MONOTONIC, NULL, &t_id))
        perror("timer_create");

    tim_spec.it_interval.tv_sec = 1;
    tim_spec.it_interval.tv_nsec = 0;
    tim_spec.it_value.tv_sec = 1;
    tim_spec.it_value.tv_nsec = 0;

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

int display_init(rgb_buffer_t *buf, video_stream_t *vid_stream) {
    struct sockaddr_un addr;
    int ok = 1;

    display_rgb_buffer = buf;

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

int display_frame(matrix_t *frame) {
    if (send(fd, frame, sizeof(matrix_t), 0) == -1) {
        perror("send");
        return -1;
    }

    return 0;
}

int display_deinit(void) {
    if (fd >= 0) {
        close(fd);
    }
    unlink (CLIENT_SOCK_FILE);

    return 0;
}