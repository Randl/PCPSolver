////////////////////////////////////////////////////////////////////////////////     
// solver.cpp                                                                   //
// Description:                                                               //  
//   Include main routines to solve the PCP instances                         //
// Created time:   March                                                      //  
// Modified time:  May 11, 2001                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////  

#include "readwrite.h"
#include "filter.h"
#include "mask.h"
#include "exclusion.h"
#include "cache.h"
#include "generator.h"
#include "solver.h"
#include "basic.h"
#include <iostream>
using namespace std;

extern int iterative_depth_threshold,FindShortestSolution_Flag,solution_count,
		   arrSelection[],Iterative_Flag,FindAllSolutions_Flag,COMPARE_DEPTH,
		   FINAL_DEPTH_THRESHOLD,ADDED_DEPTH,DETERMINANT_DEPTH,solution_length;
extern enum STATUS globalStatus;
extern unsigned long node_num, cutoff_num;
extern CConfig *hashTable[];

#ifdef SOLVER_OUTPUT

extern char buffer[];

void CSolver::PrintSolution(int *arr, int len) // print the solution to both the output and ofile;
{
	char bufferline[100];
    int i;
	
	sprintf(bufferline, "Find the solution in depth: %d (depth threshold: %d)\n", len, 
						 iterative_depth_threshold);
    strcat(buffer, bufferline);
	printf( "Money::\n");
	for (i=1;i<=len;i++)
	{
		printf( "%3d", arr[i]);
		sprintf(bufferline, "%3d", arr[i]);
        strcat(buffer, bufferline);
		if (i % 20 == 0)  strcat(buffer, "\n");
	}
	printf( "\n::Money\n");
	strcat(buffer, "\n");
}

#endif


//FILE *errfile=fopen("errfile.txt", "w");
// routine to deal with the situation when one solution is found including 
// record the solution, check the solution, update solving results and search
// parameters.
// return value:
//   >0  should return the length immediately (When only need find one solution
//   =0  continue to find other solutions
int CSolver::Solution_Found(CPCPInstance *pPCP, CConfig *pConfig)
{
	int k;

	// for test, may not be solution, so need not record length and count	
	if (globalStatus == FIND_MASK || globalStatus == EXCLUSION_METHOD)			
		return pConfig->depth;

	// just to find solution
	if (!FindShortestSolution_Flag) 
	{
		solution_length = pConfig->depth;
		solution_count = 1;
		return pConfig->depth;
	}

	// find all solutions
	if (   globalStatus == COMPARE_DIRECTION || globalStatus == ITERATIVE_SEARCH 
		|| globalStatus == DETERMINANT_SEARCH)
	{		
#ifdef SOLVER_OUTPUT
		// print the solution
        //if (pConfig->depth <= solution_length) // for special purposes
		/** BUG FIX 061031
			1. Originally, the first solution (may be optimal) is always omitted.
			   to fix that, now suboptimal solutions can be output as well.
            2. During bidirectioanl probing process, no solution should be output.
		**/
		if (globalStatus != COMPARE_DIRECTION)
			PrintSolution(arrSelection, pConfig->depth);
#endif

		// find all solutions not only including the shortest ones
		if (Iterative_Flag == 0 && FindAllSolutions_Flag == 1) 
			solution_count++;
		// find only all shortest solutions
		else if (pConfig->depth < solution_length || solution_length == 0) 
		{
			solution_length = pConfig->depth;
			solution_count = 1;
			iterative_depth_threshold = solution_length;
		}
		// find one more solution for the length
		else if (pConfig->depth == solution_length) 
			solution_count++;

		// find all configs in the solution path and free them if they are 
		// in the hashtable
		CConfig tempConfig(pConfig->depth * pPCP->offset);
		for (k=1;k<=pConfig->depth;k++)
		{
			// need improvement for exclusion method
			assert( (arrSelection[k]>=1) && (arrSelection[k]<=pPCP->size) ); 

			// add the pair
			tempConfig.MatchPair(&pPCP->arrPair[arrSelection[k]-1], arrSelection); 
			
			tempConfig.ConfigPrintToConsole();
            //tempConfig.ConfigPrint(errfile);
			// make up the visited node count
			node_num--;

			// find in the cache
			int hashvalue = tempConfig.CalHashValue();
			CConfig *p = hashTable[hashvalue];
			if (!p) continue;
			if (p->ConfigCmp(&tempConfig)) continue;

			// update the depth threshold
			p->depth = iterative_depth_threshold;
		}

		assert(tempConfig.len==0); // for debugging
	}

	return 0;
}


// return 1 if the config can be pruned
// return 0 otherwise 
inline int CSolver::IsPrunable(CPCPInstance *pPCP, CConfig *pConfig)
{
#ifdef USE_ELEMENT_BALANCE_HEURISTIC
	// balance of elements
    if (pConfig->up)
    {
        if ( abs(pConfig->num0) > 
			 pPCP->up_0drop * (iterative_depth_threshold-pConfig->depth) ) 
		    return 1; 

        if ( pConfig->len-abs(pConfig->num0) > 
			 pPCP->up_1drop*(iterative_depth_threshold-pConfig->depth) ) 
		    return 1; 
    }
    else
    {
        if ( abs(pConfig->num0) > 
			 pPCP->down_0drop*(iterative_depth_threshold-pConfig->depth) ) 
		    return 1; 

        if ( pConfig->len-abs(pConfig->num0) > 
			 pPCP->down_1drop*(iterative_depth_threshold-pConfig->depth) ) 
			return 1; 
    }
#endif

#ifdef USE_LENGTH_BALANCE_HEURISTIC
    // balance of length
	// when the length of config exceeds the limit that the instance can
	// shrink it to 0 till iterative_depth_threshold, just prune it    
	register int len = pConfig->up? pPCP->down_offset : pPCP->up_offset;
    if (pConfig->len>len*(iterative_depth_threshold-pConfig->depth)) 
		return 1; 
#endif

#ifdef NODE_EXCEEDED_EXIT
    // contrains for visited number and cutoff number
    // mainly for randomly generated instances            
    if (node_num>node_num_threshold || cutoff_num>cutoff_num_threshold)
		return 1; 
#endif

	return 0;
}


#define RETURNANDCLEAR  { return ret; }

// Solve the instance with the given config
// maxlen is the size of the maximum space allocated in pConfig
// return value:
//    -1: unsolvable
//     0: unsolved till the depth threshold
//   n>0: solved at depth of n
int CSolver::SolveConfig(CPCPInstance *pPCP, CConfig *pConfig, int maxlen)
{
	int i, j, k;
	
	// store the matched pairs and the lengths of resulting configs
	int arrRetValue[MAXSIZE];;
	int arrMatchedPair[MAXSIZE];
	int matchedPairNum;

	// ret value
	long int matchret;
	int solveret;
	int ret = -1;
	int succ;
    int newlen;

	// store the location of the config in the cache
	CConfig* pCacheConfig;
    CConfig *pNewConfig;

//	int lookaheadflag;
//   	long tmpret;

DEEP:
	//// Heuristic pruning
	if (IsPrunable(pPCP, pConfig)) 
		return 0;

    //// record all pairs which can be matched
	matchedPairNum = 0;  

/*
	if ( pConfig->up  && pPCP->maxdown*2<pConfig->len ||	
		 !pConfig->up && pPCP->maxup*2  <pConfig->len)
		lookaheadflag=1; // use traditional method
    else lookaheadflag=0;
*/	
	for (i=0;i<pPCP->size;i++)
	{
		matchret=pConfig->TestMatchingPair(&pPCP->arrPair[i]);

/*
		if (!lookaheadflag)
			matchret=pConfig->TestMatchingPair(&pPCP->arrPair[i]);
		else
		{
			matchret=2000000000;
			for (j=0;j<pPCP->size;j++)
			{
				tmpret = pConfig->TestMatchingPairs(pPCP, &pPCP->arrPair[i],
						                            &pPCP->arrPair[j]);
				if (tmpret>0 && tmpret<matchret) matchret = tmpret;
			}
			if (matchret==2000000000) matchret=-1;
		}
*/

		if (matchret==0) // find one solution
		{ 
			pConfig->depth++;
			arrSelection[pConfig->depth] = pPCP->arrPair[i].ID;
			succ = Solution_Found(pPCP, pConfig);
            if (succ>0) { return pConfig->depth; }
			else ret = 0;
			pConfig->depth--;	
		}
		else if (matchret>0) // find one match
		{
			// having exceeded the threshold, prune it
			if (pConfig->depth==iterative_depth_threshold-1) 
				ret=0; 
			else  // record the pair
			{
				arrRetValue[matchedPairNum] = matchret;
				arrMatchedPair[matchedPairNum++] = i;
			}
		}
	}

	//// no matched pair?
    if (matchedPairNum == 0) RETURNANDCLEAR;
        
	//// only one matched pair and the config can be reused? jump back!
	if (matchedPairNum==1  && arrRetValue[0]<=maxlen ) 
	{
		pConfig->MatchPair(&pPCP->arrPair[arrMatchedPair[0]], arrSelection);
		assert(pConfig->len <= maxlen);

		if (pConfig->up)
            if (pPCP->upmask) RETURNANDCLEAR;

		if (!pConfig->up)
            if (pPCP->downmask) RETURNANDCLEAR;

        succ = TryFind(hashTable, pConfig, &pCacheConfig);
        
		if (succ==1 || succ==2) // this configration is in the cache, prune it
		{ 
			Add_cutoff_node_number();
			//if (succ==2) ret = 0;
			RETURNANDCLEAR;
		}
	    goto DEEP;
	}

	//// sort the matched pair array according to its matchret value	
	for (i=0;i<matchedPairNum-1;i++)
		for (j=i+1;j<matchedPairNum;j++)
		{
			if (arrRetValue[i] > arrRetValue[j])
			{
				k = arrRetValue[i];
				arrRetValue[i] = arrRetValue[j];
				arrRetValue[j] = k;
				
				k = arrMatchedPair[i];
				arrMatchedPair[i] = arrMatchedPair[j];
				arrMatchedPair[j] = k;
			}
		}
	
	//// try the pair one by one		
	// count new length for config
	newlen = pConfig->len+pPCP->offset+MAXLOOPCOUNT*pPCP->offset;
	// create new config
#ifdef USE_CONFIG_POOL
    pNewConfig = newConfig(newlen);
#else
    pNewConfig = new CConfig(newlen);
#endif

    // try all matched pairs
	for (i=0;i<matchedPairNum;i++)
	{
		// the last matched pair and the config can be reused? Jump back!
		if (i+1==matchedPairNum && arrRetValue[i]<=maxlen )
        {

            pConfig->MatchPair(&pPCP->arrPair[arrMatchedPair[i]], arrSelection);
            if (pConfig->up)
                if (pPCP->upmask) continue;
            if (!pConfig->up)
                if (pPCP->downmask) continue;

#ifdef USE_CONFIG_POOL
            deleteConfig(newlen);
#else
            delete pNewConfig; 			
#endif                     
            succ = TryFind(hashTable, pConfig, &pCacheConfig);
                    
            if (succ==1 || succ==2) // this config is in the cache
            { 
                Add_cutoff_node_number();
                //if (succ==2) ret = 0; 
                RETURNANDCLEAR;
            }
            goto DEEP;
		}

        // duplicate pConfig to config
		pNewConfig->Copy(pConfig);

		// match it with the pair
		pNewConfig->MatchPair(&pPCP->arrPair[arrMatchedPair[i]], arrSelection);

		// test masks
		if (pNewConfig->up)
			if (pPCP->upmask) continue;
		if (!pNewConfig->up)
			if (pPCP->downmask) continue;

		// check cache
        succ = TryFind(hashTable, pNewConfig, &pCacheConfig);
		if (succ==1 || succ==2) // this config is in the cache
		{ 
			Add_cutoff_node_number();
			continue; 
		}
      
		// recursively call the SolveConfig routine
		solveret=SolveConfig(pPCP, pNewConfig, newlen);

		// check result
		if (solveret>0) // solve it
        { 
#ifdef USE_CONFIG_POOL
            deleteConfig(newlen);
#else
            delete pNewConfig;  
#endif
            return solveret; 
        }

		if (solveret==0) // cannot solve it to the iterative_depth_threshold
			ret=0; 
	}
#ifdef USE_CONFIG_POOL
    deleteConfig(newlen);
#else
    delete pNewConfig; 			
#endif

	RETURNANDCLEAR;
}

// try to solve the config
// called by mask and exclusion method
int CSolver::BeginSolveConfig(CPCPInstance *pPCP, CConfig *pConfig, int maxlen, int depth)
{
	iterative_depth_threshold = depth;
	int ret = SolveConfig(pPCP, pConfig, maxlen);
	ClearHashTable();	

	return ret;
}

// Solve the PCP instance by purely searching the space
// return value:
//    -1: failed
//     0: can not find to the depth threshold
//    n>0: find it in depth n
int CSolver::SearchSolution(CPCPInstance *pPCP)//, int startingpoint_flag = 1)
{
    int startingpoint_flag = 1;
	int i, len, succ;    
    int ret = -1, solveret = -1;
    CPair *pPair;
	int maxlen = pPCP->offset*(MAXLOOPCOUNT+1);
    CConfig *pCacheConfig;
  
	//CConfig pNewconfig(maxlen);
    CConfig *pNewConfig;

#ifdef USE_CONFIG_POOL
    pNewConfig = newConfig(maxlen);
#else
    pNewConfig = new CConfig(maxlen);
#endif

    for (i=0;i<pPCP->size;i++)
	{
 		// initialize config
		pNewConfig->depth = 0;
		pNewConfig->len = 0;
		
		// if use exclusion method, startingpoints are meaningful
        if (startingpoint_flag == 1)   
		    if (!pPCP->arrStartingPoint[i]) continue;

   	    // simply search from the start
        if (startingpoint_flag == 0) 
        {
            pPair = &pPCP->arrPair[i];
            len = (pPair->uplen < pPair->downlen) ? 
				   pPair->uplen : pPair->downlen;
            if (memcmp(pPair->up, pPair->down, len)) continue;
        }

        // find one starting point
		pNewConfig->MatchPair(&pPCP->arrPair[i], arrSelection);        

        // check masks 
		if (pNewConfig->up)
			if (pPCP->upmask) continue;
		if (!pNewConfig->up)
			if (pPCP->downmask) continue;

        // add to the cache
        succ = TryFind(hashTable, pNewConfig, &pCacheConfig);

		// this configration is in the cache, prune it
		if (succ==1 || succ==2) 
		{ 			
			Add_cutoff_node_number();
            continue;
		}

		// try to solve
		solveret=SolveConfig(pPCP, pNewConfig, maxlen);

		if (solveret>0) // solve it
			return solveret; 
		if (solveret==0) // don't solve it to the iterative_depth_threshold
            ret = 0;
	}

#ifdef USE_CONFIG_POOL
    deleteConfig(maxlen);
#else
    delete pNewConfig; 			
#endif

	return ret;
}

// Search solutions for a given PCP with the facilities of some enhancements.
// The flag decides to use iterative deepening method or determinate method.
// After the enhancements include filter, mask, exclusion method, bidirectional
// search, it will call the standard solving routine to solve the instance if 
// it has not solved by then.
// return value:
//  1  -- solved, find solution
//  -1 -- solved, no solution
//  0  -- unsolved
int CSolver::SolvePCPInstance(CPCPInstance *pPCP, int iterative_flag)
{
#ifdef SOLVER_OUTPUT
    char bufferline[4000];
    strcpy(buffer, ""); // clear buffer
#endif

	int ret;
	CPCPInstance ReversePCP;
    
	//// try to pass the filter
	ret = CFilter::PassAllFilters(pPCP);
	if (ret == 0)
	{
#ifdef SOLVER_OUTPUT
        if (globalStatus!=EXCLUSION_METHOD) 
            strcat(buffer, "Can not pass the filter\n");
#endif
        return -1;
	}
	else if (ret<0)
	{
		if (globalStatus==EXCLUSION_METHOD)
		{
			if (ret == -2)
				return -1;
		}
		else 
		{
#ifdef SOLVER_OUTPUT
            strcat(buffer, "Trivial case or some pair can not be used!\n");
#endif
            return 2;
		}
	}

	//// create the inverse instance
	pPCP->CreateReversePCP(&ReversePCP);

    //// find the masks
	// mask should use iterative method to find all of them
	globalStatus = FIND_MASK;
	CMask::FindMask(pPCP, &ReversePCP);

    if ( pPCP->upmask && pPCP->downmask || 
		 ReversePCP.upmask && ReversePCP.downmask )
        return -1;

	//// use the exclusion method to find the starting point
    globalStatus = EXCLUSION_METHOD;
    ret = CExclusion::Exclusion_Method(pPCP, iterative_flag);
    if (ret == 0) return 0;
    ret = CExclusion::Exclusion_Method(&ReversePCP, iterative_flag);
    if (ret == 0) return 0;

    //// use symmetry processing
    CMask::SymmetryProcess(pPCP, &ReversePCP);

#ifdef SOLVER_OUTPUT
    int i;
	for (i=0;i<pPCP->size;i++)
    {  
		sprintf(bufferline, "%2d", pPCP->arrStartingPoint[i]); 
		strcat(buffer, bufferline); 
	}
	strcat(buffer, "\n");
    for (i=0;i<pPCP->size;i++)
	{  
		sprintf(bufferline, "%2d", ReversePCP.arrStartingPoint[i]); 
		strcat(buffer, bufferline); 
	}
	strcat(buffer, "\n");
#endif    

#ifdef SOLVER_OUTPUT
    sprintf(bufferline, "Gcd: %d\n", pPCP->gcd);
    strcat(buffer, bufferline);
    if (pPCP->upmask || pPCP->downmask)
	{
		strcat(buffer, "Original Direction: ");
        if (pPCP->upmask) strcat(buffer, "upmask ");
		if (pPCP->downmask) strcat(buffer, "downmask ");
		strcat(buffer, "\n");
	}
    if (pPCP->turnover_upmask || pPCP->turnover_downmask)
	{
		strcat(buffer, "Original Direction: ");
        if (pPCP->turnover_upmask) strcat(buffer, "turnover_upmask ");
		if (pPCP->turnover_downmask) strcat(buffer, "turnover_downmask ");
		strcat(buffer, "\n");
	}
    if (ReversePCP.upmask || ReversePCP.downmask)
	{
		strcat(buffer, "Reverse Direction: ");
		if (ReversePCP.upmask) strcat(buffer, "upmask ");
		if (ReversePCP.downmask) strcat(buffer, "downmask ");
		strcat(buffer, "\n");
	}
    if (ReversePCP.turnover_upmask || ReversePCP.turnover_downmask)
	{
		strcat(buffer, "Reverse Direction: ");
        if (ReversePCP.turnover_upmask) strcat(buffer, "turnover_upmask ");
		if (ReversePCP.turnover_downmask) strcat(buffer, "turnover_downmask ");
		strcat(buffer, "\n");
	}
#endif

	//// find the most promising direction
	globalStatus = COMPARE_DIRECTION;
	int solved_flag = 0;
	iterative_depth_threshold = COMPARE_DEPTH;

	// try original direction
    int original_visit_nodenum;
	node_num = 0;
	ret = SearchSolution(pPCP);
	if (solution_count>=1) solved_flag = 1;
	else if (ret == -1) solved_flag = -1;
	
	/** BUG FIX: 061031
		1. node num should always recorded
		2. flag should not be returned regardless of results.
	**/
	//else if (ret == 0) 
	original_visit_nodenum = node_num;
	//if (solved_flag) return solved_flag;
	ClearHashTable();
	
	// try reverse direction
    int reverse_visit_nodenum;
    node_num = 0;
	ret = SearchSolution(&ReversePCP);
	if (solution_count>=1) solved_flag = 1;
	else if (ret == -1) solved_flag = -1;
	/** BUG FIX: 061031 (see comments above) **/
	// else if (ret == 0) 
	reverse_visit_nodenum = node_num;
	//if (solved_flag) return solved_flag;
	ClearHashTable();


#ifdef SOLVER_OUTPUT	
	// print out useful information
	sprintf( bufferline, "Visited node in original direction: %d\n", 
		     original_visit_nodenum );
    strcat(buffer, bufferline);
	sprintf( bufferline, "Visited node in reverse direction: %d\n", 
		     reverse_visit_nodenum ); 
    strcat(buffer, bufferline);
#endif

    // choose the one with less visited node number
	CPCPInstance *pGoodPCP;
	if (original_visit_nodenum <= reverse_visit_nodenum)  
	{
		pGoodPCP = pPCP;
#ifdef SOLVER_OUTPUT
        strcat(buffer, "Choose the original direction:\n");
#endif
	}
	else
	{
		pGoodPCP = &ReversePCP;
#ifdef SOLVER_OUTPUT
        strcat(buffer, "Choose the reverse direction:\n");
#endif
    }

    ////test if you need cache table

	//// try to solve the problem
    clock_t time_start,time_end;

	pPCP->poi = 0;
    if (iterative_flag)
	{
		// use the iterative method
		globalStatus = ITERATIVE_SEARCH;
		iterative_depth_threshold = 0; //COMPARE_DEPTH;
        while(iterative_depth_threshold < FINAL_DEPTH_THRESHOLD)
		{
			if (iterative_depth_threshold+ADDED_DEPTH<=FINAL_DEPTH_THRESHOLD)
				iterative_depth_threshold += ADDED_DEPTH;
			else 
                iterative_depth_threshold = FINAL_DEPTH_THRESHOLD;

#ifdef SOLVER_OUTPUT
            printf("Search to depth: %d\n", iterative_depth_threshold);
#endif
			cutoff_num = 0;
			node_num = 0;

            time_start = clock();
            ret = SearchSolution(pGoodPCP);
            time_end = clock();

            pPCP->cutoff_num[++pPCP->poi] = cutoff_num;
            pPCP->node_num[pPCP->poi] = node_num;
            pPCP->sol_time[pPCP->poi] = time_end-time_start;
            pPCP->depth[pPCP->poi] = iterative_depth_threshold;

#ifdef SOLVER_OUTPUT
            printf("node %d %d\n", node_num, cutoff_num);
			fflush(stdout);
#endif

            if (solution_count>=1)
			{
				solved_flag = 1;
				break;
			}
			else if (ret == -1) 
			{
				solved_flag = -1;
				break;
			}
			ResetHashTable();
		}
	}
	else 
	{
		// use the determinant method
		globalStatus = ITERATIVE_SEARCH;//DETERMINANT_SEARCH;
		iterative_depth_threshold = DETERMINANT_DEPTH;
		cutoff_num = 0;
		node_num = 0;

#ifdef SOLVER_OUTPUT
        printf("Search to depth: %d\n", iterative_depth_threshold);
        //strcat(buffer, bufferline);
#endif
            time_start = clock();
            ret = SearchSolution(pGoodPCP);
            time_end = clock();

            pPCP->cutoff_num[++pPCP->poi] = cutoff_num;
            pPCP->node_num[pPCP->poi] = node_num;
            pPCP->sol_time[pPCP->poi] = time_end-time_start;
            pPCP->depth[pPCP->poi] = iterative_depth_threshold;

#ifdef SOLVER_OUTPUT
            printf("node %d %d\n", node_num, cutoff_num);
#endif

            if (solution_count>=1) 	solved_flag = 1;
			else if (ret == -1) solved_flag = -1;
			else ResetHashTable();
	}
    if (solved_flag) return solved_flag;
	else return 0;
}

// more statistics added in (June 19, 2003)
long node_num_total=0, cutoff_num_total=0, time_tips_total=0;

void CSolver::OutputSolvingInfo(CPCPInstance *pPCP, FILE *outputfile, int time_tips, char *comment)
{
#ifdef SOLVER_OUTPUT
/*
    fprintf(outputfile, "Reverse:\n");
	CPCPInstance ReversePCP;
	pPCP.CreateReversePCP(&ReversePCP);
	CIO::writeInstance(sol_file, &ReversePCP);           
    fprintf(outputfile, "\n");
*/
	fprintf(outputfile, "%s\n\n", comment);
	fprintf(outputfile, buffer);
	fprintf(outputfile, "Time spent: %3f seconds.\n", 
		    ((double)time_tips)/CLOCKS_PER_SEC );

	int bOutpuIterationInfo = 0;

	// output iteration results
	
	fprintf(outputfile, "\n");
	node_num_total=0, cutoff_num_total=0, time_tips_total=0;
	for (int i=1;i<=pPCP->poi;i++)
	{
		node_num_total   += pPCP->node_num[i];
		cutoff_num_total += pPCP->cutoff_num[i];
		time_tips_total   += pPCP->sol_time[i];
	
		// output iteration results
		if (bOutpuIterationInfo)
		{
			fprintf(outputfile, "depth %3d: node %8d, ", pPCP->depth[i], pPCP->node_num[i]);
			if (i>=2)
				fprintf(outputfile, "new: %6d, ", pPCP->node_num[i]-pPCP->node_num[i-1]);
			else   
				fprintf(outputfile, "new: %6c, ", ' ');
			
			if (i>=3)
				fprintf(outputfile, "ratio: %.6f, ", (double)(pPCP->node_num[i]-pPCP->node_num[i-1])/
														(pPCP->node_num[i-1]-pPCP->node_num[i-2]));
			else fprintf(outputfile, "ratio: %.6f, ", 0);
			fprintf(outputfile, "cutoff %6d, time %f\n", pPCP->cutoff_num[i], 
				(double)pPCP->sol_time[i]/CLOCKS_PER_SEC);
		}
	}
	fprintf(outputfile, "\n");

    // store the total results
	pPCP->node_num[pPCP->poi+1] = node_num_total;
	pPCP->cutoff_num[pPCP->poi+1] = cutoff_num_total;

	// output results in total
	long time_tips_last = pPCP->sol_time[pPCP->poi];
	fprintf(outputfile, "visited node: %d, last iteration: %d\n", node_num_total, node_num);
	fprintf(outputfile, "cutoff node : %d, last iteration: %d\n", cutoff_num_total, cutoff_num);
	fprintf(outputfile, "time: %f, last iteration: %f\n", (double)time_tips/CLOCKS_PER_SEC,
		                 (double)time_tips_last/CLOCKS_PER_SEC);
	double speed1 = node_num_total /((double)time_tips/CLOCKS_PER_SEC);
    double speed2 = node_num/((double)time_tips_last/CLOCKS_PER_SEC);
	fprintf(outputfile, "Search speed: %.3fM/s, laster iteration: %.3fM/s\n\n", speed1/1000000, speed2/1000000);
	fflush(outputfile);	                 
						 
#endif
}

int CSolver::PCPSolver(char *ifile_name, char *sol_file_name, char *nosol_file_name, 
			   char*unsolved_file_name)
{
    int ret;
	int succ;
	clock_t time_start,time_end;
    int time_tips;
    int count = 0, nosol_count = 0, sol_count = 0, unsolved_count=0;
    CPCPInstance PCP, ReversePCP;

	// for experiments
	double all_node_num = 0;

    FILE *ifile = fopen(ifile_name, "r"); 
    FILE *sol_file = fopen(sol_file_name, "w");
    if (!ifile || !sol_file) printerr("Open file error!");
    FILE *nosol_file=NULL, *unsolved_file=NULL;

    if (nosol_file_name!=NULL)
    {
        nosol_file = fopen(nosol_file_name, "w");
        if (!nosol_file) printerr("Open file error!");
    }

    if (unsolved_file_name!=NULL)
    {
        unsolved_file = fopen(unsolved_file_name, "w"); 
        if (!unsolved_file) printerr("Open file error!");
    }

    printf("\nSolve instances:\n"); 

    while(1)
	{
		// read PCP data
		succ = CIO::readInstance(ifile, &PCP);
		if (succ!=1) break;

		// counting the number 
        count++;
        if (count%1==0) printf("%d\n",count);

		// try solving the PCP
		time_start=clock();
        ret = SolvePCPInstance(&PCP, Iterative_Flag);
        time_end=clock();
		time_tips = time_end-time_start;
		
		if (!ret || ret==2) // unsolved
		{
            unsolved_count++;
            if (unsolved_file)
            {
                //fprintf(unsolved_file, "(%d) ", count);
                fprintf(unsolved_file, "Instance %d:\n", unsolved_count);
                CIO::writeInstance(unsolved_file, &PCP);
                fprintf(unsolved_file, "\n");
				if (ret==2)	fprintf(unsolved_file, 
					         "Trivial case or some pairs can be removed!\n");

				OutputSolvingInfo(&PCP, unsolved_file, time_tips, "Solved!");
                fflush(unsolved_file);
            }
		}
		else if (ret==-1) // no solution
		{
            nosol_count++;
            if (nosol_file)
            {
                fprintf(nosol_file, "Instance %d:\n", nosol_count);
                CIO::writeInstance(nosol_file, &PCP);
                fprintf(nosol_file, "\n");
				OutputSolvingInfo(&PCP, nosol_file, time_tips, "Unsolvable!");
                fflush(nosol_file);
            }
		}
		else if (solution_count>=1) // have solution
		{
            fprintf(sol_file, "Instance %d:\n", ++sol_count);
	
			// warning
            //if (PCP.sol_len != solution_length || PCP.sol_num != solution_count)
			//		printf("solution may be wrong!\n");

			PCP.sol_len = solution_length;
            PCP.sol_num = solution_count;
            PCP.sol_time[0] = time_tips;

            CIO::writeInstance(sol_file, &PCP);
            fprintf(sol_file, "\n");
            
			if (!FindShortestSolution_Flag)
				fprintf(sol_file, "This may not be the shortest solution.\n");           

			OutputSolvingInfo(&PCP, sol_file, time_tips, "Solvable!");
            fflush(sol_file);
		}
        else assert(0);

		// for experiments
		all_node_num += node_num_total;
		
		// clear hash table for next PCP instance
		ClearHashTable();
		node_num = 0;
		cutoff_num = 0;
	}

	// for experiments
    printf("\nTotal nodes searched: %.0f\n", all_node_num);
	fprintf(sol_file, "Total nodes searched: %.0f\n", all_node_num);

    printf("%d instances:\n", count);   
    printf("  %d solvable.\n", sol_count);
    printf("  %d unsolvable.\n", nosol_count);
    printf("  %d unsolved.\n", unsolved_count);
	
	// free memory
	fclose(ifile);
    fclose(sol_file);
	if (nosol_file) fclose(nosol_file);
    if (unsolved_file) fclose(unsolved_file);
	return 1;
}


double GetMean(const int arr[], int size) 
{
	int i, total=0;
	 
	for (i=0;i<size;i++)
		total += arr[i];

	return (double)total/size;
}

double GetStandardDeviation(const int arr[], int size) 
{
	int i;
	double value = 0;
	double avg = GetMean(arr, size);

	for (i=0;i<size;i++)
		value += (arr[i]-avg) * (arr[i]-avg);

	return (sqrt(value/(size-1)));
}

int CSolver::Statistics(char *ifile_name)
{
	int succ, i;
    CPCPInstance PCP;

    FILE *ifile = fopen(ifile_name, "r"); 
    if (!ifile) printerr("Open file error!");

	int arr[1000];
	int arrTmp[1000];
	int count=0;

    while(1)
	{
		// read PCP data
		succ = CIO::readInstance(ifile, &PCP, EXPMODE);
		if (succ!=1) break;

		// counting the number 
        arr[count++] = PCP.sol_len;
        //if (count%1==0) printf("%d %d\n",count, PCP.sol_len);
	}
	
	fclose(ifile);

	static int arrDiv[1000];

	int len = 19;
	
	for (len=1;len<=40;len++)
	{
		
		for (i=0;i<count;i++)
		{
			//cout<<arr[i]<<endl;
			arrDiv[arr[i]%len]++;
			arrTmp[i] = arr[i]%len;
		}
		printf("%02d (%.3f) - %04.3f (%04.6f)\n", len, GetMean(arrTmp, count)/len, GetMean(arrTmp, count), 
			GetStandardDeviation(arrTmp, count));
			/*
			for (i=0;i<len;i++)
			cout<<i<<" "<<arrDiv[i]<<endl;
		*/
		//count = 6;
		//cout<<"Mean - "<<GetMean(arrTmp, count)<<endl;
		//cout<<" SD  - "<<GetStandardDeviation(arrTmp, count)<<endl;
	}

	return 1;
}
