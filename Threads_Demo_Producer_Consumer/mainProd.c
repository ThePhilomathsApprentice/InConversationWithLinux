#include"MyHeader.h"

int main()
{
  int ret; //variable to check the return values of the system call APIs
  int shm_id; //variable to save the shared memory returned from the shmget.


  /* Shared memory get and attach
   */

  //create and/or get the shared memory
  shm_id = shmget(KEY , 4096 , IPC_CREAT | 0600);
  if (shm_id < 0 )
  {
    perror("error in creating the shared memory");
    exit(1);
  }

  // attach the shared memory to the virtual memory space of the current Process.
  shma = shmat(shm_id, 0, 0);
  if (shma < 0)
  {
    perror("error in Attaching the Shared Memory");
    exit(2);
  }


  /* Thread Semaphore initialization
   */

  // initialize the thread Semaphore which is inside the shared memory.
  ret = sem_init( &(shma->sema1), 2, 1);
  if (ret < 0 )
  {
    perror("error in Initializing of the Thread Semaphore.");
    exit(3);
  }

  // initialize the thread Semaphore which is inside the shared memory.
  ret = sem_init( &(shma->sema2), 2, 0);
  if ( ret < 0 )
  {
    perror("error in Initializing of the Thread Semaphore.");
    exit(4);
  }


  /* Thread Mutex initialization
   */

  // initialize the thread Mutex which is inside the shared memory.
  pthread_mutex_init( &(shma->mutex) , NULL );
  if ( ret < 0 )
  {
    perror("error in Initializing of the Thread Semaphore.");
    exit(5);
  }

  shma->RdIndex = 0; // initializing the Read Index of the User space pipe IPC
  shma->WrIndex = 0;  // initializing the Write Index of the User space pipe IPC
  shma->BufSizeMax = 25;  // initializing the Max_Buffer_Size of the User space pipe IPC
  shma->UsedSlotCount = 0;  // initializing the Used_Slot_Count of the User space pipe IPC

  // Create a Producerthread.
  ret = pthread_create( &ThId , NULL , threadProd , NULL);
  if (ret != 0)
  {
    perror("Error in thread Creation for ThreadProducer:\n");
    printf("Error Number:%d\n",ret);
    exit(ret);
  }

  // This process/application will not be terminated without terminating the thread it has started.
  pthread_join( ThId , NULL);

  exit(0);

}//main:end


void * threadProd(void *ProdVPtr){
  // creating a buffer variable to save the given input temporarily.
  char value;
    // indicate that the thread has gotten access to the  Thread Semaphore.
    printf("---------Producer Entered CSection---------\n \
            Waiting for the User to input Character or a String...\n");

  while(1){

    // Decrement the thread semaphore to enter the critical section and modify.
    sem_wait( &(shma->sema1) );

    // lock the mutex to get extra protection.
    pthread_mutex_lock( &(shma->mutex) );

    value = getchar();

    if (shma->UsedSlotCount < shma->BufSizeMax){

      shma->BufArea[shma->WrIndex] = value;

      // as the buffer is circular I increment the write index 
      // untill it reaches BuffSizeMax,
      // after that it will automatically be reinitialized to 0.
      shma->WrIndex = (shma->WrIndex + 1) % shma->BufSizeMax;

      // after reading one character,
      // increment the Used Slot Count variable,
      // because 1 character has been produced and the slot has been consumed.
      shma->UsedSlotCount++;

    }

    // UnLock the mutex which was used to get low-level protection.
    pthread_mutex_unlock( &(shma->mutex) ) ;

    // Increment the other thread semaphore to exit the critical section and
    // indicate the consumer to get active and start consuming the data..
    sem_post( &(shma->sema2));


  }//threadProd:while:end

  // indicates that the thread has exited the critical section and
  // the consumer must consume the data now..
  printf("---------Producer Exited CSection---------\n");

  pthread_exit(0);
}//threadProd:end
