#include "head.h"



#define ERROR 1234

//����ת��
double DegreeTrans(double theta)
{
	double res = theta / CV_PI * 180;
	return res;
}

//��ʱ����תͼ��degree�Ƕȣ�ԭ�ߴ磩    
void rotateImage(Mat src, Mat& img_rotate, double degree)
{
	//��ת����Ϊͼ������    
	Point2f center;
	center.x = float(src.cols / 2.0);
	center.y = float(src.rows / 2.0);
	int length = 0;
	length = sqrt(src.cols * src.cols + src.rows * src.rows);
	//�����ά��ת�ķ���任����  
	Mat M = getRotationMatrix2D(center, degree, 1);
	warpAffine(src, img_rotate, M, Size(length, length), 1, 0, Scalar(255, 255, 255));//����任������ɫ���Ϊ��ɫ  
}

//ͨ������任����Ƕ�
double CalcDegree(const Mat& srcImage, Mat& dst)
{
	Mat midImage = srcImage, dstImage = srcImage;

	//Canny(srcImage, midImage, 50, 200, 3);
	//cvtColor(midImage, dstImage, COLOR_BGR2GRAY);

	//ͨ������任���ֱ��
	vector<Vec2f> lines;
	HoughLines(midImage, lines, 1, CV_PI / 180, 300, 0, 0);//��5������������ֵ����ֵԽ�󣬼�⾫��Խ��
	//cout << lines.size() << endl;

	//����ͼ��ͬ����ֵ�����趨����Ϊ��ֵ�趨���ߵ����޷����ֱ�ߣ���ֵ����ֱ��̫�࣬�ٶȺ���
	//���Ը�����ֵ�ɴ�С������������ֵ�����������������󣬿��Թ̶�һ���ʺϵ���ֵ��

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
		cout << "û�м�⵽ֱ�ߣ�" << endl;
		return ERROR;
	}

	float sum = 0;
	//���λ���ÿ���߶�
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
		//ֻѡ�Ƕ���С����Ϊ��ת�Ƕ�
		sum += theta;

		line(dstImage, pt1, pt2, 0, 1); //Scalar�������ڵ����߶���ɫ

		imshow("ֱ��̽��Ч��ͼ", dstImage);
	}
	float average = sum / lines.size(); //�����нǶ���ƽ������������תЧ�������

	cout << "average theta:" << average << endl;

	double angle = DegreeTrans(average) - 90;

	rotateImage(dstImage, dst, angle);
	//imshow("ֱ��̽��Ч��ͼ2", dstImage);
	return angle;
}


void ImageRecify(Mat& src, Mat& dst)
{
	double degree;
	imshow("ԭʼͼ", src);

	//��б�ǶȽ���
	degree = CalcDegree(src, dst);
	if (degree == ERROR)
	{
		cout << "����ʧ�ܣ�" << endl;
		return;
	}
	rotateImage(src, dst, degree);
	cout << "angle:" << degree << endl;
	imshow("��ת������", dst);

	Mat resulyImage = dst(Rect(0, 0, dst.cols, 500)); //��������֪ʶ�����ƺ��ı��ĳ����ٲü�����
	imshow("�ü�֮��", resulyImage);
	//imwrite("recified.jpg", resulyImage);
}

/// ������
/// x + y ���Ϊ����, ��СΪ����
/// x - y ���Ϊ���ϣ���СΪ����
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
	retPoints.push_back(rect[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);	// 2	����
	retPoints.push_back(rect[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);	// 3	����

	return retPoints;
}


double getDistance(Point2f point1, Point2f point2)
{
	return sqrtf(powf((point1.x - point2.x), 2) + powf((point1.y - point2.y), 2));;
}

double getAngle(Point2f pt1, Point2f pt2) {
	double radian = atan2((pt2.y - pt1.y), (pt2.x - pt1.x));//����   �ú�������ֵ��Χ��[-pi,pi]
	return radian / CV_PI * 180;//�Ƕ�
}


void tiltCorrection() {
    printf("tiltCorrection start\n");

    // ��ȡͼ������ �� ·��
    string name = "doc15";
    string path = "Resources/tiltCorrection/" + name + ".jpg";

    // ��ȡͼ�񲢻ҶȻ�
    Mat img, imgGray, imgBlur, imgCanny, imgDil, imgErode, imgDst;
    img = imread(path);
	imshow("ԭͼ", img);

	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	imshow("ԭ�Ҷ�ͼ", imgGray);

	Mat binaryImage;
	double thres = threshold(imgGray, binaryImage, 0, 255, THRESH_BINARY | THRESH_OTSU);

	// ȡͼ�����Ĳ��ּ���
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


	// ����
	//Mat kernel = getStructuringElement(MORPH_RECT, Size(17, 13));
	//erode(imgRoi, imgErode, kernel);
	//imshow("����ͼ", imgErode);

	// �ҵ���������
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(imgDil, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
	drawContours(imgContours, contours, -1, Scalar(0, 0, 0), 1);
	cout << "contours num : " << contours.size() << endl;

	Point2f rect[4];
	int cntLine = 0;
	double sumAngle = 0;
	for (int i = 0; i < contours.size(); i++) {
		//������С�߽����  ������ret��
		RotatedRect box = minAreaRect(contours[i]);
		box.points(rect);

		line(imgRect, rect[0], rect[1], Scalar(0, 0, 0), 1, 8);
		line(imgRect, rect[1], rect[2], Scalar(0, 0, 0), 1, 8);
		line(imgRect, rect[2], rect[3], Scalar(0, 0, 0), 1, 8);
		line(imgRect, rect[3], rect[0], Scalar(0, 0, 0), 1, 8);

		// ��ȡ���� ���� �� ���� ����
		vector<Point2f> belowPoints = getBelowPoints(rect);

		// ȥ�� �������� �ı߽����
		double distace = getDistance(belowPoints[0], belowPoints[1]);
		if (distace < imgRoi.cols / roiNum * (roiNum - 2) / 6 || distace > imgRoi.cols - 5) continue;
		cout << "angle : " << box.angle << endl;

		cntLine++;
		if (-45 < box.angle && box.angle < 0)sumAngle += box.angle;
		else if (-90 < box.angle && box.angle < -45)sumAngle += box.angle + 90;
		else cntLine--;

		line(imgRoi, belowPoints[0], belowPoints[1], Scalar(0, 0, 0), 1, 8);
	}

	// ����ƽ����б�Ƕ�
	double averageAngle = sumAngle / cntLine; 
	cout << "average angle : " << averageAngle << endl;

	// ���Ӱױߣ���ֹ��ת���µ����ݶ�ʧ
	if (cntLine) {
		int win_2 = sqrtf(img.cols * img.cols + img.rows * img.rows) / 2 * abs(sin(averageAngle / 180 * CV_PI));
		copyMakeBorder(img, img, win_2, win_2, win_2, win_2, BORDER_CONSTANT, Scalar(255, 255, 255));
	}
	// ��ת
	rotateImage(img, imgDst, averageAngle);
	// ȥ������ױ�
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