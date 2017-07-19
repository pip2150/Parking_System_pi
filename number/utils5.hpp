#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/ml/ml.hpp"
#include "time.h"

using namespace cv;
using namespace cv::ml;
using namespace std;

struct Plate {
    Mat plate_img;
    RotatedRect ro_position;
};

bool verifySizes(RotatedRect mr);

Mat readImage (string fn, int mode = 1) {
    string image_path = "/home/pi/project/number/";
    Mat image = imread(image_path + fn, mode);

    if (image.empty()) {
        cout << "File No Exist." << endl;
        exit(1);
    }

    return image;
}

void read_traindata(string fn, Mat & trainingData, Mat & classes) {
    FileStorage fs (fn, FileStorage::READ);

    if (!fs.isOpened()) {
        cout << "File Open Fail." << endl;
        exit(1);
    }

    fs["TrainingData"] >> trainingData;
    fs["classes"] >> classes;
    fs.release();

    trainingData.convertTo(trainingData, CV_32FC1);
}

void write_traindata(string fn, Mat trainingData, Mat classes) {
    FileStorage fs("SVMDATA.xml", FileStorage::WRITE);
    fs << "TrainingData" << trainingData;
    fs << "classes" << classes;
    fs.release();
}

void collect_trainImages(Mat & trainingData, Mat & classes) {
    int imagecnt = 110;
    for( int i = 0; i < imagecnt; i++ ) {
        string path = format("trainimage/%d.png", i);
        Mat img = readImage(path, 0);
        Mat tmp = img.reshape(1, 1);
        trainingData.push_back(tmp);
        if ( i < 75 )
            classes.push_back(1);
        else
            classes.push_back(0);
    }
    trainingData.convertTo(trainingData, CV_32FC1);
}

void draw_rotatedRect(Mat &img, RotatedRect mr, Scalar color=Scalar(100, 100, 200), int thickness = 2) {
    Point2f pts[4];
    mr.points(pts);

    for (int i = 0; i < 4; ++i) {
        line (img, pts[i], pts[(i+1)%4], color, thickness);
    }
}

vector<RotatedRect> find_plates(Mat gray) {
    Mat sobel, th_img, morph;

    Sobel(gray, sobel, CV_8U, 1, 0, 3);
    threshold(sobel, th_img, 0, 255, THRESH_OTSU+THRESH_BINARY);
    Mat kernel(3, 17, CV_8UC1, Scalar(1));
    morphologyEx(th_img, morph, MORPH_CLOSE, kernel);

    vector < vector< Point> > contours;
    findContours(morph.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    vector<RotatedRect> rects;
    for (int i = 0; i < (int) contours.size(); i++) {
        RotatedRect mr = minAreaRect(contours[i]);

        if (verifySizes(mr)) rects.push_back(mr);
        else contours[i].clear();
    }

    return rects;
}

bool verifySizes(RotatedRect mr) {
    float min = 800;
    float max = 70000;

    float rmin = 2.0;
    float rmax = 5.5;
    Rect_<float> rect(min, rmin, max, rmax);

    float asp_max = mr.size.width;
    float asp_min = mr.size.height;
    if(asp_min > asp_max)
        swap(asp_max, asp_min);

    Point2f pt ( (float)mr.size.area(), asp_max / asp_min);

    return rect.contains(pt);
}

RotatedRect detect_plate(Mat input, RotatedRect rect) {
    Size size = rect.size;
    int minSize = (size.width < size.height)? size.width : size.height;
    minSize = (int)cvRound(minSize*0.3);

    srand ((int)time(NULL));

    Scalar IoDiff(40, 40, 40);
    Scalar upDiff(40, 40, 40);
    int connectivity = 4;
    int flags = connectivity + 0xff00 + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;

    Rect ccomp;
    Mat mask(input.size() + Size(2, 2), CV_8UC1, Scalar(0));
    for(int j = 0; j < 10; j++) {
        int radius = rand() % (int)minSize - (minSize / 2);
        Point seed = (Point)rect.center + Point(radius, radius);
        int area = floodFill(input, mask, seed, Scalar(250, 0, 0), &ccomp, IoDiff, IoDiff, flags);
    }

    vector<Point> pointsInterest;
    Mat_<uchar>::iterator itMask = mask.begin<uchar>();
    Mat_<uchar>::iterator end = mask.end<uchar>();
    for (; itMask!=end; ++itMask) {
        if (*itMask==255) {
            pointsInterest.push_back(itMask.pos());
        }
    }

    return minAreaRect(pointsInterest);
}

Plate extract_plate(Mat input, RotatedRect minRect) {
    Mat img_rotated, img_crop, resize_img, gray;

    Size m_size = minRect.size;
    float aspect = (float)m_size.width / m_size.height;
    float angle = minRect.angle;

    if (aspect < 1) {
        angle += 90;
        swap(m_size.width, m_size.height);
    }

    Mat rotmat = getRotationMatrix2D (minRect.center, angle, 1);
    warpAffine(input, img_rotated, rotmat, input.size(), CV_INTER_CUBIC);
    getRectSubPix(img_rotated, m_size, minRect.center, img_crop);

    resize(img_crop, resize_img, Size(144, 33), 0, 0, INTER_CUBIC);

    equalizeHist(resize_img, resize_img);

    Plate plate;
    plate.plate_img = resize_img;
    plate.ro_position = minRect;
    
    return plate;
}
