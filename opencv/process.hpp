#ifndef PROCESS_HPP_
#define PROCESS_HPP_

/** @brief Process
	종합적인 진행 namespace
*/
namespace process {

	//! image를 입력받을 곳
	enum FROM {
		CAMERA = 0,		//!< Camera로 부터
		FILESYSTEM = 1	//!< File System으로 부터
	};

	//! 카메라 방향
	enum WAY {
		NONE = -1,		//!< NONE
		ENTER = 0,		//!< 입구
		EXIT = 1		//!< 출구
	};

	//! 프로그램 Mode
	enum MODE {
		NETWORK = 0x01,			//!< Network 활성화
		OCRTRAIN = 0x02,		//!< OCRTrain 활성화
		SVMTRAIN = 0x04,		//!< SVMTrain 활성화
		COSTTIME = 0x08,		//!< 소요시간 표시
		PLATESTR = 0x10,		//!< 소요시간 표시
		WINDOWON = 0x20,		//!< 각 과정을 Windows로 띄우기
		ANALYSIS = 0x40,		//!< 통계적 계산 측정
		NOTUSEML = 0x80,		//!< 인공신경망 사용하지 않기
	};

#define SEGMENTSIZE 4	//!< 구역의 갯수	
#define FROM CAMERA		
#define ESC 27

	/**
		@brief 주차장 정보
	*/
	struct ParkingInfo {
		WAY way;					//!< 주차장 출입구 방향
		int floor;					//!< 주차장 층
		std::string zoneName;		//!< 주차 구역 이름
	};

	/**
		@brief 주차 차량 정보
	*/
	struct Table {
		std::string plateStr = "";		//!< 번호판 텍스트
		int match = 0;				//!< 일치한 횟수
		bool sended = false;		//!< Sever 전송 여부
	};

	/**
		@brief Server 에게 보내기
		@param info 주차장 정보
		@param table 주차 차량 정보
	*/
	void send2Server(const ParkingInfo &info, Table table[SEGMENTSIZE + 1]);

	/**
		@brief 차량 위치 도출
		@param area SEGMENTSIZE 개수로 나뉘어진 사각형 영역
		@param position 번호판의 좌표
		@return zoneIndex 도출된 차량 위치
		position이 각각의 area 중에 포함되는지 검사한다.
	*/
	int deductIndex(const cv::Rect area[SEGMENTSIZE], const cv::Point &position);

	/**
		@brief Debug용 주차 차량 정보 테이블 출력
		@param table 출력할 테이블 배열 @see Table
	*/
	void printTable(Table table[SEGMENTSIZE + 1]);

	/**
		@brief Opencv 작업 시작
		@param width 영상 처리할 이미지의 가로 길이
		@param height 영상 처리할 이미지의 세로 길이
		@param mode 영상 처리에 대한 모드
		@param info 주차 차량 정보
		@param answer 통계 정보 또는 결과물을 훈련 데이터에 저장하기 위한 정답 @see OCRTrainer
	*/
	int startOpencv(int width, int height, int mode, process::ParkingInfo info, std::string answer);
}

#endif