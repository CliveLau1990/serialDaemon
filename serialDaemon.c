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

#include "uart_util.h"

#include "receiver.h"

/*
 * define the debug level of this file,
 * please see 'debug.h' for detail info
 */
DEBUG_SET_LEVEL(DEBUG_LEVEL_DEBUG);

typedef struct _serialDaemon {
    pthread_t       rx_thread;
    uart_st *       uart;
    receiver_st *   receiver;
} serialDaemon_st;
static serialDaemon_st stSerialDaemon;

static void msg_rx_thread(void * arg)
{
	serialDaemon_st * service = (serialDaemon_st *)arg;
	uart_st *uart = service->uart;
	receiver_st  *receiver = service->receiver;

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
					receiver->read_cb(receiver, buf[nn]);
                }
			} else {
				ERR("Error on MSG read :%s", strerror(errno));
				continue;
			}
		}
	}

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
    //daemon(0, 0);

    stSerialDaemon.uart = uart_instance("/dev/ttyPS0");
    if (NULL == stSerialDaemon.uart) {
        ERR("uart: fail to apply for instance");
        return -1;
    }

    stSerialDaemon.receiver = receiver_instance();
    if (NULL == stSerialDaemon.receiver) {
        ERR("receiver: fail to apply for instance");
        return -1;
    }

    stSerialDaemon.uart->open_cb(stSerialDaemon.uart);
    stSerialDaemon.receiver->fd = stSerialDaemon.uart->fd;
    DEBUG("stSerialDaemon.fd:%d", stSerialDaemon.uart->fd);

    ret = pthread_create(&stSerialDaemon.rx_thread, NULL, (void*)msg_rx_thread, (void *)&stSerialDaemon);
    if (ret) {
        ERR("Fail to create pthread, %s", strerror(errno));
        return 0;
    }

    //pthread_join(com_service.rx_thread, NULL);

    for(;;) {
        DEBUG("Main loop");

        sleep(1000);
    }

    stSerialDaemon.uart->close_cb(stSerialDaemon.uart);

    DEBUG("end");
    return 0;
}
