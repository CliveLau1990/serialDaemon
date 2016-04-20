/*
 * Copyright (C) 2016 CliveLiu
 * Subject to the GNU Public License, version 2.
 *
 * Created By:		Clive Liu<ftdstudio1990@gmail.com>
 * Created Date:	2016-03-07
 *
 * ChangeList:
 * Created in 2016-03-07 by Clive;
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/types.h>
#include <semaphore.h>
#include <asm-generic/ioctls.h>
#include <pthread.h>
#include <poll.h>

#include "debug.h"

#include "config.h"

#include "uart.h"

/*
 * define the debug level of this file,
 * please see 'debug.h' for detail info
 */
DEBUG_SET_LEVEL(DEBUG_LEVEL_DEBUG);

#define TX_MAX_SIZE 11

typedef struct _serialDaemon {
    pthread_t   rx_thread;
    uart_st     *uart;
} serialDaemon_st;
static serialDaemon_st stSerialDaemon;

/*
 * Debug array
 */
const char test_buf[][TX_MAX_SIZE] = {
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x01, 0x43, 0x4F, 0x4E, 0x54, 0x45}, // 握手
#if 0
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x00, 0x01, 0x00, 0x00, 0x34}, // 查询-MCU程序版本
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x00, 0x02, 0x00, 0x00, 0x35}, // 查询-FPGA程序版本
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x00, 0x03, 0x00, 0x00, 0x36}, // 查询-linux程序版本
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x00, 0x04, 0x00, 0x00, 0x37}, // 查询-硬件PCB版本
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x00, 0x07, 0x00, 0x00, 0x3A}, // 查询-传感器类型

{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x01, 0x00, 0x00, 0x00, 0x34}, // 查询-图像水平方向起始位置
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x01, 0x01, 0x00, 0x00, 0x35}, // 查询-图像水平方向长度
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x01, 0x02, 0x00, 0x00, 0x36}, // 查询-图像垂直方向起始位置
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x01, 0x03, 0x00, 0x00, 0x37}, // 查询-图像垂直方向长度
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x01, 0x04, 0x00, 0x00, 0x38}, // 查询-单次采样时间

{0xAA, 0x55, 0x08, 0x00, 0x10, 0x21, 0x01, 0x00, 0x00, 0x00, 0x32}, // 执行
#endif

{0xAA, 0x55, 0x08, 0x00, 0x10, 0x22, 0x01, 0x00, 0x00, 0x80, 0xB3}, // 设置-图像水平方向起始位置, 128
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x22, 0x01, 0x02, 0x00, 0x80, 0xB5}, // 设置-图像垂直方向起始位置, 128
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x22, 0x01, 0x01, 0x03, 0x20, 0x57}, // 设置-图像水平方向长度, 800
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x22, 0x01, 0x03, 0x02, 0x58, 0x90}, // 设置-图像垂直方向长度, 600
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x22, 0x01, 0x04, 0x05, 0xDC, 0x18}, // 设置-单次采样时间, 1500ms

{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x01, 0x00, 0x00, 0x00, 0x34}, // 查询-图像水平方向起始位置
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x01, 0x01, 0x00, 0x00, 0x35}, // 查询-图像水平方向长度
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x01, 0x02, 0x00, 0x00, 0x36}, // 查询-图像垂直方向起始位置
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x01, 0x03, 0x00, 0x00, 0x37}, // 查询-图像垂直方向长度
{0xAA, 0x55, 0x08, 0x00, 0x10, 0x23, 0x01, 0x04, 0x00, 0x00, 0x38}, // 查询-单次采样时间

{0xAA, 0x55, 0x08, 0x00, 0x10, 0x21, 0x01, 0x00, 0x00, 0x00, 0x32}, // 执行
};

static void print_tx_data(const char *buf, int len)
{
    int i;

    printf("==========\n" \
           "tx data:");
    for (i = 0; i < len; ++i) {
        printf(" 0x%x,", *((uint8_t *)(buf+i)));
    }
    printf("\n");
}

static void msg_tx_thread(void * arg)
{
    serialDaemon_st * service = (serialDaemon_st *)arg;
    uart_st *uart = service->uart;
    int ret = 0;
    int i, j;

    DEBUG("called");

    for (i = 0; i < (sizeof(test_buf)/TX_MAX_SIZE); i++) {

        print_tx_data(&test_buf[i], TX_MAX_SIZE);

        ret = write(uart->fd, &test_buf[i][0], TX_MAX_SIZE);
        if (ret < 0) {
            ERR("Error on MSG write :%s", strerror(errno));
        }
        sleep(2);
    }
}

static void msg_rx_thread(void * arg)
{
	serialDaemon_st * service = (serialDaemon_st *)arg;
	uart_st *uart = service->uart;

	DEBUG("called");

    // now loop
    for(;;) {
		char buf[128];
		int  nn, ret;
        struct timeval tv;

		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(uart->fd, &readfds);
		/* Wait up to 100 ms. */
		tv.tv_sec = 0;
		tv.tv_usec = 100;

		ret = select(uart->fd + 1, &readfds, NULL, NULL, &tv);

		if (FD_ISSET(uart->fd, &readfds)) {
			memset(buf,0,sizeof(buf));
			ret = read(uart->fd, buf, sizeof(buf));
			if (ret > 0) {
				for (nn = 0; nn < ret; nn++) {
					printf("rx data: 0x%x\n", (uint8_t)buf[nn]);
                }
			} else {
				ERR("Error on MSG read :%s", strerror(errno));
				continue;
			}
		}
	}

Exit:
	ERR("com service thread destroyed");
}

int main (int argc, char * argv[])
{
    int ret;

    DEBUG("called");
/*
    if (argc < 2) {
        ERR("input serial device");
        return -1;
    }
*/
    stSerialDaemon.uart = uart_instance(UART_DEV);
    if (NULL == stSerialDaemon.uart) {
        ERR("uart: fail to apply for instance");
        return -1;
    }

    stSerialDaemon.uart->open_cb(stSerialDaemon.uart);
    DEBUG("stSerialDaemon.fd:%d", stSerialDaemon.uart->fd);

    ret = pthread_create(&stSerialDaemon.rx_thread, NULL, (void*)msg_rx_thread, (void *)&stSerialDaemon);
    if (ret) {
        ERR("Fail to create pthread, %s", strerror(errno));
        return 0;
    }

    //pthread_join(com_service.rx_thread, NULL);

//    for(;;) {
        DEBUG("Main loop");
//        sleep(20);
        msg_tx_thread((void *)&stSerialDaemon);
//    }

    stSerialDaemon.uart->close_cb(stSerialDaemon.uart);

    DEBUG("end");
    return 0;
}
