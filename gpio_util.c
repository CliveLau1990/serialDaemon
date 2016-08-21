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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "debug.h"

#include "gpio_util.h"

/*
 * define the debug level of this file,
 * please see 'debug.h' for detail info
 */
DEBUG_SET_LEVEL(DEBUG_LEVEL_DEBUG);

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define BUF_MAX_SIZE 64

static int32_t gpio_export(gpio_t * thiz)
{
    int32_t fd, len;
    int8_t buf[BUF_MAX_SIZE];

    DEBUG("called");

    fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
    if (fd < 0) {
        ERR("%s", strerror(errno));
        return -ENODEV;
    }

    len = snprintf(buf, sizeof(buf), "%d", thiz->gpio);
    write(fd, buf, len);

    close(fd);

    return 0;
}

static int32_t gpio_unexport(gpio_t * thiz)
{
    int32_t fd, len;
    int8_t buf[BUF_MAX_SIZE];

    DEBUG("called");

    fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
    if (fd < 0) {
        ERR("%s", strerror(errno));
        return -ENODEV;
    }

    len = snprintf(buf, sizeof(buf), "%d", thiz->gpio);
    write(fd, buf, len);

    close(fd);

    return 0;
}

static int32_t gpio_set_dir(gpio_t * thiz)
{
    int32_t fd;
    int8_t buf[BUF_MAX_SIZE];

    DEBUG("called");

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", thiz->gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        ERR("%s", strerror(errno));
        return -ENODEV;
    }

    thiz->direction ? write(fd, "out", 4) : write(fd, "in", 3);

    close(fd);

    return 0;
}

static int32_t gpio_set_edge(gpio_t * thiz)
{
    int32_t fd;
    int8_t buf[BUF_MAX_SIZE];

    DEBUG("called");

    if (NULL == thiz->edge) {
        return -EINVAL;
    }

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", thiz->gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        ERR("%s", strerror(errno));
        return -ENODEV;
    }

    write(fd, thiz->edge, strlen(thiz->edge) + 1);

    close(fd);

    return 0;
}

static int32_t gpio_set_value(gpio_t * thiz, int32_t val)
{
    int32_t fd;
    int8_t buf[BUF_MAX_SIZE];

    DEBUG("called");

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", thiz->gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        ERR("%s", strerror(errno));
        return -ENODEV;
    }

    val ? write(fd, "1", 2) : write(fd, "0", 2);

    close(fd);

    return 0;
}

static int32_t gpio_get_value(gpio_t * thiz, int32_t * val)
{
    int32_t fd;
    int8_t buf[BUF_MAX_SIZE];
    int8_t ch;

    DEBUG("called");

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", thiz->gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        ERR("%s", strerror(errno));
        return -ENODEV;
    }

    read(fd, &ch, 1);

    *val = (ch != '0') ? 1 : 0;

    close(fd);

    return 0;
}

static void gpio_init(gpio_t * thiz, uint32_t gpio, uint32_t direction, int8_t * edge)
{
    DEBUG("called");

    memset(thiz, 0, sizeof(*thiz));

    thiz->gpio = gpio;
    thiz->direction = direction;
    if (NULL != edge) {
        memcpy(thiz->edge, edge, strlen(edge));
    }

    thiz->set_dir_cb = gpio_set_dir;
    thiz->set_edge_cb = gpio_set_edge;
    thiz->set_value_cb = gpio_set_value;
    thiz->get_value_cb = gpio_get_value;
}

gpio_t * gpio_instance(uint32_t gpio, uint32_t direction, int8_t * edge, int32_t def_val)
{
    DEBUG("called");

    gpio_t * thiz = NULL;

    thiz = (gpio_t *)malloc(sizeof(*thiz));
    if (NULL == thiz) {
        ERR("Fail to alloc memory");
        return NULL;
    }

    gpio_init(thiz, gpio, direction, edge);

    gpio_export(thiz);

    thiz->set_dir_cb(thiz);
    thiz->set_edge_cb(thiz);
    thiz->set_value_cb(thiz, def_val);

    return thiz;
}

void gpio_destroy(gpio_t ** thiz)
{
    DEBUG("called");

    if (NULL == *thiz) {
        INFO("Object already destroy");
        return;
    }

    gpio_unexport(*thiz);

    free(*thiz);
    *thiz = NULL;
}

