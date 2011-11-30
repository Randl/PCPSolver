//////////////////////////////////////////////////////////////////////////////// 
// basic.h                                                              	  //
// Description:                                                               //  
//   Including generic routines 											  //
// Created time:   Apr 4, 2003                                                //  
// Modified time:  Apr 4, 2003                                                //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////

#ifndef BASIC_H
#define BASIC_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// for WINDOWS or UNIX?
#ifndef UNIX
#define WINDOWS
#endif

// basic routines
void printerr(char *msg);
void rand_init();


// basic time counting class
#define WINDOWS

#ifdef WINDOWS
class CTime
{
private:
    int start_time;
    int time_tip;

public:
    void Start();
    void End();
    double GetInterval();
};

#else
// this only work under linux/unix
#include <sys/time.h>
class CTime
{
private:
    struct timeval  first,
                   second,
                   lapsed;

public:
    void Start();
    void End();
    double GetInterval();
};


#endif

#endif // BASIC_H
