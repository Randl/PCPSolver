//////////////////////////////////////////////////////////////////////////////// 
// basic.cpp                                                              	  //
// Description:                                                               //  
//   Including generic routines 											  //
// Created time:   Apr 4, 2003                                                //  
// Modified time:  Apr 4, 2003                                                //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////

#include "basic.h"

// print an error msg and exit
void printerr(char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(0);
}

// set the random seed
void rand_init() 
{
	srand((unsigned)time(NULL));
}


#ifdef WINDOWS

void CTime::Start()
{
    start_time = clock();
}

void CTime::End()
{
    time_tip = clock()-start_time;
}

double CTime::GetInterval()
{
    return time_tip/(double)CLOCKS_PER_SEC;
}

#else
void CTime::Start()
{
    struct timezone tzp;
    gettimeofday(&first, &tzp);
}

void CTime::End()
{
    struct timezone tzp;
    gettimeofday(&second, &tzp);
}

double CTime::GetInterval()
{
    if (first.tv_usec>second.tv_usec)
    {
        second.tv_usec += 1000000;
        second.tv_sec--;
    }

    lapsed.tv_usec = second.tv_usec - first.tv_usec;
    lapsed.tv_sec  = second.tv_sec  - first.tv_sec;
    return (lapsed.tv_sec+lapsed.tv_usec/(float)1000000);
}


#endif
