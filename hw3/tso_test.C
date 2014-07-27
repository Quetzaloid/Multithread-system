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

int main(int argc, char *argv[]) 
{
    ifstream iFile;
    iFile.open("tso_out");

    int count=0;
    int abnormal=0;
    int nIdx;
    while(count<1000000)
      {
        iFile>>nIdx;
        if (nIdx==0)
           {
             abnormal++;
            cout<<"abnormal at"<<count<<endl;
           }
        count++;
      }
   iFile.close();
   cout<<"There are totally "<<abnormal<<" abnormal index shows W-R reoedering"<<endl;
    return(0);
}



























