//
//  fnp.c
//  fnp
//
//  Created by Harry Reed on 11/26/14.
//  Copyright (c) 2014 Harry Reed. All rights reserved.
//


#include "sim_defs.h"
#include "sim_tmxr.h"

#include "fnp_defs.h"
#include "fnp_2.h"


/* SCP data structures and interface routines
 
 sim_name             simulator name string
 sim_PC               pointer to saved PC register descriptor
 sim_emax             number of words for examine
 sim_devices          array of pointers to simulated devices
 sim_stop_messages    array of pointers to stop messages
 sim_load             binary loader
*/

char sim_name[] = "FNP (Multics Faux FNP)";

//int32 saved_PC = 0;
int32 sim_emax = 1;

const char *sim_stop_messages[] = {
    "Quit",
    "Unknown error",
    "Unknown error",
    "Unknown error",
    "Unknown error"
};

DEVICE *sim_devices[] =
{
    &ipc_dev,
    &mux_dev,
    &clk_dev,
    //&tti_dev,
    //&tto_dev,
    NULL
};

FMTI *readAndParse(char *file);
FMTI *readDev(FILE *);

void dumpFMTI(FMTI *);

void freeFMTI(FMTI *p, bool bRecurse);

extern FMTI *fmti;

t_stat sim_load (FILE *fileref, char *cptr, char *fnam, int flag)
{
    if (flag == 1)
        return SCPE_OK;
    
    //readAndParse(fnam);
    
    FMTI *p = readDev(fileref);
 
    if (sim_switches & SWMASK ('V'))  /* verbose? */
    {
        sim_printf("Faux Multics devices loaded ...\n");
        while (p)
        {
            dumpFMTI(p);
            p = p->next;
        }
    }
    
    if (sim_switches & SWMASK ('A'))  /* append? */
    {
        if (fmti)
        {
            FMTI *q = fmti;
            while (q->next)
                q = q->next;
            q->next = p;
        } else
            fmti = p;
    } else {
        if (fmti)
            freeFMTI(fmti, true);
        fmti = p;
    }
    
    return SCPE_OK;
}

t_stat fprint_sym (FILE *of, t_addr addr, t_value *val, UNIT *uptr, int32 sw)
{
    return SCPE_OK;
}

t_stat parse_sym (char *cptr, t_addr addr, UNIT *uptr, t_value *val, int32 sw)
{
    return SCPE_OK;
}

extern MUXTERMIO ttys[MUX_MAX];
void MUXDisconnectAll();

t_stat sim_instr (void)
{
    
    /* Main instruction fetch/decode loop: check clock queue, intr, trap, bkpt */
    int reason = 0;
    int32 n = 0;

    clk(STCLK, 0, 0);        // start clock
    mux(SLS, 0, 0);
    
    
    UNIT *u = &mux_unit;
    if (u->filename == NULL || strlen(u->filename) == 0)
        sim_printf("Warning: MUX not attached.\n");
    //else
    //    mux_desc.lines = nTTYdevs;
    
    ipc_enable = !(ipc_dev.flags & DEV_DIS);
    
    ipc_verbose = (ipc_dev.dctrl & DBG_VERBOSE) && sim_deb;
    ipc_trace   = (ipc_dev.dctrl & DBG_TRACE  ) && sim_deb;
    if (!ipc_enable)
        sim_printf("Warning: IPC not enabled.\n");
    else
        ipc(ipcEnable, 0, 0, 0);                // start IPC beacon
    
    //ipc(ipcTest, 0, 0, 0);
    
    while (reason == 0)
    {                                            /* loop until ABORT */
        AIO_CHECK_EVENT;
        
        int32 temp = sim_poll_kbd ();
        
        switch(temp & 0x7f)
        {
            case 'q':                   // quit instr loop
                reason = SCPE_BREAK;
                break;
            case 'd':                   // disconnect all connected lines
                sim_printf("\r\n");
                MUXDisconnectAll();
                break;
        }
        
        if (sim_interval <= 0)
        {
            if ((reason = sim_process_event ()))
                break;
        }
        
        sim_interval = sim_interval - 1;                    /* count down */
        
        n += 1;
    }
    
    if (ipc_enable)
        ipc(ipcDisable, 0, 0, 0);     // start IPC operation

    return SCPE_OK;
}

