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

typedef struct _i2c i2c_st;

/* Callback for get i2c device fd */
typedef int (* i2c_get_fd_callback)(i2c_st * thiz);
/* Callback for read i2c device. */
typedef int (* i2c_read_callback)(i2c_st * thiz, unsigned reg_addr, unsigned char * buf, size_t len);
/* Callback for write i2c device. */
typedef int (* i2c_write_callback)(i2c_st * thiz, unsigned reg_addr, unsigned char * buf, size_t len);

struct _i2c {
    char    dev[NAME_MAX_SIZE];
    int     fd;
    unsigned int slave_addr;

    /* Interfaces */
    i2c_get_fd_callback    get_fd_cb;
    i2c_read_callback      read_cb;
    i2c_write_callback     write_cb;
};

i2c_st * i2c_instance(char * dev);
void i2c_destroy(i2c_st * thiz);

#ifdef __cplusplus
}
#endif

#endif /* end of _I2C_H_ */

