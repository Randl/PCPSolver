////////////////////////////////////////////////////////////////////////////////     
// solver.h                                                                   //
// Description:                                                               //  
//   Include main routines to solve the PCP instances                         //
// Created time:   March                                                      //  
// Modified time:  May 11, 2001                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////  

#ifndef SOLVER_H
#define SOLVER_H

#include "PCPInstance.h"
#include "config.h"

class CSolver
{
private:
	static int Solution_Found(CPCPInstance *pPCP, CConfig *pConfig);
	static int SolveConfig(CPCPInstance *pPCP, CConfig *pConfig, int maxlen);
	static int SearchSolution(CPCPInstance *pPCP);
	static inline int IsPrunable(CPCPInstance *pPCP, CConfig *pConfig);
	static void PrintSolution(int *arr, int len);
	
public:
	static void OutputSolvingInfo(CPCPInstance *pPCP, FILE *outputfile, int time_tips, char *comment);
	static int PCPSolver(char *ifile_name, char *sol_file_name, 
			             char *nosol_file_name=NULL, char*unsolved_file_name=NULL);
	static int BeginSolveConfig(CPCPInstance *pPCP, CConfig *pConfig, int maxlen, int depth);	
	static int SolvePCPInstance(CPCPInstance *pPCP, int iterative_flag);
	
	// for experiment
	static int Statistics(char *ifile_name);
};

#endif //SOLVER_H
