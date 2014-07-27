#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>

using   namespace std;

pthread_t   *ThParam;
pthread_t   *ThHandle;

int        **inputMat   = NULL;
int        **outputMat  = NULL;
int          NumThreads = 1;
int          ChunkSize  = 0;
int          RemainDer  = 0;
int          numCols, numRows;

void  *thr_fn(void *thIdx);


int main(int argc, char *argv[]) 
{
    NumThreads = 1;

    if ( argc>0 )
       {
         NumThreads = atoi(argv[1]);
       }
    ThParam  = (pthread_t *)malloc(sizeof(pthread_t)*NumThreads);
    ThHandle = (pthread_t *)malloc(sizeof(pthread_t)*NumThreads);

    /////////////////////////////
    cin>> numCols>> numRows;
    inputMat  = (int **)malloc(sizeof(int *)*numRows);
    for(int i=0; i<numRows; i++)
       {
         inputMat[i]  = (int *)malloc(sizeof(int)*numCols);
       }
    outputMat = (int **)malloc(sizeof(int *)*numRows);
    for(int i=0; i<numRows; i++)
       {
         outputMat[i] = (int *)malloc(sizeof(int)*numCols);
       }
    //Input Data
    for(int i=0; i<numRows; i++)
       {
         for(int j=0; j<numCols; j++)
            {
              cin>>  inputMat[i][j];
            }
       }

    ChunkSize = numRows/NumThreads;
    RemainDer = numRows%NumThreads;
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

    for(int i=0; i<numRows; i++)
       {
         for(int j=0; j<numCols; j++)
            {
              cout<<outputMat[i][j]<<" ";
            }
         cout<<endl;
       }


    ///////////Memory Free
    for(int i=0; i<numRows; i++)
       {
         free(inputMat[i]);
       }
    free(inputMat);
    for(int i=0; i<numRows; i++)
       {
         free(outputMat[i]);
       }
    free(outputMat);
    free(ThParam);
    free(ThHandle);
    return(0);
}

void  *thr_fn(void *thIdx)
{
  int tIndex = *((int *) thIdx);

  for(int i=tIndex; i<numCols*numRows; i+=NumThreads)
     {
       int rowIndex = i/numCols;
       int colIndex = i%numCols;
       int DotProduct = 0;
       for(int j=0; j<numRows; j++)
          {
            DotProduct += inputMat[rowIndex][j] * inputMat[j][colIndex];
          }
       outputMat[rowIndex][colIndex] = DotProduct;
     }
}