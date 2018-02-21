#ifndef IOMATRIX_H
#define IOMATRIX_H

#include <stdio.h>
#include <iostream>
#include <fstream> 
#include <iomanip>  
#include <stdlib.h>
#include <m4ri/m4ri.h>
#include <m4ri/io.h>




using namespace std;

void store_matrix(mzd_t* M, char *name);

mzd_t* read_matrix( char* name);




#endif

