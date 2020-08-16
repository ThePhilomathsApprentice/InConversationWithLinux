#include"MyHeader.h"

int main()
{
  int ret; //variable to check the return values of the system call APIs
  int shm_id; //variable to save the shared memory returned from the shmget.


  /*------------Shared memory get and attach------------*/
  shm_id = shmget(KEY , 4096 , IPC_CREAT | 0600); // create and/or get the shared memory
  if (shm_id < 0 )
  {
    perror("error in creating the shared memory");
    exit(1);
  }

  shma = shmat(shm_id, 0, 0); // attach the shared memory to the virtual memory space of the current Process.
  if (shma < 0)
  {
    perror("error in Attaching the Shared Memory");
    exit(2);
  }

  
  /*------------Thread Semaphore initialization------------*/
  ret = sem_init( &(shma->sema1), 2, 1); // initialize the thread Semaphore which is inside the shared memory.
  if (ret < 0 )
  {
    perror("error in Initializing of the Thread Semaphore.");
    exit(3);
  }
  ret = sem_init( &(shma->sema2), 2, 0);// initialize the thread Semaphore which is inside the shared memory.
  if ( ret < 0 )
  {
    perror("error in Initializing of the Thread Semaphore.");
    exit(4);
  }

  
  /*------------Thread Mutex initialization------------*/
  pthread_mutex_init( &(shma->mutex) , NULL );// initialize the thread Mutex which is inside the shared memory.
  if ( ret < 0 )
  {
    perror("error in Initializing of the Thread Semaphore.");
    exit(5);
  }

  shma->RdIndex = 0; // initializing the Read Index of the User space pipe IPC
  shma->WrIndex = 0;  // initializing the Write Index of the User space pipe IPC
  shma->BufSizeMax = 25;  // initializing the Max_Buffer_Size of the User space pipe IPC
  shma->UsedSlotCount = 0;  // initializing the Used_Slot_Count of the User space pipe IPC

  ret = pthread_create( &ThId , NULL , threadProd , NULL); // Create a Producerthread.
  if (ret != 0)
  {
    perror("Error in thread Creation for ThreadProducer:\n");
    printf("Error Number:%d\n",ret);
    exit(ret);
  }
  pthread_join( ThId , NULL); // This process/application will not be terminated without terminating rthe thread it has started.

  exit(0);

}//main:end


void * threadProd(void *ProdVPtr){
  char value; // creating a buffer variable to save the given input temporarily.
    printf("---------Producer Entered CSection---------\nWaiting for the User to input Character or a String...\n"); // indicate that the thread has gotten access to the  Thread Semaphore.
  
  while(1){
    
    sem_wait( &(shma->sema1) ); // Decrement the thread semaphore to enter the critical section and modify.
    pthread_mutex_lock( &(shma->mutex) ); // lock the mutex to get extra protection.
    
    value = getchar();

    if (shma->UsedSlotCount < shma->BufSizeMax){

      shma->BufArea[shma->WrIndex] = value;
      shma->WrIndex = (shma->WrIndex + 1) % shma->BufSizeMax; // as the buffer is circular I increment the write index untill it reaches BuffSizeMax, after that it will automatically be reinitialized to 0.
      shma->UsedSlotCount++; //after reading one character, increment the Used Slot Count variable, because 1 character has been produced and the slot has been consumed.
    
    }

    
    pthread_mutex_unlock( &(shma->mutex) ) ; // UNlock the mutex which was used to get low-level protection.
    sem_post( &(shma->sema2)); // Increment the other thread semaphore to exit the critical section and indicate the consumer to get active and start consuming the data..
    
    
  }//threadProd:while:end
    printf("---------Producer Exited CSection---------\n"); // indicates that the thread has exited the critical section and the consumer must consume the data now..

  pthread_exit(0);
}//threadProd:end
