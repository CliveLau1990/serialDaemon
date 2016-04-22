#ifndef _HCALSPERMCOUNT_H
#define _HCALSPERMCOUNT_H


#include<iostream>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>

//在OPENCV3.0版本，使用这类函数cvtColor，需加如下头文件
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

#define MAX_PATH 255

typedef struct
{
	int LittleNum;
	int AimNum;
	int LargeNum;
}Result;

class CalSpermCount
{
public:
	CalSpermCount();
	~CalSpermCount();

	int nshowcol, nshowrow ;
	int nCalcol,nCalrow;
	int minArea, maxArea;
	bool bGetImage ;
	int nShowMidRst ;

	Result result;

	float fallcount;
	float fcount;    //精子数目
	float fratio;
	float fminRatio, fmaxRatio;

	int nThresh;    //直方图阈值
	char szImageFile[MAX_PATH]; //

	Mat img,dstimgbw,dst_gray;
    int g_nThresholdValue;
    int g_nThresholdType;


	int BWconvert();                //二值化
	bool readData(Mat image);     //获取图像
	Mat clipCenterImage(Mat img);
	int ShowImage(char *szWinName, Mat img);
	Result calNum(Mat img);

};

#endif
