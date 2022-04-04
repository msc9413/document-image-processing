#include "head.h"

void addGaussNoise(Mat& img, Mat& imgNoise) {
    imgNoise = img.clone();

    //生成与原图像同尺寸、数据类型和通道数的矩阵
    Mat equalLena_noise = Mat::zeros(img.rows, img.cols, img.type());
    imshow("img原图", img);

    RNG rng; //创建一个RNG类
    rng.fill(equalLena_noise, RNG::NORMAL, 15, 30); //生成单通道的高斯分布随机数
    imshow("单通道高斯噪声", equalLena_noise);

    imgNoise = imgNoise + equalLena_noise; //在灰度图像中添加高斯噪声
    //显示添加高斯噪声后的图像
    imshow("equalLena添加噪声", imgNoise);

    
}

void removeGaussNoise(Mat& imgNoise, Mat& img) {

    //【3】阈值化生成二值图像
    Mat binaryImage;
    threshold(imgNoise, binaryImage, 0, 255, THRESH_BINARY);
    //【4】获取结构元素
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));

    //【5】腐蚀运算
    Mat erodedImage;
    erode(binaryImage, erodedImage, element);
    //【6】膨胀运算
    Mat dilatedImage;
    dilate(erodedImage, dilatedImage, element);
    img = dilatedImage;

}

void gaussNoiseProcess() {
    printf("gaussNoiseProcess start\n");

    // 获取图像名字 和 路径
    string name = "doc2";
    string path = "Resources/gaussNoiseProcess/" + name + ".jpg";

    // 获取图像并灰度化
    Mat imgGray, img, imgHist;
    img = imread(path);
    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    imshow("原灰度图", imgGray);
    imwrite("Resources/gaussNoiseProcess/" + name + "_gray.jpg", imgGray);

    // 原图像二值化
    //Mat imgBinary;
    //threshold(imgGray, imgBinary, 0, 255, THRESH_BINARY | THRESH_OTSU);
    //imshow("原二值图", imgBinary);
    //imwrite("Resources/gaussNoiseProcess/" + name + "_binary.jpg", imgBinary);

    // 增加高斯噪音
    Mat imgGauss;
    addGaussNoise(imgGray, imgGauss);
    imshow("高斯噪音图", imgGauss);
    imwrite("Resources/gaussNoiseProcess/" + name + "_gauss.jpg", imgGauss);

    // 高斯噪音图像二值化
    Mat imgGaussBinary;
    threshold(imgGauss, imgGaussBinary, 0, 255, THRESH_BINARY | THRESH_OTSU);
    imshow("高斯噪音二值图", imgGaussBinary);
    imwrite("Resources/gaussNoiseProcess/" + name + "_gaussBinary.jpg", imgGaussBinary);

    // 去除高斯噪音
    Mat imgRemoveGauss;
    removeGaussNoise(imgGaussBinary, imgRemoveGauss);
    imshow("imgRemoveGauss", imgRemoveGauss);
    imwrite("Resources/gaussNoiseProcess/" + name + "_out.jpg", imgRemoveGauss);


    waitKey(0);
}