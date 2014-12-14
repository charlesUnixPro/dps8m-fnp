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

t_stat mux_setnl( UNIT * uptr, int32 val, char * cptr, void * desc );

extern int32   mux_max;
extern TMLN    mux_ldsc[ MUX_MAX ];

#endif /* defined(__fnp__fnp_mux__) */
