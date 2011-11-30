//////////////////////////////////////////////////////////////////////////////// 
// generator.cpp                                                              //
// Description:                                                               //  
//   Routines to randomly generate PCP instances and routines to              //
//   systematically generate all PCP instances of a given size and width      //
// Created time:   May 12, 2001                                               //  
// Modified time:  Oct 25, 2001                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////   

#include "def.h"
#include "generator.h"
#include "readwrite.h"
#include "filter.h"
#include "solver.h"
#include "exclusion.h"
#include "mask.h"
#include "cache.h"
#include "basic.h"

extern int TEST_DEPTH;
extern unsigned long node_num, cutoff_num;
extern enum STATUS globalStatus;
extern int Iterative_Flag, solution_count, solution_length, DETERMINANT_DEPTH,
		   FindAllSolutions_Flag, FindShortestSolution_Flag;
extern int FINAL_DEPTH_THRESHOLD;


#ifdef SOLVER_OUTPUT
extern char buffer[];
#endif



/////////////////////////////////////////////////////////////////////
/// random PCP instance generator

// the generated instances should not be the trivial case, i.e., existing
// one pair whose upstring equals to the downstrings. There should also
// forbid the case that some pairs are the same
void CRandomGenerator::RandomGenerator(CPCPInstance *pPCP, int size, int width, FILE *file)
{
	int i, j, len, k;
	int retry_flag;
	int start;
	int ret;

	pPCP->size = size;
	pPCP->width = width;

	do
	{
		for (i=0;i<size;i++)
		{
			retry_flag = 0;
			
			// generate the upstring
			len = rand()%width+1; // len >=1 && len <=width
			start = 0;

			if (i==0) // the length of the first upstring must be width, and the 
			{	      // first digit must be 1
				pPCP->arrPair[0].up[0] = 1; //not sure it should be '0' or 0
				len = width;
				start = 1;
			}
			
			pPCP->arrPair[i].uplen = len;
			for (j=start;j<len;j++)
				pPCP->arrPair[i].up[j] = (char)(rand()%2);
			pPCP->arrPair[i].up[j] = SENTINEL;
			
			// generate the downstring
			len = rand()%width+1;       		
			pPCP->arrPair[i].downlen = len;
			for (j=0;j<len;j++)
				pPCP->arrPair[i].down[j] = rand()%2;
			pPCP->arrPair[i].down[j] = SENTINEL;
			
			// if two strings are the same, restart
			if (pPCP->arrPair[i].uplen == pPCP->arrPair[i].downlen)
				if (!memcmp(pPCP->arrPair[i].up, pPCP->arrPair[i].down, pPCP->arrPair[i].uplen))
					retry_flag =1;

			// test if there is an identical pair
			if (retry_flag == 0)  
			{
				for (k=0;k<i;k++)
				{
					if ( pPCP->arrPair[k].uplen == pPCP->arrPair[i].uplen     && 
						pPCP->arrPair[k].downlen == pPCP->arrPair[i].downlen  &&
						!memcmp( pPCP->arrPair[k].up, pPCP->arrPair[i].up, 
						         pPCP->arrPair[i].uplen )                     &&
						!memcmp( pPCP->arrPair[k].down, pPCP->arrPair[i].down, 
						         pPCP->arrPair[i].downlen))
					{
						retry_flag = 1;
						break;
					}
				}
			}

			// test if the pair consistes of the same element
			// since typically an instance having such a pair is very difficult to
			// search but may not have solutions. you may choose not to use it.
			if (retry_flag == 0) 
			{
				int allsame=1;

				for (k=1;k<pPCP->arrPair[i].uplen;k++)
					if (pPCP->arrPair[i].up[k]!=pPCP->arrPair[i].up[0])
					{
						allsame = 0;
						break;
					}
					
				if (allsame)
					for (k=0;k<pPCP->arrPair[i].downlen;k++)
						if (pPCP->arrPair[i].down[k]!=pPCP->arrPair[i].up[0])
						{
							allsame = 0;
							break;
						}
						
				if (allsame) retry_flag = 1;
			}
			
			if (retry_flag == 1) i--;
		}

		pPCP->CountOffset();

		// be sure the instance can pass filters 
		ret = CFilter::PassAllFilters(pPCP);
	} while (ret!=1);

	if (file!=NULL)
		CIO::writeInstance(file, pPCP);
}

// try to solve randomly generated instance to find hard instances 
// with very long optimal solutions. Only instances with the size and width
// provided by the parameters can be generated, and only solution length 
// no shorter than sol_len will be recorded to a file named sol_file_name.
// In case the program will get stuck in some difficult unsolvable instances,
// you can use define NODE_EXCEEDED_EXIT in def.h to allow the process to
// stop when it exceeds some node counts. Make sure you set node_num_threshold
// and cutoff_num_threshold properly in main.cpp.
void CRandomGenerator::GenerateHardInstance(int size, int width, char *sol_file_name, int sol_len, int number_tried)
{
	int generated_number_bound = number_tried;
	int generated_number = 0;
	time_t time_start,time_end,time_tips;
    int sol_count = 0;
    CPCPInstance PCP, ReversePCP;

    FILE *sol_file = fopen(sol_file_name, "w");
    if (!sol_file) printerr("Open file error!");

    Iterative_Flag = 1;
    
    FILE *output = fopen("out.txt", "w");

    rand_init();

    while(1)
	{
		// generate a PCP instance
		generated_number++;
        if (generated_number%100==0) printf("%d\n", generated_number);
        if (generated_number>generated_number_bound)
            break;
		

		RandomGenerator(&PCP, size, width, NULL);
        
		// try solving the PCP
		time_start=time(NULL);
		CSolver::SolvePCPInstance(&PCP, Iterative_Flag);
		time_end=time(NULL);
		time_tips = time_end-time_start;
		
		if (solution_count>=1 && solution_length>=sol_len) // have solution
		{
            fprintf(sol_file, "Instance %d:\n", ++sol_count);

			PCP.sol_len = solution_length;
            PCP.sol_num = solution_count;
            PCP.sol_time[0] = time_tips;

			CIO::writeInstance(sol_file, &PCP);
            fprintf(sol_file, "\n");
            
			if (!FindShortestSolution_Flag)
				fprintf(sol_file, "This may not be the shortest solution.\n");           

			CSolver::OutputSolvingInfo(&PCP, sol_file, time_tips, "Solvable!");
            fflush(sol_file);

		}
        
		// clear hash table for next PCP instance
		ClearHashTable();
		node_num = 0;
		cutoff_num = 0;
	}

	// free memory
	fclose(sol_file);
}

// normalize all instances in one file to another
// maxnum is the number of instances
int CRandomGenerator::Normalize(char *ifile_name, char *ofile_name, int maxnum)
{
    int ret, i, j, tmp;
    int count = 0;
    int bReverse, bTopdown, b01Exchange;

    FILE *ifile = fopen(ifile_name, "r");
    FILE *ofile = fopen(ofile_name, "w");
    if (!ifile || !ofile) printerr("Open file error!");

    CPCPInstance arrPCP[1000];// = new CPCPInstance[maxnum];
    CPCPScore arrMaxScore[1000];// = new CPCPScore[maxnum];
    int arr[1000];
    CPCPInstance arrTmpPCP[8];
    CPCPInstance *pStandardPCP, tmpPCP;
    CPCPScore maxScore, tmpScore;


    while(1)
	{
        // read PCP data
		ret = CIO::readInstance(ifile, &arrTmpPCP[0]); // readmode is 1
		if (ret!=1) break;

        maxScore.Count(&arrTmpPCP[0]);
        pStandardPCP = &arrTmpPCP[0];

        // find the stardard form
        for (i=1;i<=7;i++)
        {
            bReverse = i&1;
            bTopdown = (i>>1)&1;
            b01Exchange = (i>>2)&1;
            if (bReverse) arrTmpPCP[0].CreateReversePCP(&arrTmpPCP[i]);
            else arrTmpPCP[i].Copy(&arrTmpPCP[0]);
            if (bTopdown) arrTmpPCP[i].TopDownExchange();
            if (b01Exchange) arrTmpPCP[i].ZeroOneExchange();
            
            tmpScore.Count(&arrTmpPCP[i]);
            if (maxScore.Compare(&tmpScore)==0) 
            { 
                maxScore.Copy(&tmpScore);
                pStandardPCP = &arrTmpPCP[i];
            }
        }
        arrPCP[count].Copy(pStandardPCP);
        arrMaxScore[count].Copy(&maxScore);
        count++;
        if (count%100==0) printf("%d\n",count);
    }

    
    for (i=0;i<count;i++) arr[i]=i;
    
    // sort instances according to their score
    for (i=0;i<count-1;i++)
        for (j=i+1;j<count;j++)
        {
            if (arrMaxScore[arr[i]].Compare(&arrMaxScore[arr[j]])==0)
            {
                tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
  
    // remove useless pair
    j = 0;
    for (i=1;i<count;i++)
    {
        ret = arrMaxScore[arr[i]].Compare(&arrMaxScore[arr[j]]);
        if (ret==0) j=i;
        else if (ret==2) 
            arr[i]=-1;
        else
        {assert(0);}
    }

    // print out result
    j = 0;
    for (i=0;i<count;i++)
    {
        if (arr[i]<0) continue;
        fprintf(ofile, "Instance %d:\n", ++j);
        CIO::writeInstance(ofile, &arrPCP[arr[i]]);
        //arrMaxScore[arr[i]].Print(ofile);
        fprintf(ofile, "\n");
    }
    return 1;
}

/// End of random PCP instance generator
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
/// Systematical PCP instance generator
long CPairScore::CountStringScore(char *str)
{
    long score=0;
    int i=0;
	
    while (str[i]!=SENTINEL)
    {
        score += score<<1; 
        score += str[i++]+1;
    }

    return score;
}

void CPairScore::Count(CPair *pCPair)
{
    up = CountStringScore(pCPair->up);
    down = CountStringScore(pCPair->down);
}

// 0--less than
// 1--larger than
// 2--equal
int CPairScore::Compare(CPairScore *pScore)
{
    if (up<pScore->up) return 0;
    if (up>pScore->up) return 1;
    if (down<pScore->down) return 0;
    if (down>pScore->down) return 1;
    return 2;
}

void CPairScore::Exchange(CPairScore *pScore)
{
    int tmp = up;
    up = pScore->up;
    pScore->up = tmp;
    tmp = down;
    down = pScore->down;
    pScore->down = tmp;
}

void CPCPScore::Count(CPCPInstance *pPCP)
{
    int i;
    size = pPCP->size;
    if (!arrPairScore)
        arrPairScore = new CPairScore[size];
    for (i=0;i<size;i++)
        arrPairScore[i].Count(&pPCP->arrPair[i]);
    Sort(pPCP);
}

// sort the scores and exchange pairs in the instance according to the scores
void CPCPScore::Sort(CPCPInstance *pPCP)
{
    assert(arrPairScore!=NULL);
    int i, j, pointer;
    for (i=0;i<size-1;i++)
    {
        pointer = i;
        for (j=i+1; j<size;j++)
            if (arrPairScore[pointer].Compare(&arrPairScore[j])==0)
                pointer = j;
        arrPairScore[i].Exchange(&arrPairScore[pointer]);
        pPCP->PairExchange(i, pointer);
    }
}

int CPCPScore::Compare(CPCPScore *pScore)
{
    assert(arrPairScore!=NULL);   
    int i, ret;
    for (i=0; i<size; i++)
    {
        ret=arrPairScore[i].Compare(&pScore->arrPairScore[i]);
        if (ret<2) return ret;
    }
    return 2;
}

void CPCPScore::Copy(CPCPScore *pScore)
{
    
    int i;
    size = pScore->size;
    if (arrPairScore==NULL)
        arrPairScore = new CPairScore[size];
    for (i=0; i<size; i++)
    {
        arrPairScore[i].up = pScore->arrPairScore[i].up;
        arrPairScore[i].down = pScore->arrPairScore[i].down;
    }
}

// generate all strings whose length is less than or equal to width in 
// the descending order
void CSystemmaticScan::GenerateAllStrings(int width, char **pArrStr)
{
    int i, j, k;
    int count = 0;
    char *arr=new char[width+1];
 
    for (i=width-1;i>=0;i--)
    {
        // store the string
        arr[i+1] = '\0';
        for (j=i;j>=0;j--) arr[j] = '1';
        
        while (1)
        {
            // store the string
            memcpy(pArrStr[count++], arr, i+2);
            
            j = i;
            while (arr[j]=='0' && j>0) j--;
            if (j==0 && arr[j]=='0') break;
            arr[j]--;
            for (k=j+1;k<=i;k++) arr[k] = '1';
        }
    }
//    for (i=0;i<count;i++) printf("%s\n", pArrStr[i]);
}

// generate all pairs in the lexicographical order
void CSystemmaticScan::GenerateAllPairs(int width, CPair *pArrCPair)
{
    char **pArrStr;
    long str_count = (1<<(width+1)) - 2;
    // long pair_count = str_count*(str_count-1);
    long i, j, count=0;
    int len;
    int k;
 
    pArrStr= new char*[str_count];

    for (i=0;i<str_count;i++)
        pArrStr[i] = new char[MAXWIDTH+1];
    
    GenerateAllStrings(width, pArrStr);

    for (i=0;i<=str_count-1;i++)
        for (j=0; j<=str_count-1; j++)
        {
            if (i==j) continue;

            len = strlen(pArrStr[i]);
            pArrCPair[count].uplen = len;

            for (k=0;k<len;k++)
                pArrCPair[count].up[k] = pArrStr[i][k] - '0';

            pArrCPair[count].up[len] = SENTINEL;
            len = strlen(pArrStr[j]);
            pArrCPair[count].downlen = len;

            for (k=0;k<len;k++)
                pArrCPair[count].down[k] = pArrStr[j][k] - '0';

            pArrCPair[count].down[len] = SENTINEL;
            count++;
        }   

    for (i=0;i<str_count;i++)
        delete pArrStr[i];

    delete pArrStr;
}

// generate all non-isomorphic instances with the size and width
// to an output file
void CSystemmaticScan::GeneratedAllInstances(int size, int width, char *outputfile)
{
    long i, j;
    long str_count = (1<<(width+1)) - 2;
    long pair_count = str_count*(str_count-1);
    long count = 0, overallcount = 0;
    long *arr = new long[size];
    int bReverse, bTopdown, b01Exchange;
    CPCPScore score, tmpScore;
    int flag, sizeflag;
    CPCPInstance pcp, tmpPCP;
    CPair *pArrCPair = new CPair[pair_count];

    FILE *instancefile = fopen(outputfile, "w");
	if (!instancefile) printerr("output file not found");

    pcp.size = size;
    pcp.width = width;

	// for debugging
	// int scorecount = 0;
    // CPCPScore arrScore[1000];     
	
	printf("\nGenerate all instances in PCP[%d,%d]:\n", size, width);
    GenerateAllPairs(width, pArrCPair);
 
	// array arr store the indices of all pairs in an instance
    for (i=0;i<size;i++) arr[i] = i;
    //arr[0]=10000;

    while(1)
    {
        // check if width matches
        sizeflag = 0;
        for (i=0;i<size;i++)
        {
            if (pArrCPair[arr[i]].downlen==width) { sizeflag = 1; break; }
            if (pArrCPair[arr[i]].uplen==width) { sizeflag = 1; break; }
        }

        if (sizeflag)
        {
			// store the instance
            for (i=0;i<size;i++)
            {
                pcp.arrPair[i].Copy(&pArrCPair[arr[i]]);
                pcp.arrPair[i].ID = i+1;
            }
            
			// generate all other transformations and find if the current one
			// is in the normalized form (yes then go on, otherwise, pruned)
            score.Count(&pcp);
            flag = 1;
            for (i=1;i<=7;i++)
            {
                bReverse = i&1;
                bTopdown = (i>>1)&1;
                b01Exchange = (i>>2)&1;
                if (bReverse) pcp.CreateReversePCP(&tmpPCP);
                else tmpPCP.Copy(&pcp);
                if (bTopdown) tmpPCP.TopDownExchange();
                if (b01Exchange) tmpPCP.ZeroOneExchange();
                
                tmpScore.Count(&tmpPCP);
                if (score.Compare(&tmpScore)==0) { flag = 0; break; }
            }
            
            if (flag)
            {
                // arrScore[count].Count(&pcp); // for debuggin

                fprintf(instancefile, "Instance %d: \n", ++count);
                CIO::writeInstance(instancefile, &pcp);
                fprintf(instancefile, "\n");
                if (count%1000==0) printf("%d\n", count);
            }
            overallcount++;
            if (overallcount%50000==0) printf("* %d\n", overallcount);
        }
        
        // move to the next instance
        i = size-1;
        while (arr[i]==pair_count-size+i && i>0) i--;
        if (i==0 && arr[i]>=pair_count-size) break;
        arr[i]++;
        for (j=i+1;j<size;j++) arr[j] = arr[i]+j-i;
    }

    fprintf(instancefile, "0\n");
    fprintf(instancefile, "Count: %d   Overall Count: %d\n", count, overallcount);
    
    printf("%d instances overall.\n", overallcount);
	printf("%d non-isomorphic instances\n", count);


/*
    // check if every instance has its normalized form recorded, for debugging
    count = 0;
    for (i=0;i<size;i++) arr[i] = i;
    while(1)
    {
        // store the instance
        for (i=0;i<size;i++)
        {
            pcp.arrPair[i].Copy(&pArrCPair[arr[i]]);
            pcp.arrPair[i].ID = i+1;
        }
        flag = 0;
        for (i=0;i<=7;i++)
        {
            bReverse = i&1;
            bTopdown = (i>>1)&1;
            b01Exchange = (i>>2)&1;
            if (bReverse) CreateReversePCP(&pcp, &tmpPCP);
            else tmpPCP.Copy(&pcp);
            if (bTopdown) TopDownInstance(&tmpPCP);
            if (b01Exchange) ZeroOneExchange(&tmpPCP);

            tmpScore.Count(&tmpPCP);
            for (j=0;j<574;j++)
                if (arrScore[j].Compare(&tmpScore)==2)  { flag =1; break;}
        }

        if (!flag) 
            assert(0);

        count++;

        i = size-1;
        while (arr[i]==pair_count-size+i && i>0) i--;
        if (i==0 && arr[i]==pair_count-size) break;
        arr[i]++;
        for (j=i+1;j<size;j++) arr[j] = arr[i]+j-i;
    }
*/

   fclose(instancefile);
   delete pArrCPair;
   delete arr;
}

/// End of systematical PCP instance generator
/////////////////////////////////////////////////////////////////////

int CSystemmaticScan::TestFilters(char *ifile_name, char *passed_file_name, char *nosol_file_name)
{
    int ret;
    long count = 0, nosol_count = 0, passed_count = 0;
    CPCPInstance PCP;

    FILE *ifile = fopen(ifile_name, "r");
    FILE *passed_file = fopen(passed_file_name, "w");
    FILE *nosol_file = fopen(nosol_file_name, "w");
    if (!ifile || !passed_file || !nosol_file) printerr("Open file error!");
	
    //FILE *file = fopen("t.txt","w");
    //int t = 0;

    printf("\nPass filters:\n"); 
    while(1)
	{
		// read PCP data
		ret = CIO::readInstance(ifile, &PCP); // readmode is 1
		if (ret!=1) break;
        count++;
        if (count%5000==0) printf("%d\n",count);
    
        ret = CFilter::PassAllFilters(&PCP);

        if (ret==0 || ret==-2) 
        {
            ++nosol_count;
            //fprintf(nosol_file, "Instance %d:\n", ++nosol_count);
            //CIO::writeInstance(nosol_file, &PCP);
            // fprintf(nosol_file, "\n");
            /*if (ret==-2)
            {
                fprintf(file, "Instance %d:\n", ++t);
                CIO::writeInstance(file, &PCP);
                fprintf(nosol_file, "\n");
           }*/
        }
        else if (ret==1) 
        {
            fprintf(passed_file, "Instance %d:\n", ++passed_count);
            CIO::writeInstance(passed_file, &PCP);
            fprintf(passed_file, "\n");
        }
        else assert(0);
    }

    printf("%d instances: %d passed, %d filtered!\n", count, passed_count, nosol_count);
    fclose(passed_file);
    fclose(nosol_file);
    return 1;
}

int CSystemmaticScan::TestMasks(char *ifile_name, char *passed_file_name, char *nosol_file_name)
{
    int ret;
    long count = 0, nosol_count = 0, passed_count = 0;

    FILE *ifile = fopen(ifile_name, "r");
    FILE *passed_file = fopen(passed_file_name, "w");
    FILE *nosol_file = fopen(nosol_file_name, "w");
    if (!ifile || !passed_file || !nosol_file) printerr("Open file error!");
	
    // define depth parameters
	TEST_DEPTH = 10;
    CPCPInstance PCP, ReversePCP;
    printf("\nPass masks:\n"); 

    while(1)
	{

        // read PCP data
		ret = CIO::readInstance(ifile, &PCP); // readmode is 1

		if (ret!=1) break;
        count++;
        if (count%100==0) printf("%d\n",count);
    
        PCP.CreateReversePCP(&ReversePCP);

        //// find the masks
        // mask should use iterative method to find all of them
        globalStatus = FIND_MASK;
		CMask::FindMask(&PCP, &ReversePCP);

        // test masks
        if (PCP.upmask && PCP.downmask || ReversePCP.upmask && ReversePCP.downmask)
        {
            fprintf(nosol_file, "Instance %d:\n", ++nosol_count);
            CIO::writeInstance(nosol_file, &PCP);
            fprintf(nosol_file, "\n");
        }
        else
        {
            fprintf(passed_file, "Instance %d:\n", ++passed_count);
            CIO::writeInstance(passed_file, &PCP);
            fprintf(passed_file, "\n");
        }
		// clear hash table for next PCP instance
		ClearHashTable();
		node_num = 0;
		cutoff_num = 0;
    }

    printf("%d instances: %d passed, %d filtered!\n", count, passed_count, nosol_count);
    fclose(passed_file);
    fclose(nosol_file);
    return 1;
}

// The exclusion method must use masks.
int CSystemmaticScan::TestExclusions(char *ifile_name, char *passed_file_name, char *nosol_file_name)
{
    int ret, i;
    int count = 0, nosol_count = 0, passed_count = 0;

    FILE *ifile = fopen(ifile_name, "r");
    FILE *passed_file = fopen(passed_file_name, "w");
    FILE *nosol_file = fopen(nosol_file_name, "w");
    if (!ifile || !passed_file || !nosol_file) printerr("Open file error!");
	
    // define depth parameters
	TEST_DEPTH = 10;
    CPCPInstance PCP, ReversePCP;

    printf("\nPass exclusions:\n"); 

    while(1)
	{
        // read PCP data
		ret = CIO::readInstance(ifile, &PCP); // readmode is 1

		if (ret!=1) break;
        count++;
        if (count%100==0) printf("%d\n",count);
    
        PCP.CreateReversePCP(&ReversePCP);

        //// find the masks
        // mask should use iterative method to find all of them
        globalStatus = FIND_MASK;
        CMask::FindMask(&PCP, &ReversePCP);

        // test masks
        if (PCP.upmask && PCP.downmask || ReversePCP.upmask && ReversePCP.downmask)
        {
            fprintf(nosol_file, "Instance %d:\n", ++nosol_count);
            CIO::writeInstance(nosol_file, &PCP);
            ClearHashTable();
            node_num = 0;
            cutoff_num = 0;
            continue;
        }

        //// use the exclusion method to find the starting point
        globalStatus = EXCLUSION_METHOD;
        ret = CExclusion::Exclusion_Method(&PCP);
        int flag1 = 0;
        for (i=0;i<PCP.size;i++)
            if (PCP.arrStartingPoint[i]) flag1 = 1;
        if (ret==0 || !flag1)
        {
            fprintf(nosol_file, "Instance %d:\n", ++nosol_count);
            CIO::writeInstance(nosol_file, &PCP);
            ClearHashTable();
            node_num = 0;
            cutoff_num = 0;
            continue;
        }
        
        ret = CExclusion::Exclusion_Method(&ReversePCP);
        int flag2 = 0;
        for (i=0;i<ReversePCP.size;i++)
            if (ReversePCP.arrStartingPoint[i]) flag2 = 1;
        if (ret==0 || !flag2)
        {
            fprintf(nosol_file, "Instance %d:\n", ++nosol_count);
            CIO::writeInstance(nosol_file, &PCP);
            fprintf(nosol_file, "\n");
        }
        else
        {
            fprintf(passed_file, "Instance %d:\n", ++passed_count);
            CIO::writeInstance(passed_file, &PCP);
            fprintf(passed_file, "\n");
        }
		// clear hash table for next PCP instance
		ClearHashTable();
		node_num = 0;
		cutoff_num = 0;
    }

    printf("%d instances: %d passed, %d filtered!\n", count, passed_count, nosol_count);
    fclose(passed_file);
    fclose(nosol_file);
    return 1;
}


int CSystemmaticScan::ScanSubclass(int size, int width)
{

    GeneratedAllInstances(size, width, "subclass.txt");    

	// preprocess
	TestFilters("subclass.txt", "passed_filter.txt", "nosol_filter.txt");	
	TestMasks("passed_filter.txt", "passed_mask.txt", "nosol_mask.txt");
	TestExclusions("passed_mask.txt", "passed_exclusion.txt", "nosol_exclusion.txt");

	CSolver::PCPSolver("passed_exclusion.txt", "sol.txt", "nosol.txt", "unsol.txt");
	
	return 1;
}


