#include <sstream>
#include "Keccak-fTrailExtension.h"
#include "translationsymmetry.h"
#include "progress.h"
#include "Keccak-fDisplay.h"
#include "kkeccak.h"
#include "misc.h"
#include "L.h"
#include "List.h"

void compareTrails(KeccakFPropagation::DCorLC DCLC, unsigned int width, const string& inFileName)
{
    try {
        cout << "Initializing... " << flush;
        KeccakFDCLC keccakF(width);
        cout << endl;
        KeccakFTrailExtension keccakFTE(keccakF, DCLC);
        cout << keccakF << endl;

        try {
            TrailFileIterator trailsIn(inFileName, keccakFTE);
            cout << trailsIn << endl;
			ProgressMeter progress;
			progress.stack("File", trailsIn.getCount());
			vector<SliceValue>  first = (*trailsIn).states[0];
			vector<SliceValue>  second = (*trailsIn).states[1];
			vector<SliceValue>  third = (*trailsIn).states[2];
			vector<LaneValue> lanes;	
			fromSlicesToLanes(first, lanes);
			displayStateAs64bitWords(lanes);
			fromSlicesToLanes(second, lanes);
			displayStateAs64bitWords(lanes);
			fromSlicesToLanes(third, lanes);
			displayStateAs64bitWords(lanes);
			++trailsIn;
			for( ; !trailsIn.isEnd(); ++trailsIn) {
				if(first == (*trailsIn).states[0]) cout<<"first: equal"<<endl;
				else cout<<"first: not equal"<<endl;

				//if(second != (*trailsIn).states[1]) cout<<"second: not equal"<<endl;
				//else cout<<"second: equal"<<endl;
				//if(third != (*trailsIn).states[2]) cout<<"third: not equal"<<endl;
				//else cout<<"third: equal"<<endl;
				++progress;
			}
			progress.unstack();
        }
        catch(TrailException e) {
            cout << e.reason << endl;
        }
    }
    catch(KeccakException e) {
        cout << e.reason << endl;
    }
}

void readAndPrint(KeccakFPropagation::DCorLC DCLC, unsigned int width, const string& inFileName)
{
	try {
        cout << "Initializing... " << flush;
        KeccakFDCLC keccakF(width);
        cout << endl;
        KeccakFTrailExtension keccakFTE(keccakF, DCLC);
        cout << keccakF << endl;

        try {
            TrailFileIterator trailsIn(inFileName, keccakFTE);
            cout << trailsIn << endl;
			ProgressMeter progress;
			progress.stack("File", trailsIn.getCount());
			vector<LaneValue> lanes;	
            int count = 0;
			for( ; !trailsIn.isEnd(); ++trailsIn) {
				cout<<"Trail "<<(++count)<<endl;
				fromSlicesToLanes((*trailsIn).states[0], lanes);
				displayStateAs64bitWords(lanes);
				fromSlicesToLanes((*trailsIn).states[1], lanes);
				displayStateAs64bitWords(lanes);
				//fromSlicesToLanes((*trailsIn).states[2], lanes);
				//displayStateAs64bitWords(lanes);
				++progress;
			}
			progress.unstack();
        }
        catch(TrailException e) {
            cout << e.reason << endl;
        }
    }
    catch(KeccakException e) {
        cout << e.reason << endl;
    }
}

//Base stores the linear(or inverse linear) transformation matrix column by column
//After precompute(), there are 1600 columns(of size 1600*1) of LT in base.
void precompute(mzd_t* LI, vector<vector<UINT64> >& base)
{
	UINT64 M[25];
	unsigned int B[1600];
	mzd_t* res = mzd_init(1600,1);//create a 1600*1 size matrix
	for(int i=0;i<1600;i++){
		mzd_t *abase = mzd_init(1600,1);//initialize a matrix with size 1600*1
		mzd_write_bit(abase,i,0,1);//assign 1 to bit position [i][0]
        /** mzd_t * mzd_mul_m4rm(mzd_t *C, mzd_t const *A, mzd_t const *B, int k)
         *  C=AB+k, k is usually 0.
         */
		res = mzd_mul_m4rm(res, LI, abase, 0);//res=LI*abase
		for(int j=0; j<1600; j++){ 
		    //if(mzd_read_bit(res, j, 0))  A[(j/320)*5+j%5] ^= (1<<(63-((j%320)/5)));
			B[j] = mzd_read_bit(res,j,0);
		}
		Bit2State(B,M);	
		vector<UINT64> A(M,M+25);
        base.push_back(A);//Add the state B into base
		//Ltrans(M);
		//displayStateAs64bitWords(M);
		mzd_free(abase);
	}
	mzd_free(res);
}
/** With the filtered trails from forward extension, now we can further check trails from backward direction.
  */
void myExtendBackward(KeccakFPropagation::DCorLC DCLC, unsigned int width, const string& inFileName)
{
	KeccakInitializeRoundConstants();
    KeccakInitializeRhoOffsets();

    int i,j, temp, AS;
	int minAS = 320;
//***** generate DDT ****
    /** DDT stores all compatible input differences that result in the output difference.
      */
    int DDT[32][32];
    differential_distribution_table(DDT);
    print_DDT(DDT);

    int DDT_origin[32][32];
    differential_distribution_table_origin( DDT_origin );
    print_DDT(DDT_origin);

    /** "base" represents the inverse linear transformation of pi, rho and theta.
      */
    vector< vector<UINT64> > base;
	mzd_t* LI = gener_LI();
	precompute(LI, base);
	mzd_free(LI);
	
	unsigned int diff[320];
	UINT64 A[25];
	vector<LaneValue> lanes;	

	try {
        cout << "Initializing... " << flush;
        KeccakFDCLC keccakF(width);
        cout << endl;
        KeccakFTrailExtension keccakFTE(keccakF, DCLC);
        /** Abstract method that returns a string with a description of itself.
          * Implemented in KeccakFDCLC, KeccakF and Identy.
          * Here the string is "DC/LC analysis of Keccak-f[1600]".
          */
        cout << keccakF << endl;

        try {
            TrailFileIterator trailsIn(inFileName, keccakFTE);
            cout << trailsIn << endl;
			ProgressMeter progress;
			progress.stack("File", trailsIn.getCount());

            int count = 0;
			for( ; !trailsIn.isEnd(); ++trailsIn) {
				cout<<"Trail "<<(++count)<<endl;
				fromSlicesToLanes((*trailsIn).states[0], lanes);
				for(int i=0;i<25;i++) A[i] = lanes[i];
				inverseKeccakL(A);
				AS = gen_diff320(A, diff);
				if(AS<9){
					List SL = gen_SL(diff, DDT);//construct and print the list
					temp = findminASimproved(SL, base, DDT_origin, diff);
					if(temp<minAS){minAS = temp; cout<<"current minAS = "<<minAS<<endl;}
					cout<<"after findminASimproved()"<<endl;
					//SL.setNULL();
					cout<<"after setNULL()"<<endl;
				}
				else
					cout<<"#AS>=9, neglect this trail"<<endl;
				++progress;
			}
			progress.unstack();
        }
        catch(TrailException e) {
            cout << e.reason << endl;
        }
    }
    catch(KeccakException e) {
        cout << e.reason << endl;
    }
	cout<<"minAS="<<endl;//What to do next?
}



static void extracted() {
    vector<LaneValue> lanes;
}

void myExtendForward(KeccakFPropagation::DCorLC DCLC, unsigned int width, const string& inFileName)
{
	KeccakInitializeRoundConstants();
    KeccakInitializeRhoOffsets();

    int i,j, AS;//AS, number of active sboxes
//***** generate DDT ****
    //The DDT that lists all possible output differences, items including number of possible output difference, actual difference and their weights in increasing rate
    int DDT[32][32];
    differential_distribution_table_dir(DDT);//DDT for dir
    print_DDT(DDT);/*Cosider to comment this statement*/

    //The general DDT
	int DDT_origin[32][32];
	differential_distribution_table_origin(DDT_origin);
    print_DDT(DDT_origin);/*Comment this line*/
    //checkCollision[32][0], the MSB 2-bit
    //checkCollision[32][1], the MSB 3-bit
    bool checkCollision[32][2];
	generateCheckCollistion160(checkCollision);
	//generateCheckCollistion224(checkCollision);

	vector< vector<UINT64> > base;
    //Generate the matrix that abstract the linear transformation, i.e., theta,rho and pi
    //L is a matrix of the linear transformation
	mzd_t* L = gener_L();
	precompute(L, base);//Base stores the linear transformation matrix columnwise, i.e., 1600 columns, each column is of size 1600*1, or a state. In other words, "base" represents the linear transformation.
	mzd_free(L);
	
	unsigned int diff[320];//state difference
	UINT64 A[25];//state
    vector<LaneValue> lanes;//state

	try {
        cout << "Initializing... " << endl << flush;
        /** keccakF(width) initialize the basic parameters for a possible keccak permutation
          */
        /** The constructor KeccakFDCLC(width) simply initializes the permutation and a difference table for nonlinear chi.
          * diffChi is a vector consists of ListOfRowPatterns for all possible input patterns. Each of the ListOfRowPatterns element is composed of all the possible output patterns and their weights in increasing rate, the maxWeight and the MinWeight attributes.
          */
        KeccakFDCLC keccakF(width);//keccakF provides a parameter-specified permutation and diffChi at this step
        cout << endl;
        /** The object keccakFTE first initialize the bounds of trails as well as other attributes.
          * It first invocates KeccakFPropagation(keccakF, DCLC), which initializes the attributes of keccakF(a Keccak-f permutation), and DC type.
          * Then it sets bounds for trails of specific rounds.
          */
        KeccakFTrailExtension keccakFTE(keccakF, DCLC);
        cout << keccakF << endl;//???

        try {
            /** Load a set of trails in "state".
              *  Store their "count" and "unfilteredCount".
              */
            TrailFileIterator trailsIn(inFileName, keccakFTE);
            /** Output the number of total trails of a file and unfiltered trails.
             */
            cout << trailsIn << endl;
			string outFileName = inFileName + string("-check");
            ofstream fout(outFileName.c_str());/*c_str() is a public memeber function that returns a pointer to the string object*/
            /** Write the trails that pass the filter to the output file.
             */
            TrailSaveToFile trailsOut(fout);
            /** The trails are checked one by one.
              * If the block-process takes more than 10 seconds, the code will automatically type the progress of the trails checking, i.e., type something as "File: 1/503".
              */
			ProgressMeter progress;
			progress.stack("File", trailsIn.getCount());

            int count = 0;//Mark for the current trail being checked.
            /** Check collision for all trail cores.
             */
			for( ; !trailsIn.isEnd(); ++trailsIn) {
				cout<<"Trail "<<dec<<(++count)<<endl;//"Trail" acutally
				fromSlicesToLanes((*trailsIn).states.back(), lanes);//Check for one trail core or just one state from a trail core?
				for(int i=0;i<25;i++) A[i] = lanes[i];//Now that A[25] is the difference state
                AS = gen_diff320(A, diff);//obtain the number of active sboxes; diff[320] stores the input difference to the sboxes
				cout<<"AS = "<<AS<<endl;
				if((AS<10) ){
                    /** Generate a list that stores all possible output difference for all active sboxes
                     */
					List SL = gen_SL(diff, DDT);
                    /** The parameters accordingly are:
                     *  The "list" List that indicates the location of the active sboxes and their compatible output differences
                     *  "base" is just a state(I guess)
                      *  "diff", the state difference depicted by diff[320]
                     *  "checkCollision", the table that determines whether an input difference will lead to a collision
                     *  "DDT-origin", a classic DDT
                     */
					if(myCheckCollision(SL, base, diff, checkCollision, DDT_origin)){  // in List.h
						cout<<"This trail is good! "<<endl;
						trailsOut.fetchTrail(*trailsIn);
					}
				}
				else
					cout<<"#AS >=10 , neglect this trail"<<endl;
				++progress;
			}
			progress.unstack();
        }
        catch(TrailException e) {
            cout << e.reason << endl;
        }
    }
    catch(KeccakException e) {
        cout << e.reason << endl;
    }
}

void myExtendForward(UINT64 A[25])
{
	KeccakInitializeRoundConstants();
    KeccakInitializeRhoOffsets();

    int i,j, AS;
//***** generate DDT ****
    int DDT[32][32];
    differential_distribution_table_dir(DDT);//DDT for dir
    print_DDT(DDT);
	
	int DDT_origin[32][32];
	differential_distribution_table_origin(DDT_origin);

    bool checkCollision[32][2];
	generateCheckCollistion160(checkCollision);
	//generateCheckCollistion224(checkCollision);

	vector< vector<UINT64> > base;
	mzd_t* L = gener_L();
	precompute(L, base);
	mzd_free(L);
	
	unsigned int diff[320];
	vector<LaneValue> lanes;

	AS = gen_diff320(A, diff);
	cout<<"AS = "<<AS<<endl;
	if((AS<16) ){
		List SL = gen_SL(diff, DDT);
		//myCheckCollisionHW(SL, base, diff, checkCollision, DDT_origin); // in List.h
		findminASimproved(SL, base, DDT_origin, diff);
	}

}


