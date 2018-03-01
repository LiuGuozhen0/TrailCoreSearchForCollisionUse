/*
 * This text is created to implement some algorithms when necessary.
 */

 #ifndef _TESTTOOLS_H_
 #define _TESTTOOLS_H_

 #include <cstdlib>
 #include <fstream>
 #include <iostream>
 #include <sstream>
 #include <string.h>
 #include "duplex.h"
 #include "Keccak.h"
 #include "KeccakCrunchyContest.h"
 #include "Keccak-f25LUT.h"
 #include "Keccak-fCodeGen.h"
 #include "Keccak-fDCEquations.h"
 #include "Keccak-fDCLC.h"
 #include "Keccak-fEquations.h"
 #include "Keccak-fPropagation.h"
 #include "Keccak-fTrailExtension.h"
 #include "Keccak-fTrails.h"
 #include "Keyakv2-test.h"
 #include "Keccak-fTrailCoreInKernelAtC.h"
 #include "Keccak-fDisplay.h"
 #include "compareTrails.h"
 #include "misc.h"

 #include <iostream>
 //#include "efficiency.h"
 #include <sys/time.h>
 #include <time.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <string.h>//memset()

 using namespace std;

 int display_intermediate_state_on_screen(uint64_t state[5][5]);
 void generateChoices(int choices[32]);
 void fromTrailsTo64bitWords1(const string& fileNameIn);
 int computeWeightOfW1(void);
 int instantFormatTransformation(void);

 #endif
