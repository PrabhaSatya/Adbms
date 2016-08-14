#ifndef _globals_h
#define _globals_h

#define PAGESIZE 128

#define NOSPLIT   -1
#define NOTFOUND  -1
#define MAXKEYS  fanout-1
#define middleElementPosition (indexHeader.getFanout()-1)/2


#define typeInt 1
#define typeLong 5
#define typeFloat 3
#define typeDouble 4
#define typeString 2

#define indexHeaderPagePriority 'a'
#define rootPagePriority 'a'
#define indexPagePriority 'b'
#define leafPagePriority 'c'
#define defaultPriority 'b'

#define LEFT 1
#define RIGHT 2
#define PARENT 3
#define CHILD 4

#include<iostream>
#include<cstdlib>
#include<cstddef>
#include<queue>
#include<cstring>
#include<fstream>
#include<vector>
#include<math.h>



using namespace std;



bool debug_Insertion = false;
bool debug_Search = false;
bool debug_Deletion = false;


struct RID
{
	long int pageNumber;
	int slotNumber;

};

RID dummyRid = {-1,-1};



#endif

