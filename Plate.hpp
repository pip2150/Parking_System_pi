#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

class Plate {
private:
	cv::Mat img;
	cv::RotatedRect roPosition;

public:
	static std::vector<cv::RotatedRect> find(cv::Mat gray);
	static cv::RotatedRect detect(cv::Mat input, cv::RotatedRect rect);
	static Plate extract(cv::Mat input, cv::RotatedRect minRect);
	static bool verifySizes(cv::RotatedRect mr);
	static cv::Mat features(cv::Mat numbers, int sizeData);
	static cv::Mat projectedHistogram(cv::Mat img, int t);

	void findNumbers(cv::Mat src, std::vector <cv::Mat> &numbers);
	void warpingRotatedRect(cv::Mat srcMat, cv::Mat &dscMat);
	Plate(cv::Mat img, cv::RotatedRect roPosition);

	cv::Mat getImg();
	cv::Point upPoint(std::vector<cv::Point> contour);
	cv::Point downPoint(std::vector<cv::Point> contour);
	cv::Point leftPoint(std::vector<cv::Point> contour);
	cv::Point rightPoint(std::vector<cv::Point> contour);

};