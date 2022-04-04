
#include "head.h"


string name;

/// <summary>
/// import image
/// </summary>


void quickAdaptiveThreshold2(Mat grayscale, Mat& thres)
{

	/**           /
	*            | FOREGROUND, if pn < ((gs(n) + gs(n-w)) / (2*s)) *
	* color(n) = |                     ((100-t)/100)
	*            | BACKGROUND_QR, otherwise
	*            /
	* where pn = gray value of current pixel,
	*        s = width of moving average, and
	*        t = threshold percentage of brightness range
	*    gs(n) = gs(n-1) * (1-1/s) + pn
	*    gs(n-w) = gs-value of pixel above current pixel
	*
	*/

	int width = grayscale.cols;
	int height = grayscale.rows;
	cout << width << "," << height << endl;

	//printf("width = %d, height = %d\n", width, height);
	int t = 15;
	int s = 48; // s: number of pixels in the moving average (w = image width)
	//int s = 5;
	//Mat m = Mat(height + 2 * s, width + 2 * s, CV_8UC(0));
	//for (int i = s; i < s + height; i++) {
	//	for (int j = s; j < s + width; j++) {
	//		
	//	}
	//}


	//const int S = 9; // integer shift, needed to avoid floating point operations
	//const int power2S = 1 << S;
	//// for speedup: multiply all values by 2^s, and use integers instead of floats
	//int factor = power2S * (100 - t) / (100 * s); // multiplicand for threshold
	//int gn = 127 * s; // initial value of the moving average (127 = average gray value)
	//int q = power2S - power2S / s; // constant needed for average computation
	//int pn, hn;
	unsigned char* scanline = NULL;
	//int* prev_gn = new int[width];

	double* factors = new double[s + 1];
	factors[0] = 1.0;
	for (int i = 1; i <= s; i++) {
		factors[i] = factors[i - 1] * (1.0 - 1 / (2 * s + 1.0));
	}

	thres = grayscale.clone();
	for (int y = 0; y < height; y++)
	{
		cout << width << "," << height << " : " << y << endl;
		//int yh = y * width;
		scanline = grayscale.ptr<uchar>(y);
		for (int x = 0; x < width; x++)
		{
			//cout << width << "," << height << " : " <<y << "," << x << endl;
			double gn = scanline[x];
			int len = 1;
			for (int k = 1; k <= s; k++) {
				if (x + k >= width) break;
				len++;
				gn += scanline[x + k] * factors[k];
			}
			for (int k = 1; k <= s; k++) {
				if (x - k < 0) break;
				len++;
				gn += scanline[x - k] * factors[k];
			}

			for (int k = 1; k <= s; k++) {
				if (y + k >= height) break;
				len++;
				gn += grayscale.ptr<uchar>(y + k)[x] * factors[k];
			}
			for (int k = 1; k <= s; k++) {
				if (y - k < 0) break;
				len++;
				gn += grayscale.ptr<uchar>(y - k)[x] * factors[k];
			}

			//hn = gn / len;
			scanline[x] < (gn / len)* (1 - t / 100.0) ? thres.ptr<uchar>(y)[x] = 0 : thres.ptr<uchar>(y)[x] = 0xff;
		}

	}
	//delete prev_gn;
}
void quickAdaptiveThreshold(Mat grayscale, Mat& thres)
{

	/**           /
	*            | FOREGROUND, if pn < ((gs(n) + gs(n-w)) / (2*s)) *
	* color(n) = |                     ((100-t)/100)
	*            | BACKGROUND_QR, otherwise
	*            /
	* where pn = gray value of current pixel,
	*        s = width of moving average, and
	*        t = threshold percentage of brightness range
	*    gs(n) = gs(n-1) * (1-1/s) + pn
	*    gs(n-w) = gs-value of pixel above current pixel
	*
	*/
	int width = grayscale.cols;
	int height = grayscale.rows;
	cout << width << "," << height << endl;
	//printf("width = %d, height = %d\n", width, height);
	int t = 15;
	int s = width >> 7; // s: number of pixels in the moving average (w = image width)
	const int S = 9; // integer shift, needed to avoid floating point operations
	const int power2S = 1 << S;
	// for speedup: multiply all values by 2^s, and use integers instead of floats
	int factor = power2S * (100 - t) / (100 * s); // multiplicand for threshold
	int gn = 127 * s; // initial value of the moving average (127 = average gray value)
	int q = power2S - power2S / s; // constant needed for average computation
	int pn, hn;
	unsigned char* scanline = NULL;
	int* prev_gn = new int[width];

	for (int i = 0; i < width; i++) {
		prev_gn[i] = gn;
	}
	thres = grayscale.clone();
	for (int y = 0; y < height; y++)
	{
		//int yh = y * width;
		scanline = grayscale.ptr<uchar>(y);
		for (int x = 0; x < width; x++)
		{
			pn = scanline[x];
			gn = ((gn * q) >> S) + pn;
			hn = (gn + prev_gn[x]) >> 1;
			prev_gn[x] = gn;
			pn < (hn* factor) >> S ? thres.ptr<uchar>(y)[x] = 0 : thres.ptr<uchar>(y)[x] = 0xff;
		}
		y++;
		if (y == height)
			break;
		//yh = y * width;
		scanline = grayscale.ptr<uchar>(y);
		for (int x = width - 1; x >= 0; x--)
		{
			pn = scanline[x];
			gn = ((gn * q) >> S) + pn;
			hn = (gn + prev_gn[x]) >> 1;
			prev_gn[x] = gn;
			pn < (hn* factor) >> S ? thres.ptr<uchar>(y)[x] = 0 : thres.ptr<uchar>(y)[x] = 0xff;
		}
	}
	//delete prev_gn;
}

void extractShadow(Mat& imgGray, Mat& imgShadow) {

	double maxVal, minVal;
	minMaxLoc(imgGray, &minVal, &maxVal);
	printf("max = %lf, min = %lf\n", maxVal, minVal);
	int cols = imgGray.cols, rows = imgGray.rows;

	imgShadow = Mat::zeros(rows, cols, CV_8U);

	uchar a = maxVal;
	vector<Point2i> Sa, C0, C1;
	Sa.reserve(cols * rows);

	unordered_map<uchar, unordered_set<uint>> pexil;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			pexil[imgGray.ptr<uchar>(i)[j]].insert(i * cols + j);
		}
	}

	while (a > minVal) {
		printf("a = %d\n", a);
		for (uint num : pexil[a]) {
			Sa.push_back(Point2i(num % cols, num / cols));
		}
		//for (int i = 0; i < rows; i++) {
		//	for (int j = 0; j < cols; j++) {
		//		if (imgGray.ptr<uchar>(i)[j] >= a) {
		//			Sa.push_back(Point2i(j, i));
		//		}
		//	}
		//}

		convexHull(Sa, C1);
		unordered_set<uint> C0_set;
		unordered_set<uint> C1_set;
		for (Point2i point : C1) {
			C1_set.insert(point.y * cols + point.x);
		}
		for (Point2i point : C0) {
			C0_set.insert(point.y * cols + point.x);
		}

		for (uint num : C1_set) {
			if (C0_set.count(num) == 0) {
				imgShadow.ptr<uchar>(num / cols)[num % cols] = a;
			}
		}
		/*for (Point2i point : C1) {
			bool ok = true;
			for (Point2i pointC0 : C0) {
				if (point == pointC0) {
					ok = false;
					break;
				}
			}
			if (ok) {
				imgShadow.ptr<uchar>(point.y)[point.x] = a;
			}
		}*/
		C0 = C1;
		a--;
	}
}
void MinFilter(Mat& src_image, Mat& dst_image, int k_size = 3) {
	dst_image = src_image.clone();

	int max_rows = src_image.rows;          // 行像素数
	int max_cols = src_image.cols;          // 列像素数
	int p = (k_size - 1) / 2;               // -(k−1)/2 ~ (k−1)/2


	// 对每个像素点进行处理
	for (int row = 0; row < max_rows; ++row) {
		for (int col = 0; col < max_cols; ++col) {
			// 以当前像素点为中心的k*k的矩阵中，取最小值
			int mint = 255;          // 初始最大值
			for (int i = row - p; i <= row + p; ++i)
				for (int j = col - p; j <= col + p; ++j)
					if (i >= 0 && i < max_rows && j >= 0 && j < max_cols)
						if (src_image.at<uchar>(i, j) < mint)
							mint = src_image.at<uchar>(i, j);
			dst_image.at<uchar>(row, col) = mint;   // 像素值赋最小值    
		}
	}
}
void MaxFilter(Mat& src_image, Mat& dst_image, int k_size = 3) {
	dst_image = src_image.clone();

	int max_rows = src_image.rows;          // 行像素数
	int max_cols = src_image.cols;          // 列像素数
	int p = (k_size - 1) / 2;               // -(k−1)/2 ~ (k−1)/2


	// 对每个像素点进行处理
	for (int row = 0; row < max_rows; ++row) {
		cout << max_rows << ", " << row << endl;
		for (int col = 0; col < max_cols; ++col) {
			// 以当前像素点为中心的k*k的矩阵中，取最小值
			int maxt = 0;       // 初始最大值
			for (int i = row - p; i <= row + p; ++i)
				for (int j = col - p; j <= col + p; ++j)
					if (i >= 0 && i < max_rows && j >= 0 && j < max_cols)
						if (src_image.at<uchar>(i, j) > maxt)
							maxt = src_image.at<uchar>(i, j);
			dst_image.at<uchar>(row, col) = maxt;   // 像素值赋最小值    
		}
	}
}

void showHist(Mat& img, Mat& dst)
{
	/*
		-dmis 表示维度，对灰度图像来说只有一个通道值 dmis=1
		-bins  表示在维度中子区域大小划分，bins=256,划分为256个级别
		-range 表示值得范围，灰度值范围为【0~255】之间
	*/
	

	//3、定义变量范围并创建3个矩阵来存储每个直方图
	const float range[2] = { 0,255 };
	Mat grayhist = img.clone();

	//4、使用calcHist函数计算直方图
	const int channels[1] = { 0 };  //通道索引
	float inRanges[2] = { 0,255 };
	const float* ranges[1] = { inRanges };  //像素灰度值范围
	const int bins[1] = { 256 };  //直方图的维度，其实就是像素灰度值的最大值
	int numbins = 256;
	cv::calcHist(&img, 1, channels, Mat(), grayhist, img.channels(), bins, ranges);

	//5、创建一个512*300像素大小的图像，用于绘制显示
	int width = 512;
	int height = 300;
	Mat histImage(height, width, CV_8UC1, 255);

	//6、将最小值与最大值标准化直方图矩阵
	normalize(grayhist, grayhist, 0, height, NORM_MINMAX);

	//7、使用彩色通道绘制直方图
	int binStep = cvRound((float)width / (float)numbins);  //通过将宽度除以区间数来计算binStep变量

	for (int i = 1; i < numbins; i++)
	{
		line(histImage,
			Point(binStep * (i - 1), height - cvRound(grayhist.at<float>(i - 1))),
			Point(binStep * (i), height - cvRound(grayhist.at<float>(i))),
			0
		);
	}
	dst = histImage;
	imshow("直方图", dst);
	return;
}

void deleteshadow(string pic_url) {
	Mat src = cv::imread(pic_url);
	if (!src.data)
	{
		printf("err:路径错误\n");
		getchar();
		return;
	}
	//resize(src, src, Size(), 0.2, 0.2);
	cvtColor(src, src, COLOR_BGR2GRAY);
	imwrite("Resources/out/" + name + "_gray.jpg", src);
	imshow("1 src", src);

	Mat oldBinary;
	quickAdaptiveThreshold(src, oldBinary);
	//threshold(src, oldBinary, 0, 255, THRESH_BINARY | THRESH_OTSU);
	imshow("Weller Image", oldBinary);
	imwrite("Resources/out/" + name + "_oldBinary.jpg", oldBinary);

	//根据图像大小调节窗口大小
	// int n = src.cols / 80;
	int n = 25;

	//先对原灰度图做最大滤波(不就是膨胀么)，得到maxFilterMat_A
	Mat element = getStructuringElement(cv::MorphShapes::MORPH_RECT, cv::Size(n, n));
	int iteration = 1;
	Mat maxFilterMat_A;
	cv::morphologyEx(src, maxFilterMat_A, MORPH_DILATE, element, cv::Point(-1, -1), iteration, cv::BORDER_CONSTANT, cv::morphologyDefaultBorderValue());
	//MaxFilter(src, maxFilterMat_A, n);
	imshow("2 最大滤波", maxFilterMat_A);
	imwrite("Resources/out/" + name + "_dilate.jpg", maxFilterMat_A);

	//再对maxFilterMat_A做最小化滤波（不就是腐蚀么）
	Mat minFilterMat_B;
	cv::morphologyEx(maxFilterMat_A, minFilterMat_B, MORPH_ERODE, element, cv::Point(-1, -1), iteration, cv::BORDER_CONSTANT, cv::morphologyDefaultBorderValue());
	//MinFilter(maxFilterMat_A, minFilterMat_B, n);
	imshow("3 先最大再最小滤波", minFilterMat_B);
	imwrite("Resources/out/" + name + "_erode.jpg", minFilterMat_B);

	//(先最大再最小滤波) - 原灰度图
	Mat diffMat = minFilterMat_B - src;
	imshow("4 减去原图", diffMat);
	imwrite("Resources/out/" + name + "_blackhat.jpg", diffMat);

	//取反，得到白底黑字
	diffMat = 255 - diffMat;
	//resize(diffMat, diffMat, Size(), 0.2, 0.2);
	imshow("5 减去原图再取反", diffMat);
	imwrite("Resources/out/" + name + "_out.jpg", diffMat);

	Mat normalizeMat;
	normalize(diffMat, normalizeMat, 0, 255, NORM_MINMAX);
	imshow("6 结果归一化到0-255", normalizeMat);

	//Test_calcHist_cpp(src);

	quickAdaptiveThreshold(diffMat, diffMat);
	imshow("Weller Image", diffMat);
	imwrite("Resources/out/" + name + "_binary.jpg", diffMat);

	waitKey(0);
}

int main() {
	//impulseNoiseProcess();
	//gaussNoiseProcess();
	//reverseOsmosisNoiseProcess();
	name = "doc12";
	string path = "Resources/" + name + ".jpg";
	//deleteshadow(path);
	//lightCompensationProcess();
	//tiltCorrection();
	//extractTextLinesProcess();
	cropEdgeProcess();
	return 0;

	Mat imgGray, img, imgOtsu, imgWeller, imgWeller2, imgShadow, imgNoise,imgRemoveNoise, imgRemoveNoiseMedianBlur;
	img = imread(path);	// matrix data type 由opencv引入来处理图像

	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	//resize(imgGray, imgGray, Size(), 0.15, 0.15);
	imshow("直方图", showHist(imgGray));
	cout << "ok" << endl;

	waitKey(0);				// 增加延时，0表示无穷,类似sleep
	return 0;

}