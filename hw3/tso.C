#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <cassert>
#include <pthread.h>
#include <sys/time.h>

using   namespace std;

pthread_t   *ThParam;
pthread_t   *ThHandle;

int          A=0;
int          B=0;

void  *thr_fn(void *thIdx);


int main(int argc, char *argv[]) 
{
    int NumThreads = 2;

    ThParam  = (pthread_t *)malloc(sizeof(pthread_t)*NumThreads);
    ThHandle = (pthread_t *)malloc(sizeof(pthread_t)*NumThreads);

    int ThCreate;
    int count=0;
    FILE* fp = 0;
    fp = freopen("tso_out", "w" ,stdout);

    while(count<1000000)
      {
        A=0;
        B=0;
        for(int i=0; i<NumThreads; i++)
           {
             ThParam[i] = i;
             ThCreate = pthread_create(&ThHandle[i], NULL, thr_fn, (void *)&ThParam[i]);
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
        count++;
        printf(" ");
      }
    fclose(fp);
    return(0);
}

void  *thr_fn(void *thIdx)
{
  int idx = *((int *) thIdx);
  if (idx==0)
     {
       A=1;
       printf("%d", B);
     }
  else if (idx==1)
     {
       B=1;
       printf("%d", A);
     }
}




























