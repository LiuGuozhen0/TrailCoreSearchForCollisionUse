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

void generateChoices(int choices[32])
{   
    int DDT[32][32];
	differential_distribution_table_origin(DDT);
	int dout,din;
	int count8, count4;
	for(dout = 1; dout<32; dout++){
		count4=0;
		count8=0;
		for(din = 1; din<32; din++){
			if(DDT[din][dout]==8) count8++;
			if(DDT[din][dout]==4) count4++;
		}
		if(count8>0) choices[dout] = 1;
		else choices[dout] = count4;
	}
	
}
#define ROTATIONS_N(x,n) (((x)<<(n))^((x)>>(64-(n))))
#define iROTATIONS_N(x,n) (((x)>>(n))^((x)<<(64-(n))))
//(((n)>0)?(((x)<<(n))^((x)>>(64-(n)))):(x))
//pay attention to the case when n>0

#define THETA()\
c[0]=state[0][0]^state[0][1]^state[0][2]^state[0][3]^state[0][4];\
c[1]=state[1][0]^state[1][1]^state[1][2]^state[1][3]^state[1][4];\
c[2]=state[2][0]^state[2][1]^state[2][2]^state[2][3]^state[2][4];\
c[3]=state[3][0]^state[3][1]^state[3][2]^state[3][3]^state[3][4];\
c[4]=state[4][0]^state[4][1]^state[4][2]^state[4][3]^state[4][4];\
d[0]=ROTATIONS_N(c[0],1);\
d[1]=ROTATIONS_N(c[1],1);\
d[2]=ROTATIONS_N(c[2],1);\
d[3]=ROTATIONS_N(c[3],1);\
d[4]=ROTATIONS_N(c[4],1);\
state[0][0]=state[0][0]^c[4]^d[1];\
state[0][1]=state[0][1]^c[4]^d[1];\
state[0][2]=state[0][2]^c[4]^d[1];\
state[0][3]=state[0][3]^c[4]^d[1];\
state[0][4]=state[0][4]^c[4]^d[1];\
state[1][0]=state[1][0]^c[0]^d[2];\
state[1][1]=state[1][1]^c[0]^d[2];\
state[1][2]=state[1][2]^c[0]^d[2];\
state[1][3]=state[1][3]^c[0]^d[2];\
state[1][4]=state[1][4]^c[0]^d[2];\
state[2][0]=state[2][0]^c[1]^d[3];\
state[2][1]=state[2][1]^c[1]^d[3];\
state[2][2]=state[2][2]^c[1]^d[3];\
state[2][3]=state[2][3]^c[1]^d[3];\
state[2][4]=state[2][4]^c[1]^d[3];\
state[3][0]=state[3][0]^c[2]^d[4];\
state[3][1]=state[3][1]^c[2]^d[4];\
state[3][2]=state[3][2]^c[2]^d[4];\
state[3][3]=state[3][3]^c[2]^d[4];\
state[3][4]=state[3][4]^c[2]^d[4];\
state[4][0]=state[4][0]^c[3]^d[0];\
state[4][1]=state[4][1]^c[3]^d[0];\
state[4][2]=state[4][2]^c[3]^d[0];\
state[4][3]=state[4][3]^c[3]^d[0];\
state[4][4]=state[4][4]^c[3]^d[0];

//state[0][0]=ROTATIONS_N(state[0][0],0); I omit this one
#define RHO()\
state[0][1]=ROTATIONS_N(state[0][1],36);\
state[0][2]=ROTATIONS_N(state[0][2],3);\
state[0][3]=ROTATIONS_N(state[0][3],41);\
state[0][4]=ROTATIONS_N(state[0][4],18);\
state[1][0]=ROTATIONS_N(state[1][0],1);\
state[1][1]=ROTATIONS_N(state[1][1],44);\
state[1][2]=ROTATIONS_N(state[1][2],10);\
state[1][3]=ROTATIONS_N(state[1][3],45);\
state[1][4]=ROTATIONS_N(state[1][4],2);\
state[2][0]=ROTATIONS_N(state[2][0],62);\
state[2][1]=ROTATIONS_N(state[2][1],6);\
state[2][2]=ROTATIONS_N(state[2][2],43);\
state[2][3]=ROTATIONS_N(state[2][3],15);\
state[2][4]=ROTATIONS_N(state[2][4],61);\
state[3][0]=ROTATIONS_N(state[3][0],28);\
state[3][1]=ROTATIONS_N(state[3][1],55);\
state[3][2]=ROTATIONS_N(state[3][2],25);\
state[3][3]=ROTATIONS_N(state[3][3],21);\
state[3][4]=ROTATIONS_N(state[3][4],56);\
state[4][0]=ROTATIONS_N(state[4][0],27);\
state[4][1]=ROTATIONS_N(state[4][1],20);\
state[4][2]=ROTATIONS_N(state[4][2],39);\
state[4][3]=ROTATIONS_N(state[4][3],8);\
state[4][4]=ROTATIONS_N(state[4][4],14);\
i_state[0][0]=state[0][0];\
i_state[0][1]=state[0][1];\
i_state[0][2]=state[0][2];\
i_state[0][3]=state[0][3];\
i_state[0][4]=state[0][4];\
i_state[1][0]=state[1][0];\
i_state[1][1]=state[1][1];\
i_state[1][2]=state[1][2];\
i_state[1][3]=state[1][3];\
i_state[1][4]=state[1][4];\
i_state[2][0]=state[2][0];\
i_state[2][1]=state[2][1];\
i_state[2][2]=state[2][2];\
i_state[2][3]=state[2][3];\
i_state[2][4]=state[2][4];\
i_state[3][0]=state[3][0];\
i_state[3][1]=state[3][1];\
i_state[3][2]=state[3][2];\
i_state[3][3]=state[3][3];\
i_state[3][4]=state[3][4];\
i_state[4][0]=state[4][0];\
i_state[4][1]=state[4][1];\
i_state[4][2]=state[4][2];\
i_state[4][3]=state[4][3];\
i_state[4][4]=state[4][4];

#define Pi()\
state[0][0]=i_state[0][0];\
state[1][3]=i_state[0][1];\
state[2][1]=i_state[0][2];\
state[3][4]=i_state[0][3];\
state[4][2]=i_state[0][4];\
state[0][2]=i_state[1][0];\
state[1][0]=i_state[1][1];\
state[2][3]=i_state[1][2];\
state[3][1]=i_state[1][3];\
state[4][4]=i_state[1][4];\
state[0][4]=i_state[2][0];\
state[1][2]=i_state[2][1];\
state[2][0]=i_state[2][2];\
state[3][3]=i_state[2][3];\
state[4][1]=i_state[2][4];\
state[0][1]=i_state[3][0];\
state[1][4]=i_state[3][1];\
state[2][2]=i_state[3][2];\
state[3][0]=i_state[3][3];\
state[4][3]=i_state[3][4];\
state[0][3]=i_state[4][0];\
state[1][1]=i_state[4][1];\
state[2][4]=i_state[4][2];\
state[3][2]=i_state[4][3];\
state[4][0]=i_state[4][4];



int display_intermediate_state_on_screen(uint64_t state[5][5])
{
    printf("\n");
    for (int i=0; i<5; i++) {
        for (int j=0; j<5; j++) {
            printf("0x%016llx\t, ",state[j][i]);
        }
        printf("\n");
    }
    printf("\n");
    return 0;
}


int instantFormatTransformation(void)
{

/********Compute the matrices of Linear and inverse Linear Transformation**************/
    mzd_t * L=mzd_init(1600,0);
    mzd_t* res1 = mzd_init(1600,1);
    UINT64 c[5],d[5];//intermediate lanes
    UINT64 i_state[5][5],state[5][5];
    UINT64 flagg=0x0000000000000001;
    for (int v=0; v<1600; v++) {
        for (int i=0; i<5; i++) {
            for (int j=0; j<5; j++) {
                state[i][j]=0;
            }
        }
        int m=v/320;
        int n=(v/64)%5;
        UINT64 s=v%64;
        state[n][m]=flagg<<s;

        THETA()
        RHO()
        Pi()

        for (int i=0; i<5; i++) {
            for (int j=0; j<5; j++) {
                for (int t=0; t<64; t++) {
                    mzd_write_bit(res1,(320*i+64*j+t),0,(state[j][i]>>t)&0x1);
                }
            }
        }
        mzd_t * tmp=mzd_concat(NULL,L,res1);
        mzd_free(L);
        L=tmp;//L represents the linear transformation matrix
    }
/********Compute the matrices of Linear and inverse Linear Transformation**************/

    mzd_t * res=mzd_init(1600,1);
    mzd_t * ires=mzd_init(1600,1);
    mzd_t * vv=mzd_init(1600,1);

    //    unsigned int ddt[2][6]={{7,45,142,173,199,206},{16,22,6,3,26,5}};
    unsigned int ddt[2][8]={{9,35,63,127,137,170,227,234},{2,24,1,1,2,4,16,4}};//b2
    //    unsigned int ddt[2][8]={{9,35,63,127,137,170,227,234},{2,16,1,1,2,4,16,4}};//a3
    //      unsigned int ddt[2][8]={{6,48,69,130,204,240,257,287},{8,4,4,4,8,8,1,8}};//b4
    //    unsigned int ddt[2][8]={{6,48,69,130,204,240,257,287},{8,4,4,6,12,12,25,8}};//a5
    //


///*********************************Check Linear Transformation***********************************/
//    UINT64 A[25];
//    for (int j=0; j<8; j++) {
//        for (int i=0; i<5; i++) {
//            mzd_write_bit(vv,((ddt[0][j])*5+i),0,(ddt[1][j]>>i)&0x1);
//        }
//    }
//    unsigned int B2[1600]={0};
//    for (int j=0; j<1600; j++) {
//        B2[j]=mzd_read_bit(vv,j,0);
//    }
//    Bit2State(B2, A);
//
//
//    displayStateAs64bitWords(A);
//    cout<<endl;
//
//    mzd_t * x=mzd_init(1600,1);
////    unsigned int x1[1600],y[1600];
//    for (int i=0; i<25; i++) {
//        for (int j=0; j<64; j++) {
//            mzd_write_bit(x,(64*i+j),0,(A[i]>>j)&0x1);
//        }
//    }
//
//    res=mzd_mul_m4rm(NULL, L, x, 0);
//
//    UINT64 flagg1;
//    for (int i=0; i<25; i++) {
//        A[i]=0;
//        for (int j=0; j<64; j++) {
//            flagg1=(mzd_read_bit(res,(64*i+j),0));
//            A[i]=A[i]|(flagg1<<j);
//        }
//    }
//    displayStateAs64bitWords(A);
///*********************************Check Linear Transformation***********************************/



///*********************************Check Linear Transformation***********************************/
////    UINT64 A[25]={
////        0x3935800000002440, 0x95788000000018C0, 0x5627800000001E40, 0x71AF8010000005C0, 0x13C4800000004DC0,
////        0x3935800000002441, 0x9578800000001CC0, 0x5626800000001E40, 0x71AF8000000005C0, 0x13C4800000004DC0,
////        0x3935800000002441, 0x95788000000010C0, 0x5627800000001E40, 0x61AF8000000005C0, 0x13C4800000004DC0,
////        0x3935800000002441, 0x95788000000018C0, 0x5627800000001E40, 0x71AF800000000540, 0x13C4800000004DC0,
////        0x3935800000002441, 0x95788000000018C0, 0x5627800000001E40, 0x71AF8010000005C0, 0x13C4800000000DC0
////    };//a2
////    UINT64 A[25]={
////        0x0000000000000001, 0x0040000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000010000000,
////        0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
////        0x0000000000000000, 0x0040000000000000, 0x0000000000200000, 0x0000000000000000, 0x0000000000000000,
////        0x0000000000000000, 0x0000000000000000, 0x0000000000200000, 0x0000000000000000, 0x0000000010000000,
////        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
////    };//a3
//    UINT64 A[25]={
//        0x0000000000000000, 0x0000000000000000, 0x0000000000000001, 0x0000000000000000, 0x0000000000000000,
//        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
//        0x0080000000000000, 0x0000000000000000, 0x0000001000000000, 0x0000001000000000, 0x0000000000000000,
//        0x0080000000000000, 0x0000000000000000, 0x0000001000000001, 0x0000001000000000, 0x0000000000000000,
//        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
//    };//a4
//    mzd_t * LI= mzd_inv_m4ri(NULL,L,0);//LI represents the
//
//    displayStateAs64bitWords(A);
//    cout<<endl;
//
//    mzd_t * x=mzd_init(1600,1);
//    //    unsigned int x1[1600],y[1600];
//    for (int i=0; i<25; i++) {
//        for (int j=0; j<64; j++) {
//            mzd_write_bit(x,(64*i+j),0,(A[i]>>j)&0x1);
//        }
//    }
//
//    res=mzd_mul_m4rm(NULL, L, x,0);
//
//    UINT64 flagg1;
//    for (int i=0; i<25; i++) {
//        A[i]=0;
//        for (int j=0; j<64; j++) {
//            flagg1=(mzd_read_bit(res,(64*i+j),0));
//            A[i]=A[i]|(flagg1<<j);
//        }
//    }
//    displayStateAs64bitWords(A);
///*********************************Check Linear Transformation***********************************/
//


    /***********************************Test KeccakL and inverseKeccakL***********************************/
//    UINT64 A[25]={
//        0x0000000000000001, 0x0040000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000010000000,
//        0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
//        0x0000000000000000, 0x0040000000000000, 0x0000000000200000, 0x0000000000000000, 0x0000000000000000,
//        0x0000000000000000, 0x0000000000000000, 0x0000000000200000, 0x0000000000000000, 0x0000000010000000,
//        0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
//    };
//
//    displayStateAs64bitWords(A);
//    KeccakL(A);
//    displayStateAs64bitWords(A);
//    inverseKeccakL(A);
//    displayStateAs64bitWords(A);
    /***********************************Test KeccakL and inverseKeccakL***********************************/

    return 0;
}

int computeWeightOfW1(void)
{
    int DDT_origin[32][32];
    differential_distribution_table_origin(DDT_origin);
    mzd_t * res=mzd_init(1600,1);
    mzd_t * vv=mzd_init(1600,1);
        UINT64 A[25];



//    uint8_t ddt[2][8]={{35,36,99,118,160,182,224,228},{9,3,25,8,1,26,1,3}};

    //number of active rows is 135
//    uint8_t ddt[2][8]={{9,35,63,127,137,170,227,234},{2,24,1,1,2,4,16,4}};
//    for (int j=0; j<8; j++) {
//        for (int i=0; i<5; i++) {
//            mzd_write_bit(vv,ddt[0][j]*5+i,0,(ddt[1][j]>>i)&0x1);
//        }
//    }

    uint8_t ddt[2][6]={{7,45,142,173,199,206},{16,22,6,3,26,5}};
    for (int j=0; j<6; j++) {
        for (int i=0; i<5; i++) {
            mzd_write_bit(vv,ddt[0][j]*5+i,0,(ddt[1][j]>>i)&0x1);
        }
    }

    unsigned int B[1600];
    for (int j=0; j<1600; j++) {
        B[j]=mzd_read_bit(vv,j,0);
    }
    Bit2State(B, A);
    for (int i=0; i<25; i++) {
        cout<<hex<<A[i]<<endl;
    }

    mzd_t * LI=gener_LI();
    int total_weight=0;
    res=mzd_mul_m4rm(res, LI, vv, 0);
    total_weight=get_weight_of_AS(res, DDT_origin);

//    int dout, weight;
//    for (int i=0; i<320; i++) {
//        dout=0;
//        for (int j=0; j<5; j++) {
//            dout|=((mzd_read_bit(res,i*5+j,0))<<j);
//        }
//        if (dout>0) {
//            int x=0;
//            weight=0;
//            for (int k=0; k<32; k++) {
//                if (x<DDT_origin[k][dout]) {
//                    x=DDT_origin[k][dout];
//                }
//            }
//            if (x==2) {
//                weight=4;
//            }
//            else if (x==4){
//                weight=3;
//            }
//            else if (x==8){
//                weight=2;
//            }
//            total_weight=total_weight+weight;
//        }
//    }


    cout<<dec<<total_weight<<endl;
    return total_weight;
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

void fromTrailsTo64bitWords1(const string& fileNameIn)
{
    vector<LaneValue> lanes(25);
    ifstream fin(fileNameIn.c_str());
    unsigned int count = 1;
    while(!(fin.eof())) {
        try {
            cout<<"No. "<<count<<endl;
            Trail trail(fin);
            for (int i=0; i<2; i++) {
                fromSlicesToLanes(trail.states[i], lanes);
                //displayStateAs64bitWords(lanes);
                //            fromSlicesToLanes(trail.states.back(), lanes);
                displayStateAs64bitWords(lanes);
                cout<<endl;
            }

            count++;
        }
        catch(TrailException) {
        }

    }
}


int main(int argc, char *argv[])
{

	KeccakFPropagation::DCorLC aDCorLC;
	aDCorLC = KeccakFPropagation::DC;

	string fileName = "coreb52-224-check100";
	string fileName1 = "coreb52";
    string fileName11="coreb52-check";
    string fileName2 = "coreb60m64new-plus";
    readAndPrint(aDCorLC, 1600, fileName1);
    myExtendBackward(aDCorLC, 1600, fileName);  // in compareTrails.h
    extendTrails(aDCorLC, 1600, fileName1, 2, 160, false);

    myExtendForward(aDCorLC, 1600, fileName2);
    myExtendBackward(aDCorLC, 1600, fileName2);

    myExtendForward(aDCorLC, 1600, fileName1); // in compareTrails.h
    myExtendBackward(aDCorLC, 1600, fileName11);

    computeWeightOfW1();
    string fileName3 = "coreb52-1";
    string fileName33 = "coreb52-1-check1";
    string fileName333 = "coreb52-1-check1-check";
    extendForward3Rounds1(aDCorLC, 1600, fileName3);
    myExtendForward(aDCorLC, 1600, fileName33);

    string fileName3 = "coreb60m64new-plus-1";
    string fileName33 = "coreb60m64new-plus-1-check1";
    string fileName333 = "coreb60m64new-plus-1-check1-check";
    extendForward3Rounds1(aDCorLC, 1600, fileName3);
    myExtendForward(aDCorLC, 1600, fileName33);
    myExtendBackward(aDCorLC, 1600, fileName333);

    instantFormatTransformation();

    string trails="coreb52-1-check1-check";
    fromTrailsTo64bitWords1(trails);

    return EXIT_SUCCESS;
}






