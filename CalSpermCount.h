#ifndef _HCALSPERMCOUNT_H
#define _HCALSPERMCOUNT_H


#include<iostream>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>

//��OPENCV3.0�汾��ʹ�����ຯ��cvtColor���������ͷ�ļ�
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
	float fcount;    //������Ŀ
	float fratio;
	float fminRatio, fmaxRatio;

	int nThresh;    //ֱ��ͼ��ֵ
	char szImageFile[MAX_PATH]; //

	Mat img,dstimgbw,dst_gray;
    int g_nThresholdValue;
    int g_nThresholdType;


	int BWconvert();                //��ֵ��
	bool readData(Mat image);     //��ȡͼ��
	Mat clipCenterImage(Mat img);
	int ShowImage(char *szWinName, Mat img);
	Result calNum(Mat img);

};

#endif
