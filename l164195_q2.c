#include<string.h>
#include <pthread.h> 
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h> 
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <time.h>
#include <stdlib.h>


struct data
{
	char arr1[50];
	char arr2[50];
	int index;
};

sem_t general;
sem_t sem1;
sem_t sem2;


int main(int argc, char const *argv[])
{
	//printf("welcome to the semaphores \n");
	sem_init(&general, 1, 0); //1 means semaphore is used for process synchronization
	sem_init(&sem1, 1, 0);
	sem_init(&sem2, 1, 1);
	int val1;	
	int val2;
	
	//int id_sem=shmget(313, 1024, 0666 | IPC_CREAT | IPC_EXCL);
	//int id_sem2=shmget(315, 1024, 0666 | IPC_CREAT | IPC_EXCL);
	//int mem= shmget(8, 1024,  0666| IPC_CREAT | IPC_EXCL);
	
	int id_cs = shmget(37281,sizeof(struct data),0600|IPC_CREAT|IPC_EXCL);   //memory ceation
        
        if (id_cs < 0)
        {
   
             printf("ERROR!\n");
             return 1;
        }

        void* memory_creator = shmat(id_cs, NULL ,0);
   	    struct data* helper = (struct data*) memory_creator;


	int pid=fork();

	if (pid>0)   //file reading and writing from 1 
	{
		sleep(2);
	    	printf("IN PROCESS A\n");
    		char c1[50];
        	FILE *ptr1;
        	ptr1=fopen("file-1.txt","r");
        	if (ptr1==NULL)
        	{
        		printf("cannot open the file \n");
        	}
        	else
        	{
        
        	fgets(c1,50,ptr1);
    		strncpy(helper->arr1,c1,10);
      	    helper->arr1[10]='\0';
      	    helper->index=strlen(helper->arr1);   //save the lengyh for process 2 
        	sem_post(&sem2);	

        }
    }
	else
	{
		pid=fork();

		if(pid>0)
		{
			sleep(2);
			printf("IN PROCESS B\n");

			int j=0;
			FILE *ptr2;
			char c2[50];
        	
        	ptr2=fopen("file-2.txt","r");
        	if (ptr2==NULL)
        	{
        		printf("cannot open the file \n");
        	}
        	else
        	{
        		int j=0;
        	    fgets(c2,50,ptr2);

    		sem_wait(&sem2);                      //process A will be started first 
    		
    		for (int i = helper->index; i < 20; ++i)
    		{
    			helper->arr1[i]=c2[j++];                //copy all data 
    		}
      	    helper->arr1[20]='\0';
      	    printf("After Memory Writing : %s\n",helper->arr1);
      	    sem_post(&sem1);                          //semaphore opened

        }
    }
		else
		{
			pid=fork();

			if(pid>0)
			{

				sem_getvalue(&sem1,&val1);
				sem_getvalue(&sem2,&val2);     //getting values of both threads
				if(val1!=val2)                  //if both the semaphores are completed that is process is completed
				{
		    		sleep(2);                       
			    	printf("In Process C \n");
	             	strncpy(helper->arr2,helper->arr1,20);
	            	printf("Data is copied \n");
	         	}
				
			}
			else
			{
				pid=fork();

				if(pid>0)
				{
					sleep(2);
					printf("IN PROCESS D \n");
                	printf("Data from array 2 is : %s\n",helper->arr2 );
                	sem_post(&general);

                	shmdt(memory_creator);             //clearing memory 
                	shmdt(id_cs);
                	shmctl(id_cs,IPC_RMID,NULL);

					//function_processD();
				}
			}
		}
	}



	return 0;
}