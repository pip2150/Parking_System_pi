#include "Opencv/Opencv.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	int mode = 0;

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
		"{ A              |     | enable all option     }"
		;

	cv::CommandLineParser parser(argc, argv, keys);

	if (parser.has("help")) {
		parser.printMessage();
		return 0;
	}

	int width = parser.get<int>(0);
	int height = parser.get<int>(1);
	int floor = parser.get<int>(2);
	string zoneName = parser.get<string>(3);
    string answer = parser.get<string>(4);

	int way = -1;

	if (parser.has("enter") && parser.has("exit"))
		parser.printErrors();
	else {
		if (parser.has("enter"))
			way = ENTER;
		if (parser.has("exit"))
			way = EXIT;
	}

	string key = "nTptswam";

	for (int i = 0; i < key.length(); i++)
		if (parser.has(string(1, key[i])))
			mode |= 0x01 << i;

	if (parser.has("A"))
		mode |= 0xFF ^ TRAIN ^ NOTUSEML;

	if (!parser.check()) {
		parser.printErrors();
		return 0;
	}

	return startOpencv(width, height, way, floor, zoneName, answer, mode);
}
