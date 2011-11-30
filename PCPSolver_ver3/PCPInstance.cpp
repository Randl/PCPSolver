//////////////////////////////////////////////////////////////////////////////// 
// PCPInstance.cpp                                                            //
// Description:                                                               //  
//   Implementation of class CConfig, CPair and CPCPInstance                  //
// Created time:   May 12, 2001                                               //  
// Modified time:  Feb  3, 2003                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#include "PCPInstance.h"




//////////////////////////////////////////////////////////////////////
// Implementation of CPair

void CPair::CountElementDiff()
{
    int j;
    int one_upcount = 0, zero_upcount = 0, one_downcount = 0, zero_downcount = 0;

    for (j=0; j<uplen;j++)
    {
        if (up[j] == 0)
            zero_upcount++;
        else one_upcount++;
    }
    for (j=0; j<downlen;j++)
    {
        if (down[j] == 0)
            zero_downcount++;
        else one_downcount++;
    }
    diff1 = one_upcount - one_downcount;
    diff0 = zero_upcount - zero_downcount;
}

// End of CPair
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Implementation of CPCPInstance

void CPCPInstance::PairExchange(int i, int j)
{
    CPair tmp;
    tmp.Copy(&arrPair[i]);
    arrPair[i].Copy(&arrPair[j]);
    arrPair[j].Copy(&tmp);
    arrPair[i].ID = i+1;
    arrPair[j].ID = j+1;
}

void CPCPInstance::Copy(CPCPInstance *pPCP)
{
    int i;
    size = pPCP->size;
    width = pPCP->width;
    up_offset = pPCP->up_offset;
    down_offset = pPCP->down_offset;
    offset = pPCP->offset;
    downmask = pPCP->down_offset;
    upmask = pPCP->up_offset;
    up_0drop = pPCP->up_0drop;
    up_1drop = pPCP->up_1drop;
    down_0drop = pPCP->down_0drop;
    down_1drop = pPCP->down_1drop;
    for (i=0;i<size;i++) arrPair[i].Copy(&pPCP->arrPair[i]);
}

// equal: 1
// others: 0
int CPCPInstance::Compare(CPCPInstance *pPCP)
{
    int i;
    if (size != pPCP->size) return 0;
    if (width != pPCP->width) return 0;

    for (i=0;i<size;i++) 
        if (!arrPair[i].Compare(&pPCP->arrPair[i])) return 0;

    return 1;
}

void CPCPInstance::Init()
{ 
    int i;
    for (i=0;i<MAXSIZE+1;i++)
    {
        arrPair[i].uplen=0;
        arrPair[i].downlen=0;
        arrPair[i].ID = i+1;
        arrStartingPoint[i] = 1;
    }
    up_0drop = 0;
    up_1drop = 0;
    down_0drop = 0;
    down_1drop = 0;
    size = 0;
    width = 0;
    offset = 0;
    up_offset = 0;
    down_offset = 0;
    downmask = 0;
    upmask = 0;
    turnover_downmask = 0;
    turnover_upmask = 0;
    gcd = 1;
	poi = 0;
}

CPCPInstance::CPCPInstance() 
{
    Init();
}

void CPCPInstance::CountOffset()
{
    int i, j;
    offset = 0;
    up_offset = 0;
    down_offset = 0;
    gcd = 1;
    up_0drop = 0;
    up_1drop = 0;
    down_0drop = 0;
    down_1drop = 0;

    int tmp;
    int arrOffset[MAXSIZE];
    for (i=0;i<size;i++)
    {
        tmp = arrPair[i].uplen - arrPair[i].downlen;
        if (tmp>0 && up_offset<tmp)
            up_offset = tmp;
        else if (tmp<0 && down_offset<-tmp)
            down_offset = -tmp;
        arrOffset[i] = abs(tmp);
    
        arrPair[i].CountElementDiff();
        if (down_0drop<arrPair[i].diff0) down_0drop = arrPair[i].diff0;
        if (up_0drop<-arrPair[i].diff0) up_0drop = -arrPair[i].diff0;
        if (down_1drop<arrPair[i].diff1) down_1drop = arrPair[i].diff1;
        if (up_1drop<-arrPair[i].diff1) up_1drop = -arrPair[i].diff1;
    }
    offset = up_offset;
    if (offset<down_offset) offset = down_offset;

	// count gcd
    int flag;
    for (j=offset;j>1;j--)
    {
        flag = 1;
        for (i=0;i<size;i++)
            if (arrOffset[i]%j!=0) { flag=0; break;}
        if (flag) { gcd = j; break; }
    }


	// count maxup and maxdown string
	maxup = 0;
	maxdown = 0;
    for (i=0;i<size;i++)
    {
		if (arrPair[i].uplen>maxup) maxup = arrPair[i].uplen;
		if (arrPair[i].downlen>maxdown) maxdown = arrPair[i].downlen;
	}

}

// Create the inverse instance for a given PCP instance
void CPCPInstance::CreateReversePCP(CPCPInstance *pReversePCP)
{
    pReversePCP->Init();
	int i, j, len;
	CPair *from, *to;

	// copy member attributions
	pReversePCP->size = size;
	pReversePCP->width = width;
	pReversePCP->up_offset = up_offset;
	pReversePCP->down_offset = down_offset;
	pReversePCP->offset = offset;
    pReversePCP->gcd = gcd;

    // copy the strings
	for (i=0;i<size;i++)
	{
		from = &arrPair[i];
		to = &pReversePCP->arrPair[i];
		to->ID = from->ID;
		to->downlen = from->downlen;
		to->uplen = from->uplen;
		len = from->uplen;
		for (j=0;j<len;j++)
			to->up[j] = from->up[len-1-j];
		to->up[len] = SENTINEL;
		len = from->downlen;
		for (j=0;j<len;j++)
			to->down[j] = from->down[len-1-j];
		to->down[len] = SENTINEL;
	}

    pReversePCP->CountOffset();
}

void CPCPInstance::TopDownExchange()
{
    int i, len;
	char str[MAXWIDTH+1]; 
    CPair *pPair;

    // exchange len and strings
    for (i=0;i<size;i++)
    {
        pPair = &arrPair[i];
        len = pPair->uplen;
        memcpy(str, pPair->up, pPair->uplen+1);
        pPair->uplen = pPair->downlen;
	    memcpy(pPair->up, pPair->down, pPair->downlen+1);
        pPair->downlen = len;
        memcpy(pPair->down, str, pPair->downlen+1);
    }

    len = up_offset;
    up_offset = down_offset;
    down_offset = len;
    CountOffset();
}

void CPCPInstance::ZeroOneExchange()
{
    int i, j;
    CPair *pPair;

    for (i=0;i<size;i++)
    {
        pPair = &arrPair[i];
        for (j=0;j<pPair->uplen;j++) pPair->up[j]=!pPair->up[j];
        for (j=0;j<pPair->downlen;j++) pPair->down[j]=!pPair->down[j];
    }

    CountOffset();
}

// Purity feature: there is one pair consisting entirely of one symbol
// return 1 if yes, 0 otherwise
int CPCPInstance::TestPurityFeature()
{
	int allsame;
    int i, k;
    for (i=0;i<size;i++)
    {   
        allsame = 1;
        for (k=1;k<arrPair[i].uplen;k++)
            if (arrPair[i].up[k]!=arrPair[i].up[0])
            {
                allsame = 0;
                break;
            }
        if (allsame)
            for (k=0;k<arrPair[i].downlen;k++)
                if (arrPair[i].down[k]!=arrPair[i].up[0])
                {
                    allsame = 0;
                    break;
                }		
         if (allsame) return 1;
    }
    return 0;
}

// End of CPCPInstance
//////////////////////////////////////////////////////////////////////


