//////////////////////////////////////////////////////////////////////////////// 
// mask.cpp                                                                   //
// Description: Implementation of mask method.                                //  
//   There are two types of masks: side mask and turnover mask. Side mask is  //
//   the mask we typically refer to, and please look up the turnover mask in  //
//   my thesis. The routine FindSideMask is the general way to find the mask, //
//   i.e., check whether there are generable and solvable critical configs.   //
//   Another way to identify masks is to use the symemtry method, which is    //
//   also discussed at length in my thesis.                                   //
// Created time:   May 12, 2001                                               //  
// Modified time:  May 12, 2001                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#include "mask.h"
#include "solver.h"

extern int TEST_DEPTH;

/*
downstring:
-----------------
|               |
-----------------
^   ^       ^   ^
0   m 	    j   n-1 
    <-- t -->
<------ n ------>
	
arr[0..m-1] -> headstring (m>=1)
arr[m..j] -> matchedstring, equal to upstring(t = j-m)
arr[j+1..n-1] -> tailstring
  
  flag == 0 : find downmask
  flag == 1 : find upmask
  flag == 2 : find turnover_downmask
  flag == 3 : find turnover_upmask
*/
int CMask::FindSideMask(CPCPInstance *pPCP, CPCPInstance *pReversePCP, int maskflag)
{
	int i, j, k, m, n, t;
	CPair *pPair;
	int ret;
	int maxlen;
	char *longstr, *shortstr;
    int *pIntMask;
    int up = maskflag%2;

    switch (maskflag)
    {
        case 0: pIntMask = &pPCP->downmask; break;
        case 1: pIntMask = &pPCP->upmask; break;
        case 2: pIntMask = &pPCP->turnover_downmask; break;
        case 3: pIntMask = &pPCP->turnover_upmask; break;
        default: assert(0);
    }

	for (i=0;i<pPCP->size;i++)
	{
		pPair = &pPCP->arrPair[i];

		if (up)  // up
		{ 
			n = pPair->downlen;
			t = pPair->uplen;
			longstr = pPair->down;
			shortstr = pPair ->up;
		}
		else // down
		{
			t = pPair->downlen;
			n = pPair->uplen;
			shortstr = pPair->down;
			longstr = pPair ->up;
		}

		if (n > t)
		{
			for (k=0; k<n-t;k++)
			{
				j = n-1-k;
				m = j-t+1;

                // they must be the multiple of gcd
                if (m%pPCP->gcd!=0 || (n-1-j)%pPCP->gcd!=0)
                    continue;

				// the string must be matched
				if (memcmp(shortstr, longstr+m, t)) continue;
				
				// the tail string in the bottom should possibly lead to the solution
				if (j<n-1)
				{
					maxlen = n-j-1+MAXLOOPCOUNT*pPCP->offset;
					CConfig config(maxlen);
					config.ConfigAssign(longstr+j+1, n-j-1, !up);
					config.depth = 1;
					ret = CSolver::BeginSolveConfig(pPCP, &config, maxlen, TEST_DEPTH);
					if (ret == -1) continue;
				}
                else if (maskflag>=2) // find turnover
                    continue;

				// the head string should be generated, so we use the reverse PCP to parse it
				maxlen = m + MAXLOOPCOUNT*pPCP->offset;
				CConfig config(maxlen);
				int index;
				char *str = new char[m];

				for (index=0;index<m;index++)
					str[index] = longstr[m-index-1];

				config.ConfigAssign(str, m, !up);
				delete str;
				config.depth = 1;
				ret = CSolver::BeginSolveConfig(pReversePCP, &config, maxlen, TEST_DEPTH);
				if (ret == -1) continue;
				else // no mask!
				{
                    *pIntMask = 0;
					return 0;
				}
			}
		}
	}

    *pIntMask = 1;

	return 1;
}

int CMask::FindMask(CPCPInstance *pPCP, CPCPInstance *pReversePCP)
{
	FindSideMask(pPCP, pReversePCP, 1);
	FindSideMask(pPCP, pReversePCP, 0);
	FindSideMask(pReversePCP, pPCP, 1);
	FindSideMask(pReversePCP, pPCP, 0);
	
    FindSideMask(pPCP, pReversePCP, 1);
	FindSideMask(pPCP, pReversePCP, 0);
	FindSideMask(pReversePCP, pPCP, 1);
	FindSideMask(pReversePCP, pPCP, 0);

    if (pPCP->upmask) pReversePCP->downmask = 1;
    if (pPCP->downmask) pReversePCP->upmask = 1;
    if (pReversePCP->upmask) pPCP->downmask = 1;
    if (pReversePCP->downmask) pPCP->upmask = 1;

    return 1;
}

int CMask::FindTurnoverMask(CPCPInstance *pPCP, CPCPInstance *pReversePCP)
{
	FindSideMask(pPCP, pReversePCP, 2);
	FindSideMask(pPCP, pReversePCP, 3);
    return 1;
}

int CMask::SymmetryProcessSide(CPCPInstance *pPCP, CPCPInstance *pReversePCP)
{
    int i;
    
	//flag = 2:default  1:all staring is up  0:all staring is down  -1: mixed
    int flag = 2;
    CPair *pPair;

    for (i=0;i<pPCP->size;i++)
    {
        if (pPCP->arrStartingPoint[i]!=1) continue;

        pPair = &pPCP->arrPair[i];
        if (pPair->uplen > pPair->downlen)
        {
            if (flag==2) flag = 1; // up
            else if (flag==0) flag = -1;
        }
        else if (pPair->uplen < pPair->downlen)
        {
            if (flag==2) flag = 0; // up
            else if (flag==1) flag = -1;
        }
        else assert(0);
    }
    if (flag==-1 || flag==2) return 0;
    assert(flag<=1 && flag>=0);

	// the original all startinged from top, the reverse must start from bottom
    if (flag && pPCP->turnover_upmask) // have downmask
    { 
        pPCP->downmask = 1;
        pReversePCP->upmask = 1;
        for (i=0;i<pReversePCP->size;i++)
            if ( pReversePCP->arrStartingPoint[i] && 
				 pReversePCP->arrPair[i].uplen>pReversePCP->arrPair[i].downlen )
                pReversePCP->arrStartingPoint[i] = 0;
    }

	// the original all startinged from bottom, the reverse must start from top
    if (!flag && pPCP->turnover_downmask) // have upmask
    { 
        pPCP->upmask = 1;
        pReversePCP->downmask = 1;
        for (i=0;i<pReversePCP->size;i++)
            if ( pReversePCP->arrStartingPoint[i] && 
				 pReversePCP->arrPair[i].uplen<pReversePCP->arrPair[i].downlen )
                pReversePCP->arrStartingPoint[i] = 0;
    }

    return 1;
}

void CMask::SymmetryProcess(CPCPInstance *pPCP, CPCPInstance *pReversePCP)
{
    SymmetryProcessSide(pPCP, pReversePCP);
    SymmetryProcessSide(pReversePCP, pPCP);
}

