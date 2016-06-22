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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

#include "debug.h"

#include "uart_util.h"

/*
 * define the debug level of this file,
 * please see 'debug.h' for detail info
 */
DEBUG_SET_LEVEL(DEBUG_LEVEL_INFO);

static int uart_open(uart_st * u);
static int uart_close(uart_st * u);
static void uart_init(uart_st * u, char * dev);

static uart_st* pUart = NULL;
static struct termios bkup_ios;

static int uart_open(uart_st * u)
{
    DEBUG("called");

    if (-1 != u->fd) {
        uart_close(u);
    }

    do {
        u->fd = open(u->dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    } while (u->fd < 0 && errno == EINTR);

    if (u->fd < 0) {
        ERR("could not open serial device");
        return -1;
    }

    INFO("Read from %s, fd: %d", u->dev, u->fd);

    // disable echo on serial lines
    if (isatty(u->fd)) {
        struct termios  ios;
        tcgetattr(u->fd, &bkup_ios);
        bzero(&ios, sizeof(ios));
        ios.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
        ios.c_iflag = IGNPAR;
        ios.c_oflag = 0;
        ios.c_lflag = 0;
        tcflush(u->fd,TCIOFLUSH);
        tcsetattr(u->fd, TCSANOW, &ios);
    }

    return 0;
}

static int uart_close(uart_st * u)
{
    DEBUG("called");

    if (-1 != u->fd) {
        if (isatty(u->fd)) {
            tcsetattr(u->fd, TCSANOW, &bkup_ios);
        }
        close(u->fd);
        u->fd = -1;
    }

    return 0;
}

static void uart_init(uart_st * u, char * dev)
{
    DEBUG("called");

    memset(u, 0, sizeof(*u));

    if (strlen(dev) <= 0) {
        ERR("Invalid device name");
    }
    memcpy(u->dev, dev, strlen(dev));
    u->fd = -1;
    u->open_cb = uart_open;
    u->close_cb = uart_close;
}

uart_st * uart_instance(char * dev)
{
    DEBUG("called");

    if (pUart){
        INFO("Object already instance");
        return pUart;
    }

    pUart = (uart_st *)malloc(sizeof(*pUart));
    if (NULL == pUart) {
        ERR("Fail to alloc memory");
        return NULL;
    }

    uart_init(pUart, dev);

    return pUart;
}

void uart_destroy(uart_st * u)
{
    DEBUG("called");

    if (NULL == pUart) {
        INFO("Object already destroy");
        return;
    }

    free(pUart);
    pUart = NULL;
}

