/*
The Keccak sponge function, designed by Guido Bertoni, Joan Daemen,
Michaël Peeters and Gilles Van Assche. For more information, feedback or
questions, please refer to our website: http://keccak.noekeon.org/

Implementation by the designers,
hereby denoted as "the implementer".

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/
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

#include "testtools.h"

using namespace std;



void fromTrailsTo64bitWords(const string& fileNameIn)
{
    vector<LaneValue> lanes(25);
    ifstream fin(fileNameIn.c_str());
	unsigned int count = 1;
    while(!(fin.eof())) {
        try {
			cout<<"No. "<<count<<endl;
            Trail trail(fin);
			//fromSlicesToLanes(trail.states[0], lanes);
			//displayStateAs64bitWords(lanes);
			fromSlicesToLanes(trail.states.back(), lanes);
			displayStateAs64bitWords(lanes);
			cout<<endl;
		    count++;
        }
        catch(TrailException) {
        }

    }
}

void extendForward3Rounds1(KeccakFPropagation::DCorLC DCLC, unsigned int width, const string & inFileName)
{
//    KeccakInitializeRoundConstants();
//    KeccakInitializeRhoOffsets();
    KeccakFDCLC keccakF(width);
    cout<<endl;
    KeccakFTrailExtension keccakFTE(keccakF, DCLC);
    cout << keccakF << endl<<endl;//???

    TrailFileIterator iTrailsIn(inFileName, keccakFTE);
    cout << iTrailsIn << endl;
    string outFileName = inFileName + string("-check1");
    ofstream fout(outFileName.c_str());
    TrailSaveToFile iTrailsOut(fout);
    keccakFTE.forwardExtendTrails(iTrailsIn,iTrailsOut,2,36);

}

int main(int argc, char *argv[])
{

	KeccakFPropagation::DCorLC aDCorLC;
	aDCorLC = KeccakFPropagation::DC;

	// string fileName = "coreb52-224-check100";
	// string fileName1 = "coreb52";
  //   string fileName11="coreb52-check";
  //   string fileName2 = "coreb60m64new-plus";
  //   readAndPrint(aDCorLC, 1600, fileName1);
  //   myExtendBackward(aDCorLC, 1600, fileName);  // in compareTrails.h
  //   extendTrails(aDCorLC, 1600, fileName1, 2, 160, false);
  //
  //   myExtendForward(aDCorLC, 1600, fileName2);
  //   myExtendBackward(aDCorLC, 1600, fileName2);
  //
  //   myExtendForward(aDCorLC, 1600, fileName1); // in compareTrails.h
  //   myExtendBackward(aDCorLC, 1600, fileName11);
  //
  //   computeWeightOfW1();
  //   string fileName3 = "coreb52-1";
  //   string fileName33 = "coreb52-1-check1";
  //   string fileName333 = "coreb52-1-check1-check";
  //   extendForward3Rounds1(aDCorLC, 1600, fileName3);
  //   myExtendForward(aDCorLC, 1600, fileName33);
  //
  //   string fileName3 = "coreb60m64new-plus-1";
  //   string fileName33 = "coreb60m64new-plus-1-check1";
  //   string fileName333 = "coreb60m64new-plus-1-check1-check";
  //   extendForward3Rounds1(aDCorLC, 1600, fileName3);
  //   myExtendForward(aDCorLC, 1600, fileName33);
  //   myExtendBackward(aDCorLC, 1600, fileName333);
  //
  //   instantFormatTransformation();
  //
  //   string trails="coreb52-1-check1-check";
  //   fromTrailsTo64bitWords1(trails);

    // string testFile = "test.txt";
    string inFile = "DCKeccakF-1600-TwoRoundTrailCoresInKernel-Below-8-w1";
    // Extend2RTrailCoreBackaward(aDCorLC, 1600, testFile);
    // Extend2RTrailCoreBackaward(aDCorLC, 1600, inFile);

    string findWeightFile = "DCKeccakF-1600-TwoRoundTrailCoresInKernel-Below-8-w1-BackwardExtensionWithASlessThan8atA";
    extendForwardBy1ROutputMinWeight(aDCorLC, 1600, findWeightFile);

    return EXIT_SUCCESS;
}
