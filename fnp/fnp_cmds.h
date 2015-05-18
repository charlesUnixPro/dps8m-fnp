//
//  fnp_cmds.h
//  fnp
//
//  Created by Harry Reed on 5/17/15.
//  Copyright (c) 2015 Harry Reed. All rights reserved.
//

#ifndef __fnp__fnp_cmds__
#define __fnp__fnp_cmds__

#include <stdio.h>

#include "fnp_defs.h"
#include "fnp_ipc.h"

// MS: Multics state: the state that Multics has set the FNP to

extern bool MS_accept_calls;
extern bool MS_listen [MAX_LINES];
extern int MS_input_buffer_size [MAX_LINES];
t_stat fnp_command(char *nodename, char *id, char *arg3);


#endif /* defined(__fnp__fnp_cmds__) */
