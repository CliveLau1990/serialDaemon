#ifndef _HHISTGRAM_H
#define _HHISTGRAM_H

#include<iostream>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

class Histgram1D
{
private : 
	int histSize[1];
	float hranges[2];
	const float *ranges[1];
	int channels[1];
public:
	Histgram1D();


	cv::Mat getHistogram(const cv::Mat &image);


	cv::Mat getHistogramImage(const cv::Mat &image, int zoom = 1);


	static cv::Mat getImageOfHistogram(const cv::Mat &hist, int zoom);

	static cv::Mat applyLookUp(const cv::Mat &image, const cv::Mat &lookup)
	{
		cv::Mat result;
		cv::LUT(image, lookup,result);
		return result;
	}
	cv::Mat stretch(const cv:: Mat &image, int minValue  = 0)
	{
		cv::Mat hist = getHistogram(image);
		int imin = 0;
		for(; imin<histSize[0]; imin++)
		{
			if(hist.at<float>(imin) > minValue)
				break;
		}
		int imax = histSize[0] - 1;
		for(; imax >=0;imax--)
		{
			if(hist.at<float>(imax) > minValue)
			    break;
		}

		
		int dim(256);   // int dim(256);
		cv::Mat lookup(1, &dim,CV_8U);
		for(int i = 0 ; i<256; i++)
		{
			if(i <imin) lookup.at<uchar>(i) = 0;
			else if(i>imax) lookup.at<uchar>(i) = 255;
			else lookup.at<uchar>(i) = cvRound(255.0 * (i - imin)/(imax - imin));

		}
		cv::Mat result;
		result = applyLookUp(image, lookup);
		return result;



	}
	
};




#endif
