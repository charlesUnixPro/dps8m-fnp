//
//  fnp_1.c
//  fnp
//
//  Created by Harry Reed on 11/26/14.
//  Copyright (c) 2014 Harry Reed. All rights reserved.
//

#include "fnp_defs.h"
#include "sim_tmxr.h"

#include "fnp_mux.h"

#include "fnp_1.h"
#include "fnp_2.h"
#include "fnp_utils.h"

extern MUXTERMIO ttys[MUX_MAX];
extern TMLN mux_ldsc[MUX_MAX];

char *
getDevList();

/*
 * called when a MUX line in connected to a telnet client ...
 */
t_stat OnMuxConnect(TMLN *tmnl, int line)
{
    sim_printf("%s CONNECT %d\n", Now(), line);
    
    tmxr_linemsgf (tmnl, "HSLA Port (%s)? ", getDevList());
    
    ttys[line].mux_line = line;
    ttys[line].state = eInput;      // waiting for user input
    
    return SCPE_OK;
}

void MUXDisconnectLine(int line)
{
    if (line >= 0 && line < mux_max)
    {
//    MUXTERMIO *tty = &ttys[line];   // fetch tty connection info
//    if (tty->mux_line != -1)        // this is probably not the best way to test for line connection
        TMXR *tmxr = &mux_desc;
        if (tmxr->ldsc[line].conn)    // much better I think
        {
            tmxr_reset_ln( &mux_ldsc[line] ) ;
            OnMuxDisconnect(line, 0);
        }
    }
}

void MUXDisconnectAll()
{
    for (int line = 0 ; line < mux_max ; line += 1)
        MUXDisconnectLine(line);
}


/*
 * called when a MUX line is disconnected from a telnet client. (Happens after sending a char to the disconnected line.)
 */
t_stat OnMuxDisconnect(int line, int kar)
{
    sim_printf("%s DISCONNECT %d\n", Now(), line);

    MUXTERMIO *tty = &ttys[line];   // fetch tty connection info
    tty->mux_line = -1;             // line no longer connected
    tty->state = eDisconnected;
    if (tty->fmti)
        tty->fmti->inUse = false;   // multics device no longer in use
    tty->fmti = NULL;               // line no longer connected to a multics device
    
    return SCPE_OK;
}

t_stat OnMuxStalled(int line, int kar)
{
    
    sim_printf("%s STALLED %d (%c)\n", Now(), line, kar);
    
    return SCPE_OK;
}

FMTI *searchForDevice(char *dev);
char *strFMTI(FMTI *p, int line);

/*
 * called when a character is received on a MUX line ...
 */
t_stat OnMuxRx(TMXR *mp, TMLN *tmln, int line, int kar)
{
    //sim_printf("Rx: line:%d '%c'\n", line, kar);
    mux_chars_Rx += 1;
    
    FMTI *q;
    
    MUXTERMIO *tty = &ttys[line]; // fetch tty connection info
    switch (tty->state)
    {
        case eDisconnected:
            break;
        case eInput:
            switch (processUserInput(mp, tmln, tty, line, kar))
            {
                case eEndOfLine:
                    q = searchForDevice(tty->buffer);
                    if (q)
                    {
                        tty->fmti = q;              // associate line with Multics device
                        q->inUse = true;            // device is being used
                        tty->state = ePassThrough;  // device attached to tty line. Go into passthrough mode
                        
                        tmxr_linemsgf (tmln, "%s", strFMTI(q, line));
                        
                        sim_printf("%s LINE %d CONNECTED AS %s\n", Now(), line, q->multics.name);
                    }
                    else
                    {
                        tmxr_linemsgf (tmln, "\r\nDevice <%s> not available. Please re-enter.", tty->buffer);
                        tmxr_linemsgf (tmln, "\r\n" PROMPT, getDevList());
                    }
                    // reset input buffer
                    tty->nPos = 0;
                    tty->buffer[0] = '\0';
                    break;
                case eDisconnected:
                    OnMuxDisconnect(line, 0);
                    break;
                    
                default:
                    break;
            }
            break;
        case ePassThrough:
            MuxWrite(line, kar);
            break;
        default:
            break;
    }
    
    return SCPE_OK;
}

/*
 * called when a BREAK is received on a MUX line ...
 */
t_stat OnMuxRxBreak(int line, int kar)
{
    sim_printf("%s Rx (BREAK): line:%d\n", Now(), line, kar);
    
    return SCPE_OK;
}

/*
 * called when a character has been input from TTI
 */
t_stat OnTTI(int indata)
{
    sim_printf("%s TTI: '%c'\n", Now(), indata);
    return SCPE_OK;
}

t_stat MuxWrite(int line, int kar)
{
    mux(SLS, 0, line << 8); // select line
    return mux(OMC, TXD, kar);
}