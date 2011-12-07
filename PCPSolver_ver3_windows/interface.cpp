////////////////////////////////////////////////////////////////////////////////     
// interface.cpp                                                              //
// Description: textual interface                                             //  
// Created time:   March , 2001                                               //  
// Modified time:  Feb  3, 2003                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#include "cache.h"
#include "solver.h"
#include "generator.h"
#include "basic.h"
#include "interface.h"

extern int ADDED_DEPTH, FINAL_DEPTH_THRESHOLD, Iterative_Flag,
	       SEARCH_TO_OPTIMAL_LENGTH_FLAG;


void CInterface::PrintParameters()
{
	printf("Parameters: \n");
	printf("  Clock per second: %d\n", CLOCKS_PER_SEC);
	printf("  Cache size      : %d\n", MAXUSEDCONFIGSIZE);
	printf("  Hash table size : %d\n", HASHTABLESIZE);
	printf("  Iterative depth : %d\n", ADDED_DEPTH);
	printf("  Maximum depth   : %d\n\n", FINAL_DEPTH_THRESHOLD);
}

void CInterface::PrintHelpMsg()
{
	printf("pcp  : Solving tool for instances of Post\'s correspondence problem\n\n");
	printf("Usage: 1. Solve PCP instances in a file:\n");
	printf("          pcp -d depth -di inc -r run -i inputfile -o outputfile -ni\n\n");
	printf("       2. Scan a PCP subclass:\n");
	printf("          pcp -scan -s size -w width -d depth\n\n");
	printf("       3. Randomly generate difficult solvable instances:\n");
	printf("          pcp -ran -s size -w width -d depth -t times -l sol_lim\n"); 
    printf("              -o ofilename\n\n"); 
	printf("          run     - number of runs\n");
	printf("          ni      - no iterative search\n");
	printf("          di      - depth increment\n");
	printf("          depth   - search depth\n");
	printf("          times   - number of instances to be tried\n");
	printf("          sol_lim - min optimal solution length (only instances satisfying\n");
	printf("                    it will be recorded)\n\n");

	exit(0);
}

void CInterface::LackValueError(char *name)
{
	printf("Error: lack of the value for %s.\n", name);
	printf("Type pcp -h for help.\n");
	exit(0);
}

void CInterface::CheckValue(int value, char *name)
{	
	if (value<=0) 
	{
		printf("Error: the value for %s <=0\n", name);
		printf("Type pcp -h for help.\n");
		exit(0);	
	}
}

void CInterface::UnknownArgError(char *name)
{
	printf("Unknown	parameters: %s\n", name);
	printf("Type pcp -h for help.\n");
	exit(0);
}

// scan a PCP subclass
int CInterface::ProcessArgScan(int arg, char *argv[])
{
    // default value
	int size  = 2;
	int width = 3;
	int depth = 40;
	
	// get the value from arguments
    for (int i=1;i<arg;i++)
    {
        if (!strcmp("-s", argv[i])) // size
        {
            if (++i==arg) LackValueError("size");
            sscanf(argv[i], "%d", &size);
			CheckValue(size, "size");
        }
		else if (!strcmp("-w", argv[i])) // width
        {
            if (++i==arg) LackValueError("width");
            sscanf(argv[i], "%d", &width);
			CheckValue(width, "width");
        }
		else if (!strcmp("-d", argv[i])) // depth
        {
            if (++i==arg) LackValueError("depth");
            sscanf(argv[i], "%d", &depth);
			CheckValue(depth, "depth");
        }
		else if (strcmp("-scan", argv[i]))
				UnknownArgError(argv[i]);
	}	


	FINAL_DEPTH_THRESHOLD = depth;
	printf("Scan class PCP[%d,%d] (search depth=%d):\n", 
			                         size, width, depth);
	CSystemmaticScan::ScanSubclass(size, width);

	return 1;
}

// randomly generate instance to solve
int CInterface::ProcessArgRan(int arg, char *argv[])
{
    // default value
    int times = 1000;
	int size = 4;
	int width = 4;
	int len_lim = 4;
	int depth = 40;
	char ofilename[80] = "hard.txt";

	// get the value from arguments
    for (int i=1;i<arg;i++)
    {
        if (!strcmp("-s", argv[i])) // size
        {
            if (++i==arg) LackValueError("size");
            sscanf(argv[i], "%d", &size);
			CheckValue(size, "size");
        }
		else if (!strcmp("-w", argv[i])) // width
        {
            if (++i==arg) LackValueError("width");
            sscanf(argv[i], "%d", &width);
			CheckValue(width, "width");
        }
		else if (!strcmp("-d", argv[i])) // depth
        {
            if (++i==arg) LackValueError("depth");
            sscanf(argv[i], "%d", &depth);
			CheckValue(depth, "depth");
        }
		else if (!strcmp("-t", argv[i])) // times
        {
            if (++i==arg) LackValueError("times");
            sscanf(argv[i], "%d", &times);
			CheckValue(times, "times");
        }
		else if (!strcmp("-l", argv[i])) // len_lim
        {
            if (++i==arg) LackValueError("len_lim");
            sscanf(argv[i], "%d", &len_lim);
			CheckValue(len_lim, "len_lim");
        }
		else if (!strcmp("-o", argv[i])) // outputfile
        {
            if (++i==arg) LackValueError("ofilename");
            sscanf(argv[i], "%s", ofilename);
        }
		else if (strcmp("-ran", argv[i]))
				UnknownArgError(argv[i]);
	}	


	FINAL_DEPTH_THRESHOLD = depth;
	
	printf("Randomly try instances in PCP[%d,%d] ", size, width);
	printf("(search depth=%d, try %d times)\n", depth, times);
	printf("(only sol_len>=%d recorded in %s):\n\n", len_lim, ofilename);
	CRandomGenerator::GenerateHardInstance(size, width, ofilename, 
			                               len_lim, times);
	
	// normalize all instances if you uncomment the next two lines
	// char normalized_ofilename[] = "norm.txt";
	// CRandomGenerator::Normalize(ofilename, normalized_ofilename, times);

	return 1;
}

// solve instances in file
int CInterface::ProcessArgSearch(int arg, char *argv[])
{
	int i;
   	CTime timer;
    
  	// default value
	int run = 1;
	int depth = FINAL_DEPTH_THRESHOLD;
	int di = ADDED_DEPTH;
	char ifile[80] = "input.txt";
	char ofile[80] = "sol.txt";
	char nosol[80] = "nosol.txt";
	char unsol[80] = "unsol.txt";

	// get the value from arguments
    for (i=1;i<arg;i++)
    {
		if (!strcmp("-d", argv[i])) // depth
        {
            if (++i==arg) LackValueError("depth");
            sscanf(argv[i], "%d", &depth);
			CheckValue(depth, "depth");
        }
		else if (!strcmp("-di", argv[i])) // depth increment
        {
            if (++i==arg) LackValueError("depth increment");
            sscanf(argv[i], "%d", &di);
			CheckValue(di, "depth increment");
        }
		else if (!strcmp("-r", argv[i])) // run
        {
            if (++i==arg) LackValueError("run");
            sscanf(argv[i], "%d", &run);
			CheckValue(run, "run");
        }
		else if (!strcmp("-i", argv[i])) // inputfile
        {
            if (++i==arg) LackValueError("ifilename");
            sscanf(argv[i], "%s", ifile);
        }
		else if (!strcmp("-o", argv[i])) // outputfile
        {
            if (++i==arg) LackValueError("ofilename");
            sscanf(argv[i], "%s", ofile);
        }
		else if (!strcmp("-ni", argv[i])) // no iterative search, search to a depth directly
        {
			Iterative_Flag = 0;
			// for experiments
	        SEARCH_TO_OPTIMAL_LENGTH_FLAG = 1;
        }
		else UnknownArgError(argv[i]);
	}	

	FINAL_DEPTH_THRESHOLD = depth;
	ADDED_DEPTH = di;

	printf("Solve instances in file %s", ifile);
	if (run>1) printf(" (%d runs)", run);
	printf("\n(solvable => %s, unsolvable => %s, unsolved => %s):\n\n", 
			                                     ofile, nosol, unsol);
	PrintParameters();

	for (i=1;i<=run;i++)
	{
		if (run>1) printf("Run %d:", i);
		timer.Start();
		CSolver::PCPSolver(ifile, ofile, nosol, unsol);
		timer.End();
		printf("time: %f\n\n", timer.GetInterval());
	}

	return 1;
}

// process the argument and decide which functionality is to be 
// used
int CInterface::ProcessArgFunc(int arg, char *argv[])
{
	// 	1: search solutions 2. scan a subclass
	// 	3. random search  	4. print help
	//  5: experiments
	// 	default is 1
	int func = 1;
	
    for (int i=1;i<arg;i++)
    {
		if (!strcmp("-scan", argv[i]))
			func = 2;
		else if (!strcmp("-ran", argv[i]))
			func = 3;
		else if (!strcmp("-h", argv[i]))
		{
		    func = 4;
			break; // help is always urgent
		}
		else if(!strcmp("-exp", argv[i]))
			func = 5;
	}
	
	switch (func)
	{
		case 1: ProcessArgSearch(arg, argv);
				break;
		case 2: ProcessArgScan(arg, argv);
				break;
		case 3: ProcessArgRan(arg, argv);
				break;
		case 4: PrintHelpMsg();
				break;
		case 5: DoExperiments(arg, argv);
				break;
	}

	return 1;
}

int CInterface::DoExperiments(int arg, char *argv[])
{
	int i;
    
  	// default value
	char ifile[80] = "input.txt";


	// get the value from arguments
    for (i=1;i<arg;i++)
    {
		if (!strcmp("-i", argv[i])) // inputfile
        {
            if (++i==arg) LackValueError("ifilename");
            sscanf(argv[i], "%s", ifile);
        }
		else if(strcmp("-exp", argv[i]))
			UnknownArgError(argv[i]);
	}	

	return CSolver::Statistics(ifile);
}


