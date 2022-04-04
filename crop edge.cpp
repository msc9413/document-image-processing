#include "head.h"

void cropEdgeProcess() {
    printf("cropEdgeProcess start\n");

    // 获取图像名字 和 路径
    string name = "doc3";
    string path = "Resources/cropEdgeProcess/" + name + ".jpg";

	Mat img, imgBinary, imgDst, imgCanny, imgHist, grayHist, imgRoi;
	img = imread(path);
	cvtColor(img, imgBinary, COLOR_BGR2GRAY);
	threshold(imgBinary, imgBinary, 0, 255, THRESH_BINARY | THRESH_OTSU);

    imgRoi = imgBinary.clone();
    imgRoi = imgRoi(Rect(0, imgRoi.rows / 3, imgRoi.cols, imgRoi.rows * 1 / 3));
    imgHist = showHist(imgRoi, grayHist, true);
    vector<Mat>vImgs;
    Mat result;
    vImgs.push_back(imgRoi);
    vImgs.push_back(imgHist);
    vconcat(vImgs, result); //垂直方向拼接


    int NUM = 20;
    int left = 0;
    while (left < grayHist.cols / 2) {
        if (grayHist.ptr<uchar>(0)[left] == 0) left++;
        else break;
    }
    int num = NUM;
    while (left < grayHist.cols / 2) {
        if (grayHist.ptr<uchar>(0)[left] == 0)  {
            num--;
            if (num < 0) break;
        }
        else {
            num = NUM;
            left++;
        }
    }

    num = NUM;
    int right = grayHist.cols - 1;
    while (right > grayHist.cols / 2) {
        if (grayHist.ptr<uchar>(0)[right] == 0) right--;
        else break;
    }
    int right2 = right;
    while (right > grayHist.cols / 2) {
        if (grayHist.ptr<uchar>(0)[right] == 0) {
            num--;
            if (num < 0) break;
        }
        else {
            num = NUM;
            right--;
        }
    }
    if (right <= grayHist.cols / 2) right = right2;
    cout << "begin = " << 0 << ",  end  = " << grayHist.cols << endl;
    cout << "left  = " << left << ", right = " << right << endl;
    imgDst = imgBinary.clone();
    imgDst = imgDst(Rect(left, 0, right - left + 1, imgDst.rows));

    imwrite("Resources/cropEdgeProcess/" + name + "_binary.jpg", imgBinary);
    imwrite("Resources/cropEdgeProcess/" + name + "_hist.jpg", imgHist);
    imwrite("Resources/cropEdgeProcess/" + name + "_vconcat.jpg", result);
    imwrite("Resources/cropEdgeProcess/" + name + "_out.jpg", imgDst);
}