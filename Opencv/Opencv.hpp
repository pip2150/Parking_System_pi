#ifndef OPENCV_HPP_
#define OPENCV_HPP_

#define TRUE 1
#define FALSE 0

//* Camera로 부터
#define CAMERA 0
//* File System으로 부터
#define FILESYSTEM 1

/* ----- Debug Setting ----- */

//* 입구
#define ENTER 0
//* 출구
#define EXIT 1

//* 구역의 갯수
#define SEGMENTSIZE 4

//* image를 입력받을 곳
#define FROM CAMERA

//* Network 활성화
#define NETWORK		0x01

//* Train 활성화
#define TRAIN		0x02

//* Position 표시
#define POSITION 	0x04

//* 소요시간 표시
#define COSTTIME 	0x08

//* Plate의 추출된 문자열 표시
#define PLATESTR 	0x10

//* 각 과정을 Windows로 띄우기
#define WINDOWON 	0x20

//* 통계적 계산 측정
#define ANALYSIS	0x40

//* 인공신경망 사용하지 않기
#define NOTUSEML	0x80

/* ------------------------- */

/** 주차장에 대한 정보
*/
struct ParkingInfo {
	//* 주차장 출입구 방향
	int way;
	//* 주차장 층
	int floor;
	//* 주차 구역 이름
	std::string zoneName;
};

/** Opencv 작업 시작
*/
int startOpencv(int width, int height, ParkingInfo parkingInfo, std::string answer, int mode);

#endif