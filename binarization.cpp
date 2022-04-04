#include "head.h"



//src--原图像
//T--迭代结束的阈值
//a--控制前景区域范围的参数，a越大被划分为前景的区域越多
float get_threld(Mat src, float T, float a)
{
    const int level = 256;


    float hist[level] = { 0 };


    float sum = 0.0;
    for (int i = 0; i < src.rows; i++)   //统计直方图
    {
        uchar* p = src.ptr<uchar>(i);
        for (int j = 0; j < src.cols; j++)
        {
            hist[p[j]] += 1.0;
            sum += p[j];
        }
    }


    float hist_add[level] = { 0 };
    hist_add[0] = hist[0];
    for (int i = 1; i < level; i++)    //累加统计直方图
    {
        hist_add[i] = hist_add[i - 1] + hist[i];
    }


    const int img_size = src.rows * src.cols;
    float T_current = sum / img_size;   //求像素平均值作为阈值的初始值
    float T_next = 0;
    float S0 = 0.0;
    float n0 = 0.0;
    float S1 = 0.0;
    float n1 = 0.0;
    float d = abs(T_current - T_next);
    float count = 0;


    while (d >= T && count < 1000)
    {
        count++;
        cout << count << " , d = " << d << endl;

        uchar TT = (uchar)(T_current);


        n1 = hist_add[TT];   //像素值小于等于TT的点数
        n0 = img_size - hist_add[TT];   //像素值大于TT的点数


        S1 = 0.0;
        for (int i = 0; i <= TT; i++)   //使用累加直方图快速计算图像中所有像素值小于等于阈值的点的像素值之和
        {
            S1 += i * hist[i];
        }
        S0 = sum - S1;    //计算图像中所有像素值大于阈值的点的像素值之和


        float m0 = S0 / n0;    //计算图像中所有像素值大于阈值的平均值
        float m1 = S1 / n1;   //计算图像中所有像素值小于等于阈值的平均值


        T_next = m1 + (m0 - m1) * a;  //参数a控制m0、m1的权重


        d = abs(T_current - T_next);    //当前迭代得到的阈值与下一轮迭代得到的阈值之差的绝对值
        T_current = T_next;         //把下一轮迭代的阈值赋值给当前轮迭代的阈值
    }


    return T_current;
}