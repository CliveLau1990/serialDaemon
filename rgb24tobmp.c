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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#include "bmp_util.h"

#include "devmem_util.h"

#include "rgb24tobmp.h"

/*
 * define the debug level of this file,
 * please see 'debug.h' for detail info
 */
DEBUG_SET_LEVEL(DEBUG_LEVEL_INFO);

int rgb24tobmp(unsigned long addr, char* outfile, uint32_t width, uint32_t height, uint32_t depth)
{
    bmpfile_t *bmp;
    int i, j;
    unsigned char red, green, blue; // 8-bits each
    unsigned int pixel; // 24-bits per pixel

    int buffer[height * width];
    memset(&buffer, 0, height * width * sizeof(int));
    devmem_readsl(addr, (void *)&buffer, height * width);

    if ((bmp = bmp_create(width, height, depth)) == NULL) {
        ERR("Invalid depth value: '%d'. Try 1, 4, 8, 16, 24, or 32.\n", depth);
        return -1;
    }

    for (i = 0; i < width; ++i) {
        for (j = 0; j < height; ++j) {

            pixel = buffer[width * j + i];

            red     = (unsigned char)((pixel & 0xFF000000) >> 24);
            blue    = (unsigned char)((pixel & 0x00FF0000) >> 16);
            green   = (unsigned char)((pixel & 0x0000FF00) >> 8);

            rgb_pixel_t bpixel = {blue, green, red, 0};
            bmp_set_pixel(bmp, i, j, bpixel);
        }
    }

    bmp_save(bmp, outfile);
    bmp_destroy(bmp);

    return 0;
}
