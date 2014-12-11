//
//  fnp_1.c
//  fnp
//
//  Created by Harry Reed on 11/26/14.
//  Copyright (c) 2014 Harry Reed. All rights reserved.
//

#include "fnp_defs.h"

#include "fnp_1.h"

char *
getPortList();

/*
 * called when a MUX line in connected to a telnet client ...
 */
t_stat OnMuxConnect(TMLN *tmnl, int line)
{
    sim_printf("CONNECT %d\n", line);
    
    tmxr_linemsgf (tmnl, "HSLA Port (%s)? ", getPortList());
    
    return SCPE_OK;
}


/*
 * called when a MUX line is disconnected from a telnet client. (Happens after sending a char to the disconnected line.)
 */
t_stat OnMuxDisconnect(int line, int kar)
{
    sim_printf("DISCONNECT %d\n", line);

    return SCPE_OK;
}

t_stat OnMuxStalled(int line, int kar)
{
    
    sim_printf("STALLED %d (%c)\n", line, kar);
    
    return SCPE_OK;
}

/*
 * called when a character is received on a MUX line ...
 */
t_stat OnMuxRx(int line, int kar)
{
    //sim_printf("Rx: line:%d '%c'\n", line, kar);
    mux_chars_Rx += 1;
    
    MuxWrite(line, kar);
    
    return SCPE_OK;
}

/*
 * called when a BREAK is received on a MUX line ...
 */
t_stat OnMuxRxBreak(int line, int kar)
{
    sim_printf("Rx (BREAK): line:%d\n", line, kar);
    
    return SCPE_OK;
}

/*
 * called when a character has been input from TTI
 */
t_stat OnTTI(int indata)
{
    sim_printf("TTI: '%c'\n", indata);
    return SCPE_OK;
}

t_stat MuxWrite(int line, int kar)
{
    mux(SLS, 0, line << 8); // select line
    return mux(OMC, TXD, kar);
}