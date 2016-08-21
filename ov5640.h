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

#ifndef _OV5640_H_
#define _OV5640_H_

#ifdef __cplusplus
extern C {
#endif

#include <stdint.h>
#include "gpio_util.h"
#include "i2c_util.h"

typedef struct _ov5640 ov5640_t;

/* Callback for open tty device. */
typedef int32_t (* ov5640_release_vcm_callback)(ov5640_t * thiz);
typedef int32_t (* ov5640_af_ctrl_callback)(ov5640_t * thiz);

struct _ov5640 {
    gpio_t * gpio_client;
    i2c_t * i2c_client;

    /* Interfaces */
    ov5640_release_vcm_callback release_vcm_cb;
    ov5640_af_ctrl_callback af_ctrl_cb;
};

ov5640_t * ov5640_instance(void);
void ov5640_destory(ov5640_t * thiz);

#ifdef __cplusplus
}
#endif

#endif /* end of _OV5640_H_ */

