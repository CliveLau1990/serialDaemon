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

static int i2c_open(i2c_st * thiz);
static int i2c_close(i2c_st * thiz);
static void i2c_init(i2c_st * thiz, char * dev);
static int i2c_read(i2c_st * thiz, unsigned reg_addr, unsigned char * buf, size_t len);
static int i2c_write(i2c_st * thiz, unsigned reg_addr, unsigned char * buf, size_t len);

static int i2c_dev_fd = -1;
static i2c_st * pObj = NULL;

static int i2c_open(i2c_st * thiz)
{
    DEBUG("called");

    if (-1 != i2c_dev_fd) {
        goto out;
    }

    do {
        i2c_dev_fd = open(thiz->dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    } while (i2c_dev_fd < 0 && errno == EINTR);

    if (i2c_dev_fd < 0) {
        ERR("could not open i2c device");
        return -1;
    }

    INFO("Read from %s, fd: %d", thiz->dev, i2c_dev_fd);

out:
    return i2c_dev_fd;
}

static int i2c_close(i2c_st * thiz)
{
    DEBUG("called");

    if (-1 != i2c_dev_fd) {
        close(i2c_dev_fd);
        i2c_dev_fd = -1;
    }

    return 0;
}

static int i2c_get_fd(i2c_st * thiz)
{
    DEBUG("called");

    return i2c_dev_fd;
}

static int i2c_read(i2c_st * thiz, unsigned int reg_addr, unsigned char * buf, size_t len)
{
    struct i2c_rdwr_ioctl_data args;
    unsigned char reg_buf[2];
    int ret;

    if (i2c_dev_fd < 0) {
        ERR("Invalid i2c_dev_fd:%d", i2c_dev_fd);
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

    ioctl(i2c_dev_fd, I2C_TIMEOUT, 2);
    ioctl(i2c_dev_fd, I2C_RETRIES, 1);

    (args.msgs[0]).len = 2;
    (args.msgs[0]).addr = thiz->slave_addr;
    (args.msgs[0]).buf = reg_buf;

    (args.msgs[1]).len = len;
    (args.msgs[1]).flags = I2C_M_RD;
    (args.msgs[1]).addr = thiz->slave_addr;
    (args.msgs[1]).buf = buf;

    ret = ioctl(i2c_dev_fd, I2C_RDWR, (unsigned long)&args);
    if (ret < 0) {
        ERR("During I2C_RDWR ioctl with error code: %d", ret);
        len = 0;
    } else {
        INFO("read salve:%02x reg:%04x\n", thiz->slave_addr, reg_addr);
    }

    free(args.msgs);

    return len;
}

static int i2c_write(i2c_st * thiz, unsigned int reg_addr, unsigned char * val, size_t len)
{
    struct i2c_rdwr_ioctl_data args;
    unsigned char buf[len + 2];
    int ret;

    if (i2c_dev_fd < 0) {
        ERR("Invalid i2c_dev_fd:%d", i2c_dev_fd);
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

    ioctl(i2c_dev_fd, I2C_TIMEOUT, 2);
    ioctl(i2c_dev_fd, I2C_RETRIES, 1);

    (args.msgs[0]).len = len + 2;
    (args.msgs[0]).addr = thiz->slave_addr;
    (args.msgs[0]).buf = buf;

    memcpy(buf + 2, val, len);

    ret = ioctl(i2c_dev_fd, I2C_RDWR, (unsigned long)&args);
    if (ret < 0) {
        ERR("During I2C_RDWR ioctl with error code: %d", ret);
        len = 0;
    } else {
        INFO("write salve:%02x reg:%02x\n", thiz->slave_addr, reg_addr);
        return len;
    }

    free(args.msgs);

    return len;
}

static void i2c_init(i2c_st * thiz, char * dev)
{
    DEBUG("called");

    memset(thiz, 0, sizeof(*thiz));

    if (strlen(dev) <= 0) {
        ERR("Invalid device name");
    }
    memcpy(thiz->dev, dev, strlen(dev));

    i2c_dev_fd = -1;

    thiz->get_fd_cb = i2c_get_fd;
    thiz->read_cb = i2c_read;
    thiz->write_cb = i2c_write;
}

i2c_st * i2c_instance(char * dev)
{
    DEBUG("called");

    if (pObj){
        INFO("Object already instance");
        return pObj;
    }

    pObj = (i2c_st *)malloc(sizeof(*pObj));
    if (NULL == pObj) {
        ERR("Fail to alloc memory");
        return NULL;
    }

    i2c_init(pObj, dev);

    i2c_open(pObj);

    return pObj;
}

void i2c_destroy(i2c_st * thiz)
{
    DEBUG("called");

    if (NULL == pObj) {
        INFO("Object already destroy");
        return;
    }

    i2c_close(thiz);

    free(pObj);
    pObj = NULL;
}

