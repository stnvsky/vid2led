#include <opencv2/opencv.hpp>
#include <random>

using namespace cv;

const int ROWS = 27;
const int COLS = 89;

const int WIDTH = 1280;
const int HEIGHT = 640;

const int dX = WIDTH/COLS;
const int dY = HEIGHT/ROWS;

Scalar arr[ROWS][COLS] = {0};

void prepare_random_image() {
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

        prepare_random_image();

        for (auto i = 0; i < ROWS; i++) {
            for (auto j = 0; j < COLS; j++) {
                rectangle(image, Point(j * dX, i * dY), Point(j * dX + dX, i * dY + dY),
                          arr[i][j],
                          FILLED, LINE_8);
            }
        }

        namedWindow("vid2led_emulator", WINDOW_AUTOSIZE);
        imshow("vid2led_emulator", image);
        waitKey(41);
    }

    waitKey(0);
    return 0;
}
