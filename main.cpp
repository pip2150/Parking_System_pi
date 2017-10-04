#include <opencv2\opencv.hpp>
#include "Opencv\Opencv.hpp"

using namespace std;
using namespace process;

int main(int argc, char *argv[]) {
	int mode = 0;

	// 도움말 호출 시 출력할 내용
	const string keys =
		"{ help h usage ? |     | print this message    }"
		"{ @width         | 640 | width of image        }"
		"{ @height        | 480 | height of image       }"
		"{ @floor         | 0   | floor of parking area }"
		"{ @zone          | Z   | name of parking zone  }"
		"{ @answer        | -1  | answer                }"
		"{ enter          |     | camera in enterance   }"
		"{ exit           |     | camera in exit        }"
		"{ n              |     | enable netwrok        }"
		"{ T              |     | enable train          }"
		"{ p              |     | print section         }"
		"{ t              |     | print cost time       }"
		"{ s              |     | print plate's string  }"
		"{ w              |     | show debuging window  }"
		"{ a              |     | print analysis        }"
		"{ m              |     | not use ML  }"
		"{ A              |     | enable all exp. -m -T }"
		;

	cv::CommandLineParser parser(argc, argv, keys);

	if (parser.has("help")) {
		parser.printMessage();
		return 0;
	}

	// image 너비
	int width = parser.get<int>(0);
	// image 높이
	int height = parser.get<int>(1);
	ParkingInfo info = { NONE, parser.get<int>(2), parser.get<string>(3) };
	// 지도 학습 또는 통계 수치 계산을 위한 답
	string answer = parser.get<string>(4);
	string key = "nTptswam";

	// 입구 모드, 출구 모드 동시에 지정 했을 경우 오류 출력
	if (parser.has("enter") && parser.has("exit"))
		parser.printErrors();
	else {
		if (parser.has("enter"))	// 입구 모드
			info.way = ENTER;
		if (parser.has("exit"))		// 출구 모드
			info.way = EXIT;
	}

	// 모드 지정
	if (parser.has("n"))
		mode |= NETWORK;
	if (parser.has("T"))
		mode |= TRAIN;
	if (parser.has("p"))
		mode |= POSITION;
	if (parser.has("t"))
		mode |= COSTTIME;
	if (parser.has("s"))
		mode |= PLATESTR;
	if (parser.has("w"))
		mode |= WINDOWON;
	if (parser.has("a"))
		mode |= ANALYSIS;
	if (parser.has("m"))
		mode |= NOTUSEML;
	if (parser.has("A"))
		mode |= 0xFF ^ TRAIN ^ NOTUSEML;

	// 오류 발생시 오류 내용 출력
	if (!parser.check()) {
		parser.printErrors();
		return 0;
	}

	return startOpencv(width, height, mode, info, answer);
}