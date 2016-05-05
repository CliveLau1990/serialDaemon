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

#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#ifdef __cplusplus
extern C {
#endif

#include "base_frame.h"

#define RX_MAX_SIZE  11
#define TX_MAX_SIZE  32

typedef struct _receiver receiver_st;

/* Callback for read data */
typedef int (*receiver_read_callback)(receiver_st * r, char c);

/* Callback for write data */
typedef int (*receiver_write_callback)(receiver_st * r);

struct _receiver {
    BaseFrame_t    rx_base;
    BaseFrame_t    tx_base;
    int         fd;
    int         pos;
    char        in[RX_MAX_SIZE];

    /* Interfaces */
    receiver_read_callback    read_cb;
    receiver_write_callback   write_cb;
};

receiver_st * receiver_instance(void);
void receiver_destroy(receiver_st * r);

#ifdef __cplusplus
}
#endif

#endif /* end of _RECEIVER_H_ */

