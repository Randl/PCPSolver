//////////////////////////////////////////////////////////////////////////////// 
// readwrite.cpp                                                              //
// Description:                                                               //  
//   Reading routines and writing routines to read instances from the file or //
//   write the instance into the file.                                        //
// Created time:   March   2001                                               //  
// Modified time:  Feb  9, 2003                                               //  
// Author: Ling Zhao (zhao@cs.ualberta.ca)                                    // 
////////////////////////////////////////////////////////////////////////////////     

#include "readwrite.h"
#include "def.h"
#include "basic.h"

extern int FINAL_DEPTH_THRESHOLD, DETERMINANT_DEPTH, SEARCH_TO_OPTIMAL_LENGTH_FLAG;


// size and width should have been stored in pPCP
// in each of current and the next line, the number of strings must be 'size'
// return value:
//	1 -- succeed
//	0 -- fail
int CIO::readPairs(FILE *infile, CPCPInstance *pPCP)
{
	int i;
	char ch;
	int size = pPCP->size;

	/////////////////////////////
	// First line
    // get all up strings   
	for (i=0;i<size;i++)
	{
		// skip all blank or tab
		do 
		{
			fscanf(infile, "%c", &ch);
		} while (ch==' ' || ch =='\t');

		if (ch!='0' && ch!='1') return 0;

		do 
		{
			pPCP->arrPair[i].up[pPCP->arrPair[i].uplen++] = int(ch-'0');
			fscanf(infile, "%c", &ch);
		} while ( (ch=='0'||ch=='1')&&!feof(infile) );

		pPCP->arrPair[i].up[pPCP->arrPair[i].uplen] = SENTINEL;
		
	}


	// clean current line
	while (ch!='\n'&&!feof(infile)) 
		fscanf(infile, "%c", &ch);

	// check if end of file
	if (feof(infile)) return 0;

	/////////////////////////////
	// Second line
    // get all down strings   
	for (i=0;i<size;i++)
	{
		// skip all blank or tab
		do 
		{
			fscanf(infile, "%c", &ch);
		} while (ch==' ' ||  ch =='\t');

		if (ch!='0' && ch!='1') return 0;

		do 
		{
			pPCP->arrPair[i].down[pPCP->arrPair[i].downlen++] = int(ch-'0');
			fscanf(infile, "%c", &ch);
		} while ( (ch=='0'||ch=='1')&&!feof(infile) );

		pPCP->arrPair[i].down[pPCP->arrPair[i].downlen] = SENTINEL;
	}

	// clean current line
	while (ch!='\n'&&!feof(infile)) 
		fscanf(infile, "%c", &ch);

	return 1;
}


// return value:
//		1 - succeed
//		2 - no more instances
//		0 - fail
//
// readmode: see enum READMODE (default is NORMALMODE)
int CIO::readInstance(FILE *infile, CPCPInstance *pPCP, READWRITEMODE readmode) 
{
	char ch;
	int size, width;
	char str[MAXINPUTLINELEN]; // no lines of the input should be longer than it
	float tmp;

	pPCP->Init();

	// skip all comments and blank lines
	while (!feof(infile))
	{
		// every instance starts with a line with its first char being a digit
		// others are all considered as comments
		ch = getc(infile);
		if (isdigit(ch)) {ungetc(ch, infile); break;}
		if (ch!='\n') fgets(str, MAXINPUTLINELEN, infile);		
	}

	// end of input by the end of file or a line starting at '0'
    if (feof(infile)||ch=='0') return 2;

	// read the first line, its interpretation depends on readmode
	fgets(str, MAXINPUTLINELEN, infile);

	if (SEARCH_TO_OPTIMAL_LENGTH_FLAG)
		readmode = EXPMODE;

	switch (readmode)
	{
		case NORMALMODE: 	
			sscanf(str, "%d %d\n", &size, &width);
			break;

		case CHECKMODE: 
			sscanf(str, "%d %d %d %d\n", &size, &width,
				         &pPCP->sol_len, &pPCP->sol_num);	
			break;

		case EXPMODE:
			sscanf( str, "%d %d %d %d %f\n", &size, &width,&pPCP->sol_len, 
	                &pPCP->sol_num,  &tmp);
		    pPCP->sol_time[0] = (long)(tmp*data_clocks_per_sec);
			break;

		case FULLMODE:
			sscanf(str, "%d %d %d %d %d %d %f\n", &size, &width,&pPCP->sol_len, 
	               &pPCP->sol_num, &pPCP->node_num[0], &pPCP->cutoff_num[0], &tmp);
		    pPCP->sol_time[0] = (long)(tmp*data_clocks_per_sec);
			break;
	}
        
	// check if it is still valid
	if (size>MAXSIZE) printerr("The instance exceed the max size!\n");
	if (width>MAXWIDTH) printerr("The instance exceed the max width!\n");

	pPCP->size = size;
	pPCP->width = width;

	// read two lines which include all pairs in the instance
	if ( readPairs(infile, pPCP) == 0) 
		printerr("Instance format error");

	// sentinel
    pPCP->arrPair[size].uplen = 0;
   	pPCP->arrPair[size].downlen = 0;

	pPCP->CountOffset();

    if (SEARCH_TO_OPTIMAL_LENGTH_FLAG)    
	{
		FINAL_DEPTH_THRESHOLD = pPCP->sol_len;
		DETERMINANT_DEPTH = pPCP->sol_len;
	}


/*  // for experiment
    pPCP->poi=pPCP->sol_len/20;
    if (pPCP->sol_len%20!=0) pPCP->poi++;
    for (int i=1;i<=pPCP->poi;i++)
        fscanf(infile, "%3d: %10d %6d %10d", &pPCP->depth[i], 
		       &pPCP->node_num[i], &pPCP->cutoff_num[i], &pPCP->sol_time[i]);
*/

	return 1;
}

// default of writemode is EXPMODE
int CIO::writeInstance(FILE *infile, CPCPInstance *pPCP, READWRITEMODE writemode)
{
	int i, k, j;
	char ch;
	int count;
	int size, width;

	size = pPCP->size ;
	width = pPCP->width;    

	switch (writemode)
	{
		case NORMALMODE: 	
			fprintf(infile, "%d %d\n", size, width);
			break;

		case CHECKMODE: 
			fprintf(infile, "%d %d %d %d\n", size, width, 
				             pPCP->sol_len, pPCP->sol_num);
			break;

		case EXPMODE:
			fprintf(infile, "%d %d %d %d %f\n", size, width, 
				    pPCP->sol_len, pPCP->sol_num, 
					(pPCP->sol_time[0])/(float)data_clocks_per_sec);
			break;
     
		case FULLMODE:
			fprintf(infile, "%d %d %d %d %d %d %f\n", 
				             size, width, 
				             pPCP->sol_len, pPCP->sol_num, 
				             pPCP->node_num[pPCP->poi+1], pPCP->cutoff_num[pPCP->poi+1], 
		    				 (pPCP->sol_time[0])/(float)data_clocks_per_sec);
			break;
	}

	// write all up strings   
	for (i=0;i<size;i++)
	{
		k = 0;
		ch = (char)pPCP->arrPair[i].up[k++];
		count = 0;

		do 
		{
			fprintf(infile, "%c", ch+'0');
			ch = (char)pPCP->arrPair[i].up[k++];
			count++;
		} while (ch!=SENTINEL);

		if (count!=pPCP->arrPair[i].uplen) 
			printerr("Error in writing PCP instance!");

		for (j=pPCP->arrPair[i].uplen;j<=pPCP->width+1;j++)
			fprintf(infile, " ");
	}

	fprintf(infile, "\n");
    // write all down strings   
	for (i=0;i<size;i++)
	{
		k = 0;
		ch = (char)pPCP->arrPair[i].down[k++];
		count = 0;

		do 
		{
			fprintf(infile, "%c", ch+'0');
			ch = (char)pPCP->arrPair[i].down[k++];
			count++;
		} while (ch!=SENTINEL);

		if (count!=pPCP->arrPair[i].downlen) 
			printerr("Error in reading PCP instance!");

		for (j=pPCP->arrPair[i].downlen;j<=pPCP->width+1;j++)
			fprintf(infile, " ");
	}

    fprintf(infile, "\n");

    // Test element difference
/*    for (i=0;i<size;i++)
        fprintf(infile, "%d %d |", pPCP->arrPair[i].diff0,   pPCP->arrPair[i].diff1);
    fprintf(infile, "\n");
    fprintf(infile, "%d %d %d %d\n", pPCP->up_0drop, pPCP->down_0drop, 
					pPCP->up_1drop, pPCP->down_1drop);
    for (i=1;i<=pPCP->poi;i++)
        fprintf(infile, "%3d: %10d %6d %10d\n", pPCP->depth[i], 
		pPCP->node_num[i], pPCP->cutoff_num[i], pPCP->sol_time[i]);
*/    
	
	fflush(infile);

	return 1;
}

/*
// write instances in Latex format
int writeInstanceLATEX(FILE *infile, CPCPInstance *pPCP, int No)
{
	int i, j, k;
	char ch;
	int count;

	int size;
	size = pPCP->size ;

    fprintf(infile, "%d & $\\bigg(\\begin{array}{", No);
    for (i=1;i<=size;i++) fprintf(infile, "l");
    fprintf(infile, "}\n");

	// write all up strings   
	for (i=0;i<size;i++)
	{
        fprintf(infile, "\\texttt{");
		k = 0;
		ch = (char)pPCP->arrPair[i].up[k++];
		count = 0;

		do 
		{
			fprintf(infile, "%c", ch+'0');
			ch = (char)pPCP->arrPair[i].up[k++];
			count++;
		} while (ch!=SENTINEL);

		if (count!=pPCP->arrPair[i].uplen) 
			printerr("Error in writing PCP instance!");

		for (j=pPCP->arrPair[i].uplen;j<=pPCP->width-1;j++)  
			fprintf(infile, "{ }");

        fprintf(infile, "}");
        if (i!=size-1) fprintf(infile,"&");
	}

	fprintf(infile, "\\\\\n");

    // write all down strings   
	for (i=0;i<size;i++)
	{
        fprintf(infile, "\\texttt{");
        k = 0;
		ch = (char)pPCP->arrPair[i].down[k++];
		count = 0;

		do 
		{
			fprintf(infile, "%c", ch+'0');
			ch = (char)pPCP->arrPair[i].down[k++];
			count++;
		} while (ch!=SENTINEL);

		if (count!=pPCP->arrPair[i].downlen) 
			printerr("Error in reading PCP instance!");

		for (j=pPCP->arrPair[i].downlen;j<=pPCP->width-1;j++) 
			fprintf(infile, "{ }");

        fprintf(infile, "}");
        if (i!=size-1) fprintf(infile,"&");
	}

    fprintf(infile, "\n");
    fprintf(infile, "\\end{array}\\bigg)$\n");
    fprintf(infile, "& %d & %d", pPCP->sol_len, pPCP->sol_num);
    fprintf(infile, "\\\\\\hline\n\n");
	fflush(infile);

	return 1;
}
*/

// output instances in largeFile not smallFile to outFile
int CIO::RemoveInstances(char *smallFile, char *largeFile, char *outFile)
{
    FILE *smallfile = fopen(smallFile, "r");
    FILE *largefile = fopen(largeFile, "r");
    FILE *outfile   = fopen(outFile  , "w");

    if (!smallfile||!largefile||!outfile) 
    {
        printf("FILE OPEN ERROR!\n");
        exit(0);
    }

    int count1=0, count2=0, count3=0;
    int succ, flag;
    int i;

    CPCPInstance arrPCP1[100];
    CPCPInstance PCP;
    while(1)
	{
		succ = readInstance(smallfile, &arrPCP1[count1]);
		if (succ!=1) break;
        count1++;
    }
 
    while (1)
    {
		succ = readInstance(largefile, &PCP);
		if (succ!=1) break;
        count2++;
        flag=1;
        for (i=0;i<count1;i++)
            if (PCP.Compare(&arrPCP1[i]))
            {
                printf("Remove No. %d\n", count2);
                flag=0;
            }
        if (!flag) continue;  // find one

        count3++;
        fprintf(outfile, "Instance %d\n", count3); 
        writeInstance(outfile, &PCP);
        fprintf(outfile, "\n");
    }
    return 1;
}

