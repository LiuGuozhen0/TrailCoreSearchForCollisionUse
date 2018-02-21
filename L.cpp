
#include "L.h"



/**
 * \brief generation of INVERSE OF theta.rho.pi 
 *
 *  L: Matrix, the matrix of theta.rho.pi
 *
 * \return a pointer to LI
 */
mzd_t *gener_L(){
    KeccakInitializeRoundConstants();
    KeccakInitializeRhoOffsets();
    char *Lchar = new char[2560000]; 
    generLT(Lchar);
    mzd_t *L = mzd_from_str(1600, 1600, Lchar);
    delete [] Lchar;
    return L;
}

mzd_t *gener_LI()
{
    mzd_t *L = gener_L();

    mzd_t * LI = mzd_inv_m4ri( NULL, L, 0);
    mzd_free(L);
    return LI;

}



