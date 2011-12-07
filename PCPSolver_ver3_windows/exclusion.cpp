//////////////////////////////////////////////////////////////////////////////// 
// exclusion.cpp                                                              //
// Description:                                                               //  
//   Implement the exclusion method to find the excluded pairs when starting  //
//   from one pair.                                                           //
// Created time:   March , 2001                                               //  
// Modified time:  Feb  2, 2003                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#include "exclusion.h"
#include "solver.h"


extern int TEST_DEPTH;

// return value:
//  0 - fail
//  1 - succeed
// test whether the combination str[0], str[1], ..., str[num-1] can contain 
// matchedstr as the prefix called by MatchHead
int CExclusion::MatchTail(char **str, int num, char *matchedstr)
{
	int i;
	int slen, mlen;
	mlen = strlen(matchedstr);
	int succ_flag = 0;
	for (i=0;i<num;i++)
	{
		slen = strlen(str[i]);

		// try matching str[i] in matchedstr as the prefix, need do recursively
		if (slen < mlen) 
		{
			// str[i] is the prefix of matchedstr? 
			if (!memcmp(str[i], matchedstr, slen)) 
			{
				// get rid of the matched part and continue matching
				succ_flag = MatchTail(str, num, matchedstr+slen); 
				if (succ_flag) return 1;
			}
		}
		else if (!memcmp(str[i], matchedstr, mlen)) 
			return 1; // find a match if matchedstr is the prefix of str[i]		
	}

	return 0;
}

// return value:
//  0 - fail
//  1 - succeed
// test whether the combinations of str[0], str[1], ..., str[num-1] can 
// contain matchedstr. MatchHead will first examine whether there is 
// one string whose postfix is the prefix of the matchedstr, then call 
// MatchTail to match left substring.
int CExclusion::MatchHead(char **str, int num, char *matchedstr)
{
	int i, j;
	int offset; // max matched length

	// slen -- length of searched string   mlen - length of matched string
	int slen, mlen; 
	mlen = strlen(matchedstr);
	int succ_flag = 0;
	
	for (i=0;i<num;i++)
	{
		slen = strlen(str[i]);
	    
		if (mlen<=slen) // test whether matchedstr is contained in str[i]
			for (j=0;j<=slen-mlen;j++)
				if (!memcmp(str[i]+j, matchedstr, mlen)) return 1;

		offset = slen < mlen ? slen : mlen; // choose the less one

		// j -- the length of matched part
		for (j=offset;j>=1;j--)
			 // test whether the postfix of str[i] is the prefix of matchedstr
			if (!memcmp(str[i]+slen-j, matchedstr, j))
			{
				succ_flag = MatchTail(str, num, matchedstr+j);
				if (succ_flag) return 1;
			}
	}

	return 0;
}

int CExclusion::CopyToString(char *from, char *to)
{
	int i= 0;
	while (from[i] != SENTINEL)
		to[i] = from[i++] + '0';
	to[i] = '\0'; 
	return 1;
}

void CExclusion::ReversePair(const CPair *from, CPair *to)
{
	int len, j;
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

int CExclusion::Exclusion_Method(CPCPInstance *pPCP, int iterative_flag)
{
	int i;
	int j, k;
	int len;
	CPair *pPair;
	char str0[MAXWIDTH+1];
	char str1[MAXWIDTH+1];
	char *str[MAXSIZE];
	char mstr[MAXWIDTH+1];
	int upflag;
	int startflag;

	for (i=0;i<pPCP->size;i++)
		pPCP->arrStartingPoint[i] = 0; 
    
	// find the starting point
	for (i=0;i<pPCP->size;i++)
	{
		pPair = &pPCP->arrPair[i];
		if (pPair->uplen < pPair->downlen)
		{
            if (pPCP->downmask) continue;
			len = pPair->uplen;
			upflag = 0;
		}
		else
		{
            if (pPCP->upmask) continue;
			len = pPair->downlen;
			upflag = 1;
		}
		if (!memcmp(pPair->up, pPair->down, len)) // find one starting point
		{
			pPCP->arrStartingPoint[i] = 1;

			if (upflag) CopyToString(pPCP->arrPair[i].up, str0);
			else CopyToString(pPCP->arrPair[i].down, str0);
			str[0] = str0;	
			
			// test whether after choosing this pair, no other pair can be chosen.
			// one pair excludes all others
			// added on June 5
			startflag = 0;
			for (j=0;j<pPCP->size;j++) //test the pair
			{
				if (j==i) continue;
				if (upflag) CopyToString(pPCP->arrPair[j].down, mstr);
				else CopyToString(pPCP->arrPair[j].up, mstr);
				if (MatchHead(str, 1, mstr))
				{
					startflag=1;
					break;
				}
				if (upflag)
				{
					CPair *pPair=&pPCP->arrPair[j];
					int t;
					for (t=1;t<=pPair->downlen-1;t++)
					{
						int length = pPair->uplen;
						if (length>pPair->downlen-t) length = pPair->downlen-t;
						if (!memcmp(pPair->down+t, pPair->up, length))
						{
							CPCPInstance PCP;
							PCP.size = 1;
							PCP.width = 3;
							CConfig config(200);
							int l;
							ReversePair(&pPCP->arrPair[i], &PCP.arrPair[0]);
							PCP.CountOffset();
							char str[MAXWIDTH];
							for (l=0;l<t;l++)
								str[l] = pPair->down[t-l-1];
							config.ConfigAssign(str, t, 0);
							config.depth=1;
							int ret = CSolver::BeginSolveConfig(&PCP, &config, 200, TEST_DEPTH);
							if (ret>0) //can be solved 
							{
								startflag = 1;
								break;
							}
						}// if
					} // for
					if (startflag) break;
				}
				else
				{
					int t;
					CPair *pPair=&pPCP->arrPair[j];
					for (t=1;t<=pPair->uplen-1;t++)
					{
						int length = pPair->downlen;
						if (length>pPair->uplen-t) length = pPair->uplen-t;
						if (!memcmp(pPair->up+t, pPair->down, length))
						{
							CPCPInstance PCP;
							PCP.size = 1;
							PCP.width = 3;
							CConfig config(200);
							int l;
							ReversePair(&pPCP->arrPair[i], &PCP.arrPair[0]);
							PCP.CountOffset();
							char str[MAXWIDTH];
							for (l=0;l<t;l++)
								str[l] = pPair->up[t-l-1];
							config.ConfigAssign(str, t, 1);
							config.depth=1;
							int ret = CSolver::BeginSolveConfig(&PCP, &config, 200, TEST_DEPTH);
							if (ret>0) 
							{
								startflag = 1;
								break;
							}
						}//if
					}//for
					if (startflag) break;
				}//else
			}// for
			if (startflag==0)
			{
				pPCP->arrStartingPoint[i] = 0;
				continue;
			}

			// two pairs exclude one pair
            if (pPCP->size<=2) continue;
			for (j=0;j<pPCP->size;j++) //test the pair
			{
				if (j==i) continue;
				startflag = 0;
				if (upflag) CopyToString(pPCP->arrPair[j].up, str1);
				else CopyToString(pPCP->arrPair[j].down, str1);
				str[1] = str1;

				for (k=0;k<pPCP->size;k++)
				{
					if (k==i || k==j) continue;
					if (upflag) CopyToString(pPCP->arrPair[k].down, mstr);
					else CopyToString(pPCP->arrPair[k].up, mstr);
					if (MatchHead(str, 2, mstr)) 
					{
						startflag = 1;
						break;
					}
				}
				// print out debug message
				if (startflag) continue;

				// now probably you find can exclude one pair, added on May 25
				for (k=0;k<pPCP->size;k++)
				{
					if (k==i || k==j) continue;
					if (upflag)
					{
						if (pPCP->arrPair[j].uplen<pPCP->arrPair[j].downlen) 
						{ // the second pair should have the same direction as the first pair
							startflag = 1;
							break;
						}
						int t;
						CPair *pPair=&pPCP->arrPair[k];
						for (t=1;t<=pPair->downlen-1;t++)
						{
							int length = pPair->uplen;
							if (length>pPair->downlen-t) length = pPair->downlen-t;
							if (!memcmp(pPair->down+t, pPair->up, length))
							{
								CPCPInstance PCP;
								PCP.size = 2;
								PCP.width = 3;
								CConfig config(200);
								int l;
								ReversePair(&pPCP->arrPair[i], &PCP.arrPair[0]);
								ReversePair(&pPCP->arrPair[j], &PCP.arrPair[1]);
								PCP.CountOffset();
								char str[MAXWIDTH];
								for (l=0;l<t;l++)
									str[l] = pPair->down[t-l-1];
								config.ConfigAssign(str, t, 0);
								config.depth=1;
								int ret = CSolver::BeginSolveConfig(&PCP, &config, 200, TEST_DEPTH);
								if (ret>0) //can be solved
								{
									startflag = 1;
									break;
								}
								if (ret==0 && ! (PCP.arrPair[1].uplen>=PCP.arrPair[1].downlen))
								{  
									//the config can not be solved and the two pairs are not in 
									// the same direction 
									startflag = 1;
									break;
								}
							}// if
						} // for
						if (startflag) break;
					} // if
					else
					{
						if (pPCP->arrPair[j].uplen>pPCP->arrPair[j].downlen) 
						{ // the second pair should have the same direction as the first pair
							startflag = 1;
							break;
						}						
						int t;
						CPair *pPair=&pPCP->arrPair[k];
						for (t=1;t<=pPair->uplen-1;t++)
						{
							int length = pPair->downlen;
							if (length>pPair->uplen-t) length = pPair->uplen-t;
							if (!memcmp(pPair->up+t, pPair->down, length))
							{
								CPCPInstance PCP;
								PCP.size = 2;
								PCP.width = 3;
								CConfig config(200);
								int l;
								ReversePair(&pPCP->arrPair[i], &PCP.arrPair[0]);
								ReversePair(&pPCP->arrPair[j], &PCP.arrPair[1]);
								PCP.CountOffset();
								char str[MAXWIDTH];
								for (l=0;l<t;l++)
									str[l] = pPair->up[t-l-1];
								config.ConfigAssign(str, t, 1);
								config.depth=1;
								int ret = CSolver::BeginSolveConfig(&PCP, &config, 200, TEST_DEPTH);
								if (ret>0) 
								{
									startflag = 1;
									break;
								}
								if (ret==0 && !((PCP.arrPair[1].uplen=PCP.arrPair[1].downlen)))
								{  
									// the config cannot be solved and the two pairs 
									// arenot in the same direction
									startflag = 1;
									break;
								}
							}// if
						}// for		
						if (startflag) break;
					} // else
				} // for	
				if (startflag) continue;

				// now you can safely have two pairs
				
				// should be commented later
#ifdef USE_EXCLUSION
                pPCP->arrStartingPoint[i] = 0;
				continue;
#endif

/*
            	int ret;
				CPCPInstance PCP;
				PCP.size = 2;
				PCP.width = pPCP->width;
				PCP.down_offset = pPCP->down_offset;
				PCP.up_offset = pPCP->up_offset;
                PCP.offset = PCP.down_offset;
                if (PCP.offset < PCP.up_offset) PCP.offset = PCP.up_offset;
				PCP.arrPair[0].Copy(&pPCP->arrPair[i]);
				PCP.arrPair[0].ID = 1;
				PCP.arrPair[1].Copy(&pPCP->arrPair[j]);
				PCP.arrPair[1].ID = 2;
				ret = SolveInstance(&PCP, iterative_flag);
				if (ret == 0) // unresolved
				{
					fprintf(ofile, "Trivial case with pair %d and %d! Can not solve it to depth %d!\n",
					i+1, j+1, iterative_depth_threshold);
					return 0;
				}
				// find solution in trivial case, let the solution checking part to detect it, 
				// need improvement
				if (ret == 1) 
					continue;
				else if (ret == -1)
				{
					fprintf(ofile, "You can not start at pair %d and %d!\n", i+1, j+1);
					pPCP->arrStartingPoint[i] = 0;
				}*/
			}
		}
		else pPCP->arrStartingPoint[i] = 0;
	}
	return 1;
}

/*
int test()
{
	char str1[] = "1";
	char str2[] = "10";
	char *str[MAXSIZE];
	str[0] = str1;
	str[1] = str2;
	char mstr[] = "100";
	int ret = MatchHead(str, 2, mstr);
	if (ret) cout<<"Find the string!\n";
	else cout<<"String not found!\n";
	return 1;
}
*/
