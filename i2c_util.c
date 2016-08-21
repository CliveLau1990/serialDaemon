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

#include <stdio.h>
#include <linux/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "debug.h"

#include "i2c_util.h"

/*
 * define the debug level of this file,
 * please see 'debug.h' for detail info
 */
DEBUG_SET_LEVEL(DEBUG_LEVEL_INFO);

#define I2C_RW_RETRIES 1

static int i2c_open(i2c_t * thiz);
static int i2c_close(i2c_t * thiz, int dev_fd);
static void i2c_init(i2c_t * thiz, int8_t * dev, uint8_t slave_addr);
static int32_t i2c_read(i2c_t * thiz, uint16_t reg_addr, uint8_t * buf, size_t len);
static int32_t i2c_write(i2c_t * thiz, uint16_t reg_addr, uint8_t * val, size_t len);

static int i2c_open(i2c_t * thiz)
{
    DEBUG("called");

    int32_t dev_fd = -1;

    dev_fd = open(thiz->dev, O_RDWR | O_NONBLOCK);
    if (dev_fd < 0) {
        ERR("Could not open i2c device");
        return -1;
    }

    //INFO("Read from %s, fd: %d", thiz->dev, dev_fd);

    ioctl(dev_fd, I2C_SLAVE_FORCE, thiz->slave_addr);
    ioctl(dev_fd, I2C_TIMEOUT, 2);
    ioctl(dev_fd, I2C_RETRIES, 1);

    return dev_fd;
}

static int i2c_close(i2c_t * thiz, int32_t dev_fd)
{
    DEBUG("called");

    if (-1 != dev_fd) {
        close(dev_fd);
    }

    return 0;
}

static int32_t i2c_read(i2c_t * thiz, uint16_t reg_addr, uint8_t * buf, size_t len)
{
    struct i2c_rdwr_ioctl_data args;
    uint8_t reg_buf[2];
    int32_t dev_fd;
    int32_t retval, retries;

    dev_fd = i2c_open(thiz);
    if (dev_fd < 0) {
        ERR("Invalid dev_fd:%d", dev_fd);
        return -ENODEV;
    }

    reg_buf[0] = (reg_addr >> 8) & 0xFF;
    reg_buf[1] = reg_addr & 0xFF;

    args.nmsgs = 2;
    args.msgs = (struct i2c_msg *)malloc(args.nmsgs * sizeof(struct i2c_msg));
    if (NULL == args.msgs) {
        ERR("Fail to alloc memory");
        return -ENOMEM;
    }

    (args.msgs[0]).len = 2;
    (args.msgs[0]).addr = thiz->slave_addr;
    (args.msgs[0]).buf = reg_buf;

    (args.msgs[1]).len = len;
    (args.msgs[1]).flags = I2C_M_RD;
    (args.msgs[1]).addr = thiz->slave_addr;
    (args.msgs[1]).buf = buf;

    for (retries = I2C_RW_RETRIES; retries != 0; --retries) {
        retval = ioctl(dev_fd, I2C_RDWR, (unsigned long)&args);
        if (retval < 0) {
            ERR("During I2C_RDWR ioctl with error code: %d, slave:%02x, reg:%04x, retries:%d",
                retval, thiz->slave_addr, reg_addr, retries);
            continue;
        }
        break;
    }

    free(args.msgs);

    i2c_close(thiz, dev_fd);

    return retval;
}

static int32_t i2c_write(i2c_t * thiz, uint16_t reg_addr, uint8_t * val, size_t len)
{
    struct i2c_rdwr_ioctl_data args;
    uint8_t buf[len + 2];
    int32_t dev_fd;
    int32_t retval, retries;

    dev_fd = i2c_open(thiz);
    if (dev_fd < 0) {
        ERR("Invalid dev_fd:%d", dev_fd);
        return -ENODEV;
    }

    buf[0] = (reg_addr >> 8) & 0xFF;
    buf[1] = reg_addr & 0xFF;

    args.nmsgs = 1;
    args.msgs = (struct i2c_msg *)malloc(args.nmsgs * sizeof(struct i2c_msg));
    if (NULL == args.msgs) {
        ERR("Fail to alloc memory");
        return -ENOMEM;
    }

    (args.msgs[0]).len = len + 2;
    (args.msgs[0]).addr = thiz->slave_addr;
    (args.msgs[0]).buf = buf;

    memcpy(buf + 2, val, len);

    for (retries = I2C_RW_RETRIES; retries != 0; --retries) {
        retval = ioctl(dev_fd, I2C_RDWR, (unsigned long)&args);
        if (retval < 0) {
            ERR("During I2C_RDWR ioctl with error code: %d, slave:%02x, reg:%04x, retries:%d",
                retval, thiz->slave_addr, reg_addr, retries);
            continue;
        }
        break;
    }

    free(args.msgs);

    i2c_close(thiz, dev_fd);

    return retval;
}

static void i2c_init(i2c_t * thiz, int8_t * dev, uint8_t slave_addr)
{
    DEBUG("called");

    memset(thiz, 0, sizeof(*thiz));

    if (strlen(dev) <= 0) {
        ERR("Invalid device name");
    }
    memcpy(thiz->dev, dev, strlen(dev));
    thiz->slave_addr = slave_addr;
    thiz->open_cb = i2c_open;
    thiz->close_cb = i2c_close;
    thiz->read_cb = i2c_read;
    thiz->write_cb = i2c_write;
}

i2c_t * i2c_instance(int8_t * dev, uint8_t slave_addr)
{
    DEBUG("called");

    i2c_t * thiz = NULL;

    thiz = (i2c_t *)malloc(sizeof(*thiz));
    if (NULL == thiz) {
        ERR("Fail to alloc memory");
        return NULL;
    }

    i2c_init(thiz, dev, slave_addr);

    return thiz;
}

void i2c_destroy(i2c_t ** thiz)
{
    DEBUG("called");

    if (NULL == *thiz) {
        INFO("Object already destroy");
        return;
    }

    free(*thiz);
    *thiz = NULL;
}

