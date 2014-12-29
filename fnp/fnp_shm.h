//
//  fnp_shm.h
//  fnp
//
// stolen from dps8m/shm.[ch]
//

#ifndef __fnp__fnp_shm__
#define __fnp__fnp_shm__

#include <stdio.h>

void * create_shm (char * key, pid_t system_pid, size_t size);
void * open_shm (char * key, pid_t system_pid, size_t size);


#endif /* defined(__fnp__fnp_shm__) */
