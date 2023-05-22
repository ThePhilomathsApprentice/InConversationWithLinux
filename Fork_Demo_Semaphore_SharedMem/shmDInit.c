#include "myHeader.h"

extern SHM *shma;// Global pointer to struct shmarea (aka SHM) initialized.

int shmDInit()
{

   int ret,status,id;

   unsigned long int i=0;

   id =  shmget(KEY1,4096,IPC_CREAT|0600);
   if(id<0){
       perror("shmDInit:error in shmget\n"); exit(1);
    }

   shma = shmat(id,0,0);
   if(shma == (struct shmarea*)-1) {
	   perror("shmDInit:error in shmat\n"); exit(2);
   }
   else{
        printf("shmDInit(%d):Entered DInit, Reinitialzed to 0 \n",id);
   }

   shma->count1 = 0;
   shma->count2 = 0;


   return 0;
}

