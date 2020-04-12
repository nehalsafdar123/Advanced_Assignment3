#include<pthread.h>
#include<stdio.h>
#include <time.h>
#include <semaphore.h> 
#include<string.h>
#include <unistd.h>



int potentialCPatients=1;
int present=1;
int last=1;

sem_t coronaPatients;
sem_t fluPatients;
sem_t mutex;


void coronapatientTest()
{
	 
		int rand=randomGenertor();
		last=present;
		present=rand;
		if (present==0 && last == 0)
		{
			sem_wait(&coronaPatients);

		}
}


int randomGenertor()
{
	int r=rand() % 2;    ///* random int between 0 and 1 */
	return r;
}


void coronaTestCenter()   //test centre is called to add to flue and corona patient
{
   int rand=randomGenertor();
   if (rand==0)
   { 
   	 sem_post(&fluPatients);
   	 potentialCPatients--;

   }
   else
   {
      sem_post(&coronaPatients);
      potentialCPatients--; 
   }
}

void * patient()
{                                      // no of pateints equal to ebtred no are created

	sem_wait(&mutex);                  //muutually exclusive to get correct patients
	potentialCPatients++;
	sem_post(&mutex);
	pthread_exit(NULL);
}




int main(int argc, char const *argv[])
{

	sem_init(&mutex, 0, 1);
	sem_init(&coronaPatients,0,0);
	sem_init(&fluPatients,0,0);

	pthread_t ptid;
	int n;
	int value;

	printf("Enter the no of threads you want run for \n");
	scanf("%d",&n);


	printf("Enter numbere is %d \n",n);


	for(int i=0;i<n;i++)
	{
		 
          pthread_create(&ptid,NULL,&patient,NULL);   
         
	}
    printf("No of patient counts are %d\n",potentialCPatients);


    for (int i = 0; i < potentialCPatients; ++i)   //called till no of patient
    {
    	coronaTestCenter();
    	/* code */
    }

    sem_getvalue(&coronaPatients,&value); //to get the value of semaphore 

    while (value>0){                   //test of further coronapatients
    	coronapatientTest();
    	value--;
}

	return 0;
}
