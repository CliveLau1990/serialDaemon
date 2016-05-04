#ifndef _CALSPERM_H_
#define _CALSPERM_H_

#include <stddef.h>

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

int calSperm(unsigned long addr, uint32_t width, uint32_t height, size_t frameCnt, Result_cal* pResult_cal);

#ifdef __cplusplus
}
#endif

#endif /* end of _CALSPERM_H_ */
