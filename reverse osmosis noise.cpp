#include "head.h"

//img--原图像
//dst--增强图像
//alpha1--原图像系数，通常取1.0~1.5
//alpha2--高斯滤波图像系数，通常取-0.5~-0.9
//eg. img_ehance(imgGray, imgRemoveNoise, 1.0, -0.5);
void img_ehance(Mat img, Mat& dst, double alpha1, double alpha2)
{
    Mat blur;
    GaussianBlur(img, blur, Size(0, 0), 30);
    addWeighted(img, alpha1, blur, alpha2, 0, dst);
}

void reverseOsmosisNoiseProcess() {
    printf("reverseOsmosisNoiseProcess start\n");

    string name = "doc1";
    string path = "Resources/osmosisNoiseProcess/" + name + ".jpg";

    Mat imgGray, img;
    img = imread(path);	
    cvtColor(img, imgGray, COLOR_BGR2GRAY);

    imshow("imgGray", imgGray);
    imwrite("Resources/osmosisNoiseProcess/" + name + "_gray.jpg", imgGray);

    Mat imgHist = showHist(imgGray);
    imshow("imgHist", imgHist);
    imwrite("Resources/osmosisNoiseProcess/" + name + "_hist.jpg", imgHist);

    Mat imgRemoveNoise;
    threshold(imgGray, imgRemoveNoise, 0, 255, THRESH_BINARY);
    //img_ehance(imgGray, imgRemoveNoise, 1.0, -0.5);
    //imshow("imgRemoveGauss", imgRemoveNoise);
    //imwrite("Resources/osmosisNoiseProcess/" + name + "_removeNoise.jpg", imgRemoveNoise);
    imshow("ehance imgHist", showHist(imgRemoveNoise));


    waitKey(0);
}