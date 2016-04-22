#include <cv.h>
#include <highgui.h>

#include "CalSpermCount.h"
#include "Histogram.h"

CalSpermCount CalCount;

using namespace cv;

#define MAXLEN 921600
//#define MAXLEN 1000000

int read_file(const char * filename, unsigned char * dest, int maxlen)
{
    FILE *file;
    int pos, tmp;
    unsigned long i;

    file = fopen(filename, "r");
    if (NULL == file) {
        fprintf(stderr, "open %s error\n", filename);
        return -1;
    }

    pos = 0;
    for (i = 0; i < MAXLEN; i++) {
        tmp = fgetc(file);
        if (EOF == tmp) {
            break;
        }
        dest[pos++] = tmp;
    }

    fclose(file);

    dest[pos] = 0;

    return pos;
}

void initMat(Mat& m, char * num)
{
    for (int i=0; i < m.rows; i++) {
        for (int j=0; j < m.cols; j++) {
            m.at<Vec3b>(i,j)[0] = *(num + (i*m.rows+j) * 3 + 0);
            m.at<Vec3b>(i,j)[1] = *(num + (i*m.rows+j) * 3 + 1);
            m.at<Vec3b>(i,j)[2] = *(num + (i*m.rows+j) * 3 + 2);
        }
    }
}

int main(int argc, char* argv[])
{
    unsigned char buffer[MAXLEN];
    char * srcFile;
    char * secondFile;

    if (argc != 3) {
        printf("No image data\n");
        return -1;
    }

    srcFile = argv[1];
    secondFile = argv[2];

    int len = read_file(srcFile, buffer, MAXLEN);

    printf("len: %d\n", len);
#if 0
    CvSize size;
    size.width = 640;
    size.height = 480;
    cv::Mat image = cv::Mat(size, CV_8UC3);
//    cv::cvtColor(image, image, CV_BGR2RGB);

/*
    for (int i=0; i < 640; i++) {
        for (int j=0; j < 480; j++) {
            image.at<Vec3b>(i,j)[0] = buffer[(i*640+j) * 3 + 0];
            image.at<Vec3b>(i,j)[1] = buffer[(i*640+j) * 3 + 1];
            image.at<Vec3b>(i,j)[2] = buffer[(i*640+j) * 3 + 2];
        }
    }
*/


//    image = Mat(640, 480, CV_8UC3, buffer);

#endif

#if 0
    IplImage* cv_image = cvCreateImageHeader(cvSize(640,480), IPL_DEPTH_8U, 3);
    if (!cv_image) {
        printf("No cv_image\n");
    }
    cvSetData(cv_image, buffer, cv_image->widthStep);
    cvNamedWindow("win1", CV_WINDOW_AUTOSIZE);
    cvShowImage("win1", cv_image);

    Mat image2(cv_image, false);
    CalCount.ShowImage("srcFile原始图像", image2);

#else

    Mat image2(Size(640, 480), CV_8UC3, buffer, 640 * 3);
    CalCount.ShowImage("image2File原始图像", image2);
#endif

#if 0
    CalCount.ShowImage("secondFile原始图像", imread(secondFile, CV_8U));
#endif

    if (CalCount.readData(image2)) {
        CalCount.img = CalCount.clipCenterImage(CalCount.img);
        CalCount.ShowImage("中心图像", CalCount.img);
    }

    if (CalCount.bGetImage) {
        CalCount.calNum(CalCount.img);
        CalCount.bGetImage = false;
    }

    waitKey(0);
    return 0;
}
