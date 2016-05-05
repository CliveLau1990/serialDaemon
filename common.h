/*
 * Copyright (C) 2016 CliveLiu
 * Subject to the GNU Public License, version 2.
 *
 * Created By:		Clive Liu<ftdstudio1990@gmail.com>
 * Created Date:	2016-03-07
 *
 * ChangeList:
 * Created in 2016-05-04 by Clive;
 */
#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>

typedef struct {
    uint16_t u16sn;
    uint16_t u16count;
    uint16_t u16motility;
    uint16_t u16Rsv1;
    uint16_t u16Rsv2;
    uint16_t u16Rsv3;
    uint16_t u16Rsv4;
    uint16_t u16Rsv5;
} Result_cal;

#endif /* end of _COMMON_H_ */
