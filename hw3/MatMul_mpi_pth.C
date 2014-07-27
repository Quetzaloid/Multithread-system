#include <mpi.h>
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

int    *inputMat   = NULL;
int    *chunkMat   = NULL;
int    *outputMat  = NULL;
int     numsize    = 0;
int     chunksize  = 0;
int     NumThreads = 1;
int     ChunkSize  = 0;

int     myid;
int     numprocs;

void    mas_fn(); 
void    sla_fn();
void   *thr_fn(void *thIdx);
void   *mas_thr_fn(void *thIdx);

int main(int argc, char *argv[]) 
{
    MPI_Status status; 

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs); 

    if (numprocs<1) 
       { 
         cout<<"!!!!!ERROR: THERE SHOULD BE AT LEAST 2 PROCESSES ..."<<endl;  
         MPI_Abort(MPI_COMM_WORLD, 99); 
       }
    ////////////////////////////////////////////////////////////////////////
    ifstream iFile;

    string fileName = argv[2];
    iFile.open(fileName.c_str());
    if (!iFile)
       {
         cout<<"Error: Cannot open file !"<<endl;
         exit(1);
       }
    iFile>>numsize>>numsize;
    chunksize = numsize/numprocs;
    MPI_Bcast(&numsize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&chunksize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    inputMat  = (int*)malloc(sizeof(int)*numsize*numsize);
    chunkMat  = (int*)malloc(sizeof(int)*chunksize*numsize);
    outputMat = (int*)malloc(sizeof(int)*numsize*numsize);

    /////////////////////////////////////////////////////////////////////////
    for(int i=0; i<numsize*numsize; i++)
       {
         iFile>> inputMat[i];
       }
    iFile.close();

    NumThreads = atoi(argv[1]);
    MPI_Bcast(&NumThreads, 1, MPI_INT, 0, MPI_COMM_WORLD);
    ThParam  = (pthread_t *)malloc(sizeof(pthread_t)*NumThreads);
    ThHandle = (pthread_t *)malloc(sizeof(pthread_t)*NumThreads);

    if (myid==0)
       {
         mas_fn();
       } 
    else
       {
         sla_fn();
       }

    MPI_Finalize();
    return EXIT_SUCCESS;
}

void mas_fn() 
{ 
    MPI_Status status;

    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);     

    for(int i=1; i<numprocs; i++)
       {
         MPI_Send(inputMat, numsize*numsize, MPI_INT, i, 0, MPI_COMM_WORLD);
       }

    ////////////////////////////////////////////////////////////////////////////////////////
    ChunkSize = chunksize/NumThreads;
    //////////////////////////////////////////////
    ///////MATRIX MULTIPLICATION//////////////////
    //////////////////////////////////////////////
    int ThCreate;
    for(int i=0; i<NumThreads; i++)
       {
         ThParam[i] = i;
         ThCreate   = pthread_create(&ThHandle[i], NULL, mas_thr_fn, (void *)&ThParam[i]);
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

    ////////////////////////////////////////////////////////////////////////////////////////
    for(int i=1; i<numprocs; i++)
       {
         MPI_Recv(chunkMat, chunksize*numsize, MPI_INT, i, 3, MPI_COMM_WORLD, &status);
         for(int j=0; j<chunksize; j++)
            {
              for(int k=0; k<numsize; k++)
                 {
                   outputMat[(i*chunksize+j)*numsize+k] = chunkMat[j*numsize+k];
                 }
            }
       }
    ////OUTPUT
    for(int i=0; i<numsize; i++) 
       { 
         for(int j=0; j<numsize; j++) 
            { 
              cout<< outputMat[i*numsize+j]<< " "; 
            } 
         cout<<endl;
       }
    return;
}

void sla_fn() 
{ 
    MPI_Status status;

    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    MPI_Recv(inputMat, numsize*numsize, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    ////////////////////////////////////////////////////////////////////////////////////////
    ChunkSize = chunksize/NumThreads;
    //////////////////////////////////////////////
    ///////MATRIX MULTIPLICATION//////////////////
    //////////////////////////////////////////////
    int ThCreate;
    for(int i=0; i<NumThreads; i++)
       {
         ThParam[i] = i;
         ThCreate   = pthread_create(&ThHandle[i], NULL, thr_fn, (void *)&ThParam[i]);
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

    ////////////////////////////////////////////////////////////////////////////////////////
    MPI_Send(chunkMat, chunksize*numsize, MPI_INT, 0, 3, MPI_COMM_WORLD);
    return;
}

void  *thr_fn(void *thIdx)
{
  int tIndex = *((int *) thIdx);

  for(int n=0; n<ChunkSize; n++)
     {
       int i = myid*chunksize + tIndex + n*NumThreads;
       for(int j=0; j<numsize; j++)
          {
            int DotProduct=0;
            for(int k=0; k<numsize; k++)
               {
                 DotProduct += inputMat[i*numsize+k]*inputMat[k*numsize+j];
               }
            chunkMat[(i-myid*chunksize)*numsize+j] = DotProduct;
          }
     }
}

void  *mas_thr_fn(void *thIdx)
{
  int tIndex = *((int *) thIdx);

  for(int n=0; n<ChunkSize; n++)
     {
       int i = tIndex + n*NumThreads;
       for(int j=0; j<numsize; j++)
          {
            int DotProduct=0;
            for(int k=0; k<numsize; k++)
               {
                 DotProduct += inputMat[i*numsize+k]*inputMat[k*numsize+j];
               }
            outputMat[i*numsize+j] = DotProduct;
          }
     }
}