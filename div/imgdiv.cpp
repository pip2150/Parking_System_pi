#include "opencv2/opencv.hpp"

using namespace cv;

int main() {
    VideoCapture cap;
    cap.open(0);

    if (!cap.isOpened()) {
        printf("카메라가 연결되지 않았습니다.\n");
        exit(-1);
    }
    Mat frame;
    cap.read(frame);

    Rect areaA(0, 0, frame.cols / 4, frame.rows);
    Rect areaB(frame.cols * 1 / 4, 0, frame.cols / 4, frame.rows);
    Rect areaC(frame.cols * 2 / 4, 0, frame.cols / 4, frame.rows);
    Rect areaD(frame.cols * 3 / 4, 0, frame.cols / 4, frame.rows);

    Mat areaA_image = frame(areaA);
    Mat areaB_image = frame(areaB);
    Mat areaC_image = frame(areaC);
    Mat areaD_image = frame(areaD);

    imshow("Basic", frame);
    imshow("Area A", areaA_image);
    imshow("Area B", areaB_image);
    imshow("Area C", areaC_image);
    imshow("Area D", areaD_image);

    waitKey(0);
}
