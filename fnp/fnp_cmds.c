//
//  fnp_cmds.c
//  fnp
//
//  Created by Harry Reed on 5/17/15.
//  Copyright (c) 2015 Harry Reed. All rights reserved.
//

#include "fnp_cmds.h"

#ifdef COMMENT
"bootload"
This command indicates that Multics has indicated that
FNP should restart, reload it's configuration tables,
and reset it's state data.

"accept_calls"
Sets the state of the FNP to accepting
incoming calls. Reset state is not accepting.

"dont_accept_calls"
Sets the state of the FNP to not accepting
incoming calls. Reset state is not accepting.

"listen %d %d %d"
First parameter is 0 (disable) or 1 (enable).
Second parameter is line number (0 - 31) of a HSLA line.
Third parameter is input buffer size.
Enable or disable listening for incoming calls on the indicated
HSLA line.
#endif

t_stat fnp_command(char *nodename, char *id, char *arg3)
{
    ipc_printf("fnp_command(\"%s\", \"%s\", \"%s\")\n", nodename, id, arg3);
    
    if (strcmp(arg3, "bootload") == 0)
    {
        ipc_printf("Received BOOTLOAD command...\n");
        
    } else if (strcmp(arg3, "accept_calls") == 0)
    {
        ipc_printf("Received ACCEPT_CALLS command...\n");
    } else if (strcmp(arg3, "dont_accept_calls") == 0)
    {
        ipc_printf("Received DONT_ACCEPt_CALLS command...\n");
    } else if (strncmp(arg3, "listen", 6) == 0)
    {
        int p1, p2, p3;
        int n = sscanf(arg3, "%*s %d %d %d", &p1, &p2, &p3);
        if (n != 3)
            return SCPE_ARG; // listen is supposed to have 3 args
        ipc_printf("received LISTEN %d %d %d ...\n", p1, p2, p3);
        
    }
    
    return SCPE_OK;
}
