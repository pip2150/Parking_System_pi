#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#define VERTICAL 0
#define HORIZONTAL 1

class Plate {
private:
	enum { UP, DOWN, LEFT, RIGHT };

public:
	Plate(cv::Mat& img);

	class Number {
	public:
		Number(cv::Mat &src);
		cv::Mat img;
		cv::Mat canonical;
		void Number::canonicalize(int sampleSize);
	};

	cv::Mat img;
	std::vector<Number> numbers;

	static void find(cv::Mat &gray, std::vector<cv::RotatedRect> &rects);
	static cv::RotatedRect detect(cv::Mat &input, cv::RotatedRect &rect);
	static void extract(cv::Mat &input, std::vector<Plate> &PossiblePlates);
	static bool verifySizes(cv::RotatedRect &mr);

	void findNumbers();
	cv::Mat Plate::canonicalize();
	void Plate::endPoint(std::vector<cv::Point> &contour, cv::Point mPoint[4]);
	void warpingRotatedRect(cv::Mat &srcMat, cv::Mat &dscMat, cv::RotatedRect roPosition);
};