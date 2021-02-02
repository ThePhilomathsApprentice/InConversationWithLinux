#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>    /* fork(), etc.                        */
#include <time.h>      /* nanosleep(), etc.                   */
#include <sys/types.h> /* various type definitions.           */
#include <sys/ipc.h>   /* general SysV IPC structures         */
#include <sys/sem.h>   /* semaphore functions and structs.    */
#include <sys/wait.h>  /* wait(), etc.                        */

/*header files for shared memory. */
#include <sys/shm.h>

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

typedef struct shmarea
{

    int count1;
    int count2;

} SHM; /*The structure of data that is on the shared memory. */

SHM *shma; /*I will use this pointer to acess the shared memory segment. */

int main()
{
    /*ParentChildInfo */
    int child_pid, ret, status; /* PID of our child process. */

    /*Shared Memoryinfo*/
    int id;

    /*SemaphoreInfo */
    int sem_set_id;        /* ID of the semaphore set.       */
    union semun semun_val; /* semaphore value, for semctl(). */
    int i;                 /* counter for loop operation.    */
    int rc;                /* return value of system calls.  */

    struct sembuf sem_op; /* structure for semaphore operations. */

    /* create a semaphore set with ID 250, with one semaphore   */
    /* in it, with access only to the owner.                    */
    sem_set_id = semget(SEM_ID, 1, IPC_CREAT | 0600);
    if (sem_set_id == -1)
    {
        perror("semaSHM: ain: semget\n");
        exit(1);
    }

    semun_val.val = 1; /* intialize the first (and the single) semaphore in our set to '1'. */
    rc = semctl(sem_set_id, 0, SETVAL, semun_val);
    if (rc == -1)
    {
        perror("semaSHM: main: semctl\n");
        exit(1);
    }

    /* create a set of child processes that will compete on the semaphore */
    for (i = 0; i < CHLD_PROCS; i++)
    {
        //while(i++ < CHLD_PROCS){
        child_pid = fork();
        switch (child_pid)
        {
        case -1:
            perror("semaSHM: fork");
            exit(1);
        case 0:
            /* we're at child process. */
            /* wait on the semaphore, unless it's value is non-negative. */
            sem_op.sem_num = 0;
            sem_op.sem_op = -1;
            sem_op.sem_flg = 0;
            semop(sem_set_id, &sem_op, 1);
            printf("semaSHM: ---------- Child with PID:%lu, PPID:%lu ----------\n", getpid(), getppid());
            printf("semaSHM: Entered SHM, SHM Locked.\n");

            id = shmget(KEY1, 4096, IPC_CREAT | 0600);
            if (id < 0)
            {
                perror("semaSHM: error in shmget\n");
                exit(1);
            }
            shma = shmat(id, 0, 0);
            if (shma == (struct shmarea *)-1)
            {
                perror("semaSHM: errro in shmat\n");
                exit(2);
            }
            printf("semaSHM: Initial pointer value is %xl\nsemaSHM: Accessed from Child:\t%d\t%d\n", shma, shma->count1, shma->count2);
            shma->count1 += 10;
            shma->count2 -= 20;
            printf("semaSHM: Pointer value is %xl\nsemaSHM: Manipulated from Child:\t%d\t%d\n", shma, shma->count1, shma->count2);
            //printf("semaSHM: Child Exiting SHM, SHM UnLocked.\n");
            printf("semaSHM: ---------- Child Exiting SHM, SHM UnLocked. ---------- \n");

            sem_op.sem_num = 0;
            sem_op.sem_op = 1; /* <-- Comment 3 */
            sem_op.sem_flg = 0;
            semop(sem_set_id, &sem_op, 1);
            exit(0);
        default: /* we're at parent process. */
            /* we're at Parent process. */

            /* wait on the semaphore, unless it's value is non-negative. */
            sem_op.sem_num = 0;
            sem_op.sem_op = -1;
            sem_op.sem_flg = 0;
            semop(sem_set_id, &sem_op, 1);
            printf("semaSHM: ---------- Parent with PID:%lu, PPID:%lu ----------\n", getpid(), getppid());
            printf("semaSHM: Entered SHM, SHM Locked.\n");
            id = shmget(KEY1, 4096, IPC_CREAT | 0600);
            if (id < 0)
            {
                perror("semaSHM: error in shmget\n");
                exit(1);
            }
            shma = shmat(id, 0, 0);
            if (shma == (struct shmarea *)-1)
            {
                perror("semaSHM: errro in shmat\n");
                exit(2);
            }
            printf("semaSHM: Initial pointer value is %xl\nsemaSHM: Accessed from Parent:\t%d\t%d\n", shma, shma->count1, shma->count2);
            shma->count1 = 100;
            shma->count2 = 200;
            printf("semaSHM: Pointer value is %xl\nsemaSHM: Manipulated from Parent:\t%d\t%d\n", shma, shma->count1, shma->count2);
            //printf("semaSHM: Parent Exiting SHM, SHM UnLocked.\n");
            printf("semaSHM: ---------- Parent Exiting SHM, SHM UnLocked. ---------- \n");

            sem_op.sem_num = 0;
            sem_op.sem_op = 1;
            sem_op.sem_flg = 0;
            semop(sem_set_id, &sem_op, 1);
            continue;

        } /*End of Switch*/
    }     /*End of For Loop*/

    if (child_pid > 0)
    {
        while (1)
        { /* Kill all the Children processes After the Manipulation. */
            ret = waitpid(-1, &status, 0);
            if (ret < 0)
            {
                //pause();
                printf("semaSHM: Values of shared memory counter is:\t%d\t%d\n", shma->count1, shma->count2);
                exit(0);
            } //no child is in any state for this process
        }
        printf("semaSHM: main: we're done\n");
    }

    return 0;
}
