//////////////////////////////////////////////////////////////////////////////// 
// global.h                                                                   //
// Description:                                                               //  
//   Definition and declaration of global variables                           //
// Created time:   May 12, 2001                                               //  
// Modified time:  Feb  3, 2003                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////   

#ifndef GLOBAL_H
#define GLOBAL_H

#include "def.h"


// status of the program
enum STATUS globalStatus;

//////////////////////////////////////////////////////////////////////
// search flags
//////////////////////////////////////////////////////////////////////
// use iterative deepening or not
int Iterative_Flag = 1;

// search to the known optimal length directly
int SEARCH_TO_OPTIMAL_LENGTH_FLAG = 0;
       
// find the shortest solution or just a solution     
int	FindShortestSolution_Flag = 1; 

// find all solutions or just optimal solutions
// valid only when the above two flags are both 0
// the solver will find all solutions till depth DETERMINANT_DEPTH
int FindAllSolutions_Flag = 0;     

//////////////////////////////////////////////////////////////////////
// depth and node number parameters, can be set to other values 
// by function SetGlobalValues() in file solver.cpp.
//////////////////////////////////////////////////////////////////////
// final depth threshold
int	FINAL_DEPTH_THRESHOLD = 400;               

// for mask and exclusion methods
int	TEST_DEPTH = 20;       

// for bidirectional probing        
int	COMPARE_DEPTH = 40;

// the increment            
int	ADDED_DEPTH = 20;              

// final depth threshold if the direct depth first search not ID is used
int	DETERMINANT_DEPTH = 100;        

// depth threshold in ID
int iterative_depth_threshold = FINAL_DEPTH_THRESHOLD;     

#ifdef NODE_EXCEEDED_EXIT
unsigned long node_num_threshold = 0;
unsigned long cutoff_num_threshold = 0;
#endif


//////////////////////////////////////////////////////////////////////
// variables for solutions and node count
//////////////////////////////////////////////////////////////////////
// store current selections of pairs in the path
int arrSelection[MAX_DEPTH];                 

int solution_length = 0;
int solution_count = 0;

// visited node count and cutoff node count
unsigned long node_num = 0;
unsigned long cutoff_num = 0;


// for debugging
#ifdef SOLVER_OUTPUT
char buffer[40000];
#endif

#endif

