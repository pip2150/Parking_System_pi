#include "utils1.hpp"

int main() {
    char str[50] = "";

    VideoCapture cap;
    cap.open(0);

    if (!cap.isOpened()) {
        printf("카메라가 연결되지 않았습니다.\n");
        exit(-1);
    }

    Mat frame;
    //         관심영역 설정 후 저장
    cap.read(frame);
    Rect rect(178, 328, 60, 45);
    Mat storetempl = frame(rect);
    imwrite("templ.jpg", storetempl);
    
    Mat templ = readImage("templ.jpg");
    
    for (;;) {
        Mat result;
        double minVal, maxVal;
        Point minLoc, maxLoc;
        
        cap.read(frame);
        //Rect testrect(170, 350, 135, 100);
        //Mat testframe = frame(testrect);

        matchTemplate(frame, templ, result, TM_CCOEFF_NORMED);
        minMaxLoc(result, NULL, &maxVal, NULL, &maxLoc);
        
        rectangle(frame, maxLoc,
              Point(maxLoc.x + templ.cols, maxLoc.y+templ.rows), Scalar(255, 0, 255), 2);
        
        if (maxVal >=  0.9)
            printf("%4.2f\n", maxVal);
        imshow("영상", frame);

        if(waitKey(10) == 27)
            break;
    }
    
}
