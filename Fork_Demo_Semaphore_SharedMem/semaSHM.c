#include "myHeader.h"

SHM *shma = NULL;// Global pointer to struct shmarea (aka SHM) initialized.

int shmDInit();
int shmInit();

int main()
{
    /*ParentChildInfo */
    int child_pid[CHLD_PROCS]; /* PID of our child process. */
    int ret, status;

    /*Shared Memoryinfo*/
    int id;

    /*SemaphoreInfo */
    int sem_set_id;        /* ID of the semaphore set.       */
    union semun semun_val; /* semaphore value, for semctl(). */
    int i;                 /* counter for loop operation.    */
    int rc;                /* return value of system calls.  */

    struct sembuf sem_op; /* structure for semaphore operations. */

    shmInit();

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
        child_pid[i] = fork();
        switch (child_pid[i])
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
            printf("semaSHM: ---------- Child with PID:%u, PPID:%u ----------\n", getpid(), getppid());
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
            printf("semaSHM: Initial pointer value is %p\nsemaSHM: Accessed from Child:\t%d\t%d\n", shma, shma->count1, shma->count2);
            shma->count1 += 50;
            shma->count2 -= 50;
            printf("semaSHM: Pointer value is %p\nsemaSHM: Manipulated from Child:\t%d\t%d\n", shma, shma->count1, shma->count2);

            sem_op.sem_num = 0;
            sem_op.sem_op = 1; /* <-- Comment 3 */
            sem_op.sem_flg = 0;
            semop(sem_set_id, &sem_op, 1);
            printf("semaSHM: ---------- Child Exiting SHM, SHM UnLocked. ---------- \n");
            exit(0);
        default: 
            /* we're at parent process. */
            /* we're at Parent process. */

            /* wait on the semaphore, unless it's value is non-negative. */
            sem_op.sem_num = 0;
            sem_op.sem_op = -1;
            sem_op.sem_flg = 0;
            semop(sem_set_id, &sem_op, 1);
            printf("semaSHM: ---------- Parent with PID:%u, PPID:%u ----------\n", getpid(), getppid());
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
            printf("semaSHM: Initial pointer value is %p\nsemaSHM: Accessed from Parent:\t%d\t%d\n", shma, shma->count1, shma->count2);
            shma->count1 = 100;
            shma->count2 = 200;
            printf("semaSHM: Pointer value is %p\nsemaSHM: Manipulated from Parent:\t%d\t%d\n", shma, shma->count1, shma->count2);

            sem_op.sem_num = 0;
            sem_op.sem_op = 1;
            sem_op.sem_flg = 0;
            semop(sem_set_id, &sem_op, 1);
            printf("semaSHM: ---------- Parent Exiting SHM, SHM UnLocked. ---------- \n");

            continue;

        } /*End of Switch*/
    }     /*End of For Loop*/

    for (i = 0; i < CHLD_PROCS; i++)
    {
        /* wait for all the Children processes After the Manipulation. */
        if( waitpid(child_pid[i], &status, 0) == (-1) )
        {
            perror("SemaSHM: Erro waiting for the child Process\n");

        }else{
            if( WEXITSTATUS(status)){
                perror("SemaSHM: status not zero\n");
            }
        }

        printf("semaSHM:from parent main: we're done\n");


    }

    shmDInit();
    exit(0);
}
