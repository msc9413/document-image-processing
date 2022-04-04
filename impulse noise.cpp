#include "head.h"


void addImpulseNoise(Mat& image, Mat& imgNoise, int n)
{
    imgNoise = image.clone();
    for (int k = 0; k < n; k++)
    {
        //���ȡ��
        int i, j;
        i = rand() % imgNoise.cols;
        j = rand() % imgNoise.rows;

        imgNoise.at<uchar>(j, i) = 0;
    }

	for (int k = 0; k < n; k++)
	{
		//���ȡ��
		int i, j;
		i = rand() % imgNoise.cols;
		j = rand() % imgNoise.rows;

		imgNoise.at<uchar>(j, i) = 255;
	}

}

// ��ֵ�˲�
///��ֵ�˲��ǻ�������ͳ�����۵�һ���ܹ���Ч�����������ķ������źŴ�����,
/// �����ԭ���ǰ�����ͼ�������������һ���ֵ�øõ��һ�������ڸ���ֵ����ֵ����,����Χ������ֵ�ӽ���ʵֵ,�Ӷ�����������������,
/// ���ڰߵ�����(Speckle noise)�ͽ�������(salt-pepper noise)��˵��������,
/// ��Ϊ������������������Щ�����ֵ���ܴ��ֵ;
void medianBlurRemoveImpulseNoise(Mat& imgNoise, Mat& image, int ksize) {
	medianBlur(imgNoise, image, ksize);
}

/// <summary>
/// ��һ�����������е�ĻҶ�ֵ��ƽ��ֵ��Ϊ�����ĻҶ�ֵ
/// ��ú����������ĻҶ�ֵ����Ȩ�����������ûҶ�ֵ�������Ĳ����ֽо�����������˲����������˲�����
/// </summary>
/// <param name="image">����ͼ��</param>
/// <param name="imgNoise">ȥ�������ͼ��</param>
void blurRemoveImpulseNoise(Mat& imgNoise, Mat& image) {
	blur(imgNoise, image, Size(3, 3));
}


//bool getValue(Mat& image, int i, int j) {
//	if (i < 0 || i >= image.rows || j < 0 || j >= image.cols) {
//		return -1;
//	}
//	else return ''
//}

#define x1 (bool)(imgNoise.ptr<uchar>(i-1)[j-1])
#define x2 (bool)(imgNoise.ptr<uchar>(i-1)[j])
#define x3 (bool)(imgNoise.ptr<uchar>(i-1)[j+1])
#define x4 (bool)(imgNoise.ptr<uchar>(i)[j-1])
#define x5 (bool)(imgNoise.ptr<uchar>(i)[j])
#define x6 (bool)(imgNoise.ptr<uchar>(i)[j+1])
#define x7 (bool)(imgNoise.ptr<uchar>(i+1)[j-1])
#define x8 (bool)(imgNoise.ptr<uchar>(i+1)[j])
#define x9 (bool)(imgNoise.ptr<uchar>(i+1)[j+1])
void removeImpulseNoise(Mat& imgNoise, Mat& image) {
	image = imgNoise.clone();
	cout << "removeImpulseNoise start" << endl;
	//Mat kern = (Mat_<char>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
	//filter2D(image, imgNoise, testImage.depth(), kern);
	for (int i = 1; i < image.rows - 1; i++) {
		for (int j = 1; j < image.cols - 1; j++) {
			// ��ǰΪ��
			if (x5) {
				if (!x6 && !x8 && (!x2 || !x4)) {
					image.ptr<uchar>(i)[j] = 0;
				}
				else if (!x2 && !x4 && (!x6 || !x8)) {
					image.ptr<uchar>(i)[j] = 0;
				}
			} else {  // ��ǰΪ��
				if (x1 && x2 && x4 && ((x7 && x8) || (x3 && x6))) {
					image.ptr<uchar>(i)[j] = 255;
				}
				if (x6 && x8 && x9 && ((x2 && x3) || (x4 && x7))) {
					image.ptr<uchar>(i)[j] = 255;
				}

			}
		}
	}
}

void removeImpulseNoise2(Mat& image, Mat& imgNoise, float u)
{


	// �ҵ���������
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(image, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
	cout << "contours num : " << contours.size() << endl;

	vector<Rect> boundRect(contours.size());

	// �ҵ������������
	// ����Ĭ�� ������� Ϊ �ĵ�����
	//const double u = 0.01;
	int maxWidth = 0, maxHeight = 0;
	for (int i = 0; i < contours.size(); i++) {
		boundRect[i] = boundingRect(contours[i]);//����߽����
		//����ߺͿ�
		if (boundRect[i].width > image.rows * 0.5) continue;
		if (boundRect[i].height > image.cols * 0.5) continue;
		if (abs(boundRect[i].height - boundRect[i].width) > 5)continue;
		maxWidth = max(maxWidth, boundRect[i].width);
		maxHeight = max(maxHeight, boundRect[i].height);

		//rectangle(image, boundRect[i].tl(), boundRect[i].br(), 0, 0.5);
	}
	cout << "maxWidth : " << maxWidth << endl;
	cout << "maxHeight : " << maxHeight << endl;

	for (int i = 0; i < contours.size(); i++) {
		//int area = contourArea(boundRect[i]);
		
		if (boundRect[i].width * boundRect[i].width < u * maxWidth * maxHeight) {
			drawContours(image, contours, i, 255, FILLED);
		}
		//rectangle(image, boundRect[i].tl(), boundRect[i].br(), 0, 0.5);
	}


	//drawPoints(conPoly, Scalar(0, 0, 255));
	cout << "begin draw" << endl;
	//drawContours(image, contours, -1, 0, FILLED);
	//rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
	imgNoise = image;

}

void impulseNoiseProcess() {
	printf("impulseNoiseProcess start\n");

	string name = "doc1";
	string path = "Resources/impulseNoiseProcess/" + name + ".jpg";

	Mat imgGray, img, imgHist;
	img = imread(path);
	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	imwrite("Resources/impulseNoiseProcess/" + name + "_gray.jpg", imgGray);


	//��3����ֵ�����ɶ�ֵͼ��
	Mat binaryImage;
	threshold(imgGray, binaryImage, 0, 255, THRESH_BINARY | THRESH_OTSU);
	imshow("ԭ��ֵͼ", binaryImage);
	imwrite("Resources/impulseNoiseProcess/" + name + "_binary.jpg", binaryImage);


	Mat imgMedianBlur, imgBlur, imgMask, imgMask2, imgImpulse;
	addImpulseNoise(binaryImage, imgImpulse, binaryImage.cols * 5);
	imshow("��������ͼ", imgImpulse);
	imwrite("Resources/impulseNoiseProcess/" + name + "_impulseNoise.jpg", imgImpulse);

	blurRemoveImpulseNoise(imgImpulse, imgBlur);
	threshold(imgBlur, imgBlur, 0, 255, THRESH_BINARY | THRESH_OTSU);
	imshow("����ƽ��", imgBlur);
	imwrite("Resources/impulseNoiseProcess/" + name + "_blur.jpg", imgBlur);

	medianBlurRemoveImpulseNoise(imgImpulse, imgMedianBlur, 3);
	imshow("��ֵ�˲�", imgMedianBlur);
	imwrite("Resources/impulseNoiseProcess/" + name + "_medianBlur.jpg", imgMedianBlur);

	removeImpulseNoise(imgImpulse, imgMask);
	imshow("��Ĥ��", imgMask);
	imwrite("Resources/impulseNoiseProcess/" + name + "_mask.jpg", imgMask);

	removeImpulseNoise(imgMask, imgMask2);
	imshow("��Ĥ��*2", imgMask2);
	imwrite("Resources/impulseNoiseProcess/" + name + "_mask2.jpg", imgMask2);

	waitKey(0);
}