
#include "List.h"

using namespace std;


void List::addAtEnd(int dout, int lev, int indiff[32][32])
{
    cout<<"Add the "<<lev<<"-th AS to Sbox List, "<<"din (or dout)= "<<dout<<": ";
    Node *t;
    BigNode *here = pos;
    BigNode *q;
    q = (BigNode*) new(BigNode);

    q->level = lev;
    int i;
    for(i=1;i<indiff[dout][0];i++){
        t = (Node*)new(Node);
        t->din = indiff[dout][i];
        t->next = NULL;
        cout<<indiff[dout][i]<<' ';

        if(i == 1){
            q->head = t;
            q->current = t;
        }
        else{
            (q->current)->next = t;
            (q->current) = t;
        }
    }
    cout<<endl;
    q->current = q->head;


    if( here == NULL){
        pos = q;
        q->next = NULL;
        q->pre = NULL;
    }
    else
    {
         while(here->next != NULL){
             here = here->next;
         }
         here->next = q;
         q->pre = here;
         q->next = NULL;
    }
}

void deleteNode(BigNode * target)
{
    Node *qq, *pp;
    pp = target->head;
    qq = pp->next;
    while(qq!=NULL){
        delete(pp);
        pp = qq;
        qq = qq->next;
    }
    delete(target->head);
    delete(pp);
    target->head = NULL;
    target->current = NULL;
}

void List::setNULL()
{
    BigNode *q, *p;
    p = pos->next;
    q = p->next;
    while(q!= NULL){
        deleteNode(p);
	delete(p);
	p=q;
	q=q->next;
    }
    if(p!=NULL){
        delete(p);
    }

    p = pos->pre;
    q = p->pre;
    while(q!= NULL){
        deleteNode(p);
	delete(p);
	p=q;
	q=q->pre;
    }
    if(p!=NULL){
        delete(p);
    }
    if(pos!=NULL){
        delete(pos);
    }
    pos = NULL;
}

void List::outputlist(int DDT[32][32], unsigned int outdiff[320])
{

    BigNode* p = pos;
    Node*q;
	int thisweight =  0;
    while(p->pre != NULL){ p = p->pre;}
    while(p!=NULL)
    {
        cout<<(p->level)<<": ";
        q = p->current;
        cout<<q->din;
        switch (DDT[q->din][outdiff[p->level]]) {
            case 8:  { thisweight += 2; break; }
            case 4:  { thisweight += 3; break; }
            case 2:  { thisweight += 4; break; }
            default: cout<<" impossible differntials "<<endl;
        }
        /*while(q!=NULL){
            cout<<q->din<<" ";
            q = q->next;
        }*/
        cout<<endl;
        //p = p->pre;
        p = p->next;
    }
	cout<<"the weight of this chi is "<<thisweight<<endl;
    cout<<endl;
}

void List::outputlist_forward(int DDT[32][32], unsigned int outdiff[320])
{
    BigNode* p=pos;
    Node* q;
    int thisweight=0;
    while (p->pre != NULL) {
        p=p->pre;
    }
    while (p != NULL) {
        cout<<(p->level)<<": ";
        q=p->current;
        cout<<q->din;
        switch(DDT[outdiff[p->level]][q->din]){
        case 8:{thisweight+=2; break;}
        case 4:{thisweight+=3; break;}
        case 2:{thisweight+=4; break;}
        default: cout<<" impossible differentials "<<endl;
        }
        cout<<endl;
        p=p->next;
    }
    cout<<"the weight of this chi is "<<thisweight<<endl;
    cout<<endl;
}

void List::outputlist( unsigned int outdiff[320])
{

    BigNode* p = pos;
    Node*q;
	int thisweight =  0;
    while(p->pre != NULL){ p = p->pre;}
    while(p!=NULL)
    {
        cout<<dec<<(p->level)<<": ";
        q = p->current;
        cout<<q->din;
        /*while(q!=NULL){
            cout<<q->din<<" ";
            q = q->next;
        }*/
        cout<<endl;
        //p = p->pre;
        p = p->next;
    }
    cout<<endl;
}


List gen_SL( unsigned int diff[320], int DDT[32][32])
{
    List list;
    int i,j;
    for(i=0;i<320;i++){
        if(diff[i]>0){
            cout<<i<<"-th AS, difference: "<<diff[i]<<endl;
            list.addAtEnd(diff[i], i, DDT);
        }

    }
    return list;
}

mzd_t* initial_SL(List *list, mzd_t* vv)//pos points to the end after this function
{
    BigNode *p;
    int din;
    int i,lev;
    p = (*list).pos;
    while(p!=NULL){
        (*list).pos = p;
        lev = p->level;
        din = (p->current)->din;
        cout<<din<<" ";
        for(i=0;i<5;i++){
             mzd_write_bit(vv, lev*5+i, 0, (din >> i)&0x1);
        }
        p = p->next;
    }
    cout<<endl;
    //cout<<(*list).pos->level<<endl;
    return vv;
}



int get_num_of_AS(mzd_t * res)
{
    int total_active = 0;
    int active, i, j;

    for(i=0;i<320;i++){
        active = 0;
        for(j=0;j<5;j++){
            active |= mzd_read_bit(res,i*5+j,0);
        }
        total_active += active;
    }
    return total_active;
}


int get_num_of_AS(vector<UINT64> & a2)
{
    int total_active = 0;
    int active, i, j;

    for(i=0;i<320;i++){
        active = 0;
        for(j=0;j<5;j++){
            active |= (a2[(i/64)*5 + j]>>(i%64))&0x1;
        }
        total_active += active;
    }
    return total_active;
}


int get_weight_of_AS( mzd_t * res, int DDT_origin[32][32])
{
    int total_weight = 0;
    int weight, i,j, dout, num;

    for(i=0;i<320;i++){
        dout = 0;
        for(j=0;j<5;j++){
            dout |= ((mzd_read_bit(res,i*5+j,0))<<j);
        }
        if(dout>0){
            if((dout==1) || (dout==2) || (dout==4) || (dout==8) ||(dout==16)){
                weight =  2;
            }
            else{
                num = 0;
                for(j=0;j<32;j++){
                    if(DDT_origin[j][dout] > num) {
                        num = DDT_origin[j][dout];
                    }
                }
                weight = log(32/num)/log(2);
            }
            total_weight += weight;
        }
    }
    return total_weight;
}

int getWeight_Of_State_InLanes(vector<UINT64> &A)
{
  int i, j, din;
  unsigned int weight = 0, totalWeight=0;
  int num=0;

  for (i = 0; i < 320; i++) {
    weight = 0;//Here is the bug. I forgot to update the weight.
    din = 0;
    for (j = 0; j < 5; j++) {
      din ^= ((A[(i/64)*5 + j]>>(i%64))&0x1)<<j;
    }
    if (din > 0) {
      num++;
      if ((din == 1) || (din == 2) || (din == 4) || (din == 8) || (din == 16)) {
        weight = 2;
      }
      else if ((din==3)||(din==5)||(din==6)||(din==9)||(din==10)||(din==11)||(din==12)||(din==13)||(din==17)||(din==18)||(din==20)||(din==21)||(din==22)||(din==26)) {
        weight = 3;
      }
      else if ((din==7)||(din==14)||(din==15)||(din==19)||(din==23)||(din==24)||(din==25)||(din==27)||(din==28)||(din==29)||(din==30)||(din==31)){
        weight = 4;
      }
    }
    totalWeight += weight;
  }
  // cout << num << " active sboxes in total!" << endl;
  return totalWeight;
}


int combine_for_num(List list, mzd_t* vv, mzd_t* LI, mzd_t* res)//pos points to the last BigNode
{
    int total_active, active, lev, din, i, j;
    BigNode* p;
    p = list.pos;
    //cout<<"lev: ";
    while(p!=NULL){
        lev = p->level;
        //cout<<lev<<" ";
        din = (p->current)->din;
        for(i=0;i<5;i++){
            mzd_write_bit(vv,lev*5+i,0, (din>>i)&0x1);
        }
        p = p->pre;
    }
    //cout<<endl;

    res = mzd_mul_m4rm(res, LI, vv, 0);
    total_active = get_num_of_AS(res);
    return total_active;
}

int combine_for_weight(List list, mzd_t* vv, mzd_t* LI, mzd_t* res, int DDT_origin[32][32])//pos points to the last BigNode
{
    int total_weight, lev, din, i, j;
    BigNode* p;
    p = list.pos;
    //cout<<"lev: ";
    while(p!=NULL){
        lev = p->level;
        //cout<<lev<<" ";
        din = (p->current)->din;
        for(i=0;i<5;i++){
            mzd_write_bit(vv,lev*5+i,0, (din>>i)&0x1);
        }
        p = p->pre;
    }
    //cout<<endl;

    res = mzd_mul_m4rm(res, LI, vv, 0);
    total_weight = get_weight_of_AS(res, DDT_origin);
    return total_weight;
}

mzd_t* combine_for_collisiontest(List list, mzd_t* vv, mzd_t* L, mzd_t* res)//pos points to the last BigNode
{
    int total_weight, lev, din, i, j;
    BigNode* p;
    p = list.pos;
    //cout<<"lev: ";
    while(p!=NULL){
        lev = p->level;
        //cout<<lev<<" ";
        din = (p->current)->din;
        for(i=0;i<5;i++){
            mzd_write_bit(vv,lev*5+i,0, (din>>i)&0x1);
        }
        p = p->pre;
    }
    //cout<<endl;

    res = mzd_mul_m4rm(res, L, vv, 0);

    return res;
}

int findminAS(List list, int DDT_origin[32][32], unsigned int diff[320])
{
    int temp;
    BigNode *p;
    int total_active = 0;
    mzd_t* LI = gener_LI();
    mzd_t* vv = mzd_init(1600,1);
    mzd_t* res = mzd_init(1600,1);

    vv = initial_SL(&list, vv);
    res = mzd_mul_m4rm(res, LI, vv, 0);
    total_active = get_num_of_AS(res);
    cout<<total_active<<endl;

    int num=1;
    p = list.pos;
    //cout<<list.pos->level<<endl;
    while(p!=NULL){
        //cout<<p->level<<endl;
        p->current = (p->current)->next;
        if((p->current)!=NULL){
            //cout<<(p->current)->din<<endl;
            temp = combine_for_num(list,vv,LI,res);
            num++;
            //cout<<"temp="<<temp<<endl;
            if(temp<total_active){
                cout<<temp<<endl;
                total_active = temp;
                list.outputlist(DDT_origin, diff);
            }
            while(p->next!=NULL){ p = p->next;}
        }
        else{
            p->current = p->head;
            p = p->pre;
        }
    }
    cout<<"the min total activs sbox="<<total_active<<endl;
    mzd_free(LI);
    mzd_free(res);
    mzd_free(vv);
    cout<<"num="<<num<<endl;
    return total_active;
}


int findminWeight(List list, int DDT_origin[32][32], unsigned int diff[320])
{
    int temp;
    BigNode *p;
    int total_weight = 0;
    mzd_t* LI = gener_LI();
    mzd_t* vv = mzd_init(1600,1);
    mzd_t* res = mzd_init(1600,1);

    vv = initial_SL(&list, vv);
    res = mzd_mul_m4rm(res, LI, vv, 0);
    total_weight = get_weight_of_AS(res, DDT_origin);
    cout<<total_weight<<endl;

    int num=1;
    p = list.pos;
    //cout<<list.pos->level<<endl;
    while(p!=NULL){
        //cout<<p->level<<endl;
        p->current = (p->current)->next;
        if((p->current)!=NULL){
            //cout<<(p->current)->din<<endl;
            temp = combine_for_weight(list,vv,LI,res, DDT_origin);
            num++;
            //cout<<"temp="<<temp<<endl;
            if(temp<total_weight){
                cout<<temp<<endl;
                total_weight = temp;
                list.outputlist(DDT_origin, diff);
            }
            while(p->next!=NULL){ p = p->next;}
        }
        else{
            p->current = p->head;
            p = p->pre;
        }
    }
    cout<<"the min total weight ="<<total_weight<<endl;
    mzd_free(LI);
    mzd_free(res);
    mzd_free(vv);
    cout<<"num="<<num<<endl;
    return total_weight;
}

int findMinForwardWeight(List list, vector< vector < UINT64> > & base, int DDT_origin[32][32], unsigned int diff[320])
{
  int tempWeight;
  BigNode *p;
  int totalWeight = 0;
  vector <UINT64> a5(25,0);

  initial_SL(&list, a5, base);
  // cout << "TEST:" << endl;
  // for (int i = 0; i < a5.size(); i++) {
  //   cout.width(16); cout.fill('0');
  //   cout << hex << a5[i] << endl;;
  // }
  totalWeight = getWeight_Of_State_InLanes(a5);
  cout << "Initial: " << totalWeight << endl;

  UINT64 A[25] = {
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000};
  vector <UINT64> ZERO(A,A+25);

  int num = 1;
  p = list.pos;
  while (p != NULL) {
    p->current = (p->current)->next;
    if (p->current != NULL) {
      a5.clear();
      a5.assign(ZERO.begin(), ZERO.end());
      combine_for_state(list, a5, base);
      tempWeight = getWeight_Of_State_InLanes(a5);
      num++;
      if(tempWeight < totalWeight){
        cout << "Temp minal weight: " << tempWeight << endl;
        totalWeight = tempWeight;
        list.outputlist(DDT_origin, diff);
      }
      while (p->next != NULL) {
        p = p->next;
      }
    }
    else{
      p->current = p->head;
      p = p->pre;
    }
  }
  cout << "The min total Weight = " << totalWeight << endl;
  cout << "The number of all output diff of b4, i.e. a5: " << num << endl << endl;
  return totalWeight;
}

bool testCollision(mzd_t* res, bool checkCollision[32][2])
{
	UINT64 lane3 = 0, one = 1;
    int sbox, i, j;

    for(i=0;i<64;i++){
        sbox = 0;
        for(j=0;j<5;j++){
            sbox ^= (mzd_read_bit(res,i*5+j,0)<<j);
        }
		if(!checkCollision[sbox][0]){
			return false;
		}
		lane3 ^= checkCollision[sbox][1]? (one<<i) : 0;
    }

	UINT64 mask = 0xFFFFFFFF;
	for( i = 0; i<64; i++){
		if((mask&lane3)==mask){
			cout<<"yes!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! a satisfied trail is found!"<<endl;
			return true;
		}
		mask = rol64(mask,1);
	}
    return false;
}

int findCollision(List list, bool checkCollision[32][2], int DDT_origin[32][32], unsigned int diff[320])
{
    int temp;
    BigNode *p;
    int total_weight = 0;
    mzd_t* L = gener_L();
    mzd_t* vv = mzd_init(1600,1);
    mzd_t* res = mzd_init(1600,1);

    vv = initial_SL(&list, vv);
    res = mzd_mul_m4rm(res, L, vv, 0);

	if(testCollision(res, checkCollision)) {
		cout<<"find one solution, return now!"<<endl;
		return 1;
	}

    int num=1;
	bool flag = false;
    p = list.pos;
    //cout<<list.pos->level<<endl;
    while(p!=NULL){
        //cout<<p->level<<endl;
        p->current = (p->current)->next;
        if((p->current)!=NULL){
            //cout<<(p->current)->din<<endl;
            res = combine_for_collisiontest(list,vv,L,res);
            num++;
            //cout<<"temp="<<temp<<endl;
            if(testCollision(res, checkCollision)){
		        cout<<"find one solution, break now!"<<endl;
				list.outputlist(DDT_origin, diff);
				flag = true;
		        break;
            }
            while(p->next!=NULL){ p = p->next;}
        }
        else{
            p->current = p->head;
            p = p->pre;
        }
    }

    mzd_free(L);
    mzd_free(res);
    mzd_free(vv);
    cout<<"num="<<num<<endl;
    return flag;
}

/** Technically, this function implements linear transformation of theta, rho and pi on a single active sbox(note that here we have chosen an input difference as well as a compatible output difference) by vector xor operation.
  * Linear transformation of differences.
  * "a2" is the value of output difference after linear transformation.
  * Note that here the parameters are both references.
  */
void xorVector(vector<UINT64>& a2, vector<UINT64>& abase)
{
	if(a2.size()==abase.size()){
		for(int i=0;i<a2.size();i++){
			a2[i] ^= abase[i];
		}
	}
	else{
		cout<<"a2.size()!=abase.size()"<<endl;
	}
}

int combine_for_num(List list, vector<UINT64>& a2, vector< vector<UINT64> >& base)//pos points to the last BigNode
{
    int total_active, active, lev, din, i, j;
    BigNode* p;
    p = list.pos;
    //cout<<"lev: ";
    while(p!=NULL){
        lev = p->level;
        //cout<<lev<<" ";
        din = (p->current)->din;
        for(i=0;i<5;i++){
            if(( (din >> i)&0x1) ) xorVector(a2,base[lev*5+i]);
        }
        p = p->pre;
    }

    total_active = get_num_of_AS(a2);
    return total_active;
}



void initial_SL(List *list, vector<UINT64>& a2, vector< vector<UINT64> >& base)//pos points to the end after this function
{
	//vector<UINT64> temp(25,0);
	//a2 = temp;
    BigNode *p;
    int din;
    int i,lev;
    p = (*list).pos;
    while(p!=NULL){
        (*list).pos = p;
        lev = p->level;
        din = (p->current)->din;
        cout<<din<<" ";
        /* Process linear operation on the chosen output difference "din" */
        for(i=0;i<5;i++){
			if((din >> i)&0x1) xorVector(a2,base[lev*5+i]);
        }
        p = p->next;
    }
    cout<<endl;
    //cout<<(*list).pos->level<<endl;
}

int findminASimproved(List list, vector< vector<UINT64> >& base, int DDT_origin[32][32], unsigned int diff[320])
{
	int temp;
    BigNode *p;
    int total_active = 0;
	vector<UINT64> a2(25,0);

    /** print the state difference on screen
      * pos points to the end of List
      * "a2" represents the state differernce at alpha_2.
      */
    initial_SL(&list, a2, base);
    /** Return the number of active sboxes of this state difference alpha_2.
      */
    total_active = get_num_of_AS(a2);
    cout<<"initial: "<<total_active<<endl;

	UINT64 A[25] = {
0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000};
	vector<UINT64> ZERO(A,A+25);

    int num=1;
    p = list.pos;
    //cout<<list.pos->level<<endl;
    while(p!=NULL){
        //cout<<p->level<<endl;
        p->current = (p->current)->next;
        if((p->current)!=NULL){
            //cout<<(p->current)->din<<endl;
			a2.clear();
			a2.assign(ZERO.begin(), ZERO.end());
            temp = combine_for_num(list,a2, base);
            num++;
            //cout<<"temp="<<temp<<endl;
            if(temp<total_active){
                cout<<temp<<endl;
                total_active = temp;
                list.outputlist(DDT_origin, diff);
            }
            while(p->next!=NULL){ p = p->next;}
        }
        else{
            p->current = p->head;
            p = p->pre;
        }
    }
    cout<<"the min total activs sbox="<<total_active<<endl;

    cout<<"num="<<num<<endl;
    return total_active;
}
bool testCollisionForIndiff( vector<UINT64>& a2, bool checkCollision[32][2], int DDT_origin[32][32])
{
	UINT64 laneFull = 0, laneHalf = 0, one = 1;
	int lanesize = 64;
	int sbox;

	UINT64 A[25];
	for(int i=0;i<25;i++) A[i] = a2[i];
	unsigned int diff[320];
	gen_diff320(A, diff );/* Yes, we need to update state difference table. */

    //Compute the mask for the validity of full lane input difference and half lane input difference
    int kk;
	for(unsigned int i =319; i> 255; i--){
		sbox = diff[i];
        kk=319-i;
		if(!checkCollision[sbox][0]){
			//return false;
		}
		else{
			laneFull ^= (one<<kk);//change from i to (319-i)
		}
		laneHalf ^= checkCollision[sbox][1]? (one<<kk) : 0;//change from i to (319-i)
	}

    //First check for the validity of full lane case
	if(laneFull!=0xFFFFFFFFFFFFFFFF) return false;
	UINT64 mask = 0xFFFFFFFF;
	UINT64 MASK;
	cout<<hex<<laneFull<<endl;
	cout<<hex<<(laneHalf)<<endl;
    cout<<hex<<(mask)<<endl;

    //Then check for the validity of half lane case(????)
    //It seems that the position of the sbox doesn't affect as long as they are connected
    //Compute the total weight as well
	for(int i = 0; i<64; i++){
		MASK = rol64(mask,i);
		if((MASK&laneHalf)==MASK){
			cout<<"yes!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! a satisfied trail is found!"<<endl;
			cout<<"shift to left by "<<dec<<i<<" bits"<<endl;
			int TotalWeight = 0;
			for(int k=319; k>255;k--){
				int kk = 319-k;//k-256,pay attention here
				if(diff[k]>0){
					int weight;
					cout<<"Sbox "<<k<<" : "<<diff[k]<<" : ";
					int ddt = 0;
					if(i>32){
						if( (kk<i)&&(kk>=((i+32)%64)) ){ // full lane part
							for(int out=0; out<32; out++){
						        if( ( (out&0x18)==0 ) )
									ddt += DDT_origin[diff[k]][out];

						    }
					    }
						else{ // half lane part
							for(int out=0; out<32; out++){
						        if( ( (out&0x1c)==0 ) )
									ddt += DDT_origin[diff[k]][out];

						    }
					    }
					}
					else{
						if( (kk<i)||(kk>=(i+32)) ){ // full lane part
							for(int out=0; out<32; out++){
                                if( ( (out&0x18)==0 ) )
									ddt += DDT_origin[diff[k]][out];

						    }
						}
						else{
							for(int out=0; out<32; out++){
                                if( ( (out&0x1c)==0 ) )
									ddt += DDT_origin[diff[k]][out];

						    }
						}
					}
					//cout<<"ddt = "<<ddt<<endl;
					switch(ddt){
						case 32: {weight = 0; break;}
						case 16: {weight = 1; break;}
						case 8 : {weight = 2; break;}
						case 4:  {weight = 3; break;}
						case 2 : {weight = 4; break;}
                        default: cout<<"error"<<endl;
					}
					cout<<weight<<endl;
					TotalWeight += weight;
				}
			}
			cout<<"TotalWeight = "<<TotalWeight<<endl;
			return true;
		}
	}
	return false;
}




int HW(UINT64 a, int n){
	int res = 0;
	for(int i=0; i<n; i++){
		res += ((a>>i)&0x1);
	}
	return res;
}

int testCollisionForIndiffHW( vector<UINT64>& a2, bool checkCollision[32][2])
{

	UINT64 laneFull = 0, laneHalf = 0, one = 1;
	int lanesize = 64;
	int sbox;

	UINT64 A[25];
	for(int i=0;i<25;i++) A[i] = a2[i];
	unsigned int diff[320];
	gen_diff320(A, diff );
	for(unsigned int i =0; i< lanesize; i++){
		sbox = diff[i];
		laneFull ^= checkCollision[sbox][0]? (one<<i) : 0;
		laneHalf ^= checkCollision[sbox][1]? (one<<i) : 0;
	}
	UINT64 mask = 0xFFFFFFFF;
	int max = 0;
	for(int i = 0; i<64; i++){
		if(HW(mask&laneHalf, lanesize)>max){
			max = HW(mask&laneHalf, lanesize);
		}
		mask = rol64(mask,1);
	}
	int sum = HW(laneFull, lanesize)+max;
	if(sum>84) {
		cout<<hex<<laneFull<<endl;
		cout<<hex<<laneHalf<<endl;
	}
	return sum;
}
void combine_for_state(List list, vector<UINT64>& a2, vector< vector<UINT64> >& base)//pos points to the last BigNode
{
    int lev, din, i;
    BigNode* p;
    p = list.pos;
    //cout<<"lev: ";
//    cout<<endl;//
    while(p!=NULL){
        lev = p->level;
        //cout<<lev<<" ";
        din = (p->current)->din;
//        cout<<din<<" ";//
        for(i=0;i<5;i++){
            if(( (din >> i)&0x1) ) xorVector(a2,base[lev*5+i]);
        }
        p = p->pre;
    }
//    cout<<endl;//
}

bool myCheckCollision(List list, vector< vector<UINT64> >& base, unsigned int diff[320], bool checkCollision[32][2], int DDT_origin[32][32])
{
	int temp;
    BigNode *p;
    int total_active = 0;
	vector<UINT64> a2(25,0);
    bool flag = false;
    /** After the initialization, pos points to the last list node
      * "a2" is updated as the state difference before chi.
      * In other words after processing linear transformation on chosen compatible output difference of the core trail, "a2" is obtained.
      */
    initial_SL(&list, a2, base);
	if(testCollisionForIndiff( a2, checkCollision, DDT_origin)){
		cout<<"collision is possible! "<<endl;
		list.outputlist_forward(DDT_origin,diff);
		flag=true;
		//return true;
	}

    unsigned int diffBeforeChi[320];
	vector<UINT64> ZERO(25,0);


	int positive = 0;
	int negative = 1;
    p = list.pos;
    //cout<<list.pos->level<<endl;
    while(p!=NULL){
        //cout<<p->level<<endl;
        p->current = (p->current)->next;
        if((p->current)!=NULL){
            //cout<<(p->current)->din<<endl;
			a2.clear();
			a2.assign(ZERO.begin(), ZERO.end());
            //Process the chosen output difference pattern after chi with linear
            combine_for_state(list,a2, base);
            //cout<<"temp="<<temp<<endl;
            if(testCollisionForIndiff( a2, checkCollision, DDT_origin)){
                cout<<"collision is possible! "<<endl;
				list.outputlist_forward(DDT_origin,diff);
				//gen_diff320(a2,diffBeforeChi);

				positive++;
				flag = true;
				//return true;
            }
			else
				negative++;
            while(p->next!=NULL){ p = p->next;}
        }
        else{
            p->current = p->head;
            p = p->pre;
        }
    }

	cout<<"negtive = "<<dec<<negative<<endl;
	cout<<"positive = "<<dec<<positive<<endl;
    return flag;
}

int myCheckCollisionHW(List list, vector< vector<UINT64> >& base, unsigned int diff[320], bool checkCollision[32][2], int DDT_origin[32][32])
{
	int temp, ttemp;
    BigNode *p;
	vector<UINT64> a2(25,0);
    initial_SL(&list, a2, base);
	temp = testCollisionForIndiffHW( a2, checkCollision);
	cout<<"initial #collisionBit :"<<temp<<endl;

   cout<<"ddd"<<endl;
    unsigned int diffBeforeChi[320];
	vector<UINT64> ZERO(25,0);

    p = list.pos;
    //cout<<list.pos->level<<endl;
    while(p!=NULL){
        //cout<<p->level<<endl;
        p->current = (p->current)->next;
        if((p->current)!=NULL){
            //cout<<(p->current)->din<<endl;
			a2.clear();
			a2.assign(ZERO.begin(), ZERO.end());
            combine_for_state(list,a2, base);
			ttemp = testCollisionForIndiffHW( a2, checkCollision);
            if(ttemp>=temp){
                temp = ttemp;
				cout<<"cureent #collisionBit :"<<temp<<endl;
				list.outputlist(DDT_origin,diff);
				gen_diff320(a2,diffBeforeChi);
				for(int i=0; i<64;i++){
					if(diffBeforeChi[i]>0)
						cout<<diffBeforeChi[i]<<" ";
				}
				cout<<endl;
				//return true;
            }
            while(p->next!=NULL){ p = p->next;}
        }
        else{
            p->current = p->head;
            p = p->pre;
        }
    }
	cout<<"largest #collisionBit = "<<temp<<endl;

    return temp;
}
