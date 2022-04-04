#include "head.h"



Scalar colors[] = { Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255),
					Scalar(255,255,0), Scalar(255,0,255), Scalar(0,255,255),
					Scalar(0,0,0)
					};




Mat polyfit(vector<Point>& in_point, int n);

//int polyfit(vector<Point>& in_point)
//{
//	//数据输入
//	Point in[19] = { Point(50,120),Point(74,110),Point(98,100),Point(122,100),Point(144,80)
//		,Point(168,80),Point(192,70),Point(214,50),Point(236,40),Point(262,20)
//		,Point(282,20),Point(306,30),Point(328,40),Point(356,50),Point(376,50)
//		,Point(400,50),Point(424,50),Point(446,40),Point(468,30) };
//
//	vector<Point> in_point(begin(in), end(in));
//
//	//n:多项式阶次
//	int n = 9;
//	Mat mat_k = polyfit(in_point, n);
//
//
//	//计算结果可视化
//	Mat out(150, 500, CV_8UC3, Scalar::all(0));
//
//	//画出拟合曲线
//	for (int i = in[0].x; i < in[size(in) - 1].x; ++i)
//	{
//		Point2d ipt;
//		ipt.x = i;
//		ipt.y = 0;
//		for (int j = 0; j < n + 1; ++j)
//		{
//			ipt.y += mat_k.at<double>(j, 0) * pow(i, j);
//		}
//		circle(out, ipt, 1, Scalar(255, 255, 255), FILLED);
//	}
//
//	//画出原始散点
//	//for (int i = 0; i < in_point.size(); ++i)
//	//{
//	//	Point ipt = in_point[i];
//	//	circle(out, ipt, 3, Scalar(0, 0, 255), FILLED);
//	//}
//
//	imshow("9次拟合", out);
//	//waitKey(0);
//
//	return 0;
//}

Mat polyfit(vector<Point>& in_point, int n)
{
	int size = in_point.size();
	//所求未知数个数
	int x_num = n + 1;
	//构造矩阵U和Y
	Mat mat_u(size, x_num, CV_64F);
	Mat mat_y(size, 1, CV_64F);

	for (int i = 0; i < mat_u.rows; ++i)
		for (int j = 0; j < mat_u.cols; ++j)
		{
			mat_u.at<double>(i, j) = pow(in_point[i].x, j);
		}

	for (int i = 0; i < mat_y.rows; ++i)
	{
		mat_y.at<double>(i, 0) = in_point[i].y;
	}

	//矩阵运算，获得系数矩阵K
	Mat mat_k(x_num, 1, CV_64F);
	mat_k = (mat_u.t() * mat_u).inv() * mat_u.t() * mat_y;
	//cout << mat_k << endl;
	return mat_k;
}

void extractTextLinesProcess() {

    printf("extractTextLinesProcess start\n");

    string name = "doc8";
    string path = "Resources/extractTextLinesProcess/" + name + ".jpg";

    // imwrite("Resources/extractTextLinesProcess/" + name + "_gray.jpg", imgGray);
	Mat img, imgBinary, imgDil, imgCanny;
	img = imread(path);
	cvtColor(img, imgBinary, COLOR_BGR2GRAY);
	threshold(imgBinary, imgBinary, 0, 255, THRESH_BINARY | THRESH_OTSU);

	//Canny(imgBinary, imgCanny, 25, 75);
	imgCanny = imgBinary.clone();
	imgCanny = 255 - imgCanny;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(1, 1));
	dilate(imgCanny, imgDil, kernel);


	// 找到所有轮廓
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	Mat imgWrite(imgBinary.size(), CV_8UC1, Scalar(255));
	//findContours(imgDil, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	drawContours(imgBinary, contours, -1, Scalar(0, 0, 0), 1);
	//drawContours(imgWrite, contours, -1, Scalar(0, 0, 0), 1);
	cout << "contours num : " << contours.size() << endl;


	vector<Rect> boundRect(contours.size());
	unordered_map<int, int> heightCount;
	for (int i = 0; i < contours.size(); i++) {
		if (contourArea(contours[i]) < 4)continue;
		//计算最小边界矩形  保存在ret中
		boundRect[i] = boundingRect(contours[i]);
		heightCount[boundRect[i].height]++;
		rectangle(imgBinary, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 0, 0), 1);
	}

	// 次数最多的高度，作为标准高度
	int height = 0;
	int count = 0;
	for (auto pa : heightCount) {
		if (pa.second > count) {
			height = pa.first;
			count = pa.second;
		}
	}
	cout << "height : " << height << ">> num : " << heightCount[height] << endl;

	// 过滤过大过小的矩形，绘制边界矩形
	unordered_map<uchar, vector<Rect>> grayRects;
	unordered_map<unsigned long, bool> used;
	for (int i = 0; i < boundRect.size(); i++) {
		if (boundRect[i].height > 3 * height || boundRect[i].height < height / 2.5) {
			used[boundRect[i].tl().y * imgWrite.cols + boundRect[i].tl().x] = true;
			continue;
		}
		//if (boundRect[i].height == height) {
		//	rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 2);
		//}
		rectangle(imgWrite, boundRect[i].tl(), boundRect[i].br(), Scalar(i % 255), FILLED);
		grayRects[i % 255].push_back(boundRect[i]);
		used[boundRect[i].tl().y * imgWrite.cols + boundRect[i].tl().x] = false;
	}

	int num = 0;
	// 寻找文本矩形行
	int color = 1;
	int emptyCnt;
	Rect root, r;
	for (int i = 0; i < boundRect.size(); i++) {
		//cout << "get Rect line" << boundRect.size() << " : " << i << endl;
		root = boundRect[i];
		if (used[boundRect[i].tl().y * imgWrite.cols + boundRect[i].tl().x]) continue;
		//rectangle(img, boundRect[i].tl(), boundRect[i].br(), colors[color % 7], 5);
		used[boundRect[i].tl().y * imgWrite.cols + boundRect[i].tl().x] = true;

		// 当前矩形右侧矩形
		r = root;
		vector<Rect> rectLine_l2r;
		rectLine_l2r.push_back(r);
		while (true) {
			bool isOk = false;
			int x = r.x + r.width;
			emptyCnt = 0;
			for (; x < imgWrite.cols; x++) {
				for (int y = r.y + r.height - 1; y >= r.y; y--) 
				{
					int grayVal = imgWrite.ptr<uchar>(y)[x];
					// 当前像素点在矩形中
					if (grayVal != 255) {
						// 查找当前矩形
						for (Rect& rect : grayRects[grayVal]) {
							//if (used[rect.tl().y * imgWrite.cols + rect.tl().x]) continue;

							if (x >= rect.x && x < rect.x + rect.width && y >= rect.y && y < rect.y + rect.height) {
								if (used[rect.tl().y * imgWrite.cols + rect.tl().x]) {
									/*isOk = true;
									x = imgWrite.cols;*/
									y = r.y;
									break;
								}
								emptyCnt = 0;
								rectLine_l2r.push_back(rect);
								used[rect.tl().y * imgWrite.cols + rect.tl().x] = true;
								isOk = true;
								r = rect;
								break;
							}
						}

					}
					if (isOk) break;
				} // y
				if (isOk) break;
				emptyCnt++;

				if (emptyCnt > height * 5) {
					x = imgWrite.cols;
				}
			}
			if (x >= imgWrite.cols) break;
		}

		// 当前矩形左侧矩形
		r = root;
		emptyCnt = 0;
		vector<Rect> rectLine_r2l;
		while (true) {
			bool isOk = false;
			int x = r.x - 1;
			for (; x >= 0; x--) {
				for (int y = r.y + r.height - 1; y >= r.y; y--) {
					int grayVal = imgWrite.ptr<uchar>(y)[x];
					if (grayVal != 255) {	// 当前像素点在矩形中
						// 查找当前矩形
						for (Rect& rect : grayRects[grayVal]) {
							

							if (x >= rect.x && x < rect.x + rect.width && y >= rect.y && y < rect.y + rect.height) {
								if (used[rect.tl().y * imgWrite.cols + rect.tl().x]) {
									/*isOk = true;
									x = -1;*/
									y = r.y;
									break;
								}
								emptyCnt = 0;
								rectLine_r2l.push_back(rect);
								used[rect.tl().y * imgWrite.cols + rect.tl().x] = true;
								isOk = true;
								r = rect;
								break;
							}
						}
					} 
					if (isOk) break;
				}// y end
				if (isOk) break;
				emptyCnt++;
				if (emptyCnt > height * 5) {
					x = -1;
				}
			}
			if (x < 0) break;
		}

		vector<Point> in_point;
		for (int i = rectLine_r2l.size() - 1; i >= 0; i--) {
			Rect temp = rectLine_r2l[i];
			circle(img, Point(temp.x + temp.width / 2, temp.y + temp.height - 1), 4, colors[color % 7], FILLED);
			in_point.push_back(Point(temp.x + temp.width / 2, temp.y + temp.height - 1));
			//putText(img, to_string(num++), Point(temp.x + temp.width / 2, temp.y + temp.height - 1), FONT_HERSHEY_DUPLEX, 0.75, colors[color % 7], 2);
		}
		for (int i = 0; i < rectLine_l2r.size(); i++) {
			Rect temp = rectLine_l2r[i];
			circle(img, Point(temp.x + temp.width / 2, temp.y + temp.height - 1), 4, colors[color % 7], FILLED);
			in_point.push_back(Point(temp.x + temp.width / 2, temp.y + temp.height - 1));
			//putText(img, to_string(num++), Point(temp.x + temp.width / 2, temp.y + temp.height - 1), FONT_HERSHEY_DUPLEX, 0.75, colors[color % 7], 2);
		}

		if (in_point.size() > 10) 
		{
			rectangle(img, rectLine_l2r[0].tl(), rectLine_l2r[0].br(), colors[color % 7], 5);
			//cout << "曲线拟合开始 " << endl;

			//n:多项式阶次
			int n = 5;
			Mat mat_k = polyfit(in_point, n);

			//画出拟合曲线
			for (int i = in_point[0].x; i <= in_point[in_point.size() - 1].x; ++i)
			{
				Point2d ipt;
				ipt.x = i;
				ipt.y = 0;
				for (int j = 0; j < n + 1; ++j)
				{
					ipt.y += mat_k.at<double>(j, 0) * pow(i, j);
				}
				circle(img, ipt, 1, colors[color % 7], FILLED);
				
				//if (i == in_point[0].x) putText(img, to_string(num++), ipt, FONT_HERSHEY_DUPLEX, 0.75, colors[color % 7], 2);
			}
		}
		color++;

	}

	cout << "get line ok ! line num = " << num << endl;

	imshow("contours", imgBinary);
	imshow("imgCanny", imgCanny);
	imshow("imgDil", imgDil);
	imwrite("Resources/extractTextLinesProcess/" + name + "_contours.jpg", imgBinary);
	imwrite("Resources/extractTextLinesProcess/" + name + "_Rect.jpg", imgWrite);
	imwrite("Resources/extractTextLinesProcess/" + name + "_dilate.jpg", imgDil);
	imwrite("Resources/extractTextLinesProcess/" + name + "_img.jpg", img);

	waitKey(0);
}
