#include "utils.hpp"

using namespace cv;
using namespace std;

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
		//cerr << "Fail To Write." << endl;
		return 1;
	}
	return 0;
}