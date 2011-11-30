//////////////////////////////////////////////////////////////////////////////// 
// generator.h                                                                //
// Description:                                                               //  
//   Routines to randomly generate PCP instances and routines to              //
//   systematically generate all PCP instances of a given size and width      //
// Created time:   May 12, 2001                                               //  
// Modified time:  Oct 25, 2001                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////   

#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include <assert.h>
#include "PCPInstance.h"


// use randomly generated instances to search for hard instances
class CRandomGenerator
{
private:
	static void RandomGenerator(CPCPInstance *pPCP, int size, int width, FILE *file);

public:
	static void GenerateHardInstance(int size, int width, char *sol_file_name, int sol_len, int number_tried);
	static int Normalize(char *ifile_name, char *ofile_name, int maxnum);

};

// for computing the score for PCP instances based on lexicographical order
class CPairScore
{
public:
    long up;
    long down;
    long CountStringScore(char *str);
    void Count(CPair *pCPair);
    void Exchange(CPairScore *pScore);
    int Compare(CPairScore *pScore);
};


class CPCPScore
{
public:
    CPairScore *arrPairScore;
    int size;
    void Count(CPCPInstance *pPCP);
    void Sort(CPCPInstance *pPCP);
    int Compare(CPCPScore *pScore);
    void Copy(CPCPScore *pScore);
    
	CPCPScore() 
	{ 
		arrPairScore = NULL;
	}

    ~CPCPScore() 
	{ 
		if (arrPairScore!=NULL) 
			delete arrPairScore;
	}
    
	void Print(FILE *file)
    {
        assert(arrPairScore);
        int i;
        for (i=0;i<size;i++) 
            fprintf(file, "%d.%d ", arrPairScore[i].up, arrPairScore[i].down);
        fprintf(file, "\n");
    }
};

// systematically scan a pcp class
class CSystemmaticScan
{
private:
	static void GenerateAllStrings(int width, char **pArrStr);
	static void GenerateAllPairs(int width, CPair *pArrCPair);
	static void GeneratedAllInstances(int size, int width, char *outputfile);
	static int TestFilters(char *ifile_name, char *passed_file_name, char *nosol_file_name);
	static int TestMasks(char *ifile_name, char *passed_file_name, char *nosol_file_name);
	static int TestExclusions(char *ifile_name, char *passed_file_name, char *nosol_file_name);

public:
	static int ScanSubclass(int size, int width);
};

#endif // GENERATOR_H

