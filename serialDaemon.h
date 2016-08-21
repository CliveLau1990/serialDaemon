/*
 * Copyright (C) 2016 CliveLiu
 * Subject to the GNU Public License, version 2.
 *
 * Created By:		Clive Liu<ftdstudio1990@gmail.com>
 * Created Date:	2016-03-07
 *
 * ChangeList:
 * Created in 2016-08-19 by Clive;
 */

#ifndef _SERIALDAEMON_H_
#define _SERIALDAEMON_H_

#ifdef __cplusplus
extern C {
#endif

#include <stdint.h>
#include <pthread.h>
#include "ov5640.h"
#include "receiver.h"
#include "uart_util.h"

typedef struct _serialDaemon {
    pthread_t       rx_thread;
    ov5640_t *      ov5640;
    uart_st *       uart;
    receiver_st *   receiver;
} serialDaemon_t;

#ifdef __cplusplus
}
#endif

#endif /* end of _SERIALDAEMON_H_ */

