//////////////////////////////////////////////////////////////////////////////// 
// PCPInstance.h                                                              //
// Description:                                                               //  
//   Implementation of class CConfig, CPair and CPCPInstance                  //
// Created time:   May 12, 2001                                               //  
// Modified time:  May 13, 2001                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#ifndef PCPINSTANCE_H
#define PCPINSTANCE_H

#include "def.h"


class CPair
{
public:
	char up[16+1]; // the last char is always SENTINEL
	char down[16+1];
	int uplen;
	int downlen;
    int diff0, diff1;
	int ID;
    void CountElementDiff();
	
	void Copy(CPair *pPair)
	{
		uplen = pPair->uplen;
		downlen = pPair->downlen;
		ID = pPair->ID;
        diff0 = pPair->diff0;
        diff1 = pPair->diff1;
		memcpy(up, pPair->up, uplen+1);
		memcpy(down, pPair->down, downlen+1);
	}
	
    int Compare(CPair *pPair)
    {
        if (uplen != pPair->uplen || downlen != pPair->downlen || ID != pPair->ID) return 0;
		if (memcmp(up, pPair->up, uplen+1)!=0) return 0;
		if (memcmp(down, pPair->down, downlen+1)!=0) return 0;
        return 1;
    }
};


class CPCPInstance
{
public:
	int size;
	int width;
	int up_offset;
	int down_offset;
	int offset;
	int downmask;
	int upmask;
    int turnover_downmask;
    int turnover_upmask;
    int gcd;
    
    // for experiment
    char comment[600];
    int sol_len;
    int sol_num;
    long sol_time[600]; 
    long node_num[600];
    long cutoff_num[600];
    long depth[600];
    int poi;  // number of iterations

    // for pruning
    int up_0drop;
    int up_1drop;
    int down_0drop;
    int down_1drop;

	int arrStartingPoint[MAXSIZE+1];
	CPair arrPair[MAXSIZE+1]; // the uplen of the last one is always 0 as the guard


	// max up & down strings (for lookahead heuristic)
	int maxup;
	int maxdown;
   

    CPCPInstance();    
	void Init();    
	void CountOffset();
    
    void Copy(CPCPInstance *pPCP);
    int Compare(CPCPInstance *pPCP);
    void CreateReversePCP(CPCPInstance *pReversePCP);
    void TopDownExchange();
    void ZeroOneExchange();
    void PairExchange(int i, int j);
	int TestPurityFeature();
};

#endif

