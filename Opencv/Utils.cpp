#include "utils.hpp"

using namespace cv;
using namespace std;

void utils::drawRotatedRect(Mat &img, RotatedRect &mr, Scalar color, int thickness) {
	Point2f pts[4];

	mr.points(pts);

	for (int i = 0; i < 4; ++i) {
		line(img, pts[i], pts[(i + 1) % 4], color, thickness);
	}
}
bool utils::isOverlap(Rect &A, Rect &B) {
	return (A & B).area() > 0;
}

int utils::readImage(string fn, Mat& image, int mode) {
	image = imread(fn, mode);

	if (image.empty()) {
		//cerr << "File No Exist." << endl;
		return 1;
	}
	return 0;
}

int utils::writeImage(string fn, Mat &image, int mode) {
	if (!imwrite(fn, image)) {
		cerr << "Fail To Write." << endl;
		return 1;
	}
	return 0;
}

//template <typename T>
//string utils::to_string(T input) {
//	ostringstream ss;
//	ss << input;
//	return ss.str();
//}