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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "debug.h"

#include "reg.h"

#include "rgb24tobmp.h"

#include "receiver.h"

/*
 * define the debug level of this file,
 * please see 'debug.h' for detail info
 */
DEBUG_SET_LEVEL(DEBUG_LEVEL_DEBUG);

typedef void (*receiver_cmd_callback)(receiver_st * r);

static receiver_st * pReceiver = NULL;
static bool isConnected = false;

static inline uint8_t _get_checksums(receiver_st* r)
{
    int i;

    r->tx_base.ucChecksums = 0;
    r->tx_base.ucChecksums += r->tx_base.aId[0] + r->tx_base.aId[1];
    r->tx_base.ucChecksums += r->tx_base.cCmd;
    for (i = 0; i < 4; ++i) {
        r->tx_base.ucChecksums += r->tx_base.aParm[i];
    }
}

/*****************************************************************/
/*****************************************************************/
/*****                                                       *****/
/*****           CMD   CONNECT                               *****/
/*****                                                       *****/
/*****************************************************************/
/*****************************************************************/
static void receiver_connect(receiver_st* r)
{
    DEBUG("called");

    memcpy(&r->tx_base, &r->rx_base, r->rx_base.ucLen + 1);

    r->write_cb(r);
}

/*****************************************************************/
/*****************************************************************/
/*****                                                       *****/
/*****           CMD   PERFORM                               *****/
/*****                                                       *****/
/*****************************************************************/
/*****************************************************************/
static void receiver_perform(receiver_st* r)
{
//#define ADDR_BASE (~(0x3uL))
#define MAX_DIR_CNT 3

    DEBUG("called");

    static uint32_t dirCnt = 0;

    char dirPath[255], outfile[255], tmpCmd[255];
    uint32_t reg;
    unsigned long addr;
    uint32_t width, height;
    uint32_t i;

    // reset DMA controller
    devmem_set32(REG_ADDR_CTRL, 0x1, 1);
    usleep(1);
    devmem_set32(REG_ADDR_CTRL, 0x0, 1);

    // start to capture picture
    devmem_set32(REG_ADDR_CTRL, 0x2, 1);

    devmem_readsl(REG_ADDR_TMR, (void *)&reg, 1);
    DEBUG("0x43C00008 reg:%u", reg);
    usleep((reg+200) * 1000);

    //sleep(1);

    devmem_readsl(REG_ADDR_X, (void *)&reg, 1);
    width = reg & 0x0000FFFF;
    DEBUG("0x43C00000 width:%u", width);

    devmem_readsl(REG_ADDR_Y, (void *)&reg, 1);
    height = reg & 0x0000FFFF;
    DEBUG("0x43C00004 height:%u", height);

    devmem_readsl(REG_ADDR_SAMPLE, (void *)&reg, 1);
    DEBUG("0x43C00018 reg:%u", reg);

    // create directory
    sprintf(dirPath, "/mnt/bmp%d", dirCnt);
    if (!access(dirPath, F_OK)) {
        sprintf(tmpCmd, "rm -rf %s", dirPath);
        system(tmpCmd);
        sync();
    }
    mkdir(dirPath, S_IRWXU | S_IRWXG | S_IRWXO);
    DEBUG("DIR:%s", dirPath);

    //printf("Generating bmp files...\n");
    // generate bmp files
    for (i = 0; i < reg ; ++i) {
        putchar('.');
        fflush(stdout);
        sprintf(outfile, "/mnt/bmp%d/file%d.bmp", dirCnt, i);
        rgb24tobmp((unsigned long)(((unsigned long *)0x18000000) + width*height*i), outfile, width, height, 24);
    }

    dirCnt = (dirCnt + 1) % MAX_DIR_CNT;

    sync();

    puts("");

    //sprintf(tmpCmd, "calSperm %s", dirPath);
    //system(tmpCmd);

    DEBUG("end");
}

/*****************************************************************/
/*****************************************************************/
/*****                                                       *****/
/*****           CMD   GET   LIST                            *****/
/*****                                                       *****/
/*****************************************************************/
/*****************************************************************/
static void _get_mcu_version(receiver_st* r)
{
    #define MCU_VERSION 0x0001 // 每4位表示一节，HH,HL,LH,LL

    DEBUG("called");

    memcpy(&r->tx_base, &r->rx_base, r->rx_base.ucLen + 1 - 3);

    r->tx_base.aParm[2] = MCU_VERSION >> 8;
    r->tx_base.aParm[3] = MCU_VERSION & 0xFF;

    _get_checksums(r);

    r->write_cb(r);
}

static void _get_fpga_version(receiver_st* r)
{
    int reg;

    DEBUG("called");

    memcpy(&r->tx_base, &r->rx_base, r->rx_base.ucLen + 1 - 3);

    devmem_readsl(REG_ADDR_VER, (void *)&reg, 1);
    r->tx_base.aParm[2] = reg >> 8;
    r->tx_base.aParm[3] = reg & 0xFF;

    _get_checksums(r);

    r->write_cb(r);
}

static void _get_linux_version(receiver_st* r)
{
    #define LINUX_VERSION 0x0318 // 每4位表示一节，HH,HL,LH,LL

    DEBUG("called");

    memcpy(&r->tx_base, &r->rx_base, r->rx_base.ucLen + 1 - 3);

    r->tx_base.aParm[2] = LINUX_VERSION >> 8;
    r->tx_base.aParm[3] = LINUX_VERSION & 0xFF;

    _get_checksums(r);

    r->write_cb(r);
}

static void _get_hw_version(receiver_st* r)
{
    #define HW_VERSION 0x0001 // 每4位表示一节，HH,HL,LH,LL

    DEBUG("called");

    memcpy(&r->tx_base, &r->rx_base, r->rx_base.ucLen + 1 - 3);

    r->tx_base.aParm[2] = HW_VERSION >> 8;
    r->tx_base.aParm[3] = HW_VERSION & 0xFF;

    _get_checksums(r);

    r->write_cb(r);
}

static void _get_sensor_type(receiver_st* r)
{
    DEBUG("called");

    memcpy(&r->tx_base, &r->rx_base, r->rx_base.ucLen + 1 - 3);

    r->tx_base.aParm[2] = 0x0;
    // type: ov5640
    r->tx_base.aParm[3] = 0x1;

    _get_checksums(r);

    r->write_cb(r);
}

static void _get_pic_x_start(receiver_st* r)
{
    int reg;

    DEBUG("called");

    memcpy(&r->tx_base, &r->rx_base, r->rx_base.ucLen + 1 - 3);

    devmem_readsl(REG_ADDR_X, (void *)&reg, 1);
    r->tx_base.aParm[2] = (reg & 0xFF000000) >> 24;
    r->tx_base.aParm[3] = (reg & 0x00FF0000) >> 16;

    _get_checksums(r);

    r->write_cb(r);
}

static void _get_pic_width(receiver_st* r)
{
    int reg;

    DEBUG("called");

    memcpy(&r->tx_base, &r->rx_base, r->rx_base.ucLen + 1 - 3);

    devmem_readsl(REG_ADDR_X, (void *)&reg, 1);
    r->tx_base.aParm[2] = (reg & 0xFF00) >> 8;
    r->tx_base.aParm[3] = reg & 0x00FF;

    _get_checksums(r);

    r->write_cb(r);
}

static void _get_pic_y_start(receiver_st* r)
{
    int reg;

    DEBUG("called");

    memcpy(&r->tx_base, &r->rx_base, r->rx_base.ucLen + 1 - 3);

    devmem_readsl(REG_ADDR_Y, (void *)&reg, 1);
    r->tx_base.aParm[2] = (reg & 0xFF000000) >> 24;
    r->tx_base.aParm[3] = (reg & 0x00FF0000) >> 16;

    _get_checksums(r);

    r->write_cb(r);
}

static void _get_pic_height(receiver_st* r)
{
    int reg;

    DEBUG("called");

    memcpy(&r->tx_base, &r->rx_base, r->rx_base.ucLen + 1 - 3);

    devmem_readsl(REG_ADDR_Y, (void *)&reg, 1);
    r->tx_base.aParm[2] = (reg & 0xFF00) >> 8;
    r->tx_base.aParm[3] = reg & 0x00FF;

    _get_checksums(r);

    r->write_cb(r);
}

static void _get_sample_time(receiver_st* r)
{
    int reg;

    DEBUG("called");

    memcpy(&r->tx_base, &r->rx_base, r->rx_base.ucLen + 1 - 3);

    devmem_readsl(REG_ADDR_TMR, (void *)&reg, 1);
    r->tx_base.aParm[2] = (reg & 0xFF00) >> 8;
    r->tx_base.aParm[3] = reg & 0x00FF;

    _get_checksums(r);

    r->write_cb(r);
}

receiver_cmd_callback receiver_cmd_get_cb[5][16] = {
{
NULL,
_get_mcu_version,
_get_fpga_version,
_get_linux_version,
_get_hw_version,
NULL,
NULL,
_get_sensor_type
},
{
_get_pic_x_start,
_get_pic_width,
_get_pic_y_start,
_get_pic_height,
_get_sample_time
},
{NULL},
{NULL},
{NULL},
};

/*****************************************************************/
/*****************************************************************/
/*****                                                       *****/
/*****           CMD   SET   LIST                            *****/
/*****                                                       *****/
/*****************************************************************/
/*****************************************************************/
static void _set_pic_x_start(receiver_st* r)
{
    DEBUG("called");

    uint32_t reg = 0;
    uint32_t tmp = 0;

    tmp = (uint8_t)r->rx_base.aParm[2] << 8;
    tmp |= (uint8_t)r->rx_base.aParm[3];

    if (tmp > 4096U) {
        tmp = 4096;
    }

    devmem_readsl(REG_ADDR_X, (void *)&reg, 1);
    reg &= 0x0000FFFF;
    reg |= tmp << 16;

    devmem_set32(REG_ADDR_X, reg, 1);
}

static void _set_pic_width(receiver_st* r)
{
    DEBUG("called");

    uint32_t reg = 0;
    uint32_t tmp = 0;

    tmp = (uint8_t)r->rx_base.aParm[2] << 8;
    tmp |= (uint8_t)r->rx_base.aParm[3];

    devmem_readsl(REG_ADDR_X, (void *)&reg, 1);
    reg &= 0xFFFF0000;
    reg |= tmp;

    devmem_set32(REG_ADDR_X, reg, 1);
}

static void _set_pic_y_start(receiver_st* r)
{
    DEBUG("called");

    uint32_t reg = 0;
    uint32_t tmp = 0;

    tmp = (uint8_t)r->rx_base.aParm[2] << 8;
    tmp |= (uint8_t)r->rx_base.aParm[3];

    if (tmp > 2048U) {
        tmp = 2048;
    }

    devmem_readsl(REG_ADDR_Y, (void *)&reg, 1);
    reg &= 0x0000FFFF;
    reg |= tmp << 16;

    devmem_set32(REG_ADDR_Y, reg, 1);

}

static void _set_pic_height(receiver_st* r)
{
    DEBUG("called");

    uint32_t reg = 0;
    uint32_t tmp = 0;

    tmp = (uint8_t)r->rx_base.aParm[2] << 8;
    tmp |= (uint8_t)r->rx_base.aParm[3];

    devmem_readsl(REG_ADDR_Y, (void *)&reg, 1);
    reg &= 0xFFFF0000;
    reg |= tmp;

    devmem_set32(REG_ADDR_Y, reg, 1);
}

static void _set_sample_time(receiver_st* r)
{
    DEBUG("called");

    uint32_t reg = 0;

    reg |= (uint8_t)r->rx_base.aParm[2] << 8;
    reg |= (uint8_t)r->rx_base.aParm[3];

    if (reg > 65535U) {
        reg = 0xFFFF;
    }

    devmem_set32(REG_ADDR_TMR, reg, 1);
}

receiver_cmd_callback receiver_cmd_set_cb[5][16] = {
{NULL},
{
_set_pic_x_start,
_set_pic_width,
_set_pic_y_start,
_set_pic_height,
_set_sample_time
},
{NULL},
{NULL},
{NULL},
};

/*****************************************************************/
/*****************************************************************/
/*****                                                       *****/
/*****           MSG   P A R S E R                           *****/
/*****                                                       *****/
/*****************************************************************/
/*****************************************************************/
static void receiver_parse(receiver_st* r)
{
    int pos;
    unsigned char checksums;
    receiver_cmd_callback cmd_cb = NULL;

    //DEBUG("called");

#ifdef CONFIG_ENABLE_DEBUG
    printf("==========\n" \
           "Recv data as follows:\n");
    for (pos = 0; pos < RX_MAX_SIZE; pos++) {
        printf("r->in[%d]:0x%x\n", pos, (uint8_t)r->in[pos]);
    }
#endif

    for (checksums = 0, pos = POS_ID_1ST; pos < r->in[POS_LEN] + 2; pos++) {
        checksums += (unsigned char)r->in[pos];
    }
    if (checksums != (unsigned char)r->in[RX_MAX_SIZE - 1]) {
        ERR("Wrong checksums, cal checksums:%x", checksums);
        return;
    }

    r->rx_base.ucChecksums = (unsigned char)r->in[POS_CHECKSUMS];
    r->rx_base.ucLen = (unsigned char)r->in[POS_LEN];
    r->rx_base.aId[ID_1ST] = r->in[POS_ID_1ST];
    r->rx_base.aId[ID_2ND] = r->in[POS_ID_2ND];
    r->rx_base.cCmd = r->in[POS_CMD];
    for (pos = 0; pos < PARMS_MAX_SIZE; ++pos) {
        r->rx_base.aParm[pos] = r->in[POS_CMD + pos + 1];
    }

    switch (r->rx_base.cCmd) {
        case CMD_CONNECT: {
            DEBUG("recv cmd: CMD_CONNECT");
            //do something
            isConnected = true;
            receiver_connect(r);
            break;
        }
        case CMD_PERFORM: {
            DEBUG("recv cmd: CMD_PERFORM");
            //do something
            if (isConnected) {
                receiver_perform(r);
            }
            break;
        }
        case CMD_SET: {
            DEBUG("recv cmd: CMD_SET");

            if (!isConnected) {
                break;
            }

            cmd_cb = receiver_cmd_set_cb[(uint8_t)r->rx_base.aParm[0]][(uint8_t)r->rx_base.aParm[1]];
            //do something
            if (NULL != cmd_cb) {
                cmd_cb(r);
            }
            break;
        }
        case CMD_GET: {
            DEBUG("recv cmd: CMD_GET");

            if (!isConnected) {
                break;
            }

            cmd_cb = receiver_cmd_get_cb[(uint8_t)r->rx_base.aParm[0]][(uint8_t)r->rx_base.aParm[1]];
            //do something
            if (NULL != cmd_cb) {
                cmd_cb(r);
            }
            break;
        }
        default: {
            ASSERT();
            break;
        }
    }
}

static int receiver_write(receiver_st* r)
{
    char buf[TX_MAX_SIZE];
    int retVal;

    buf[POS_HEADER_1ST] = HEADER_1ST;
    buf[POS_HEADER_2ND]= HEADER_2ND;

    memcpy(&buf[POS_LEN], &r->tx_base, TX_MAX_SIZE - 2);

#ifdef CONFIG_ENABLE_DEBUG
    printf("==========\n");
    printf("Tx data as follows:");
    int i;
    for (i = 0; i < TX_MAX_SIZE; i++) {
        printf(" 0x%x,",(uint8_t)buf[i]);
    }
    printf("\n");
#endif

    retVal = write(r->fd, buf, TX_MAX_SIZE);
    if (retVal < 0) {
        ERR("Error on MSG write :%s", strerror(errno));
        return -1;
    }

    return 0;
}

static int receiver_read(receiver_st* r, char c)
{
    switch(r->pos) {
        case 0: {
            if ((uint8_t)c != HEADER_1ST) {
                ERR("Wrong header, c=0x%x",(uint8_t)c);
                return -1;
            }
            break;
        }
        case 1: {
            if ((uint8_t)c != HEADER_2ND) {
                r->pos = 0;
                ERR("Wrong header, c=0x%x",(uint8_t)c);
                return -1;
            }
            break;
        }
        default:
            break;
    }

    r->in[r->pos] = c;
    r->pos += 1;

    if (RX_MAX_SIZE == r->pos) {
        receiver_parse(r);
        r->pos = 0;
    }

    return 0;
}

static void receiver_init(receiver_st*  r)
{
    DEBUG("called");

    memset(r, 0, sizeof(*r));

    r->read_cb = receiver_read;
    r->write_cb = receiver_write;
}

receiver_st * receiver_instance(void)
{
    DEBUG("called");

    if (pReceiver){
        INFO("Object already instance");
        return pReceiver;
    }

    pReceiver = (receiver_st *)malloc(sizeof(*pReceiver));
    if (NULL == pReceiver) {
        ERR("Fail to alloc memory");
        return NULL;
    }

    receiver_init(pReceiver);

    return pReceiver;
}

void receiver_destroy(receiver_st * r)
{
    DEBUG("called");

    if (NULL == pReceiver) {
        INFO("Object already destroy");
        return;
    }

    free(pReceiver);
    pReceiver = NULL;
}

