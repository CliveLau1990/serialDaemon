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

#ifndef _GPIO_UTIL_H_
#define _GPIO_UTIL_H_

#ifdef __cplusplus
extern C {
#endif

#include <stdint.h>

typedef struct _gpio gpio_t;

/* Callback for setting the direction of gpio. */
typedef int32_t (* gpio_set_dir_callback)(gpio_t * thiz);
/* Callback for setting the edge of gpio. */
typedef int32_t (* gpio_set_edge_callback)(gpio_t * thiz);
/* Callback for setting the value of gpio. */
typedef int32_t (* gpio_set_value_callback)(gpio_t * thiz, int32_t val);
/* Callback for getting the value of gpio. */
typedef int32_t (* gpio_get_value_callback)(gpio_t * thiz, int32_t * val);

struct _gpio {
    int8_t      edge[8];
    uint32_t    gpio;
    uint32_t    direction;

    /* Interfaces */
    gpio_set_dir_callback    set_dir_cb;
    gpio_set_edge_callback   set_edge_cb;
    gpio_set_value_callback  set_value_cb;
    gpio_get_value_callback  get_value_cb;
};

gpio_t * gpio_instance(uint32_t gpio, uint32_t direction, int8_t * edge, int32_t def_val);
void gpio_destroy(gpio_t ** thiz);

#ifdef __cplusplus
}
#endif

#endif /* end of _GPIO_UTIL_H_ */

