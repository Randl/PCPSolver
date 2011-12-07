////////////////////////////////////////////////////////////////////////////////     
// interface.h                                                                //
// Description: textual interface                                             //  
// Created time:   March , 2001                                               //  
// Modified time:  Feb  3, 2003                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#ifndef INTERFACE_H
#define INTERFACE_H


class CInterface
{
private:
	// print basic info
	static void PrintParameters();
	static void PrintHelpMsg();

	// validate the input parameters
	static void LackValueError(char *name);
	static void CheckValue(int value, char *name);
	static void UnknownArgError(char *name);

	// for different functionalities
	static int ProcessArgScan(int arg, char *argv[]);
	static int ProcessArgRan(int arg, char *argv[]);
	static int ProcessArgSearch(int arg, char *argv[]);
	static int DoExperiments(int arg, char *argv[]);

public:
	static int ProcessArgFunc(int arg, char *argv[]);
	
};

//int ProcessArgFunc(int arg, char *argv[]);
#endif


