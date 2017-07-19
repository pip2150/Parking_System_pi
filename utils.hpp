#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class Utils {
public:

	static void drawRotatedRect(Mat &img, RotatedRect mr, Scalar color = Scalar(100, 100, 200), int thickness = 2) {
		Point2f pts[4];

		mr.points(pts);

		for (int i = 0; i < 4; ++i) {
			line(img, pts[i], pts[(i + 1) % 4], color, thickness);
		}
	}
	static bool isOverlap(Rect A, Rect B) {
		return (A & B).area() > 0;
	}

	static int readImage(string fn, Mat& image, int mode = 1) {
		image = imread(fn, mode);

		if (image.empty()) {
			//cerr << "File No Exist." << endl;
			return 1;
		}
		return 0;
	}

	static int writeImage(string fn, Mat image, int mode = 1) {
		if (!imwrite(fn, image)) {
			cerr << "Fail To Write." << endl;
			return 1;
		}
		return 0;
	}
    template <typename T>
    static string to_string(T input){
        ostringstream ss;
        ss<< input ;
        return ss.str();
    }
};
