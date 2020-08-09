#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>

#include<stdio.h>
#include<stdlib.h>

#define KEY1 1995

typedef struct shmarea {
  int count1;
  int count2;
}SHM; 

SHM *shma;// Global pointer to struct shmarea (aka SHM) initialized.

int main()
{

   int ret,status,id;

   unsigned long int i=0;

   id =  shmget(KEY1,4096,IPC_CREAT|0600);
   if(id<0){
       perror("shmInit:error in shmget\n"); exit(1);
    }
     
   shma = shmat(id,0,0);  
   if(shma == (struct shmarea*)-1) { 
	   perror("shmInit:error in shmat\n"); exit(2); 
   }
   else{
        printf("shmInit:Entered Init, Initialized to 0 \n");
   }

   shma->count1 = 0;
   shma->count2 = 0;
   return 0;
}

