#include "head.h"


//src--原图像
//win_2 -- 矩形邻域窗口的长、宽均为(2*win_2+1)
void get_background(Mat src, Mat& imgBackground, int win_2 = 5)
{
    printf("get background start\n");

    int winsize = 2 * win_2 + 1;
    Mat src_tmp;
    //为了使原图所有点都能取到完整的矩形邻域，首先对原图进行边缘填充
    copyMakeBorder(src, src_tmp, win_2, win_2, win_2, win_2, BORDER_REPLICATE);
    //中值滤波去噪，减少背景杂志
    medianBlur(src_tmp, src_tmp, 9);


    imgBackground = Mat(src.size(), CV_8UC1);
    for (int i = win_2; i < src_tmp.rows - win_2; i++)
    {
        uchar* pd = imgBackground.ptr<uchar>(i - win_2);
        for (int j = win_2; j < src_tmp.cols - win_2; j++)
        {
            Mat tmp;
            //截取每一个点周围的矩形邻域
            src_tmp(Rect(j - win_2, i - win_2, winsize, winsize)).copyTo(tmp);
            //将二维矩阵转换成一维数据
            tmp.reshape(1, 1).copyTo(tmp);
            //从大到小排序
            cv::sort(tmp, tmp, SORT_EVERY_ROW + SORT_ASCENDING);

            uchar* p = tmp.ptr<uchar>(0);
            //取排序之后的前5个像素值计算均值作为背景值
            pd[j - win_2] = (uchar)((p[tmp.cols - 1] + p[tmp.cols - 2] + p[tmp.cols - 3] + p[tmp.cols - 4] + p[tmp.cols - 5]) * 0.2);
        }
        printf("%d : %d\n", src_tmp.rows - win_2, i);

    }

    return ;
}

void deleteshadow(Mat src, Mat& imgBackground) {

    //根据图像大小调节窗口大小
    // int n = src.cols / 80;
    int n = 25;

    //先对原灰度图做最大滤波(不就是膨胀么)，得到maxFilterMat_A
    Mat element = getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(n, n));
    Mat maxFilterMat_A;
    cv::morphologyEx(src, maxFilterMat_A, MORPH_DILATE, element, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, cv::morphologyDefaultBorderValue());

    //再对maxFilterMat_A做最小化滤波（不就是腐蚀么）
    Mat minFilterMat_B;
    cv::morphologyEx(maxFilterMat_A, minFilterMat_B, MORPH_ERODE, element, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, cv::morphologyDefaultBorderValue());

    imgBackground = minFilterMat_B;
}

float getK(uchar backLigt) {
    if (backLigt < 20) return 2.5;
    else if (20 <= backLigt && backLigt <= 100) return 1.0 + 1.5 * (100 - backLigt) / 80;
    else if (100 < backLigt && backLigt < 220) return 1.0;
    else return 1.0 + (backLigt - 220) / 35;
}
void lightCompensation(Mat img, Mat& imgBackground, Mat& dst) {
    printf("lightCompensation start\n");
    dst = Mat(img.size(), CV_8UC1);
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            uchar backLigt = imgBackground.ptr<uchar>(i)[j];
            uchar foreLigt = img.ptr<uchar>(i)[j];
            if (backLigt > foreLigt) {
                int k = getK(backLigt);
                dst.ptr<uchar>(i)[j] = 255 - k * (backLigt - foreLigt);
                if (dst.ptr<uchar>(i)[j] < 255 * 0.75) {
                    dst.ptr<uchar>(i)[j] = 255 * 0.75;
                }
            } else {
                dst.ptr<uchar>(i)[j] = 255;
            }
        }
    }
}

void lightCompensationProcess() {
    printf("lightCompensationProcess start\n");

    // 获取图像名字 和 路径
    string name = "doc17";
    string path = "Resources/lightCompensationProcess/" + name + ".jpg";

    // 获取图像并灰度化
    Mat imgGray, img;
    img = imread(path);

    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    imshow("原灰度图", imgGray);
    imwrite("Resources/lightCompensationProcess/" + name + "_gray.jpg", imgGray);

    Mat imgBackground;
    deleteshadow(imgGray, imgBackground);
    imshow("灰度背景图", imgGray);
    imwrite("Resources/lightCompensationProcess/" + name + "_background.jpg", imgBackground);

    Mat dst;
    lightCompensation(imgGray, imgBackground, dst);
    imwrite("Resources/lightCompensationProcess/" + name + "_out.jpg", dst);
    
    Mat imgHist = showHist(dst);
    imwrite("Resources/lightCompensationProcess/" + name + "_hist.jpg", imgHist);

    Mat imgBinary;
    threshold(dst, imgBinary, 0, 255, THRESH_BINARY | THRESH_OTSU);
    //threshold(dst, imgBinary, 200, 255, THRESH_BINARY);
    //double thres = get_threld(imgGray, 0.01, 0.5);
    //cout << "thres = " << thres << endl;
    //threshold(dst, imgBinary, thres, 255, THRESH_BINARY);
    imwrite("Resources/lightCompensationProcess/" + name + "_binary.jpg", imgBinary);

    //Mat removeNoise;
    //removeImpulseNoise2(imgBinary, removeNoise, 0.008);
    //imwrite("Resources/lightCompensationProcess/" + name + "_removeNoise.jpg", removeNoise);


    //Mat imgEnhance;
    ////img_ehance(dst, imgEnhance, 1.0, -0.5);
    //normalize(dst, imgEnhance, 0, 255, NORM_MINMAX);
    //imshow("增强图", showHist(imgEnhance));
    //imwrite("Resources/lightCompensationProcess/" + name + "_enhance.jpg", imgEnhance);

    waitKey(0);
}