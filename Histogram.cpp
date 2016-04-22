#include "Histogram.h"

Histgram1D::Histgram1D()
{
	histSize[0] = 256;
	hranges[0] = 0.0;
	hranges[1] = 256.0;
	ranges[0] = hranges;
	channels[0] = 0;
}
cv::Mat Histgram1D::getHistogram(const cv::Mat &image)
{
    cv:Mat hist;
	calcHist(&image,1,channels,cv::Mat(),hist,1,histSize,ranges);
	return hist;
}

cv::Mat Histgram1D::getHistogramImage(const cv::Mat &image, int zoom )
{
    cv:Mat hist = getHistogram(image);
	return getImageOfHistogram(hist, zoom);
}

cv::Mat Histgram1D::getImageOfHistogram(const cv::Mat &hist, int zoom)
{
	double maxVal= 0;
	double minVal= 0;
	cv::minMaxLoc(hist, &minVal,&maxVal,0 ,0);
	int histSize = hist.rows;
	Mat histImg(histSize *zoom, histSize *zoom,CV_8U, cv::Scalar(255));
	int hpt = static_cast<int>(0.9*histSize);
	for(int h = 0 ;h<histSize;h++)
	{
		float binVal = hist.at<float>(h);
		if(binVal>0)
		{
			int intensity = static_cast<int>(binVal * hpt /maxVal);
			cv::line(histImg,cv::Point(h*zoom, histSize*zoom),	cv::Point(h*zoom, (histSize - intensity)*zoom), cv::Scalar(0), zoom);
		}
	}
	return histImg;
}
