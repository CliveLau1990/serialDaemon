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

#ifndef _UART_H_
#define _UART_H_

#ifdef __cplusplus
extern C {
#endif

#include <stdint.h>

#define NAME_MAX_SIZE 32

typedef struct _uart uart_st;

/* Callback for open tty device. */
typedef int (* uart_open_callback)(uart_st * u);
/* Callback for close tty device. */
typedef int (* uart_close_callback)(uart_st * u);

struct _uart {
    char    dev[NAME_MAX_SIZE];
    int     fd;

    /* Interfaces */
    uart_open_callback      open_cb;
    uart_close_callback     close_cb;
};

uart_st * uart_instance(char * dev);
void uart_destroy(uart_st * u);

#ifdef __cplusplus
}
#endif

#endif /* end of _UART_H_ */

