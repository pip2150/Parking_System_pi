#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"

using namespace cv;
using namespace std;

Mat readImage (string fn, int mode = 1) {
    string image_path = "/home/pi/project/webcam/";

    Mat image = imread(image_path + fn, mode);

    if (image.empty()) {
        cout << "파일이 없습니다." << endl;
        exit(1);
    }

    return image;
}

