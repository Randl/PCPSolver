//////////////////////////////////////////////////////////////////////////////// 
// config.cpp                                                            //
// Description:                                                               //  
//   Implementation of config matching routines:                              //
//   one for testing matching, one for the real matching                      //
// Created time:   May 12, 2001                                               //  
// Modified time:  Feb  2, 2003                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#include "config.h"
#include "PCPInstance.h"

// print matrix to standard output for debugging
void PrintArray(char *arr, int n)
{
    int i;

    for (i=0;i<n;i++)     
    {
        printf("%1d", arr[i]);
        if ((i+1)%10==0) printf("\n");
    }
    if (n%10!=0) printf("\n");
    fflush(stdin);
}

void PrintBit(STRINGTYPE temp, int pos)
{
    for (int j=0;j<pos;j++)
	    printf("%d", (temp>>j)&1);
	printf("\n");
}


void CConfig::Reset()
{
	len = 0;
	depth = 0;
	up = 1;
    num0 = 0;
}

// assign values to a config
void CConfig::ConfigAssign(char *arr, int len, int up)
{
	int i, j;
	int round = len / STRINGTYPEBITSIZE;
	int pos = len % STRINGTYPEBITSIZE;
	int index = 0;

	this->len = len;
	this->up = up;
    num0 = 0;

	// fully fill the new strings
	for (i=0;i<round;i++)
	{
		this->arr[round] = 0;
		for (j=0;j<STRINGTYPEBITSIZE;j++)
			this->arr[round] |= (arr[index+j]<<j);
		index += STRINGTYPEBITSIZE;
	}

	// fill the last one
	if (pos>0)
	{
		this->arr[round] = 0;
		for (j=0;j<pos;j++)
			this->arr[round] |= (arr[index+j]<<j);
	}

    for (i=0;i<len;i++)
        if (arr[i]==0) num0++;

    if (!up) num0=-num0;
}

// compare itself with pConfig
// return 0 if matched, 1 otherwise
int CConfig::ConfigCmp(CConfig *pConfig)
{
	if (len != pConfig->len) return 1;
	if (up != pConfig->up) return 1;
	int round = ConvertBittoString(len);

    return memcmp(arr, pConfig->arr, round*STRINGTYPEBYTESIZE);
}


// print the config for debugging
void CConfig::ConfigPrint(FILE *output)
{
	int i, j;
	STRINGTYPE temp;
	int count = len / STRINGTYPEBITSIZE;
	int pos = len % STRINGTYPEBITSIZE;
	int counter = 0;

	//	fprintf(output, "%d -- %d -- %d\n", node_num, depth, len);
	for (i=0;i<count;i++)
	{
		temp = arr[i];
		for(j=0;j<STRINGTYPEBITSIZE;j++)
		{
			fprintf(output, "%d", (temp>>j)&1);
			counter++;
			//	if (counter%5==0) fprintf(output, " ");
		}
	}

	if (pos!=0)
	{
		temp = arr[count];
	    for (j=0;j<pos;j++)
		{
		    fprintf(output, "%d", (temp>>j)&1);
		    counter++;
			//	if (counter%5==0) fprintf(output, " ");
		}
	}

	fprintf(output, "\n");
	// fprintf(output, "%d\n", num0);
	fflush(output);
}

// initialize
CConfig::CConfig(int size)
{
	Reset();
	arr = new STRINGTYPE[ConvertBittoString(size)];
	assert(arr);
}

// finalize
CConfig::~CConfig()
{
	delete arr;
}

void CConfig::Copy(CConfig *pConfig)
{
	len = pConfig->len;
	depth = pConfig->depth;
	up = pConfig->up;
    num0 = pConfig->num0;
    int configsize = ConvertBittoString(len)*STRINGTYPEBYTESIZE;
	memcpy(arr, pConfig->arr, configsize);
}


// match the pair pointed by pPair with the config
// the matching has been checked, so it always works
// must call TestMatchingPair first before calling this one
int CConfig::MatchPair(CPair *pPair, int *arrSelection)
{
	int len;
    int num0=this->num0;
	
	// update the string in config 
	if (this->up) // up is longer
	{
		if (this->len >= pPair->downlen) // down string will be all matched
		{
			// cut those matched to down string
			this->ConfigMove(pPair->downlen);
			// add the up string to the config
		    this->ConfigAppend(pPair->up, pPair->uplen);
		}
		else  // all letters in the config are matched
		{
			len = pPair->downlen - this->len; // length of left unmatched dwnn string 
			if (pPair->uplen>len) // up is longer
				this->ConfigAssign(pPair->up+len, pPair->uplen-len, 1);
			else  // down is longer, should change the direction of config
				this->ConfigAssign(pPair->down+pPair->uplen+this->len, len-pPair->uplen, 0);
		}
	}
	else 
	{
		if (this->len >= pPair->uplen) // up string will be all matched
		{
			// cut those matched to down string
			this->ConfigMove(pPair->uplen);
			// add the up string to the config
			this->ConfigAppend(pPair->down, pPair->downlen);
        }
		else  // config will be all matched
		{
			len = pPair->uplen - this->len; // length of left unmatched dwnn string 
			if (pPair->downlen>len) // down is longer
				this->ConfigAssign(pPair->down+len, pPair->downlen-len, 0);
			else  // up is longer, should change the direction of config
				this->ConfigAssign(pPair->up+pPair->downlen+this->len, len-pPair->downlen, 1);
		}
	}
    this->num0 = num0+pPair->diff0;
	
	// update depth, selection, and number of visited node
    this->depth++;
	arrSelection[this->depth]=pPair->ID;
	Add_visited_node_number();

    return 1;
}

// check the configuration can be matched with the pair
// output:
//   -1: cannot be matched
//    0:  solved
//    n:  n number left in the configuration
int CConfig::TestMatchingPair(CPair *pPair)
{
///*
	char *str;
	int len;
	int configlength;

	if (this->up) // up is longer
	{
		if (this->len >= pPair->downlen)
		{
			if (this->ConfigCmp(pPair->down, pPair->downlen))
     			return -1;
		}
		else 
		{
			if (this->ConfigCmp(pPair->down, this->len))
				return -1;
			str = pPair->down + this->len;
			len = pPair->downlen - this->len;
			if (len>pPair->uplen) len = pPair->uplen;
			if (memcmp(str, pPair->up, len*sizeof(char)))
				return -1;
		}
		configlength = this->len+pPair->uplen - pPair->downlen;
	}
	else 
	{
		if (this->len >= pPair->uplen)
		{
			if (this->ConfigCmp(pPair->up, pPair->uplen))
				return -1;
		}
		else 
		{
			if (this->ConfigCmp(pPair->up, this->len))
				return -1;
			str = pPair->up + this->len;
			len = pPair->uplen - this->len;
			if (len>pPair->downlen) len = pPair->downlen;
			if (memcmp(str, pPair->down, len*sizeof(char)))
				return -1;
		}
		configlength = this->len+pPair->downlen - pPair->uplen;
	}
	return abs(configlength);
//*/
/*  
    int len;
	register int clen = pConfig->len;
	register char *down = pPair->down;
	register char *up   = pPair->up;
	register int downlen = pPair->downlen;
	register int uplen = pPair->uplen;

	if (pConfig->up) // up is longer
	{

		if (clen >= downlen)
		{
			if (pConfig->ConfigCmp(down, downlen))
     			return -1;
		}
		else 
		{
			if (pConfig->ConfigCmp(down, clen))
				return -1;
			
			len = downlen - clen;
			if (len>uplen) len = uplen;
			
			if (memcmp(down + clen, up, len*sizeof(char)))
				return -1;
		}
	
		return abs(clen + uplen - downlen);
	}
	else 
	{
		if (clen >= uplen)
		{
			if (pConfig->ConfigCmp(up, uplen))
				return -1;
		}
		else 
		{
			if (pConfig->ConfigCmp(up, clen))
				return -1;
			
			len = uplen - clen;
			if (len>downlen) len = downlen;
			
			if (memcmp(up+clen, down, len*sizeof(char)))
				return -1;
		}
		
		return abs(clen + downlen - uplen);
	}
*/
}


char strTemp[2*MAXWIDTH];

// precondition: the config is longer enough to match the strings in both pairs
// return value:
//     0 - find solution when only using the first pair
int CConfig::TestMatchingPairs(CPCPInstance *pcp, CPair *pPair1, CPair *pPair2)
{
	int downlen = pPair1->downlen+pPair2->downlen;
	int uplen = pPair1->uplen+pPair2->uplen;

	if (up)
	{
		strTemp[0]='\0';
		memcpy(strTemp, pPair1->down, pPair1->downlen);
		memcpy(strTemp+pPair1->downlen, pPair2->down, pPair2->downlen);
		if (ConfigCmp(strTemp, downlen)) return -1;
		else return len+uplen-downlen;
	}
	else
	{
		strTemp[0]='\0';
		memcpy(strTemp, pPair1->up, pPair1->uplen);
		memcpy(strTemp+pPair1->uplen, pPair2->up, pPair2->uplen);
		if (ConfigCmp(strTemp, uplen)) return -1;
		else return len+downlen-uplen;
	}
}

/*
int CConfig::TestMatchingPairs(const CPCPInstance *pcp, const int *arr, const int num)
{
	int ret, i;
	const CPair *pPair;

	for (i=0;i<num;i++)
	{
		pPair = &pcp->arrPair[arr[i]];
		ret = TestMatchingPair(pPair);

		if (ret==0)
		{
			if (i==0) return 0;
			else return -(i+1);
		}
	}

	return ret;
}
*/
