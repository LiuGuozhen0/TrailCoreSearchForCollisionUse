#ifndef _COMPARETRAILS_H_
#define _COMPARETRAILS_H_

#include <map>
#include <vector>
#include "Keccak-fPropagation.h"
#include "progress.h"
#include "kkeccak.h"
#include "List.h"
#include "misc.h"
#include "L.h"

void compareTrails(KeccakFPropagation::DCorLC DCLC, unsigned int width, const string& inFileName);
void readAndPrint(KeccakFPropagation::DCorLC DCLC, unsigned int width, const string& inFileName);
void precompute(mzd_t* LI, vector<vector<UINT64> >& base);
void myExtendBackward(KeccakFPropagation::DCorLC DCLC, unsigned int width, const string& inFileName);

void myExtendForward(KeccakFPropagation::DCorLC DCLC, unsigned int width, const string& inFileName);
void myExtendForward(UINT64 A[25]);

//new added functions
void Extend2RTrailCoreBackaward(KeccakFPropagation::DCorLC DCLC, unsigned int width, const string& inFileName);

#endif
