#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t readCountAccess; //semaphore for the readCount var
sem_t databaseAccess;  //semaphore for the SharedValue
int readCount=0;
int sharedValue=0;
int writeCount=0;
void *Reader(void *arg);
void *Writer(void *arg);

int main(  int argc, char * argv [])
{int WThread, RThread;   //The variables to hold the inputed number of reader and writer threads

	if(argc!=3){ printf("input the number of readers and number of writers \n");  //check the command line arguments
				return 0;}
	else{
		int d = atoi(argv[1]); 
		if(d<1){printf("The number of readers should be a positive integer\n");
					return 0;}
		RThread=d;
		d = atoi(argv[2]); 
		if(d<1){printf("The number of writers should be a positive integer\n");
				return 0;}
		WThread=d;
	}
	
    	


int i=0;
sem_init(&readCountAccess,0,1); //initalize the semaphores
sem_init(&databaseAccess,0,1);

pthread_t Readers_thr[100],Writer_thr[100];

for(i=0;i<WThread;i++)  //creat the writer threads
	{ pthread_create(&Writer_thr[i],NULL,Writer,(void *)i);
}

for(i=0;i<WThread;i++)  //create the reader threads
	{ pthread_create(&Readers_thr[i],NULL,Reader,(void *)i);
}

for(i=0;i<WThread;i++)  //join writer threads
	{ pthread_join(Writer_thr[i],NULL);
}

for(i=0;i<RThread;i++) //join the reader threads
	{ pthread_join(Readers_thr[i],NULL);
}

sem_destroy(&databaseAccess);
sem_destroy(&readCountAccess);

return 0;
}

void * Writer(void *arg)
{

//sleep(1);
int temp=(int)arg;  //the writer number

sem_wait(&databaseAccess); //lock the CS
printf("Writer %d is entering the database\n",temp);
sharedValue++; //Update the Shared Value
printf("Writer %d updated the value to %d \n",temp, sharedValue);
printf("Writer %d is leaving the database \n");
sem_post(&databaseAccess); //unlock the shared value
}

void *Reader(void *arg)
{
//sleep(1);
int temp=(int)arg;  //the reader number

sem_wait(&readCountAccess); //lock the readaccess
printf("Reader %d is entering into the Database \n",temp);
readCount++; //increment the reader count
if(readCount==1)
{
sem_wait(&databaseAccess);  //if it is the only reader enter the database
printf("Reader %d is reading the database %d \n",temp, sharedValue);
}
sem_post(&readCountAccess); //exit CS
sem_wait(&readCountAccess);
readCount--;  //decrement readcount for exiting reader
if(readCount==0)  //if readcount is 0, it is done exiting
{
printf("Reader %d is leaving the database \n",temp);
sem_post(&databaseAccess);
}
sem_post(&readCountAccess); // unlock the readaccess
}


// My views
// The the results were as expected, after a writer has completed, usually the reader took over and so on. But because the threads can loose cpu, the output is sometimes mixed up
//forexample a writer enters but before it updates and exits a reader enters the database, prints its message and exits.
//