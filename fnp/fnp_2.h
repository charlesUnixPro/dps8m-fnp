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

#include "fnp_defs.h"

struct muxtermio
{
    char *raw;
    struct
    {
        char *name;     // Multics device name
        char *baud;     // baud rate
        char *comment;  // comment
        char *terminal_type;
        char *attributes;
        char *initial_command;
    } multics;
    int mux_line;       // multiplexor line used for this terminal (-1 == unused)
    char buffer[1024];  // line buffer for initial device selection
    char *uti;          // UNIX terminfo terminal type
};

typedef struct muxtermio MUXTERMIO;

#endif /* defined(__fnp__fnp_2__) */
