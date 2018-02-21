/**
 * functions to generate
 * 1. L^-1 = (theta.rho.pi)^-1 
 * 2. L_cplus8, (c+8) rows chosen according to (c+8) zero difference bits
 * 3. L_nonactiveSbox, 5*#nonactiveSbox rows which set some DeltaT varables to be zero
 * DeltaS0 and DeltaT are constant in this file.
 */
#ifndef L_H
#define L_H


#include <stdio.h>
#include <string.h>
#include <m4ri/m4ri.h>
#include "kkeccak.h"



/**
 * \brief generation of INVERSE OF theta.rho.pi 
 *
 *  L: Matrix, the matrix of theta.rho.pi
 *
 * \return a pointer to LI
 */
mzd_t *gener_L();

mzd_t *gener_LI();




#endif


