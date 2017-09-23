#ifndef OPENCV_HPP_
#define OPENCV_HPP_

#define CAMERA 0
#define FILESYSTEM 1
#define TRUE 1
#define FALSE 0
#define ENTER 0
#define EXIT 1

/* ----- Debug Setting ----- */
#define SEGMENTSIZE 4
#define FROM CAMERA
#define NETWORK		0x01
#define TRAIN		0x02
#define POSITION 	0x04
#define COSTTIME 	0x08
#define PLATESTR 	0x10
#define WINDOWON 	0x20
#define ANALYSIS	0x40
#define NOTUSEML	0x80
/* ------------------------- */

struct ParkingInfo {
	int way;
	int floor;
	std::string zoneName;
};

int startOpencv(int width, int height, ParkingInfo parkingInfo, std::string answer, int mode);

#endif