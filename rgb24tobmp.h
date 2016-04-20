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

#ifndef _RGB24TOBMP_H_
#define _RGB24TOBMP_H_

#ifdef __cplusplus
extern C {
#endif

#include <stdint.h>

int rgb24tobmp(unsigned long addr, char* outfile, uint32_t width, uint32_t height, uint32_t depth);

#ifdef __cplusplus
}
#endif

#endif /* end of _RGB24TOBMP_H_ */

