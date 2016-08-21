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

#ifndef _I2C_H_
#define _I2C_H_

#ifdef __cplusplus
extern C {
#endif

#include <stdint.h>

#define NAME_MAX_SIZE 32

typedef struct _i2c i2c_t;

/* Callback for open i2c device. */
typedef int32_t (* i2c_open_callback)(i2c_t * thiz);
/* Callback for close i2c device. */
typedef int32_t (* i2c_close_callback)(i2c_t * thiz, int32_t dev_fd);
/* Callback for read i2c device. */
typedef int32_t (* i2c_read_callback)(i2c_t * thiz, uint16_t reg_addr, uint8_t * buf, size_t len);
/* Callback for write i2c device. */
typedef int32_t (* i2c_write_callback)(i2c_t * thiz, uint16_t reg_addr, uint8_t * buf, size_t len);

struct _i2c {
    int8_t      dev[NAME_MAX_SIZE];
    uint8_t     slave_addr;

    /* Interfaces */
    i2c_open_callback      open_cb;
    i2c_close_callback     close_cb;
    i2c_read_callback      read_cb;
    i2c_write_callback     write_cb;
};

i2c_t * i2c_instance(int8_t * dev, uint8_t slave_addr);
void i2c_destroy(i2c_t ** thiz);

#ifdef __cplusplus
}
#endif

#endif /* end of _I2C_H_ */

