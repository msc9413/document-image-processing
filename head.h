#pragma once
#ifndef HEAD_H
#define HEAD_H

#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include <opencv2/photo/cuda.hpp>
#include<iostream>
#include<unordered_set>
#include<unordered_map>
#include<stack>
#include<vector>


using namespace std;
using namespace cv;

void img_ehance(Mat img, Mat& dst, double alpha1, double alpha2);

// tools.cpp
Mat showHist(Mat& src_img);
Mat showHist(Mat& src_img, Mat& grayhist2, bool is_horizontal);


// light compensation.cpp
void lightCompensationProcess();

// impulse noise.cpp
void impulseNoiseProcess();
void removeImpulseNoise2(Mat& image, Mat& imgNoise, float u);


//
void showHist(Mat& img, Mat& dst);



//
void reverseOsmosisNoiseProcess();


// 
void addImpulseNoise(Mat& image, Mat& imgNoise, int n);
void removeImpulseNoise(Mat& imgInput, Mat& imgOutput);
void medianBlurRemoveImpulseNoise(Mat& image, Mat& imgNoise, int ksize);


//
void gaussNoiseProcess();


//
void tiltCorrection();

//
float get_threld(Mat src, float T, float a);

//
void extractTextLinesProcess();

//
void cropEdgeProcess();

#endif // !HEAD_H
