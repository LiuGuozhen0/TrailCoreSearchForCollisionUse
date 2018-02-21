/*
KeccakTools

The Keccak sponge function, designed by Guido Bertoni, Joan Daemen,
Michaël Peeters and Gilles Van Assche. For more information, feedback or
questions, please refer to our website: http://keccak.noekeon.org/

Implementation by the designers,
hereby denoted as "the implementer".

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include <fstream>
#include "Keccak-fDisplay.h"
#include "Keccak-fPropagation.h"
#include "Keccak-fTrails.h"

using namespace std;

Trail::Trail()
    : totalWeight(0), firstStateSpecified(true), stateAfterLastChiSpecified(false)
{
}

Trail::Trail(istream& fin)
{
    load(fin);
}

unsigned int Trail::getNumberOfRounds() const
{
    return states.size();
}

void Trail::clear()
{
    states.clear();
    weights.clear();
    totalWeight = 0;
    firstStateSpecified = true;
    stateAfterLastChiSpecified = false;
}

void Trail::setFirstStateReverseMinimumWeight(unsigned int weight)
{
    if (states.size() == 0) {
        states.push_back(vector<SliceValue>());
        firstStateSpecified = false;
    }
    if (weights.size() == 0)
        weights.push_back(weight);
    else {
        totalWeight -= weights[0];
        weights[0] = weight;
    }
    totalWeight += weight;
}

void Trail::append(const vector<SliceValue>& state, unsigned int weight)
{
    states.push_back(state);
    weights.push_back(weight);
    totalWeight += weight;
}

void Trail::append(const vector<SliceValue>& sBeforeChi, const vector<SliceValue>& sAfterChi, unsigned int weight)
{
    states.push_back(sBeforeChi);
	stateAfterLastChi=sAfterChi;
	stateAfterLastChiSpecified = true;
    weights.push_back(weight);
    totalWeight += weight;
}

void Trail::prepend(const vector<SliceValue>& state, unsigned int weight)
{
    states.insert(states.begin(), state);
    weights.insert(weights.begin(), weight);
    totalWeight += weight;
}

void Trail::display(const KeccakFPropagation& DCorLC, ostream& fout) const
{
	cout<<"calling Trail::display()"<<endl;
    if (states.size() == 0) {
        fout << "This trail is empty." << endl;
        return;
    }

    fout << dec << states.size() << "-round ";
    if (DCorLC.getPropagationType() == KeccakFPropagation::DC)
        fout << "differential ";
    else
        fout << "linear ";
    if (firstStateSpecified) {
        if (stateAfterLastChiSpecified) 
            fout << "fully specified trail ";
        else
            fout << "trail prefix ";
    }
    else
        fout << "trail core ";
    fout << "of total weight " << dec << totalWeight << endl;

    bool thetaJustAfterChi = DCorLC.isThetaJustAfterChi();
    vector<vector<SliceValue> > allAfterPreviousChi;
    vector<vector<SliceValue> > allBeforeTheta;

    string trailKernelType;
    vector<unsigned int> thetaGaps, activeRows;
    unsigned int offsetIndex = (firstStateSpecified ? 0 : 1);
    for(unsigned int i=offsetIndex; i<states.size(); i++) {
        vector<SliceValue> stateAfterChi;
        DCorLC.reverseLambda(states[i], stateAfterChi);
        allAfterPreviousChi.push_back(stateAfterChi);
        vector<SliceValue> stateBeforeTheta;
        DCorLC.directLambdaBeforeTheta(stateAfterChi, stateBeforeTheta);
        allBeforeTheta.push_back(stateBeforeTheta);

        bool kernel = true;
        for(unsigned int z=0; z<stateBeforeTheta.size(); z++) {
            if (getParity(stateBeforeTheta[z]) != 0) {
                kernel = false;
                break;
            }
        }
        trailKernelType += (kernel ? 'k' : 'N');

        vector<LaneValue> stateBeforeThetaLanes;
        fromSlicesToLanes(stateBeforeTheta, stateBeforeThetaLanes);
        thetaGaps.push_back(DCorLC.parent.getThetaGap(stateBeforeThetaLanes));

        activeRows.push_back(getNrActiveRows(states[i]));
    }

    fout << "* Profile related to \xCF\x87:" << endl;
    fout << "Propagation weights: ";
    for(unsigned int i=0; i<weights.size(); i++) {
        fout.width(4); fout.fill(' ');
        fout << dec << weights[i];
    }
    fout << endl;
    fout << "Active rows:         ";
    if (!firstStateSpecified) fout << "    ";
    for(unsigned int i=0; i<activeRows.size(); i++) {
        fout.width(4); fout.fill(' ');
        fout << dec << activeRows[i];
    }
    fout << endl;

    fout << "* Profile related to \xCE\xB8:" << endl;
    fout << "Gaps:   ";
    if (!firstStateSpecified) fout << "    ";
    for(unsigned int i=0; i<thetaGaps.size(); i++) {
        fout.width(4); fout.fill(' ');
        fout << dec << thetaGaps[i];
    }
    fout << endl;
    fout << "Kernel: ";
    if (!firstStateSpecified) fout << "    ";
    for(unsigned int i=0; i<trailKernelType.size(); i++) {
        fout << "   " << trailKernelType[i];
    }
    fout << endl;

    if (!firstStateSpecified) 
        fout << "Round 0 would have weight at least " << weights[0] << endl;
    for(unsigned int i=offsetIndex; i<states.size(); i++) {
        fout << "Round " << dec << i << " (weight " << weights[i];
        if (thetaJustAfterChi)
            fout << ", \xCE\xB8-gap " << thetaGaps[i-offsetIndex];
        fout << ") after previous \xCF\x87";
        if (!thetaJustAfterChi)
            fout << ", then before \xCE\xB8 of gap " << thetaGaps[i-offsetIndex];
        fout << ", then before \xCF\x87";
        fout << " (" << activeRows[i-offsetIndex] << " active rows):" << endl;
        if (thetaJustAfterChi)
            displayStates(fout, allAfterPreviousChi[i-offsetIndex], true, states[i], false);
        else
            displayStates(fout, allAfterPreviousChi[i-offsetIndex], false, allBeforeTheta[i-offsetIndex], true, states[i], false);
    }
    if (stateAfterLastChiSpecified) {
        if (DCorLC.getPropagationType() == KeccakFPropagation::DC) {
            fout << "After \xCF\x87 of round " << dec << (states.size()-1) << ":" << endl;
            displayState(fout, stateAfterLastChi);
        }
        else {
            vector<SliceValue> stateBeforeTheta;
            vector<SliceValue> stateAfterTheta;
            DCorLC.directLambdaBeforeTheta(stateAfterLastChi, stateBeforeTheta);
            DCorLC.directLambda(stateAfterLastChi, stateAfterTheta);
            fout << "After \xCF\x87 of round " << dec << (states.size()-1) << ", then before \xCE\xB8 and finally after \xCE\xB8:" << endl;
            displayStates(fout, stateAfterLastChi, false, stateBeforeTheta, true, stateAfterTheta, false);
        }
    }
}
/** Output the valid trail core to a file.
  * For one trail core, the number of states in the core, the weight and total weight of the core, whether the first state and the state after last chi are specified as well as the states of the core are stored as one entry to the file.
  */
void Trail::save(ostream& fout) const
{
//40 c 0 p 1 c 1 401 0 0 0 0 0 0 0 0 20004 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 10800 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 

    fout << hex;
    if (states.size() > 1){
		//cout<<"states.size()="<<states.size()<<endl;
        fout << states[1].size() << " ";
	}
    else if (states.size() > 0){
		//cout<<"states.size()="<<states.size()<<endl;
        fout << states[0].size() << " ";//number of slices, e.g. 64
	}
    else{
		//cout<<"states.size()="<<states.size()<<endl;
        fout << 0 << " ";
	}
	//cout<<"totalWeight="<<totalWeight<<endl;
    fout << totalWeight << " ";//load from the file containing trail cores
    fout << "0 ";
	//cout<<"firstStateSpecified="<<firstStateSpecified<<endl;
	//cout<<"stateAfterLastChiSpecified="<<stateAfterLastChiSpecified <<endl;
    if (firstStateSpecified) {
        if (!stateAfterLastChiSpecified)
            fout << "p";
    }
    else 
        fout << "c";
    if (stateAfterLastChiSpecified) fout << "l";
    fout << " ";
    fout << weights.size() << " ";
    for(unsigned int i=0; i<weights.size(); i++)
        fout << weights[i] << " ";
    fout << (states.size() - (firstStateSpecified ? 0 : 1)) << " ";
    for(unsigned int i=(firstStateSpecified ? 0 : 1); i<states.size(); i++)
    for(unsigned int j=0; j<states[i].size(); j++){
		cout <<hex<< states[i][j] << " ";
		fout << states[i][j] << " ";
	}
	cout<<endl;
    if (stateAfterLastChiSpecified) {
    fout << "1 ";
    for(unsigned int j=0; j<stateAfterLastChi.size(); j++)
        fout << stateAfterLastChi[j] << " ";
    }
    else
    fout << "0 ";
    fout << endl;
}

//Each time load one trail core from the input stream(file).
void Trail::load(istream& fin)
{
    unsigned int laneSize = 0;
    /** Techniques that help make heximal input to laneSize happen
     */
    fin >> hex;
    fin >> laneSize;
    if ((laneSize == 0) || (fin.eof()))
        throw TrailException();
    fin >> totalWeight;//weight of the trail

    firstStateSpecified = true;
    stateAfterLastChiSpecified = false;
    stateAfterLastChi.clear();

    unsigned int size;
    fin >> size;//check the first and last difference state
    if (size == 0) {//e.g. \beta_3 case
        string c;
        fin >> c;//further check the assignment of the first state and the state after the last chi
        for(unsigned int i=0; i<c.size(); i++) {
            if (c[i] == 'c') firstStateSpecified = false;
            if (c[i] == 'l') stateAfterLastChiSpecified = true;
        }
        fin >> size;//the rounds that the trail covers, 1 round for \beta_3 case
        weights.resize(size);//change the size to "size"
        for(unsigned int i=0; i<size; i++)
            fin >> weights[i];
        fin >> size;
        states.clear();
        if (!firstStateSpecified)
            states.push_back(vector<SliceValue>());
        for(unsigned int i=0; i<size; i++) {
            vector<SliceValue> state(laneSize);
            for(unsigned int j=0; j<laneSize; j++)
                fin >> state[j];
            states.push_back(state);    
        }       
        fin >> size;
        if (size == 1) {
            stateAfterLastChi.resize(laneSize);
            for(unsigned int j=0; j<laneSize; j++)
                fin >> stateAfterLastChi[j];
        }
    }
    else {
        weights.resize(size);
        for(unsigned int i=0; i<size; i++)
            fin >> weights[i];
        states.clear();
        for(unsigned int i=0; i<size; i++) {
            vector<SliceValue> state(laneSize);
            for(unsigned int j=0; j<laneSize; j++)
                fin >> state[j];
            states.push_back(state);    
        }
    }
}

void Trail::append(const Trail& otherTrail)
{
    for(unsigned int i=0; i<otherTrail.weights.size(); i++)
        append(otherTrail.states[i], otherTrail.weights[i]);
}

UINT64 Trail::produceHumanReadableFile(const KeccakFPropagation& DCorLC, const string& fileName, bool verbose, unsigned int maxWeight)
{
	cout<<"calling Trail::produceHumanReadableFile()"<<endl;
    string fileName2 = fileName+".txt";
    ofstream fout(fileName2.c_str());
    if (verbose)
        cout << "Writing " << fileName2 << flush;
    UINT64 count = DCorLC.displayTrailsAndCheck(fileName, fout, maxWeight);
    if (verbose)
        cout << endl;
    return count;
}

bool Trail::testCollision(bool checkCollision[32][2])
{
	cout<<"running Trail::testCollision() now.."<<endl;
	vector<SliceValue> slices = states.back();
	LaneValue lane2 = 0, lane3 = 0, one = 1;
	int lanesize = slices.size();
	int sbox;
	
	for(unsigned int i =0; i< lanesize; i++){
		sbox = slices[i]&0x1F;
		if(!checkCollision[sbox][0]){
			return false;
		}
		else{
			lane2 ^= (one<<i);
		}
		lane3 ^= checkCollision[sbox][1]? (one<<i) : 0;
	}
	UINT64 mask = 0xFFFFFFFF;
	cout<<hex<<mask<<endl;
	cout<<hex<<lane3<<endl;
	cout<<hex<<(mask^lane3)<<endl;
	for(int i = 0; i<32; i++){
		if((mask&lane3)==mask){
			cout<<"yes!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! a satisfied trail is found!"<<endl;
			return true;
		} 
		mask = (mask<<1);
	}
	
	//
/*
	int i;
	vector<LaneValue> lanes;	
    fromSlicesToLanes(states.back(), lanes);

	if( (lanes[0] == 0) && (lanes[1] ==0) )
    {   
	    UINT64 mask = 0xFFFFFFFF;

		for(i = 0; i<32; i++){
			if((mask&lanes[2])==0){
				printf("yes! a satisfied trail is found!\n");
				return true;
			} 
			mask = (mask<<1);
		}
    }*/
    return false;
	
}

// -------------------------------------------------------------
//
// TrailFilterAND
//
// -------------------------------------------------------------

TrailFilterAND::TrailFilterAND() 
{
}

TrailFilterAND::TrailFilterAND(TrailFilter *filter1, TrailFilter *filter2)
{
    filters.push_back(filter1);
    filters.push_back(filter2);
}

bool TrailFilterAND::filter(const KeccakFPropagation& DCorLC, const Trail& trail) const
{
    bool pass = true;
    for(vector<TrailFilter *>::const_iterator i=filters.begin(); i != filters.end(); ++i) {
        pass &= (*i)->filter(DCorLC, trail);
        if (!pass)
            break;
    }
    return pass;
}

// -------------------------------------------------------------
//
// TrailFileIterator
//
// -------------------------------------------------------------

TrailFileIterator::TrailFileIterator(const string& aFileName, const KeccakFPropagation& aDCorLC, 
                                     bool aPrefetch)
    : TrailIterator(aDCorLC), fileName(aFileName), prefetch(aPrefetch)
{
    initialize();
    next();
}

TrailFileIterator::TrailFileIterator(const string& aFileName, const KeccakFPropagation& aDCorLC, 
                                     TrailFilter *aFilter, bool aPrefetch)
    : TrailIterator(aDCorLC, aFilter), fileName(aFileName), prefetch(aPrefetch)
{
    initialize();
    next();
}

TrailFileIterator::TrailFileIterator(const string& aFileName, const KeccakFPropagation& aDCorLC, 
                                     TrailFilter *aFilter1, TrailFilter *aFilter2, bool aPrefetch)
    : TrailIterator(aDCorLC, aFilter1, aFilter2), fileName(aFileName), prefetch(aPrefetch)
{
    initialize();
    next();
}
/** The initialization process just outputs the number of trails read from a file.
 */
void TrailFileIterator::initialize()
{
    fin.open(fileName.c_str());
    if (!fin)
        throw TrailException((string)"File '" + fileName + (string)"' cannot be read.");
    i = 0;
    count = unfilteredCount = 0;
    if (prefetch) {
        /** Load trails from a file and stored them in a "state" vector.
         *  Record "count" and "unfilteredCount" as well.
         */
        while(!(fin.eof())) {
            try {
                Trail trail(fin);/*load a trail from input stream to a state vector, including the states and the weights*/
                unfilteredCount++;/*increment to the number of unfiltered trails*/
                if (filter) {/*Here "filter" is an object of the class TrailFilter, and returns the result of the filter: whether to keep or discard the given trail.*/
                    if (filter->filter(DCorLC, trail))
                        count++;
                }
                else
                    count++;/*count may indicate the number of all trails*/
            }
            catch(TrailException) {
            }
        }
        fin.close();
        fin.open(fileName.c_str());
        fin.seekg(0, ios_base::beg);
        if (!fin)
            throw TrailException((string)"File '" + fileName + (string)"' cannot be read anymore.");
    }
    else
        count = ~(UINT64)0;
}
//Let end = true?
//Make sure that all trails in the file are successfully uploaded.
void TrailFileIterator::next()
{
    if (fin.eof())//
        end = true;
    else {
        bool filterPass;
        do {
            try {
                current.load(fin);
                if (filter)
                    filterPass = filter->filter(DCorLC, current);
                else
                    filterPass = true;
            }
            catch(TrailException) {
                filterPass = false;
            }
        } while((!(fin.eof())) && (!filterPass));
        end = !filterPass;
    }
}

void TrailFileIterator::display(ostream& fout) const
{
    fout << "'" << fileName << "'";
    if (prefetch) {
        fout << " containing " << dec << count << " trails";
        if (filter)
            fout << " (" << dec << unfilteredCount << " before filtering)";
    }
    else
        fout << " (unknown trail count)";
}

bool TrailFileIterator::isEnd()
{
    return end;
}

bool TrailFileIterator::isEmpty()
{
    return (prefetch && (count == 0));
}

void TrailFileIterator::operator++()
{
    next();
    i++;
}

const Trail& TrailFileIterator::operator*()
{
    return current;
}

bool TrailFileIterator::isBounded()
{
    return prefetch;
}

UINT64 TrailFileIterator::getIndex()
{
    return i;
}

UINT64 TrailFileIterator::getCount()
{
    return count;
}

UINT64 TrailFileIterator::getUnfilteredCount() const
{
    return unfilteredCount;
}

ostream& operator<<(ostream& a, const TrailFileIterator& tfi)
{
    tfi.display(a);
    return a;
}

TrailSaveToFile::TrailSaveToFile(ostream& aFout)
    : fout(aFout)
{
}

void TrailSaveToFile::fetchTrail(const Trail& trail)
{
    trail.save(fout);
}
