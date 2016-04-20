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

#ifndef _BASE_FRAME_H_
#define _BASE_FRAME_H_

#ifdef __cplusplus
extern C {
#endif

#include <stdint.h>

/* Frame - header */
#define HEADER_1ST  0xAA
#define HEADER_2ND  0x55

/* Frame - machine id */
enum stMachineId {
    ID_1ST = 0,
    ID_2ND,
    ID_MAX_SIZE,
};

/* Frame - command code */
enum stCommandSet {
    CMD_CONNECT = 0x01,
    CMD_PERFORM = 0x21,
    CMD_SET     = 0x22,
    CMD_GET     = 0x23,
};

/* Frame - parameter format */
enum stParmFormat {
    PARM_TYPE = 0,
    PARM_INDEX,
    PARM_VAL_1ST,
    PARM_VAL_2ND,
    PARMS_MAX_SIZE,
};

/* Frame format */
enum stFrameFormat {
    POS_HEADER_1ST = 0,
    POS_HEADER_2ND,
    POS_LEN,
    POS_ID_1ST,
    POS_ID_2ND,
    POS_CMD,
    POS_CHECKSUMS = POS_CMD + 4 + 1,
};

typedef struct stBaseFrame {
    uint8_t     ucLen;
    char        aId[ID_MAX_SIZE];
    char        cCmd;
    char        aParm[PARMS_MAX_SIZE];
    uint8_t     ucChecksums;
} BaseFrame_t;

#ifdef __cplusplus
}
#endif

#endif /* end of _BASE_FRAME_H_ */

