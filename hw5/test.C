#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

using namespace std;

#define TEST_NUM  4

pthread_t  *ThParam;
pthread_t  *ThHandle;
int        waiting;
int        count=0;
int        NumThreads;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition = PTHREAD_COND_INITIALIZER;

void barrier ();
void  *thr_fn(void *thIdx);


int main(int argc, char *argv[])
{
    struct timeval start, finish;
    double error;
    
    if (argc < 2) 
       {
         exit(1);
       }
    else
       {
         NumThreads = atoi(argv[1]);
       }

    waiting  = NumThreads;
    ThParam  = (pthread_t *)malloc(sizeof(pthread_t)*NumThreads);
    ThHandle = (pthread_t *)malloc(sizeof(pthread_t)*NumThreads);
    if (!ThParam || !ThHandle)  
       {
         cout<<"!!!!ERROR: MEMORY ALLOCATING FAILS!!!"<<endl;
         exit(1);
       }

    int ThCreate;
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);    

    //////////////////////////////////////////////////////
    for(int i=0; i<NumThreads; i++)
       {
         ThParam[i] = i;
         ThCreate   = pthread_create(&ThHandle[i], &attr, thr_fn, (void *)&ThParam[i]);
         if (ThCreate != 0)
            {
              cout<<"!!!!!THREAD "<<i<<" CREATION FAILED ... Exiting abruptly"<<endl<<endl;
              exit(1);
            }
       }
    for(int i=0; i<NumThreads; i++)
       {
         pthread_join(ThHandle[i], NULL);
       }
    ///////////////////////////////////////////////////////

    return 0;
}

void barrier()
{
    pthread_mutex_lock (&mutex);

    waiting--;
    if (waiting > 0)
       {
         pthread_cond_wait (&condition, &mutex);
       }
    else 
       {
         waiting = NumThreads;
         pthread_cond_broadcast (&condition);
       }
    pthread_mutex_unlock (&mutex);
}


void  *thr_fn(void *thIdx)
{
    int tIndex = *((int *) thIdx);
    
    for(int i=0; i<TEST_NUM; i++) 
       {         
         barrier();
         count++;
         barrier();
         if (tIndex==0)
            {
              cout<<"There are totally "<<count<<" threads"<<endl;
              count=0;
            }
       }
}
