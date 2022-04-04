#include "head.h"



#define ERROR 1234

//度数转换
double DegreeTrans(double theta)
{
	double res = theta / CV_PI * 180;
	return res;
}

//逆时针旋转图像degree角度（原尺寸）    
void rotateImage(Mat src, Mat& img_rotate, double degree)
{
	//旋转中心为图像中心    
	Point2f center;
	center.x = float(src.cols / 2.0);
	center.y = float(src.rows / 2.0);
	int length = 0;
	length = sqrt(src.cols * src.cols + src.rows * src.rows);
	//计算二维旋转的仿射变换矩阵  
	Mat M = getRotationMatrix2D(center, degree, 1);
	warpAffine(src, img_rotate, M, Size(length, length), 1, 0, Scalar(255, 255, 255));//仿射变换，背景色填充为白色  
}

//通过霍夫变换计算角度
double CalcDegree(const Mat& srcImage, Mat& dst)
{
	Mat midImage = srcImage, dstImage = srcImage;

	//Canny(srcImage, midImage, 50, 200, 3);
	//cvtColor(midImage, dstImage, COLOR_BGR2GRAY);

	//通过霍夫变换检测直线
	vector<Vec2f> lines;
	HoughLines(midImage, lines, 1, CV_PI / 180, 300, 0, 0);//第5个参数就是阈值，阈值越大，检测精度越高
	//cout << lines.size() << endl;

	//由于图像不同，阈值不好设定，因为阈值设定过高导致无法检测直线，阈值过低直线太多，速度很慢
	//所以根据阈值由大到小设置了三个阈值，如果经过大量试验后，可以固定一个适合的阈值。

	if (!lines.size())
	{
		HoughLines(midImage, lines, 1, CV_PI / 180, 200, 0, 0);
	}
	//cout << lines.size() << endl;

	if (!lines.size())
	{
		HoughLines(midImage, lines, 1, CV_PI / 180, 150, 0, 0);
	}
	//cout << lines.size() << endl;
	if (!lines.size())
	{
		cout << "没有检测到直线！" << endl;
		return ERROR;
	}

	float sum = 0;
	//依次画出每条线段
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		Point pt1, pt2;
		//cout << theta << endl;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		//只选角度最小的作为旋转角度
		sum += theta;

		line(dstImage, pt1, pt2, 0, 1); //Scalar函数用于调节线段颜色

		imshow("直线探测效果图", dstImage);
	}
	float average = sum / lines.size(); //对所有角度求平均，这样做旋转效果会更好

	cout << "average theta:" << average << endl;

	double angle = DegreeTrans(average) - 90;

	rotateImage(dstImage, dst, angle);
	//imshow("直线探测效果图2", dstImage);
	return angle;
}


void ImageRecify(Mat& src, Mat& dst)
{
	double degree;
	imshow("原始图", src);

	//倾斜角度矫正
	degree = CalcDegree(src, dst);
	if (degree == ERROR)
	{
		cout << "矫正失败！" << endl;
		return;
	}
	rotateImage(src, dst, degree);
	cout << "angle:" << degree << endl;
	imshow("旋转调整后", dst);

	Mat resulyImage = dst(Rect(0, 0, dst.cols, 500)); //根据先验知识，估计好文本的长宽，再裁剪下来
	imshow("裁剪之后", resulyImage);
	//imwrite("recified.jpg", resulyImage);
}

/// 矩形中
/// x + y 最大为右下, 最小为左上
/// x - y 最大为右上，最小为左下
vector<Point2f> getBelowPoints(Point2f rect[4]) {
	vector<float> sumPoints, subPoints;
	for (int i = 0; i < 4; i++) {
		Point2f point = rect[i];
		sumPoints.push_back(point.x + point.y);
		subPoints.push_back(point.x - point.y);
	}

	vector<Point2f> retPoints;
	//retPoints.push_back(rect[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);	// 0
	//retPoints.push_back(rect[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);	// 1
	retPoints.push_back(rect[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);	// 2	左下
	retPoints.push_back(rect[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);	// 3	右下

	return retPoints;
}


double getDistance(Point2f point1, Point2f point2)
{
	return sqrtf(powf((point1.x - point2.x), 2) + powf((point1.y - point2.y), 2));;
}

double getAngle(Point2f pt1, Point2f pt2) {
	double radian = atan2((pt2.y - pt1.y), (pt2.x - pt1.x));//弧度   该函数返回值范围是[-pi,pi]
	return radian / CV_PI * 180;//角度
}


void tiltCorrection() {
    printf("tiltCorrection start\n");

    // 获取图像名字 和 路径
    string name = "doc15";
    string path = "Resources/tiltCorrection/" + name + ".jpg";

    // 获取图像并灰度化
    Mat img, imgGray, imgBlur, imgCanny, imgDil, imgErode, imgDst;
    img = imread(path);
	imshow("原图", img);

	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	imshow("原灰度图", imgGray);

	Mat binaryImage;
	double thres = threshold(imgGray, binaryImage, 0, 255, THRESH_BINARY | THRESH_OTSU);

	// 取图像中心部分计算
	const int roiNum = 6;
	Mat imgRoi = binaryImage(Rect(binaryImage.cols / roiNum, binaryImage.rows / roiNum, binaryImage.cols / roiNum * (roiNum - 2), binaryImage.rows / roiNum * (roiNum - 2)));
	Mat imgRect = imgRoi.clone(), imgContours = imgRoi.clone();
	//copyMakeBorder(imgRoi, imgRoi, binaryImage.rows / roiNum, binaryImage.rows / roiNum, binaryImage.cols / roiNum, binaryImage.cols / roiNum, BORDER_CONSTANT, Scalar(255, 255, 255));

	GaussianBlur(imgRoi, imgBlur, Size(3, 3), 3, 0);
	Canny(imgBlur, imgCanny, 25, 75);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(13, 3));
	dilate(imgCanny, imgDil, kernel);
	imshow("imgCanny", imgCanny);
	imshow("imgDil", imgDil);


	// 扩张
	//Mat kernel = getStructuringElement(MORPH_RECT, Size(17, 13));
	//erode(imgRoi, imgErode, kernel);
	//imshow("扩张图", imgErode);

	// 找到所有轮廓
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(imgDil, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
	drawContours(imgContours, contours, -1, Scalar(0, 0, 0), 1);
	cout << "contours num : " << contours.size() << endl;

	Point2f rect[4];
	int cntLine = 0;
	double sumAngle = 0;
	for (int i = 0; i < contours.size(); i++) {
		//计算最小边界矩形  保存在ret中
		RotatedRect box = minAreaRect(contours[i]);
		box.points(rect);

		line(imgRect, rect[0], rect[1], Scalar(0, 0, 0), 1, 8);
		line(imgRect, rect[1], rect[2], Scalar(0, 0, 0), 1, 8);
		line(imgRect, rect[2], rect[3], Scalar(0, 0, 0), 1, 8);
		line(imgRect, rect[3], rect[0], Scalar(0, 0, 0), 1, 8);

		// 获取矩形 左下 和 右下 两点
		vector<Point2f> belowPoints = getBelowPoints(rect);

		// 去除 过长过短 的边界矩形
		double distace = getDistance(belowPoints[0], belowPoints[1]);
		if (distace < imgRoi.cols / roiNum * (roiNum - 2) / 6 || distace > imgRoi.cols - 5) continue;
		cout << "angle : " << box.angle << endl;

		cntLine++;
		if (-45 < box.angle && box.angle < 0)sumAngle += box.angle;
		else if (-90 < box.angle && box.angle < -45)sumAngle += box.angle + 90;
		else cntLine--;

		line(imgRoi, belowPoints[0], belowPoints[1], Scalar(0, 0, 0), 1, 8);
	}

	// 计算平均倾斜角度
	double averageAngle = sumAngle / cntLine; 
	cout << "average angle : " << averageAngle << endl;

	// 增加白边，防止旋转导致的数据丢失
	if (cntLine) {
		int win_2 = sqrtf(img.cols * img.cols + img.rows * img.rows) / 2 * abs(sin(averageAngle / 180 * CV_PI));
		copyMakeBorder(img, img, win_2, win_2, win_2, win_2, BORDER_CONSTANT, Scalar(255, 255, 255));
	}
	// 旋转
	rotateImage(img, imgDst, averageAngle);
	// 去除多余白边
	imgDst = imgDst(Rect(0,0, img.cols, img.rows));


	imwrite("Resources/tiltCorrection/" + name + "_img.jpg", img);
	imwrite("Resources/tiltCorrection/" + name + "_rect.jpg", imgRect);
	imwrite("Resources/tiltCorrection/" + name + "_contours.jpg", imgContours);
	imwrite("Resources/tiltCorrection/" + name + "_gray.jpg", imgGray);
	imwrite("Resources/tiltCorrection/" + name + "_binary.jpg", binaryImage);
	imwrite("Resources/tiltCorrection/" + name + "_roi.jpg", imgRoi);
	imwrite("Resources/tiltCorrection/" + name + "_erode.jpg", imgDil);
	imwrite("Resources/tiltCorrection/" + name + "_out.jpg", imgDst);

	waitKey(0);

}