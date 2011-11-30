//////////////////////////////////////////////////////////////////////////////// 
// exclusion.h                                                                //
// Description:                                                               //  
//   Implement the exclusion method to find the excluded pairs when starting  //
//   from one pair.                                                           //
// Created time:   March                                                      //  
// Modified time:  May 11, 2001                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#ifndef EXCLUSION_H
#define EXCLUSION_H

#include "PCPInstance.h"

class CExclusion
{
private:
	static int CopyToString(char *from, char *to);
	static void ReversePair(const CPair *from, CPair *to);
	
	static int MatchTail(char **str, int num, char *matchedstr);
	static int MatchHead(char **str, int num, char *matchedstr);	

public:
	static int Exclusion_Method(CPCPInstance *pPCP, int iterative_flag=1);
};

#endif //EXCLUSION_H
