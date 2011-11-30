//////////////////////////////////////////////////////////////////////////////// 
// config.h                                                              //
// Description:                                                               //  
//   Implementation of config matching routines                               //
// Created time:   May 12, 2001                                               //  
// Modified time:  May 12, 2001                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
//////////////////////////////////////////////////////////////////////////////// 
    
#ifndef CONFIG_H
#define CONFIG_H

#include "PCPInstance.h"

extern unsigned long node_num;



#define STRINGTYPEBITSIZE  (sizeof(STRINGTYPE)*8)
#define STRINGTYPEBYTESIZE sizeof(STRINGTYPE)
// the letters in config is cramed into bits of a string whose type
// is defined in def.h. Default is int
#define ConvertBittoString(size) ((size+STRINGTYPEBITSIZE-1)/STRINGTYPEBITSIZE)

class CConfig
{
private:
	inline int ConfigCmp(char *arr, int n);
	inline void ConfigMove(int offset);
	inline void ConfigAppend(char *arr, int len);
	inline void Add_visited_node_number();
	
public:
    STRINGTYPE *arr;	
	unsigned short int len;
	unsigned short int depth;
	short int num0;  // #0 in the config, minus for bottom config
	char up;         // top or bottom
	char status;	
    // int size;
		
   	//Config();
	CConfig(int size);
	~CConfig();
	
	void Copy(CConfig *config);
	void Reset();

	// used by exclusion method and mask method
	void ConfigAssign(char *arr, int len, int up);

	int TestMatchingPair(CPair *pPair);
	int MatchPair(CPair *pPair, int *arrSelection);

	inline unsigned long CalHashValue();
	int ConfigCmp(CConfig *pConfig);

	// for debugging
	void ConfigPrint(FILE *output=stdout);

	//int TestMatchingPairs(const CPCPInstance *pcp, const int *arr, const int num);
	int TestMatchingPairs(CPCPInstance *pcp, CPair *pPair1, CPair *pPair2);
};

inline void CConfig::Add_visited_node_number()
{
	node_num++;
#ifdef OUTPUTNODENUM
    if (node_num %10000 ==0) 
		printf("%d\n" ,node_num);
#endif
}
unsigned long CConfig::CalHashValue()
{
	int round = ConvertBittoString(len);
	STRINGTYPE str = 0;

	for (int i=0;i<round;i++)
		str ^= arr[i];

	str ^= (len<<9);	
	str &=(HASHTABLESIZE-1);
	return (unsigned long)str;
}

// compare the letters in config with string arr
// return 0 if matched, 1 otherwise
int CConfig::ConfigCmp(char *arr, int n)
{
	register int i ,j;
	register STRINGTYPE temp;
	register char *array=arr;       // array for the input

		
	// In default, STRINGTYPEBITSIZE is 32, and n is no more 
	// than than MAXWIDTH, which is 16. Thus typically the if statement 
	// will always evaluate true.
	if (n <= STRINGTYPEBITSIZE)
	{

		temp = *(this->arr);
		register char *end = array+n;
		
		do 
		{
			if ((char)(temp & 1) != (*array++))
				return 1;
			temp >>= 1;
		}  while (array<end);

		return 0;
	}

	// typically you won't reach here except your changed the default.	
	register int round = n / STRINGTYPEBITSIZE;
	register int pos = n % STRINGTYPEBITSIZE;

	for (i=0;i<round;i++)
	{
		temp=this->arr[round];
		for (j=0;j<STRINGTYPEBITSIZE;j++)
		{
			if ((char)(temp & 1) != *array++)
				return 1;
			temp >>= 1;
		}
	}
	
	// last round
	if (pos>0) 
	{
		temp=this->arr[round];
		
		for (j=0;j<pos;j++)
		{
			if ((char)(temp & 1) != *array++)
				return 1;
			temp >>= 1;
		}
	}
	
	return 0;
}

// move string left with the offset
void CConfig::ConfigMove(int offset) 
{
    // patching for normal cases
	// mostly you will go through the if statements	
	// good for CC
	if (offset < STRINGTYPEBITSIZE)
	{
		register STRINGTYPE start, next;
		register int r_offset = STRINGTYPEBITSIZE - offset;

 		register int i;
		register int endindex = ConvertBittoString(len)-1;
		
		len -= offset;
		start = arr[0]>>offset;

		for (i=0;i<endindex;i++)
		{
			next  = arr[i+1];
			arr[i] = start | (next<<r_offset);
			start = next>>offset;
		}
		arr[endindex] = start;
	
		return;
	}
/*		
	// good for g++
	if (offset < STRINGTYPEBITSIZE)
	{
		register STRINGTYPE start, next;
		register int r_offset = STRINGTYPEBITSIZE - offset;
 
		register STRINGTYPE *pstart = arr, 
		                    *pnext=pstart+1, 
		                    *pend = pstart + ConvertBittoString(len)-1;

		len -= offset;
		start = *pstart>>offset;

		while (pstart<pend)
		{
			next = *pnext++;
			*pstart++ = start | (next<<r_offset);
			start = next>>offset;
		}
		*pend = start;

		return;
	}
*/
		
// old implementation
	int i ;
    int round = ConvertBittoString(len);
	int offset_round = offset / STRINGTYPEBITSIZE;
	int offset_pos = offset % STRINGTYPEBITSIZE;
	STRINGTYPE temp1, temp2;
	int r_offset = STRINGTYPEBITSIZE - offset;
	
	len -= offset;
	temp1 = arr[offset_round];

	for (i=offset_round;i<round-1;i++)
	{
		temp2 = arr[i+1];
	

		arr[i-offset_round] =  (temp1>>offset_pos) & 
			                    ((1<<(STRINGTYPEBITSIZE-offset_pos))-1)
			                    | (temp2<<(STRINGTYPEBITSIZE-offset_pos)) 
							    & ~((1<<(STRINGTYPEBITSIZE-offset_pos))-1);
		
		arr[i-offset_round] =  (temp1>>offset_pos) | (temp2<<r_offset); 

		temp1 = temp2;
	}

	arr[round-1-offset_round] = arr[round-1]>>offset_pos;
	
	// clear to 0 for used part
	arr[round-1-offset_round] &= (1<<(STRINGTYPEBITSIZE-offset_pos))-1; 

}

void CConfig::ConfigAppend(char *arr, int len)
{
	int round = this->len / STRINGTYPEBITSIZE;
	int pos = this->len % STRINGTYPEBITSIZE;
		
	// patching for normal cases
	// fill the element wholly in one string
	if (len<=(int)STRINGTYPEBITSIZE-pos) 
	{
		register STRINGTYPE p = 0;
		
		for (int i=0;i<len;i++)
			p |= arr[i] << (pos+i);

		this->arr[round] |= p;
		this->len += len;		
	}
	else 
	{
		int i, j;

		int index;

		this->len += len;
	
		// fill the first string
		for (i=0;i<(int)STRINGTYPEBITSIZE-pos;i++)
			this->arr[round] |= (arr[i]<<(pos+i));

		len -= (STRINGTYPEBITSIZE-pos);
		index = STRINGTYPEBITSIZE-pos;
        int arr_round = len / STRINGTYPEBITSIZE;
        int arr_pos = len % STRINGTYPEBITSIZE;

		// fill fully the new strings
		for (i=1;i<=arr_round;i++)
		{
			this->arr[round+i] = 0;
			for (j=0;j<STRINGTYPEBITSIZE;j++)
				this->arr[round+i] |= (arr[index+j]<<j);
			index += STRINGTYPEBITSIZE;
		}

		// fill the last one
		if (arr_pos>0)
		{
			this->arr[round+arr_round+1] = 0;
			for (j=0;j<arr_pos;j++)
				this->arr[round+arr_round+1] |= (arr[index+j]<<j);
		}
	}
}

#endif


