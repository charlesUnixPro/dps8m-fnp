//
//
//  mux_clk.h
//
//  Created by Harry Reed on 11/26/14.
//  Copyright (c) 2014 Harry Reed. All rights reserved.
//
// derived from .....
//
//   nova_clk.c: NOVA real-time clock simulator
//
//   Copyright (c) 1993-2008, Robert M. Supnik
//

//#include "nova_defs.h"
#include "fnp_defs.h"

extern int32 int_req, dev_busy, dev_done, dev_disable ;

int32 clk_sel = 0;                                      /* selected freq */
int32 clk_time[4] = { 16000, 100000, 10000, 1000 };     /* freq table */
int32 clk_tps[4] = { 60, 10, 100, 1000 };               /* ticks per sec */
int32 clk_adj[4] = { 1, -5, 2, 20 };                    /* tmxr adjust */
int32 mux_tmxr_poll = 16000;                            /* tmxr poll */

int32  clk (int32 pulse, int32 code, int32 AC);
t_stat clk_svc (UNIT *uptr);
t_stat clk_reset (DEVICE *dptr);
t_stat clk_set_freq (UNIT *uptr, int32 val, char *cptr, void *desc);
t_stat clk_show_freq (FILE *st, UNIT *uptr, int32 val, void *desc);

/* CLK data structures

   clk_dev      CLK device descriptor
   clk_unit     CLK unit descriptor
   clk_reg      CLK register list
*/

DIB clk_dib = { MUX_INT_CLK, MUX_INT_CLK, PI_CLK, &clk };

UNIT clk_unit = { UDATA (&clk_svc, UNIT_IDLE, 0) };
//UNIT clk_unit = { UDATA (&clk_svc, 0, 0) };

REG clk_reg[] = {
    { ORDATA (SELECT, clk_sel, 2) },
    { FLDATA (BUSY, mux_busy, MUX_INT_V_CLK) },
    { FLDATA (DONE, mux_done, MUX_INT_V_CLK) },
    { FLDATA (DISABLE, mux_disable, MUX_INT_V_CLK) },
    { FLDATA (INT, mux_int_req, MUX_INT_V_CLK) },
    { DRDATA (TIME0, clk_time[0], 24), REG_NZ + PV_LEFT },
    { DRDATA (TIME1, clk_time[1], 24), REG_NZ + PV_LEFT },
    { DRDATA (TIME2, clk_time[2], 24), REG_NZ + PV_LEFT },
    { DRDATA (TIME3, clk_time[3], 24), REG_NZ + PV_LEFT },
    { DRDATA (TPS0, clk_tps[0], 6), PV_LEFT + REG_HRO },
    { NULL }
    };

MTAB clk_mod[] = {
    { MTAB_XTD|MTAB_VDV, 50, NULL, "50HZ",
      &clk_set_freq, NULL, NULL },
    { MTAB_XTD|MTAB_VDV, 60, NULL, "60HZ",
      &clk_set_freq, NULL, NULL },
    { MTAB_XTD|MTAB_VDV, 0, "LINE", NULL,
      NULL, &clk_show_freq, NULL },
    { 0 }
    };

DEVICE clk_dev = {
    "RTC", &clk_unit, clk_reg, clk_mod,
    1, 0, 0, 0, 0, 0,
    NULL, NULL, &clk_reset,
    NULL, NULL, NULL,
    &clk_dib, DEV_DISABLE
    };


/* IOT routine */

int32 clk (int32 pulse, int32 code, int32 AC)
{
    if (code == SELECT) {                                    /* DOA */
        clk_sel = AC & 3;                                   /* save select */
        sim_rtc_init (clk_time[clk_sel]);                   /* init calibr */
    }

    switch (pulse) {                                        /* decode IR<8:9> */
        case STCLK:                                            /* start */
            MUX_SET_BUSY( MUX_INT_CLK ) ;
            MUX_CLR_DONE( MUX_INT_CLK ) ;
            MUX_UPDATE_INTR ;
            if (!sim_is_active (&clk_unit))                     /* not running? */
                sim_activate (&clk_unit,                        /* activate */
                              sim_rtc_init (clk_time[clk_sel]));          /* init calibr */
            break;

        case CLCLK:                                            /* clear */
            MUX_CLR_BUSY( MUX_INT_CLK ) ;
            MUX_CLR_DONE( MUX_INT_CLK ) ;
            MUX_UPDATE_INTR ;
            sim_cancel (&clk_unit);                             /* deactivate unit */
            break;
    }                                                   /* end switch */

    return 0;
}

/* Unit service */

t_stat clk_svc (UNIT *uptr)
{
    int32 t;

    if ( MUX_IS_BUSY(MUX_INT_CLK) )
    {
        MUX_CLR_BUSY( MUX_INT_CLK ) ;
        MUX_SET_DONE( MUX_INT_CLK ) ;
        MUX_UPDATE_INTR ;
    }
    t = sim_rtc_calb (clk_tps[clk_sel]);                    /* calibrate delay */
    sim_activate_after (uptr, 1000000/clk_tps[clk_sel]);    /* reactivate unit */
    if (clk_adj[clk_sel] > 0)                               /* clk >= 60Hz? */
        mux_tmxr_poll = t * clk_adj[clk_sel];                   /* poll is longer */
    else
        mux_tmxr_poll = t / (-clk_adj[clk_sel]);                /* poll is shorter */
    return SCPE_OK;
}

/* Reset routine */

t_stat clk_reset (DEVICE *dptr)
{
    sim_register_clock_unit (&clk_unit);                    /* declare clock unit */
    clk_sel = 0;
    MUX_CLR_BUSY( MUX_INT_CLK ) ;
    MUX_CLR_DONE( MUX_INT_CLK ) ;
    MUX_UPDATE_INTR ;

    sim_cancel (&clk_unit);                                 /* deactivate unit */
    mux_tmxr_poll = clk_time[0];                                /* poll is default */
    return SCPE_OK;
}

/* Set line frequency */

t_stat clk_set_freq (UNIT *uptr, int32 val, char *cptr, void *desc)
{
    if (cptr)
        return SCPE_ARG;
    if ((val != 50) && (val != 60))
        return SCPE_IERR;
    clk_tps[0] = val;
    return SCPE_OK;
}

/* Show line frequency */

t_stat clk_show_freq (FILE *st, UNIT *uptr, int32 val, void *desc)
{
    fprintf (st, (clk_tps[0] == 50)? "50Hz": "60Hz");
    return SCPE_OK;
}
