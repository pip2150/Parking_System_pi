#include <opencv2/opencv.hpp>
#include "opencv/process.hpp"

using namespace std;
using namespace process;

/**
	@mainpage   Parking System
	@section	�Ұ�
	- ������Ʈ �̸�      :   Parking System on Device(Raspberry) with OpenCV Library

	- ������Ʈ ���� :		�����忡 ��ġ�� Camera ���� ���� ���� ó���ϰ� ��� �н��Ͽ� ���� ������ ��Ʈ��ũ�� ���� ������ �޴� ��ȭ�� �����Ѵ�.
					->ó�� ����
						1. Camera �� ���� ���� �̹����� ���� ó��
						2. ����Ʈ ���� �ӽ��� �̿��Ͽ�(SVM) ��ȣ���� �ν�
						3. ��ȣ���� ��ǥ�� ���� ����� ��ǥ�� ����
						4. �ΰ�Ű���� �̿��� ���� ���� �ǵ���(OCR) ���� ���� �ν�
						5. ���� ����

	@section   ���� ȯ��
	- �ü��    : raspian
	- ������     : C++ 11
				 OpenCV 3.2 �̻�
				 Cmake 2.8 �̻�
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

	// ���� ȣ�� �� ����� ����
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

	// image �ʺ�
	int width = parser.get<int>(0);
	// image ����
	int height = parser.get<int>(1);
	ParkingInfo info = { NONE, parser.get<int>(2), parser.get<string>(3) };
	// ���� �н� �Ǵ� ��� ��ġ ����� ���� ��
	string answer = parser.get<string>(4);
	string key = "novtswam";

	// �Ա� ���, �ⱸ ��� ���ÿ� ���� ���� ��� ���� ���
	if (parser.has("enter") && parser.has("exit"))
		parser.printErrors();
	else {
		if (parser.has("enter"))	// �Ա� ���
			info.way = ENTER;
		if (parser.has("exit"))		// �ⱸ ���
			info.way = EXIT;
	}

	// ��� ����
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

	// ���� �߻��� ���� ���� ���
	if (!parser.check()) {
		parser.printErrors();
		return 0;
	}

	return startOpencv(width, height, mode, info, answer);
}
