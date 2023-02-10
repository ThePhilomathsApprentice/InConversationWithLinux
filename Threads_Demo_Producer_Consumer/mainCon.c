#include"MyHeader.h"

int main()
{
	/* Variable to check the return values of the system call APIs
	 */
	int ret;

	/* variable to save the shared memory returned from the shmget.
	 */
	int shm_id;

	/* Create and/or get the shared memory
	 */
	shm_id = shmget( KEY , 4096 , IPC_CREAT | 0600);
	if (shm_id < 0 )
	{
		perror("Error in creating the shared memory");
		exit(1);
	}

	/* Attach the shared memory to the virtual memory space of the current Process.
	 */
	shma = shmat(shm_id, 0, 0);
	if (shma < 0)
	{
		perror("error in attaching the shared memory");
		exit(1);
	}

	ret = pthread_create(&ThId , NULL , threadCons , NULL);
	if (ret != 0)
	{
		perror("Error in thread Creation for ThreadConsumer:\n");
		printf("Error Number:%d\n",ret);
		exit(ret);
	}

	/* This process/application will not be terminated without terminating the thread it has started.
	 */
	pthread_join( ThId , NULL);

	exit(0);
}//main:end

void * threadCons(void *ConsVPtr){
	/* Creating a buffer variable to save the data recieved from the producer temporarily.
	 */
	char value;
	printf("----------------Consumer Entered CSection----------------\n \
            Waiting for the Producer to produce content to be read here, \
            and free the semaphore...\n");
	while(1){

		/* Decrement the thread semaphore to enter the critical section and modify.
		 */
		sem_wait( &(shma->sema2) );

		/* Lock the mutex to get extra protection.
		 */
		pthread_mutex_lock ( &(shma->mutex) );

		if(shma->UsedSlotCount != 0){
			value = shma->BufArea[shma->RdIndex];

			printf("Consumer:%c\n",value);

			/* As the buffer is circular, increment the read index untill it reaches BuffSizeMax,
			 * after that it will automatically be reinitialized to 0.
			 */
			shma->RdIndex = (shma->RdIndex+1)%shma->BufSizeMax;

			/* After reading one character,
			 * decrement the Used Slot Count variable,
			 * because 1 data-content has been consumed.
			 */
			shma->UsedSlotCount--;

		}

		/* UnLock the mutex which was used to get low-level protection.
		 */
		pthread_mutex_unlock ( &(shma->mutex) );

		/* Increment the other thread semaphore to exit the critical section
		 * and indicate the producer to get active and start producing the data..
		 */
		sem_post( &(shma->sema1) );


	}//threadCons:while:end

	/* Indicates that the thread has exited the critical section and
	 * the producer must produce new data now..
	 */
	printf("----------------Consumer Exited CSection----------------\n");

	pthread_exit(0);

}//threadCons:end
