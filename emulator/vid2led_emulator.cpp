#include <opencv2/opencv.hpp>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <string>

#include "buffer.h"

using namespace cv;

const int WIDTH = VID2LEN_FRAME_COLS*6;
const int HEIGHT = VID2LEN_FRAME_ROWS*6;

matrix_t arr = {0};

class SocketThread {
public:
    static SocketThread* Instance();

    bool start();
    void stop();
    void set_socket_path(const std::string& path);

    enum {
        MAX_BACKLOG = 20,
        BUFFER_SIZE = 8192
    };

private:
    SocketThread();
    ~SocketThread();

    static void* thread_starter(void* obj);
    static void thread_stopper(int sig, siginfo_t* siginfo, void* context);
    void* run_server();
    void handle_signals();
    void cleanup_socket();

    pthread_t pid_;
    pthread_mutex_t mutex_;
    std::string socket_path_;
    int sockfd_;
    int curr_sock_fd_;
    static SocketThread* instance_;
};

SocketThread* SocketThread::instance_ = NULL;

SocketThread* SocketThread::Instance() {
    if (!instance_) {
        instance_ = new SocketThread;
    }
    return instance_;
}

SocketThread::SocketThread()
        : pid_(0),
          socket_path_(".socket"),
          sockfd_(0),
          curr_sock_fd_(0) {
    fprintf(stdout, "Init mutex...\n");
    pthread_mutex_init(&mutex_, NULL);
}

SocketThread::~SocketThread() {
    stop();
}

bool SocketThread::start() {
    fprintf(stdout, "Creating thread...\n");
    if (pthread_create(&pid_, NULL, &(SocketThread::thread_starter), this) != 0) {
        fprintf(stderr, "Error: Failed to start thread\n");
        return false;
    }
    fprintf(stdout, "Thread created\n");
    handle_signals();
    return true;
}

void SocketThread::stop() {
    fprintf(stdout, "Destroying mutex...\n");
    pthread_mutex_destroy(&mutex_);
    if (pid_) {
        pthread_cancel(pid_);
        pid_ = 0;
    }
    if (sockfd_) {
        close(sockfd_);
        sockfd_ = 0;
    }
    cleanup_socket();
}

void SocketThread::set_socket_path(const std::string& path) {
    socket_path_ = path;
}

void *SocketThread::thread_starter(void *obj) {
    fprintf(stdout, "Starting thread starter...\n");
    return reinterpret_cast<SocketThread *>(obj)->run_server();
}

void SocketThread::thread_stopper(int sig, siginfo_t *siginfo, void *context) {
    fprintf(stdout, "Starting thread stopper...\n");
    Instance()->stop();
    exit(0);
}

#define SERVER_SOCK_FILE ".socket"

void *SocketThread::run_server() {
    int fd;
    struct sockaddr_un addr;
    struct sockaddr_un from;
    int ok = 1;
    socklen_t fromlen = sizeof(from);

    if ((fd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        ok = 0;
    }

    if (ok) {
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, SERVER_SOCK_FILE);
        unlink(SERVER_SOCK_FILE);
        if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("bind");
            ok = 0;
        }
    }

    while ((recvfrom(fd, arr, sizeof(arr), 0, (struct sockaddr *)&from, &fromlen)) > 0) {
    }
}

void SocketThread::handle_signals() {
    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_sigaction = &thread_stopper;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGTERM, &act, NULL) == -1) {
        fprintf(stderr, "Failed to register SIGTERM\n");
    }
    if (sigaction(SIGINT, &act, NULL) == -1) {
        fprintf(stderr, "Failed to register SIGTINT\n");
    }
}

void SocketThread::cleanup_socket() {
    if (access(socket_path_.c_str(), F_OK) != -1) {
        fprintf(stdout, "Cleanup socket\n");
        unlink(socket_path_.c_str());
    }
}

int main(int argc, char** argv) {

    Mat image(HEIGHT, WIDTH, CV_8UC3,
              Scalar(255, 255, 255));
    if (!image.data) {
        std::cout << "Could not open image\n";
        return -1;
    }

    SocketThread::Instance()->start();
    char buffer[SocketThread::BUFFER_SIZE];
    while (1) {
        const int dX = WIDTH/VID2LEN_FRAME_COLS;
        const int dY = HEIGHT/VID2LEN_FRAME_ROWS;

        for (auto i = 0; i < VID2LEN_FRAME_ROWS; i++) {
            for (auto j = 0; j < VID2LEN_FRAME_COLS; j++) {
                rectangle(image,
                          Point(j * dX, i * dY),
                          Point(j * dX + dX, i * dY + dY),
                          Scalar(arr[i][j].R, arr[i][j].G, arr[i][j].B),
                          FILLED, LINE_8);
            }
        }

        namedWindow("vid2led_emulator", WINDOW_AUTOSIZE);
        imshow("vid2led_emulator", image);

        if (waitKey(20) == 27) {
            break;
        }
    }

    SocketThread::Instance()->stop();
    return 0;
}
