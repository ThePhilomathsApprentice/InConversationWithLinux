#ifndef __MYHEADER_H
#define __MYHEADER_H

/* General Header Files. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Header Files for Shared memory */
#include <sys/shm.h>

/*defining the common key by which
 * the process/applications will search for the shared memory region.
 */
#define KEY 3995


#include <sys/types.h>
#include <unistd.h> 

/* Header File for thread semaphore */
#include <semaphore.h>

/* Header File for threads */
#include<pthread.h>

typedef struct shm_area
{
  sem_t sema1, sema2; // 2 Thread Semaphores
  //sema1: will be used by this (consumer) thread to exit the critical section \
  //sema2: will be used by this (consumer) thread to enter the critical section.
  //and indicate the other thread to enter the crtical section from its side.

  // One thread Mutex for Low level-Hardware Locking of the shared memory
  pthread_mutex_t mutex;

  // Read index for the circular buffer
  unsigned short int RdIndex;

  // Write index for  the circular buffer
  unsigned short int WrIndex;

  unsigned short int BufSizeMax;  //max buffer size
  char BufArea[25];           //this must be treated as circular buffer
  unsigned int UsedSlotCount; //no elements used in the circular buffer
} SHM;

SHM *shma;

void * threadCons(void *ConsVPtr);
void * threadProd(void *ProdVPtr);

pthread_t ThId;

#endif
