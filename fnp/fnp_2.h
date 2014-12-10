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

struct terminfo
{
    struct multics
    {
        char *name;     // Multics device name
        char *baud;     // baud rate
        char *comment;  // comment
        char *terminal_type;
        char *attributes;
        char *initial_command;
    };
    char *uti;          // UNIX terminfo terminal type
};

#endif /* defined(__fnp__fnp_2__) */
