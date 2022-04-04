#include "head.h"

Mat showHist(Mat& src_img, Mat& grayhist2, bool is_horizontal)
{

	Mat grayhist = Mat::zeros(1, src_img.cols, CV_8UC1);
	for (int x = 0; x < src_img.cols; x++) {
		for (int y = 0; y < src_img.rows; y++) {
			grayhist.ptr<uchar>(0)[x] += !src_img.ptr<uchar>(y)[x];
		}
	}
	grayhist2 = grayhist.clone();
	cout << "�Ҷ�ֵ: " << endl << grayhist << endl;

	//�ҳ�ֱ��ͼͳ�Ƶĸ��������ֵ��������Ϊֱ��ͼ������ĸ�
	double maxValue = 0;
	//�Ҿ����������Сֵ����Ӧ�����ĺ���
	minMaxLoc(grayhist, 0, &maxValue);
	//���ֵȡ��
	int rows = cvRound(maxValue);
	//����ֱ��ͼͼ��ֱ��ͼ������ĸ���Ϊ����������Ϊ256(�Ҷ�ֵ�ĸ���)
	//��Ϊ��ֱ��ͼ��ͼ�������Ժڰ���ɫΪ���֣���ɫΪֱ��ͼ��ͼ��
	//Mat histImage = Mat::ones(rows, src_img.cols, CV_8UC1);
	Mat histImage(Size(src_img.cols, rows), CV_8UC1, Scalar(255));

	//ֱ��ͼͼ���ʾ
	for (int i = 0; i < src_img.cols; i++)
	{
		//ȡÿ��bin����Ŀ
		int temp = (int)(grayhist.ptr<uchar>(0)[i]);
		//���bin��ĿΪ0����˵��ͼ����û�иûҶ�ֵ��������Ϊ��ɫ
		//���ͼ�����иûҶ�ֵ���򽫸��ж�Ӧ������������Ϊ��ɫ
		if (temp)
		{
			//����ͼ�������������Ͻ�Ϊԭ�㣬����Ҫ���б任��ʹֱ��ͼͼ�������½�Ϊ����ԭ��
			histImage.col(i).rowRange(Range(rows - temp, rows)) = 0;
		}
	}
	//����ֱ��ͼͼ���и߿��ܸܺߣ���˽���ͼ�����Ҫ���ж�Ӧ��������ʹֱ��ͼͼ���ֱ��
	//resize(histImage, histImage, Size(256, 256));

	return histImage;
}

Mat showHist(Mat& src_img)
{

	//2������ͼ���е����ֵ��Сֵ�����ڼ���histSize��range����;
	//double minVal, maxVal;
	//minMaxLoc(src_img, &minVal, &maxVal);
	//float nMin = static_cast<float>(minVal);
	//float nMax = static_cast<float>(maxVal);
	//int nsize = nMax - nMin + 1;

	//3��׼���������;
	vector<Mat> vImg{ src_img };
	//vImg.push_back(src_img);
	vector<int>    vChannels = { 0 };
	vector<int> vHistSizes = { 256 };
	vector<float>  vRanges = { 0 , 256 };
	Mat grayhist;
	calcHist(vImg, vChannels, noArray(), grayhist, vHistSizes, vRanges);

	//cout << "�Ҷ�ֵ: " << endl << grayhist << endl;
	//�ҳ�ֱ��ͼͳ�Ƶĸ��������ֵ��������Ϊֱ��ͼ������ĸ�

	double maxValue = 0;
	//�Ҿ����������Сֵ����Ӧ�����ĺ���
	minMaxLoc(grayhist, 0, &maxValue);
	//���ֵȡ��
	int rows = cvRound(maxValue);
	//����ֱ��ͼͼ��ֱ��ͼ������ĸ���Ϊ����������Ϊ256(�Ҷ�ֵ�ĸ���)
	//��Ϊ��ֱ��ͼ��ͼ�������Ժڰ���ɫΪ���֣���ɫΪֱ��ͼ��ͼ��
	Mat histImage = Mat::zeros(rows, 256, CV_8UC1);

	//ֱ��ͼͼ���ʾ
	for (int i = 0; i < 256; i++)
	{
		//ȡÿ��bin����Ŀ
		int temp = (int)(grayhist.at<float>(i, 0));
		//���bin��ĿΪ0����˵��ͼ����û�иûҶ�ֵ��������Ϊ��ɫ
		//���ͼ�����иûҶ�ֵ���򽫸��ж�Ӧ������������Ϊ��ɫ
		if (temp)
		{
			//����ͼ�������������Ͻ�Ϊԭ�㣬����Ҫ���б任��ʹֱ��ͼͼ�������½�Ϊ����ԭ��
			histImage.col(i).rowRange(Range(rows - temp, rows)) = 255;
		}
	}
	//����ֱ��ͼͼ���и߿��ܸܺߣ���˽���ͼ�����Ҫ���ж�Ӧ��������ʹֱ��ͼͼ���ֱ��
	Mat resizeImage;
	resize(histImage, resizeImage, Size(256, 256));


	//int numbins = 256;
	////5������һ��512*300���ش�С��ͼ�����ڻ�����ʾ
	//int width = 512;
	//int height = 300;
	//Mat histImage(height, width, CV_8UC1, 255);

	////6������Сֵ�����ֵ��׼��ֱ��ͼ����
	//normalize(grayhist, grayhist, 0, height, NORM_MINMAX);

	////7��ʹ�ò�ɫͨ������ֱ��ͼ
	//int binStep = cvRound((float)width / (float)numbins);  //ͨ������ȳ���������������binStep����

	//for (int i = 1; i < numbins; i++)
	//{
	//	line(histImage,
	//		Point(binStep * (i - 1), height - cvRound(grayhist.at<float>(i - 1))),
	//		Point(binStep * (i), height - cvRound(grayhist.at<float>(i))),
	//		0
	//	);
	//}
	//imshow("�Ҷ�ֱ��ͼ", histImage);
	//imshow("�Ҷ�ֱ��ͼ", resizeImage);
	return resizeImage;
}