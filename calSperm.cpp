#include "CalSpermCount.h"
#include "Histogram.h"

#include "devmem.h"

#include "calSperm.h"

// 获取图像帧对应的起始地址
#define FRAME2ADDR(addr,width,height,frame) \
    ((unsigned long)((unsigned long *)addr + width * height * frame))

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

extern "C" int calSperm(unsigned long addr, uint32_t width, uint32_t height, size_t frameCnt, Result_cal* pResult_cal)
{
    char buffer[width * height * 3];
    Result st_result = {
        .LittleNum = 0,
        .AimNum = 0,
        .LargeNum = 0,
    };

    getRawRGB24(FRAME2ADDR(addr,width,height,0), width, height, (void *)&buffer);

    Mat image(Size(width, height), CV_8UC3, buffer, width * 3);
    CalCount.ShowImage("srcImage", image);

    if (CalCount.readData(image)) {
        CalCount.img = CalCount.clipCenterImage(CalCount.img);
        CalCount.ShowImage("中心图像", CalCount.img);
    }

    if (CalCount.bGetImage) {
        st_result = CalCount.calNum(CalCount.img);
        CalCount.bGetImage = false;
    }

    pResult_cal->u16count = st_result.LargeNum;

    return 0;
}
