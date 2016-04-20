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

#ifndef _REG_H_
#define _REG_H_

#ifdef __cplusplus
extern C {
#endif

#define APB_BASE_ADDR   0x43C00000
enum _REG_ADDR {
    REG_ADDR_X      = APB_BASE_ADDR + 0x0,
    REG_ADDR_Y      = APB_BASE_ADDR + 0x4,
    REG_ADDR_TMR    = APB_BASE_ADDR + 0x8,
    REG_ADDR_CTRL   = APB_BASE_ADDR + 0xC,
    REG_ADDR_DEST   = APB_BASE_ADDR + 0x10,
    REG_ADDR_SETUP  = APB_BASE_ADDR + 0x14,
    REG_ADDR_SAMPLE = APB_BASE_ADDR + 0x18,
    REG_ADDR_VER    = APB_BASE_ADDR + 0x1C,
};

#ifdef __cplusplus
}
#endif

#endif /* end of _REG_H_ */

