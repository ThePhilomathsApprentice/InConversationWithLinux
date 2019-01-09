#include"MyHeader.h"

int main()
{
  int ret; //variable to check the return values of the system call APIs
  int shm_id; //variable to save the shared memory returned from the shmget.

  shm_id = shmget( KEY , 4096 , IPC_CREAT | 0600); // create and/or get the shared memory 
  if (shm_id < 0 )
  {
    perror("error in creating the shared memory");
    exit(1);
  }

  shma = shmat(shm_id, 0, 0); // attach the shared memory to the virtual memory space of the current Process.
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

  pthread_join( ThId , NULL); // This process/application will not be terminated without terminating rthe thread it has started.

  exit(0);
}

void * threadCons(void *ConsVPtr){
  char value; // creating a buffer variable to save the data recieved from the producer temporarily.
  while(1){
    printf("----------------Consumer Entered CSection----------------\nWaiting for the Producer to produce content to be read here, and free the semaphore...\n");
    
    sem_wait( &(shma->sema2) ); // Decrement the thread semaphore to enter the critical section and modify.
    pthread_mutex_lock ( &(shma->mutex) ); // lock the mutex to get extra protection.
    
    if(shma->UsedSlotCount != 0){
      value = shma->BufArea[shma->RdIndex];
      
      printf("Consumer:%c\n",value); 
      shma->RdIndex = (shma->RdIndex+1)%shma->BufSizeMax; // as the buffer is circular I increment the read index untill it reaches BuffSizeMax, after that it will automatically be reinitialized to 0.
      shma->UsedSlotCount--; //after reading one character, decrement the Used Slot Count variable, because 1 data-content has been consumed.
    
    }

    pthread_mutex_unlock ( &(shma->mutex) );  // UNlock the mutex which was used to get low-level protection.
    sem_post( &(shma->sema1) ); // Increment the other thread semaphore to exit the critical section and indicate the producer to get active and start producing the data..
    
    printf("----------------Consumer Exited CSection----------------\n"); // indicates that the thread has exited the critical section and the producer must produce new data now..
    
  }//threadCons:while:end

  pthread_exit(0);

}//threadCons:end
