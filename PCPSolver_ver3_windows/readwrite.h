//////////////////////////////////////////////////////////////////////////////// 
// readwrite.h                                                                //
// Description:                                                               //  
//   Reading routines and writing routines to read instances from the file or //
//   write the instance into the file.                                        //
// Created time:   March                                                      //  
// Modified time:  May 11, 2001                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#ifndef READWRITE_H
#define READWRITE_H

#include "PCPInstance.h"

enum  READWRITEMODE
{
	NORMALMODE= 1, // the first line contains size and width
	CHECKMODE = 2, // solution number & length are in the 1st line too
	EXPMODE   = 3, // solving time are in the 1st line too
	FULLMODE  = 4  // cutoff number and node number are included too

};

class CIO
{
private:	
	static int readPairs(FILE *infile, CPCPInstance *pPCP);
	
public:
	static int readInstance(FILE *infile, CPCPInstance *pPCP, READWRITEMODE readmode=NORMALMODE); 
	static int writeInstance(FILE *infile, CPCPInstance *pPCP, READWRITEMODE writemode=EXPMODE);
	static int writeInstanceLATEX(FILE *infile, CPCPInstance *pPCP, int No);
	static int RemoveInstances(char *smallFile, char *largeFile, char *outFile);
};

//void printhelpmsg();
//void process_arg(int arg, char*argv[], char *infilename, char* outfilename);

#endif //READWRITE_H

