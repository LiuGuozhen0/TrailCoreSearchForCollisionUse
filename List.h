#ifndef LIST_H
#define LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "L.h"
#include "misc.h"
#include <m4ri/m4ri.h>
#include <vector>

#define rol64(a, offset) ((offset != 0) ? ((((UINT64)a) << offset) ^ (((UINT64)a) >> (64-offset))) : a)


using namespace std;

struct Node
{
    int din;
    Node*next;
};

struct BigNode
{
    int level;
    Node* current;
    Node* head;
    BigNode *pre;
    BigNode *next;
};

class List
{
public:
    BigNode *pos;

    List(){pos=NULL;}

    void addAtEnd(int n, int lev, int indiff[32][32]);
    void setNULL();
    void outputlist(int DDT_origin[32][32], unsigned int outdiff[320]);
    void outputlist_forward(int DDT_origin[32][32], unsigned int outdiff[320]);
    void outputlist( unsigned int outdiff[320]);
};



void deleteNode(BigNode * target);  





List gen_SL( unsigned int diff[320], int DDT[32][32]);

mzd_t* initial_SL(List *list, mzd_t* vv);//pos points to the end after this function




int get_num_of_AS(mzd_t * res);


int get_num_of_AS(vector<UINT64> & a2);

int get_weight_of_AS( mzd_t * res, int DDT_origin[32][32]);


int combine_for_num(List list, mzd_t* vv, mzd_t* LI, mzd_t* res);//pos points to the last BigNode
void combine_for_state(List list, vector<UINT64>& a2, vector< vector<UINT64> >& base);//pos points to the last BigNode

int combine_for_weight(List list, mzd_t* vv, mzd_t* LI, mzd_t* res, int DDT_origin[32][32]);//pos points to the last BigNode


mzd_t* combine_for_collisiontest(List list, mzd_t* vv, mzd_t* L, mzd_t* res);//pos points to the last BigNode


int findminAS(List list, int DDT_origin[32][32], unsigned int diff[320]);


int findminWeight(List list, int DDT_origin[32][32], unsigned int diff[320]);

bool testCollision(mzd_t* res, bool checkCollision[32][2]);

int findCollision(List list, bool checkCollision[32][2], int DDT_origin[32][32], unsigned int diff[320]);

void xorVector(vector<UINT64>& a2, vector<UINT64>& abase);

int combine_for_num(List list, vector<UINT64>& a2, vector< vector<UINT64> >& base);//pos points to the last BigNode



void initial_SL(List *list, vector<UINT64>& a2, vector< vector<UINT64> >& base);//pos points to the end after this function


int findminASimproved(List list, vector< vector<UINT64> >& base, int DDT_origin[32][32], unsigned int diff[320]);
/**
 */
bool testCollisionForIndiff( vector<UINT64>& a2, bool checkCollision[32][2], int DDT_origin[32][32]);

/**
 */
bool myCheckCollision(List list, vector< vector<UINT64> >& base, unsigned int diff[320], bool checkCollision[32][2], int DDT_origin[32][32]);


int HW(UINT64 a, int n);

int testCollisionForIndiffHW( vector<UINT64>& a2, bool checkCollision[32][2]);
int myCheckCollisionHW(List list, vector< vector<UINT64> >& base, unsigned int diff[320], bool checkCollision[32][2], int DDT_origin[32][32]);

#endif
