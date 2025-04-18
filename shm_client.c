/*
 * shm-client 
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

#include "quirks.h"
#include "env.h"

#define SHMSZ     65536
#define SHMVARS sizeof(struct Z80vars)+(sizeof(union Z80Regs)*2)+sizeof(struct CPU_flags) \
                +sizeof(union Z80IX)+sizeof(union Z80IY)+1

unsigned char * attach_speccy_shared_ram()
{
    int shmid;
    key_t key;
    unsigned char *shm;

    /*
     * We need to get the segment named
     * "5678", created by the server.
     */
    key = 5678;

    /*
     * Locate the segment.
     */
    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (unsigned char *) -1) {
        perror("shmat");
        exit(1);
    }

    /*
     * Now read what the server put in the memory.
     */
    return shm;
}

unsigned char * attach_speccy_shared_vars()
{   
    int shmid;
    key_t key;
    unsigned char *shm;
    
    /* 
     * We need to get the segment named
     * "5678", created by the server.
     */ 
    key = 1234;
    
    /* 
     * Locate the segment.
     */
    if ((shmid = shmget(key, SHMVARS, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    
    /* 
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (unsigned char *) -1) {
        perror("shmat");
        exit(1);
    }
    
    // counts number of "shm clients"
    (*(shm+SHMVARS-1))++;
    /* 
     * Now read what the server put in the memory.
     */ 
    return shm;
}

void dealloc_shared(unsigned char * mem, unsigned char * vars)
{
   if ( (--(*(vars+SHMVARS-1))) == 0 )
   {
      shmdt(vars);
      shmdt(mem);
   }
}
