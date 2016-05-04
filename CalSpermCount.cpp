#include "CalSpermCount.h"
#include "Histogram.h"

extern CalSpermCount CalCount;

#define WINDOWNAME "DealWindows"

CalSpermCount::CalSpermCount()
{
	g_nThresholdValue = 60;
    g_nThresholdType = 1;
	nShowMidRst = 1;
	nshowcol = 640;
	nshowrow = 480;
	nCalcol = 800;
	nCalrow = 600;
	bGetImage =false;
	fratio = 0.17;
	fminRatio = 0.1;
	fmaxRatio = 0.1;

	memset(&result, 0, sizeof(result));
    //void on_Theshold(int, void *);  //回调函数
}

CalSpermCount::~CalSpermCount()
{
	;
}


int CalSpermCount::BWconvert()
{
	return 1;

}

int CalSpermCount::ShowImage(char *szWinName,Mat img)
{
#if 0 // No GUI
	Size dsize = Size(nshowcol,nshowrow);
	Mat image2show = Mat(dsize,CV_8U);
	::resize(img, image2show,dsize);
	cvNamedWindow(szWinName);
	imshow(szWinName, image2show);
#endif
	return 1;
}

bool CalSpermCount::readData(Mat image)
{
    cvtColor(image, img, CV_RGB2GRAY);
	//反色处理
	img = ~img;
	if(img.empty())
		return false;
	else
	{
        bGetImage  = true;
		return true;
	}
}

Mat CalSpermCount::clipCenterImage(Mat img)
{
	Mat imageROI ,imgResult;
	int w = nCalcol, h = nCalrow;
	imgResult = img;
	if(img.cols >= w)
	{
		imageROI = img.colRange(img.cols/2 - w/2,  img.cols/2 + w/2);
		imgResult = img;
	}

	if(img.rows >= h)
	{
		imgResult = imageROI.rowRange(imageROI.rows/2 - h/2,  imageROI.rows/2 + h/2);
	}
	return imgResult;


}

void on_Theshold(int, void *)
{

	int blocksize = 81;
	double offset =15;
	int threshold=100;
	int adaptive_method = CV_ADAPTIVE_THRESH_GAUSSIAN_C;
	::threshold(CalCount.dst_gray,CalCount.dstimgbw,CalCount.g_nThresholdValue, 255, CalCount.g_nThresholdType);

	//adaptiveThreshold(CalCount.img,CalCount.dstimgbw,255,adaptive_method,CV_THRESH_BINARY,blocksize,0);
   	//cvNamedWindow("src2");
	if(CalCount.nShowMidRst)
	    CalCount.ShowImage(WINDOWNAME,CalCount.dstimgbw);

	//imshow(WINDOWNAME, CalCount.dstimgbw);

}


Result CalSpermCount::calNum(Mat img)
{
	Mat tmp, tmp_back;
//	int col = 800, row = 600;
	Size dsize = Size(nshowcol, nshowrow);
	Mat image2show = Mat(dsize,CV_8U);

	GaussianBlur(img,img,cv::Size(5,5), 1.5);

	if(nShowMidRst)
	    ShowImage("after filtering", img);
	//cvSmooth(img, img, CV_MEDIAN, 3, 0, 0, 0); //中值滤波，消除小的噪声；

	//Mat element(9,9,CV_8U,Scalar(1));
	Mat element = getStructuringElement(MORPH_RECT,Size(10,10));
	erode(img, tmp,element);
    //ShowImage("erode",tmp);

	dilate(tmp,tmp_back,element);
	::resize(tmp_back, image2show,dsize);
	//cvNamedWindow("dilate");
	//imshow("dilate", image2show);
	//ShowImage("dilate",tmp_back);

	morphologyEx(img,dst_gray,MORPH_TOPHAT,element);
	//morphologyEx(img,dst_gray,MORPH_BLACKHAT,cv::Mat());
	//dst_gray = img;

	//绘制直方图
	Histgram1D hist;
	hist.stretch(dst_gray,0.01f);
	if(nShowMidRst)
	    ShowImage("Histogram", hist.getHistogramImage(dst_gray, 1));
	Mat tmpImage;

	//::equalizeHist(dst_gray,tmpImage);
	//ShowImage("拉伸后", tmpImage);
	//ShowImage("拉伸后Histogram",  hist.getHistogramImage(tmpImage, 1));

    if(nShowMidRst)
   {
		ShowImage("Picture EQ", hist.stretch(dst_gray, 50));
	    ShowImage("after draw, Histogram",  hist.getHistogramImage(hist.stretch(dst_gray,50), 1));
	}

	dst_gray =  hist.stretch(dst_gray, 50);
	//dst_gray =  ::equalizeHist(dst_gray,dst_gray);


	//dst_gray = tmp_back  - img  ;
	//dst_gray = img  ;
	::resize(dst_gray, image2show,dsize);
#if 0 // No GUI
	if(nShowMidRst)
	    cvNamedWindow(WINDOWNAME);

	::createTrackbar("filter pattern",WINDOWNAME,&g_nThresholdType,4,on_Theshold);
	createTrackbar("theshold value",WINDOWNAME,&g_nThresholdValue,255,on_Theshold);
#endif
	//初始化自定义回调函数
	on_Theshold(0,0);

	//adaptiveThreshold(dst_gray,dstimgbw,255,adaptive_method,CV_THRESH_BINARY,blocksize,0);
    //cvAdaptiveThreshold(img, dst_bw,255,adaptive_method,//自适应阀值，blocksize为奇数
    // CV_THRESH_BINARY,blocksize,offset);


//	::resize(dstimgbw, image2show,dsize);
//	cvNamedWindow("src2");
//	imshow("src2", image2show);
//		waitKey(0);

	vector<vector<Point> > contours;

	findContours(dstimgbw,contours,CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

	Mat rst(img.size(),CV_8U,Scalar(0));
	drawContours(rst,contours,-1,255,1);

	int cmin = minArea;
	int cmax = maxArea;
	vector<vector<Point> >::iterator itc = contours.begin();
	int i = 0;
	result.LittleNum = 0;
	result.AimNum = 0;
	result.LargeNum = 0;
	fallcount = contours.size();
	while(itc!=contours.end())
	{

		if(itc->size() < cmin )
		{
			result.LittleNum ++;
			itc = contours.erase(itc);
			//i++;
		}
		else if(itc->size() > cmax)
		{
			itc = contours.erase(itc);
			result.LargeNum ++;
		}
		else
		{
			result.AimNum ++;
		    cout <<i<<" = "<< itc->size()<<endl;
			i++;
			++itc;
		}
	}

	Mat rst2(img.size(),CV_8UC3,Scalar(0,0,0));
	drawContours(rst2,contours,-1,cv::Scalar(255,255,255),1);

	char sz1[MAX_PATH],sz2[MAX_PATH],sz3[MAX_PATH];
	char sz4[MAX_PATH];
	char szError[MAX_PATH] = " ";

	fcount = fratio * result.AimNum;


	if(result.LittleNum/fallcount > fminRatio  || result.LargeNum/fallcount > fmaxRatio)
			sprintf(szError,"Sample is dirty");

	sprintf(sz1,"Aim Num = %d",   result.AimNum);
	sprintf(sz2,"Little Num=%d",  result.LittleNum);
	sprintf(sz3,"Large Num=%d",   result.LargeNum);
	sprintf(sz4,"Count =%3.3f",   fcount);

	putText(rst2,sz1,cv::Point(10,10),cv::FONT_HERSHEY_PLAIN, 1.0,   cv::Scalar(255,0,0),     2);
	putText(rst2,sz2,cv::Point(10,30),cv::FONT_HERSHEY_PLAIN, 1.0,   cv::Scalar(255,0,0),     2);
	putText(rst2,sz3,cv::Point(10,50),cv::FONT_HERSHEY_PLAIN, 1.0,   cv::Scalar(255,0,0),     2);
	putText(rst2,sz4,cv::Point(10,70),cv::FONT_HERSHEY_PLAIN, 1.0,   cv::Scalar(255,0,255),     2);
	putText(rst2,szError,cv::Point(10,90),cv::FONT_HERSHEY_PLAIN, 1.0,   cv::Scalar(0,0,255),     2);

	cout << "Aim Num = "<<result.AimNum<<endl;
	cout << "Little Num = "<<result.LittleNum<<endl;
	cout << "Large Num = "<<result.LargeNum<<endl;
	cout << "Count = "<<fcount<<endl;
	cout << "all = "<<contours.size()<<endl;

	ShowImage("result",rst2);

	//waitKey(0);
	return result;
}
