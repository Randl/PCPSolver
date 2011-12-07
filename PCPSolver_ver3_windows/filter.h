//////////////////////////////////////////////////////////////////////////////// 
// filter.h                                                                   //
// Description: Implement filters                                             //  
// Created time:   March                                                      //  
// Modified time:  May 11, 2001                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#ifndef FILTER_H
#define FILTER_H

#include "PCPInstance.h"

// class CFilter contains only methods and no member variables
// thus all its methods are declared as static
class CFilter
{
private:
	static int Pass_Trivialcase_Filter(const CPCPInstance *pPCP);
	static int Pass_Prefix_Filter(const CPCPInstance *pPCP);
	static int Pass_Postfix_Filter(const CPCPInstance *pPCP);
	static int Pass_Length_Balance_Filter(const CPCPInstance *pPCP);
	static int Pass_Element_Balance_Filter(const CPCPInstance *pPCP);

public:
	static int PassAllFilters(const CPCPInstance *pPCP);
};

#endif   //FILTER_H

