#include <opencv2/opencv.hpp>
#include <random>
using namespace cv;

const int ROWS = 32;
const int COLS = 32;

const int WIDTH = 640;
const int HEIGHT = 640;

Scalar arr[ROWS][COLS] = {0};

void receive_frame() {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 255); // define the range

    for (auto& x: arr) {
        for (auto& y : x) {
            y = Scalar(distr(gen), distr(gen), distr(gen));
        }
    }
}

int main(int argc, char** argv) {

    Mat image(HEIGHT, WIDTH, CV_8UC3,
              Scalar(255, 255, 255));
    if (!image.data) {
        std::cout << "Could not open image\n";
        return -1;
    }

    while (1) {
        receive_frame();

        const int dX = WIDTH/COLS;
        const int dY = HEIGHT/ROWS;

        for (auto i = 0; i < ROWS; i++) {
            for (auto j = 0; j < COLS; j++) {
                rectangle(image,
                          Point(j * dX, i * dY),
                          Point(j * dX + dX, i * dY + dY),
                          arr[i][j],FILLED, LINE_8);
            }
        }

        namedWindow("vid2led_emulator", WINDOW_AUTOSIZE);
        imshow("vid2led_emulator", image);

        if (waitKey(41) == 27) {
            break;
        }
    }

    return 0;
}
