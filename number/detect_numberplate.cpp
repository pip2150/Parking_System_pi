#include "utils5.hpp"

int main() {
    Mat trainingData, classes;

    //collect_trainImages(trainingData, classes);
    //write_traindata("SVMDATA.xml", trainingData, classes);

    read_traindata("SVMDATA.xml", trainingData, classes);

    Ptr<SVM> svm = SVM::create();
    
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::LINEAR);
    svm->setDegree(0);
    svm->setGamma(1);
    svm->setCoef0(0);
    svm->setC(1);
    svm->setNu(0);
    svm->setP(0);
    svm->setTermCriteria(TermCriteria(CV_TERMCRIT_ITER, 1000, 0.01));

    svm->train(trainingData, ROW_SAMPLE, classes);

    int img_num;
    cout << "Car Number (0~16) : ";
    cin >> img_num;

    char fn[50] = "";
    sprintf(fn, "%d.jpg", img_num);
    Mat image = readImage(fn);

    Mat gray;
    cvtColor(image, gray, CV_BGR2GRAY);
    blur(gray, gray, Size(3, 3));

    vector<RotatedRect> rects = find_plates(gray);

    vector<Plate> posible_plates;
    
    for (int i = 0; i < (int)rects.size(); i++) {
        RotatedRect ro_rect = detect_plate(image, rects[i]);

        if (verifySizes(ro_rect)) {
            Plate plate = extract_plate(gray, ro_rect);
            posible_plates.push_back(plate);
        }
    }
    
    for (int i = 0; i < (int)posible_plates.size(); i++) {
        Mat img = posible_plates[i].plate_img.reshape(1, 1);
        img.convertTo(img, CV_32FC1);

        int response = (int)svm->predict(img);
        if (response == 1) {
            draw_rotatedRect(image, posible_plates[i].ro_position, Scalar(0, 0, 255), 2);
            imshow("Number Find", image);
        }
    }

    waitKey(0);
}
