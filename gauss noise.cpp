#include "head.h"

void addGaussNoise(Mat& img, Mat& imgNoise) {
    imgNoise = img.clone();

    //������ԭͼ��ͬ�ߴ硢�������ͺ�ͨ�����ľ���
    Mat equalLena_noise = Mat::zeros(img.rows, img.cols, img.type());
    imshow("imgԭͼ", img);

    RNG rng; //����һ��RNG��
    rng.fill(equalLena_noise, RNG::NORMAL, 15, 30); //���ɵ�ͨ���ĸ�˹�ֲ������
    imshow("��ͨ����˹����", equalLena_noise);

    imgNoise = imgNoise + equalLena_noise; //�ڻҶ�ͼ������Ӹ�˹����
    //��ʾ��Ӹ�˹�������ͼ��
    imshow("equalLena�������", imgNoise);

    
}

void removeGaussNoise(Mat& imgNoise, Mat& img) {

    //��3����ֵ�����ɶ�ֵͼ��
    Mat binaryImage;
    threshold(imgNoise, binaryImage, 0, 255, THRESH_BINARY);
    //��4����ȡ�ṹԪ��
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));

    //��5����ʴ����
    Mat erodedImage;
    erode(binaryImage, erodedImage, element);
    //��6����������
    Mat dilatedImage;
    dilate(erodedImage, dilatedImage, element);
    img = dilatedImage;

}

void gaussNoiseProcess() {
    printf("gaussNoiseProcess start\n");

    // ��ȡͼ������ �� ·��
    string name = "doc2";
    string path = "Resources/gaussNoiseProcess/" + name + ".jpg";

    // ��ȡͼ�񲢻ҶȻ�
    Mat imgGray, img, imgHist;
    img = imread(path);
    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    imshow("ԭ�Ҷ�ͼ", imgGray);
    imwrite("Resources/gaussNoiseProcess/" + name + "_gray.jpg", imgGray);

    // ԭͼ���ֵ��
    //Mat imgBinary;
    //threshold(imgGray, imgBinary, 0, 255, THRESH_BINARY | THRESH_OTSU);
    //imshow("ԭ��ֵͼ", imgBinary);
    //imwrite("Resources/gaussNoiseProcess/" + name + "_binary.jpg", imgBinary);

    // ���Ӹ�˹����
    Mat imgGauss;
    addGaussNoise(imgGray, imgGauss);
    imshow("��˹����ͼ", imgGauss);
    imwrite("Resources/gaussNoiseProcess/" + name + "_gauss.jpg", imgGauss);

    // ��˹����ͼ���ֵ��
    Mat imgGaussBinary;
    threshold(imgGauss, imgGaussBinary, 0, 255, THRESH_BINARY | THRESH_OTSU);
    imshow("��˹������ֵͼ", imgGaussBinary);
    imwrite("Resources/gaussNoiseProcess/" + name + "_gaussBinary.jpg", imgGaussBinary);

    // ȥ����˹����
    Mat imgRemoveGauss;
    removeGaussNoise(imgGaussBinary, imgRemoveGauss);
    imshow("imgRemoveGauss", imgRemoveGauss);
    imwrite("Resources/gaussNoiseProcess/" + name + "_out.jpg", imgRemoveGauss);


    waitKey(0);
}