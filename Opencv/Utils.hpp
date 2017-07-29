#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class Utils {
public:
	static void drawRotatedRect(Mat &img, RotatedRect mr, Scalar color = Scalar(100, 100, 200), int thickness = 2);
	static bool isOverlap(Rect A, Rect B);
	static int readImage(string fn, Mat& image, int mode = 1);
	static int writeImage(string fn, Mat image, int mode = 1);
    template <typename T>
    static string to_string(T input);
};
