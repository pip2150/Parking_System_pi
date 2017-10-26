#include <opencv2/opencv.hpp>
#include "opencv/process.hpp"

using namespace std;
using namespace process;

/**
	@mainpage   Parking System
	@section	소개
	- 프로젝트 이름      :   Parking System on Device(Raspberry) with OpenCV Library

	- 프로젝트 내용 :		주차장에 설치된 Camera 에서 얻은 영상 처리하고 기계 학습하여 얻은 정보를 네트워크를 통해 서버와 휴대 전화와 공유한다.
					->처리 순서
						1. Camera 로 부터 받은 이미지를 영상 처리
						2. 서포트 벡터 머신을 이용하여(SVM) 번호판을 인식
						3. 번호판의 좌표를 통해 상대적 좌표를 도출
						4. 인경신경망을 이용한 광학 문자 판독기(OCR) 통해 문자 인식
						5. 서버 전송

	@section   동작 환경
	- 운영체제    : raspian
	- 의존성     : C++ 11
				 OpenCV 3.2 이상
				 Cmake 2.8 이상
				 OpenMP
	@section	src
	- Network
				@see socket.hpp
				@see http.hpp
				@see psAPI.hpp
	- OpenCV
				@see ocr.hpp
				@see plate.hpp
				@see process.hpp
				@see svm.hpp
				@see tools.hpp
	- main
				@see main.cpp
	*/

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
		"{ o              |     | enable OCRtrain       }"
		"{ v              |     | enable SVMtrain       }"
		"{ t              |     | print cost time       }"
		"{ s              |     | print plate's string  }"
		"{ w              |     | show debuging window  }"
		"{ a              |     | print analysis        }"
		"{ m              |     | not use ML  }"
		"{ A              |     | enable all exp. -m -o -v }"
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
	string key = "novtswam";

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
	if (parser.has("o"))
		mode |= OCRTRAIN;
	if (parser.has("v"))
		mode |= SVMTRAIN;
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
		mode |= 0xFF ^ OCRTRAIN ^ SVMTRAIN ^ NOTUSEML;

	// 오류 발생시 오류 내용 출력
	if (!parser.check()) {
		parser.printErrors();
		return 0;
	}

	return startOpencv(width, height, mode, info, answer);
}
