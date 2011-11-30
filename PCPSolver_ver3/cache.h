//////////////////////////////////////////////////////////////////////////////// 
// cache.h                                                                    //
// Description:                                                               //  
//   Implementation of cache scheme                                           //
// Created time:   May 12, 2001                                               //  
// Modified time:  Feb  2, 2003                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#ifndef CACHE_H
#define CACHE_H

#include "PCPInstance.h"
#include "config.h"

void Add_cutoff_node_number();
void ClearHashTable();
void ResetHashTable();
int TryFind(CConfig **arr, CConfig *pConfig, CConfig **pCacheConfig); 

#ifdef USE_CONFIG_POOL
	CConfig* newConfig(int size);
	void deleteConfig(int size);
#endif


#endif // CACHE_H

