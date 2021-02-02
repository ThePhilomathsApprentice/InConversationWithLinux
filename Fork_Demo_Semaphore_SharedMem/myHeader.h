#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>    /* fork(), etc.                        */
#include <time.h>      /* nanosleep(), etc.                   */
#include <sys/types.h> /* various type definitions.           */
#include <sys/ipc.h>   /* general SysV IPC structures         */
#include <sys/sem.h>   /* Semaphore functions and structs.    */
#include<sys/shm.h>   /* Shared Memory functions and structs.    */
#include <sys/wait.h>  /* wait(), etc.                        */


#define KEY1 1995 /* This is the Key of the shared Memory. */

#define CHLD_PROCS 2 /* number of processes to launch.      */
#define SEM_ID 250   /* ID for the semaphore.               */

union semun
{
    int val;                   /* value for SETVAL */
    struct semid_ds *buf;      /* buffer for IPC_STAT, IPC_SET */
    unsigned short int *array; /* array for GETALL, SETALL */
    struct seminfo *__buf;     /* buffer for IPC_INFO */
};

typedef struct shmarea {
  int count1;
  int count2;
}SHM; 
