#include "CalSpermCount.h"
#include "Histogram.h"

#include "devmem.h"

#include "calSperm.h"

CalSpermCount CalCount;

static inline int getRawRGB24(unsigned long addr, uint32_t width, uint32_t height, void *buf)
{
    unsigned long i, j;
    int buffer[height * width];
    char * pBuf = (char *)buf;

    devmem_readsl(addr, (void *)&buffer, height * width);

    for (i = j = 0; i < width * height; ++i) {
        pBuf[j++] = *((char *)buffer[i] + 3);
        pBuf[j++] = *((char *)buffer[i] + 2);
        pBuf[j++] = *((char *)buffer[i] + 1);
    }
}

extern "C" int calSperm(unsigned long addr, uint32_t width, uint32_t height, size_t frameCnt)
{
    char buffer[width * height * 3];

    getRawRGB24(addr, width, height, (void *)&buffer);

    Mat image(Size(width, height), CV_8UC3, buffer, width * 3);
    CalCount.ShowImage("srcImage", image);

    if (CalCount.readData(image)) {
        CalCount.img = CalCount.clipCenterImage(CalCount.img);
        CalCount.ShowImage("中心图像", CalCount.img);
    }

    if (CalCount.bGetImage) {
        CalCount.calNum(CalCount.img);
        CalCount.bGetImage = false;
    }

    return 0;
}
