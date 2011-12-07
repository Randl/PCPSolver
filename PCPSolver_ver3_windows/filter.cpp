//////////////////////////////////////////////////////////////////////////////// 
// filter.cpp                                                                 //
// Description: It includes five types of filters.                            //  
//  Trivialcase filter, prefix/postfix filter, element balance filter         //
//  and length blance filter                                                  //                   
// Created time:   March, 2001                                                //  
// Modified time:  Feb 3, 2003                                                //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#include <stdio.h>
#include "filter.h"


// use for outputing filter information
// #define DEBUG_FILTER

// identify the trivial case where some string has its downstring 
// and upstring are identical
int CFilter::Pass_Trivialcase_Filter(const CPCPInstance *pPCP)
{
	int i;
	const CPair *p;
	for (i=0;i<pPCP->size;i++)
	{
		p = &pPCP->arrPair[i];
		if (p->downlen == p->uplen)
			if (!memcmp(p->down, p->up, p->downlen))
				return 0;
	}
	return 1;
}

// check whether there exists one pair whose one string is the
// prefix of the other
int CFilter::Pass_Prefix_Filter(const CPCPInstance *pPCP)
{
	int i, len;
	const CPair *p;

	for (i=0;i<pPCP->size;i++)
	{
		p = &pPCP->arrPair[i];
		
		if (p->downlen>p->uplen) 
			len = p->uplen;
		else if (p->downlen<p->uplen) 
			len = p->downlen;
		
		if (!strncmp(p->down, p->up, len)) return 1;
	}

	return 0;
}

// check whether there exists one pair whose one string is the
// postfix of the other
int CFilter::Pass_Postfix_Filter(const CPCPInstance *pPCP)
{
	int i;
	const CPair *p;
	const char *str;
	for (i=0;i<pPCP->size;i++)
	{
		p = &pPCP->arrPair[i];

		if (p->downlen>p->uplen) 
		{
			str = p->down+(p->downlen - p->uplen);
			if (!strncmp(str, p->up, p->uplen)) return 1;
		}
		else if (p->downlen<p->uplen)
		{
			str = p->up+(p->uplen-p->downlen);
			if (!strncmp(str, p->down, p->downlen)) return 1;
		}
	}

	return 0;
}

// Check whether there exists one pair with upstring longer 
// than downstring and one pair with downstring longer than upstring
int CFilter::Pass_Length_Balance_Filter(const CPCPInstance *pPCP)
{
	int i;
	int upflag = 0;
	int downflag = 0;
	const CPair *p;

	for (i=0;i<pPCP->size;i++)
	{
		p = &pPCP->arrPair[i];
		if (p->downlen>p->uplen)
			downflag = 1;
		else if (p->downlen<p->uplen) 
			upflag = 1;
	}

	if (downflag&&upflag) return 1;
	else return 0;
}

// Check whether there exists one pair whose upstring containing 
// more 1's than downstring, and another pair whose downstring 
// containing more 1's than upstring. The same situation for element 0
int CFilter::Pass_Element_Balance_Filter(const CPCPInstance *pPCP)
{
	int i ,j;
	int zero_upflag = 0;
	int zero_downflag = 0;
	int one_upflag = 0;
	int one_downflag = 0;
	int one_upcount;
	int zero_upcount;
	int one_downcount;
	int zero_downcount;
	const CPair *p;

	for (i=0;i<pPCP->size;i++)
	{
		one_upcount = 0;
		zero_upcount = 0;
		one_downcount = 0;
		zero_downcount = 0;
		p = &pPCP->arrPair[i];

		for (j=0; j<p->uplen;j++)
		{
			if (p->up[j] == 0)
				zero_upcount++;
			else one_upcount++;
		}

		for (j=0; j<p->downlen;j++)
		{
			if (p->down[j] == 0)
				zero_downcount++;
			else one_downcount++;
		}

		if (zero_upcount>zero_downcount)
			zero_upflag = 1;
		else if (zero_upcount<zero_downcount)
			zero_downflag = 1;

		if (one_upcount>one_downcount)
			one_upflag = 1;
		else if (one_upcount<one_downcount)
			one_downflag = 1;
	}

	if (zero_upflag == 1 && zero_downflag==0 || 
		zero_upflag == 0 && zero_downflag==1 ) 
		return 0; 

	if (one_upflag == 1 && one_downflag==0 || 
		one_upflag == 0 && one_downflag==1  ) 
		return 0;

	return 1;
}

// reture value:
// -1: trivial case
// -2: element balance violated  
// 0: no solution
// 1: pass all filters
int CFilter::PassAllFilters(const CPCPInstance *pPCP)
{
	// has solution, but it is the trivial case
	if (!Pass_Trivialcase_Filter(pPCP)) 
	{
#ifdef DEBUG_FILTER
		cout<<"Trivial case: one pair contains the same strings\n"; 
#endif
		return -1;
	}

	if (!Pass_Prefix_Filter(pPCP))
	{
#ifdef DEBUG_FILTER
		cout<<"Cannot pass the prefix filter!\n";
#endif
		return 0;
	}

	if (!Pass_Postfix_Filter(pPCP))
	{
#ifdef DEBUG_FILTER
		cout<<"Cannot pass the postfix filter!\n";
#endif
		return 0;
	}

	if (!Pass_Length_Balance_Filter(pPCP))
	{
#ifdef DEBUG_FILTER
		cout<<"Cannot pass the length balance filter!\n";
#endif
		return 0;
	}

	if (!Pass_Element_Balance_Filter(pPCP))
	{
#ifdef DEBUG_FILTER
		cout<<"Cannot pass the element balance filter!\n";
		cout<<"Some pair can not be used!\n";
#endif
		if (pPCP->size<=2) return 0;
#ifdef USE_EXCLUSION
        else return -2; //It may have solution. Need improvement
#endif
	}

	return 1;
}


