////////////////////////////////////////////////////////////////////////////////     
// main.cpp                                                                 //
// Description: central routines                                              //  
// Created time:   March, 2001                                               //  
// Modified time:  Nov 16, 2003                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     
// Set tab size to 4 in your IDE for better indentation



#include <time.h>
#include <stdio.h>

#include "cache.h"
#include "interface.h"
#include "global.h"
#include "solver.h"

void Init()
{
	ClearHashTable();
}

void SetGlobalValues()
{

#ifdef NODE_EXCEEDED_EXIT
    
	// set node number threshold
	// node_num_threshold = 23115285*4; //~((unsigned long)(0));
	// cutoff_num_threshold = 5000000;
    node_num_threshold = 50000000; 
    cutoff_num_threshold = 100000;

#endif

	// initialize
	Init();

	// flags
    Iterative_Flag = 1;
	if (Iterative_Flag==0)
		SEARCH_TO_OPTIMAL_LENGTH_FLAG = 1;
	else SEARCH_TO_OPTIMAL_LENGTH_FLAG = 0;
	//SEARCH_TO_OPTIMAL_LENGTH_FLAG = 1;

	FindShortestSolution_Flag = 1; 
    FindAllSolutions_Flag = 0;     // only find optimal solutions
    
    // depth parameters
	FINAL_DEPTH_THRESHOLD = 320;   
	TEST_DEPTH = 20;                
	COMPARE_DEPTH = 40;            
	ADDED_DEPTH = 20;              
	DETERMINANT_DEPTH = 112;       
	iterative_depth_threshold = FINAL_DEPTH_THRESHOLD;     
}

int main(int arg, char *argv[])
{
	time_t start, end;	

	SetGlobalValues();

	start=time(NULL);
	printf("PCPSolver Ver 0.0.3 by Ling Zhao (zhao@cs.ualberta.ca) Nov 16, 2003\n\n");

	CInterface::ProcessArgFunc(arg, argv);
	//CSolver::Statistics("input.txt");
	
	end=time(NULL);
    printf("Total time spent: %.0f seconds.\n ", difftime(end, start));

	return 1;
}


