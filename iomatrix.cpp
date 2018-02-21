#include "iomatrix.h"
#include <stdio.h>
#include <iostream>
#include <fstream> 
#include <iomanip>  
#include <stdlib.h>
#include <m4ri/m4ri.h>
#include <m4ri/io.h>




using namespace std;

void store_matrix(mzd_t* M, char *name)
{
    int data;
    int i,j;

    ofstream outfile;
    outfile.open(name, ios::out);  
    outfile<<M->nrows<<endl;
    outfile<<M->ncols<<endl;

    for(i=0; i<M->nrows; i++){
        for(j=0;j<M->ncols; j++){
            outfile<<mzd_read_bit(M,i,j)<<endl;
        }
     }
    outfile.close();
    
}

mzd_t* read_matrix( char* name)
{
    int i;
    int data;
    int rows, cols;

    ifstream infile;
    infile.open(name,ios::in); 
    infile >> rows;
    //cout<<rows<<endl;
    infile >> cols;
    //cout<<cols<<endl;

    char str[rows*cols]; 
    for(i=0;i<rows*cols;i++)
    {
       infile>>data ; 
       if(data==0)  str[i]='0';
       else  str[i]='1';
    }
    infile.close();
    return mzd_from_str(rows, cols, str);
}



