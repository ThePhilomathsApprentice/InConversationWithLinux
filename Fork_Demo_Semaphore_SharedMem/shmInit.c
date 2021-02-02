#include "myHeader.h"

extern SHM *shma;// Global pointer to struct shmarea (aka SHM) initialized.

int shmInit()
{

   int ret,status,id;

   unsigned long int i=0;

   id =  shmget(KEY1 , 4096 , IPC_CREAT | IPC_EXCL | 0666 );
   if(id<0){
       perror("shmInit:error in shmget\n"); exit(1);
    }
     
   shma = shmat(id,0,0);  
   if(shma == (struct shmarea*)-1) { 
	   perror("shmInit:error in shmat\n"); exit(2); 
   }
   else{
      if( shmctl(id , IPC_RMID , 0) == -1 ){
         perror("shmInit: Failed to configure shared memory.\n");
         exit(1);
      }else{
         printf("shmInit(%d): Entered Init, Initializid to 0 \n",id);
      }
   }

   shma->count1 = 0;
   shma->count2 = 0;
   return 0;
}

