//
//  fnp_mux.h
//  fnp
//
//  Created by Harry Reed on 11/26/14.
//  Copyright (c) 2014 Harry Reed. All rights reserved.
//

#ifndef __fnp__fnp_mux__
#define __fnp__fnp_mux__

#include <stdio.h>

#include "fnp_defs.h"

t_stat mux_setnl( UNIT * uptr, int32 val, char * cptr, void * desc );
int32 muxWhatUnitAttached();

extern int32   mux_max;
extern TMLN    mux_ldsc[ MAX_LINES ];
extern TMXR    mux_desc;

// names of FNP units? may need to change
#define _FNP0     "fnp0"     // FNP A
#define _FNP1     "fnp1"     // FNP B
#define _FNP2     "fnp2"     // FNP C
#define _FNP3     "fnp3"     // FNP D

extern char *fnpNames[4];       // array of FNP names

#endif /* defined(__fnp__fnp_mux__) */
