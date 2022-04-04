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
	cout << "灰度值: " << endl << grayhist << endl;

	//找出直方图统计的个数的最大值，用来作为直方图纵坐标的高
	double maxValue = 0;
	//找矩阵中最大最小值及对应索引的函数
	minMaxLoc(grayhist, 0, &maxValue);
	//最大值取整
	int rows = cvRound(maxValue);
	//定义直方图图像，直方图纵坐标的高作为行数，列数为256(灰度值的个数)
	//因为是直方图的图像，所以以黑白两色为区分，白色为直方图的图像
	//Mat histImage = Mat::ones(rows, src_img.cols, CV_8UC1);
	Mat histImage(Size(src_img.cols, rows), CV_8UC1, Scalar(255));

	//直方图图像表示
	for (int i = 0; i < src_img.cols; i++)
	{
		//取每个bin的数目
		int temp = (int)(grayhist.ptr<uchar>(0)[i]);
		//如果bin数目为0，则说明图像上没有该灰度值，则整列为黑色
		//如果图像上有该灰度值，则将该列对应个数的像素设为白色
		if (temp)
		{
			//由于图像坐标是以左上角为原点，所以要进行变换，使直方图图像以左下角为坐标原点
			histImage.col(i).rowRange(Range(rows - temp, rows)) = 0;
		}
	}
	//由于直方图图像列高可能很高，因此进行图像对列要进行对应的缩减，使直方图图像更直观
	//resize(histImage, histImage, Size(256, 256));

	return histImage;
}

Mat showHist(Mat& src_img)
{

	//2、计算图像中的最大值最小值；用于计算histSize和range参数;
	//double minVal, maxVal;
	//minMaxLoc(src_img, &minVal, &maxVal);
	//float nMin = static_cast<float>(minVal);
	//float nMax = static_cast<float>(maxVal);
	//int nsize = nMax - nMin + 1;

	//3、准备各类参数;
	vector<Mat> vImg{ src_img };
	//vImg.push_back(src_img);
	vector<int>    vChannels = { 0 };
	vector<int> vHistSizes = { 256 };
	vector<float>  vRanges = { 0 , 256 };
	Mat grayhist;
	calcHist(vImg, vChannels, noArray(), grayhist, vHistSizes, vRanges);

	//cout << "灰度值: " << endl << grayhist << endl;
	//找出直方图统计的个数的最大值，用来作为直方图纵坐标的高

	double maxValue = 0;
	//找矩阵中最大最小值及对应索引的函数
	minMaxLoc(grayhist, 0, &maxValue);
	//最大值取整
	int rows = cvRound(maxValue);
	//定义直方图图像，直方图纵坐标的高作为行数，列数为256(灰度值的个数)
	//因为是直方图的图像，所以以黑白两色为区分，白色为直方图的图像
	Mat histImage = Mat::zeros(rows, 256, CV_8UC1);

	//直方图图像表示
	for (int i = 0; i < 256; i++)
	{
		//取每个bin的数目
		int temp = (int)(grayhist.at<float>(i, 0));
		//如果bin数目为0，则说明图像上没有该灰度值，则整列为黑色
		//如果图像上有该灰度值，则将该列对应个数的像素设为白色
		if (temp)
		{
			//由于图像坐标是以左上角为原点，所以要进行变换，使直方图图像以左下角为坐标原点
			histImage.col(i).rowRange(Range(rows - temp, rows)) = 255;
		}
	}
	//由于直方图图像列高可能很高，因此进行图像对列要进行对应的缩减，使直方图图像更直观
	Mat resizeImage;
	resize(histImage, resizeImage, Size(256, 256));


	//int numbins = 256;
	////5、创建一个512*300像素大小的图像，用于绘制显示
	//int width = 512;
	//int height = 300;
	//Mat histImage(height, width, CV_8UC1, 255);

	////6、将最小值与最大值标准化直方图矩阵
	//normalize(grayhist, grayhist, 0, height, NORM_MINMAX);

	////7、使用彩色通道绘制直方图
	//int binStep = cvRound((float)width / (float)numbins);  //通过将宽度除以区间数来计算binStep变量

	//for (int i = 1; i < numbins; i++)
	//{
	//	line(histImage,
	//		Point(binStep * (i - 1), height - cvRound(grayhist.at<float>(i - 1))),
	//		Point(binStep * (i), height - cvRound(grayhist.at<float>(i))),
	//		0
	//	);
	//}
	//imshow("灰度直方图", histImage);
	//imshow("灰度直方图", resizeImage);
	return resizeImage;
}