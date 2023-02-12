#include "buffer.h"
#include "video.h"
#include "debug_msg.h"
// sockets
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

rgb_buffer_t rgb_buffer = {0};
video_stream_t video_stream = {0};
matrix_t video_frame = {0};

#define CLIENT_SOCK_FILE "client.sock"
#define SERVER_SOCK_FILE "/home/szymon/workspace/vid2led/emulator/cmake-build-debug/.socket"

#include <time.h>
#include <errno.h>

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

int main() {
    rgb_buffer_t *buf = &rgb_buffer;
    video_stream_t *vid = &video_stream;
    matrix_t *frame = &video_frame;
    int retval;

    int fd;
    struct sockaddr_un addr;
    char buff[8192];
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

    if (init_rgb_buffer(buf, STATIC_BUFFER_LENGTH)) {
        DEBUG_MSG("Could not initialize RGB matrix buffer");
        return -1;
    }

    if (init_video(vid, "./video.avi", VID_TYPE_DUMMY)) {
        DEBUG_MSG("Could not initialize RGB matrix buffer");
        return -1;
    }

    do {
        vid->get_frame(frame);
        write_rgb_buffer(buf, frame);
        retval = read_rgb_buffer(buf, frame);
        strcpy (buff, "iccExchangeAPDU");
        if (send(fd, frame, sizeof(matrix_t), 0) == -1) {
            perror("send");
            ok = 0;
        }
        msleep(42);
    } while (!retval);


    if (fd >= 0) {
        close(fd);
    }
    unlink (CLIENT_SOCK_FILE);

    return 0;
}
