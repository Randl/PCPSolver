//////////////////////////////////////////////////////////////////////////////// 
// cache.cpp                                                                  //
// Description:                                                               //  
//   Implementation of the cache table:                                       //
//   initialize and reset the cache table, search config in the cache and     //
//   store it if applicable.												  //          
// Created time:   May 12, 2001                                               //  
// Modified time:  Feb  9, 2003                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#include "def.h"
#include "cache.h"


// extern declaration
extern unsigned long cutoff_num;
extern int solution_count, solution_length;

// local variables, will be used by solver.cpp (???)
CConfig *hashTable[HASHTABLESIZE];
char Cache[CACHE_SIZE];
int cache_pos=0;

// local variables, only used by this file
char ConfigPool[CONFIG_SIZE];
int configPool_pos=0;
int usedconfiglength=0;

// increment the count for cutoff nodes
void Add_cutoff_node_number()
{
	cutoff_num++; 
#ifdef OUTPUTNODENUM
    if (cutoff_num%10==0)
        printf("%d\n" ,cutoff_num);
#endif
}


// clear all entries and free memory used by configs in the hash table
void ClearHashTable()
{
#ifdef USE_OWN_CACHE

    memset(hashTable, 0, sizeof(hashTable[0])*HASHTABLESIZE); 
    cache_pos = 0;
    usedconfiglength = 0;

#else    

    int i;
	for (i=0;i<HASHTABLESIZE;i++)
		if (hashTable[i])
		{
			delete hashTable[i];
			hashTable[i] = NULL;
		}
    usedconfiglength = 0;

#endif

	// should put them somewhere else, ???
    solution_count = 0;
	solution_length = 0;
}

// Only clear those configs which are not dead
// seems I implemented, but did not used it
// find out if it really works ???
void ResetHashTable()
{
//#ifdef USE_OWN_CACHE
    ClearHashTable();
//#else
 /*   int i;
	for (i=0;i<HASHTABLESIZE;i++)
		if (hashTable[i])
			if (hashTable[i]->status!=STATUS_DEAD)
			{
    			delete hashTable[i];
	    		hashTable[i] = NULL;
				usedconfiglength--;
			}
    solution_count = 0;
	solution_length = 0;
#endif*/
  
}

// allocate a block for a new config of length 'size' in a pool.
// return NULL if no enough space
// return its head address otherwise
inline CConfig* AllocateInPool(int size, char *pool, int &pool_pos)
{
	// space statically allocated for a CConfig object
	int size_of_config = sizeof(CConfig); 

	// the whole space including the configuration
	int size_whole_space = size_of_config + ConvertBittoString(size) * sizeof(STRINGTYPE);
    
    if (pool_pos + size_whole_space > CACHE_SIZE) 
        return NULL; // cache has been used up

    // allocate space and initialize
	CConfig *pConfig = (CConfig*)(pool + pool_pos);
    pConfig->Reset();
    pConfig->arr = (STRINGTYPE*)(pool + pool_pos + size_of_config);
    pool_pos += size_whole_space;

    return pConfig;
}

inline void ReleaseInPool(int size, char *pool, int &pool_pos)
{
	int size_whole_space = sizeof(CConfig) + ConvertBittoString(size) * sizeof(STRINGTYPE);
	pool_pos -= size_whole_space;
}

#ifdef USE_OWN_CACHE
inline CConfig* newCacheConfig(int size)
{
	return AllocateInPool(size, Cache, cache_pos);
}
#endif

#ifdef USE_CONFIG_POOL
CConfig* newConfig(int size)
{
	return AllocateInPool(size, ConfigPool, configPool_pos);
}

void deleteConfig(int size)
{
	ReleaseInPool(size, ConfigPool, configPool_pos);
}
#endif

// Search config in the hash table, if it is not there, this config will be 
// inserted. If inserted, *pCacheConfig will point to the location.
// return value
// 0 -- not found or can not be pruned
// 1 -- find a config that can be pruned in the cache
int TryFind(CConfig **arr, CConfig *pConfig, CConfig **pCacheConfig) 
{
	*pCacheConfig = NULL;
    //return 0;

    int index = pConfig->CalHashValue();
    CConfig *p = arr[index];

	if (!p) // the config is not found and insert this pConfig to it
	{ 
       
#ifdef USE_OWN_CACHE
        if (usedconfiglength>=MAXUSEDCONFIGSIZE) return 0;
		CConfig* pNewConfig = newCacheConfig(pConfig->len);
        if (!pNewConfig) return 0; // cache has been used up, just return;
#else
		if (usedconfiglength>=MAXUSEDCONFIGSIZE) return 0;
        CConfig* pNewConfig = new CConfig(pConfig->len);
		assert(pNewConfig);
#endif

        usedconfiglength++;
		pNewConfig->Copy(pConfig);
		arr[index] = pNewConfig;
		*pCacheConfig = pNewConfig;
		return 0;
	}

	if (p->ConfigCmp(pConfig)) return 0;
	
	// now the configs are the same
	if (p->depth <= pConfig->depth) 
	{
		// if a config hits the hash table with length no less than that in the 
		// table, it cannot be on the path of any optimal solutions not found
		// yet. Remember if an optimal solution is found, all configs along
		// the solution path stored in the hash table are released in 
		// Solution_Found() in solver.cpp
		return 1; 
	}
	else 
	{
		p->depth = pConfig->depth;
		*pCacheConfig = p;
		return 0;
	}
}


