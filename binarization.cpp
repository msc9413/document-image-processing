#include "head.h"



//src--ԭͼ��
//T--������������ֵ
//a--����ǰ������Χ�Ĳ�����aԽ�󱻻���Ϊǰ��������Խ��
float get_threld(Mat src, float T, float a)
{
    const int level = 256;


    float hist[level] = { 0 };


    float sum = 0.0;
    for (int i = 0; i < src.rows; i++)   //ͳ��ֱ��ͼ
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
    for (int i = 1; i < level; i++)    //�ۼ�ͳ��ֱ��ͼ
    {
        hist_add[i] = hist_add[i - 1] + hist[i];
    }


    const int img_size = src.rows * src.cols;
    float T_current = sum / img_size;   //������ƽ��ֵ��Ϊ��ֵ�ĳ�ʼֵ
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


        n1 = hist_add[TT];   //����ֵС�ڵ���TT�ĵ���
        n0 = img_size - hist_add[TT];   //����ֵ����TT�ĵ���


        S1 = 0.0;
        for (int i = 0; i <= TT; i++)   //ʹ���ۼ�ֱ��ͼ���ټ���ͼ������������ֵС�ڵ�����ֵ�ĵ������ֵ֮��
        {
            S1 += i * hist[i];
        }
        S0 = sum - S1;    //����ͼ������������ֵ������ֵ�ĵ������ֵ֮��


        float m0 = S0 / n0;    //����ͼ������������ֵ������ֵ��ƽ��ֵ
        float m1 = S1 / n1;   //����ͼ������������ֵС�ڵ�����ֵ��ƽ��ֵ


        T_next = m1 + (m0 - m1) * a;  //����a����m0��m1��Ȩ��


        d = abs(T_current - T_next);    //��ǰ�����õ�����ֵ����һ�ֵ����õ�����ֵ֮��ľ���ֵ
        T_current = T_next;         //����һ�ֵ�������ֵ��ֵ����ǰ�ֵ�������ֵ
    }


    return T_current;
}