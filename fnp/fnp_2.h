//
//  fnp_2.h
//  fnp
//
//  Created by Harry Reed on 12/9/14.
//  Copyright (c) 2014 Harry Reed. All rights reserved.
//

#ifndef __fnp__fnp_2__
#define __fnp__fnp_2__

#include <stdio.h>
#include <stdbool.h>

#include "fnp_defs.h"

enum muxtermstate
{
    eDisconnected = 0,  // disconnected
    eInput,             // waiting for user input
    ePassThrough        // passthought mode
};

typedef enum muxtermstate MUXTERMSTATE;

struct fauxMulticsTerminalInfo
{
    char *raw;
    struct
    {
        char    *name;     // Multics device name
        char    *baud;     // baud rate
        char    *comment;  // comment
        char    *terminal_type;
        char    *attributes;
        char    *initial_command;
    } multics;
    char    *uti;         // UNIX terminfo terminal type
    bool    inUse;        // True if device is being used
};

typedef struct fauxMulticsTerminalInfo FMTI;

struct muxtermio
{
    FMTI        *fmti;
    int32       mux_line;    // multiplexor line# used for this terminal (-1 == unused)
    TMLN        *tmln;         // terminal line
    char        buffer[1024];  // line buffer for initial device selection
    int32       nPos;          // position where *next* user input is to be stored
    MUXTERMSTATE state;         // state of tty (eDisconnected, eInput ePassThrough)
};

typedef struct muxtermio MUXTERMIO;

int32 processUserInput(TMXR *mp, TMLN *tmln, MUXTERMIO *tty, int32 line, int32 kar);

#endif /* defined(__fnp__fnp_2__) */
