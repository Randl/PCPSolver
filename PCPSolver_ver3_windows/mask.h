//////////////////////////////////////////////////////////////////////////////// 
// mask.h                                                                     //
// Description:                                                               //  
//   Implementation of mask method.											  //
// Created time:   May 12, 2001                                               //  
// Modified time:  May 12, 2001                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#ifndef MASK_H
#define MASK_H

#include "PCPInstance.h"

class CMask
{
private:
	static int FindSideMask(CPCPInstance *pPCP, CPCPInstance *pReversePCP, int maskflag);
	static int FindTurnoverMask(CPCPInstance *pPCP, CPCPInstance *pReversePCP);
	static int SymmetryProcessSide(CPCPInstance *pPCP, CPCPInstance *pReversePCP);
	
public:
	static int FindMask(CPCPInstance *pPCP, CPCPInstance *pReversePCP);
	static void SymmetryProcess(CPCPInstance *pPCP, CPCPInstance *pReversePCP);
};

#endif

