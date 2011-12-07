using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindowsGame1
{
    static public class Exclusion
    {
        static private int MatchHead(String str,int num , String matchedStr){

            for (int i = 0; i < num ; i++){
                if(str.Substring(i) == matchedStr){
                    return 1;
                }
            }
            return 0;

        }
        /*
        static private void ReversePair(Pair from, Pair to){
            int len, j;
	        to.downlen = from.downlen;
	        to.uplen = from.uplen;

            //TODO: REVERSE downlen & uplen

            return;
        }
        */
        //static public int Exclusion_Method(PCPInstance instance, int iterative){

        //    int i;
        //    int j, k;
        //    int len;
        //    Pair pair;
        //    String str0;
        //    String str1;
        //    String str;
        //    String mstr;
        //    int upflag;
        //    int startflag;

        //    for (i=0;i<instance.size;i++)
        //        instance.arrStartingPoint[i] = 0; 
    
        //    // find the starting point
        //    for (i=0;i<instance.size;i++)
        //    {
        //        pair = instance.arrPair[i];
        //        if (pair.uplen < pair.downlen)
        //        {
        //            if (instance.downmask) continue;
        //            len = pair.uplen;
        //            upflag = 0;
        //        }
        //        else
        //        {
        //            if (instance.upmask) continue;
        //            len = pair.downlen;
        //            upflag = 1;
        //        }

        //        //TODO: CONVERT STRING MATCH
        //        if (!memcmp(pair.up, pair.down, len)) // find one starting point
        //        {
        //            instance.arrStartingPoint[i] = 1;

        //            if (upflag==1) {//CopyToString(pPCP->arrPair[i].up, str0);
        //                str0 = instance.arrPair[i].up;
        //            }
        //            else{ 
        //                //CopyToString(pPCP->arrPair[i].down, str0);
        //                str0 = instance.arrPair[i].down;
        //            }
        //            str[0] = str0;	
			
        //            // test whether after choosing this pair, no other pair can be chosen.
        //            // one pair excludes all others
        //            // added on June 5
        //            startflag = 0;
        //            for (j=0;j<instance.size;j++) //test the pair
        //            {
        //                if (j==i) continue;
        //                if (upflag==1){ //CopyToString(pPCP->arrPair[j].down, mstr);
        //                    mstr = instance.arrPair[j].down;
        //                }
        //                else {//CopyToString(pPCP->arrPair[j].up, mstr);
        //                    mstr = instance.arrPair[j].up;
        //                }
        //                if (MatchHead(str, 1, mstr)==1)
        //                {
        //                    startflag=1;
        //                    break;
        //                }
        //                if (upflag==1)
        //                {
        //                    Pair *pPair=instance.arrPair[j];
        //                    int t;
        //                    for (t=1;t<=pPair.downlen-1;t++)
        //                    {
        //                        int length = pPair->uplen;
        //                        if (length>pPair.downlen-t) length = pPair.downlen-t;
        //                        //if (!memcmp(pPair->down+t, pPair->up, length))
        //                        if (!(pPair.down+t == pPair.up))
        //                        {
        //                            PCPInstance PCP;
        //                            PCP.size = 1;
        //                            PCP.width = 3;
        //                            Config config= new Config(200);
        //                            int l;
        //                            ReversePair(instance.arrPair[i], PCP.arrPair[0]);
        //                            PCP.CountOffset();
        //                            //char str[16];
        //                            String str3;
        //                            for (l=0;l<t;l++)
        //                                str3[l] = pPair.down[t-l-1];
							        
        //                            config.ConfigAssign(str3, t, 0);
        //                            config.depth=1;

        //                            int ret = Solver.BeginSolveConfig(instance, config, 200, TEST_DEPTH);
        //                            if (ret>0) //can be solved 
        //                            {
        //                                startflag = 1;
        //                                break;
        //                            }
        //                        }// if
        //                    } // for
        //                    if (startflag) break;
        //                }
        //                else
        //                {
        //                    int t;
        //                    CPair *pPair=&pPCP->arrPair[j];
        //                    for (t=1;t<=pPair->uplen-1;t++)
        //                    {
        //                        int length = pPair->downlen;
        //                        if (length>pPair->uplen-t) length = pPair->uplen-t;
        //                        if (!memcmp(pPair->up+t, pPair->down, length))
        //                        {
        //                            CPCPInstance PCP;
        //                            PCP.size = 1;
        //                            PCP.width = 3;
        //                            CConfig config(200);
        //                            int l;
        //                            ReversePair(&pPCP->arrPair[i], &PCP.arrPair[0]);
        //                            PCP.CountOffset();
        //                            char str[MAXWIDTH];
        //                            for (l=0;l<t;l++)
        //                                str[l] = pPair->up[t-l-1];
        //                            config.ConfigAssign(str, t, 1);
        //                            config.depth=1;
        //                            int ret = CSolver::BeginSolveConfig(&PCP, &config, 200, TEST_DEPTH);
        //                            if (ret>0) 
        //                            {
        //                                startflag = 1;
        //                                break;
        //                            }
        //                        }//if
        //                    }//for
        //                    if (startflag) break;
        //                }//else
        //            }// for
        //            if (startflag==0)
        //            {
        //                pPCP->arrStartingPoint[i] = 0;
        //                continue;
        //            }

        //            // two pairs exclude one pair
        //            if (pPCP->size<=2) continue;
        //            for (j=0;j<pPCP->size;j++) //test the pair
        //            {
        //                if (j==i) continue;
        //                startflag = 0;
        //                if (upflag) CopyToString(pPCP->arrPair[j].up, str1);
        //                else CopyToString(pPCP->arrPair[j].down, str1);
        //                str[1] = str1;

        //                for (k=0;k<pPCP->size;k++)
        //                {
        //                    if (k==i || k==j) continue;
        //                    if (upflag) CopyToString(pPCP->arrPair[k].down, mstr);
        //                    else CopyToString(pPCP->arrPair[k].up, mstr);
        //                    if (MatchHead(str, 2, mstr)) 
        //                    {
        //                        startflag = 1;
        //                        break;
        //                    }
        //                }
        //                // print out debug message
        //                if (startflag) continue;

        //                // now probably you find can exclude one pair, added on May 25
        //                for (k=0;k<pPCP->size;k++)
        //                {
        //                    if (k==i || k==j) continue;
        //                    if (upflag)
        //                    {
        //                        if (pPCP->arrPair[j].uplen<pPCP->arrPair[j].downlen) 
        //                        { // the second pair should have the same direction as the first pair
        //                            startflag = 1;
        //                            break;
        //                        }
        //                        int t;
        //                        CPair *pPair=&pPCP->arrPair[k];
        //                        for (t=1;t<=pPair->downlen-1;t++)
        //                        {
        //                            int length = pPair->uplen;
        //                            if (length>pPair->downlen-t) length = pPair->downlen-t;
        //                            if (!memcmp(pPair->down+t, pPair->up, length))
        //                            {
        //                                CPCPInstance PCP;
        //                                PCP.size = 2;
        //                                PCP.width = 3;
        //                                CConfig config(200);
        //                                int l;
        //                                ReversePair(&pPCP->arrPair[i], &PCP.arrPair[0]);
        //                                ReversePair(&pPCP->arrPair[j], &PCP.arrPair[1]);
        //                                PCP.CountOffset();
        //                                char str[MAXWIDTH];
        //                                for (l=0;l<t;l++)
        //                                    str[l] = pPair->down[t-l-1];
        //                                config.ConfigAssign(str, t, 0);
        //                                config.depth=1;
        //                                int ret = CSolver::BeginSolveConfig(&PCP, &config, 200, TEST_DEPTH);
        //                                if (ret>0) //can be solved
        //                                {
        //                                    startflag = 1;
        //                                    break;
        //                                }
        //                                if (ret==0 && ! (PCP.arrPair[1].uplen>=PCP.arrPair[1].downlen))
        //                                {  
        //                                    //the config can not be solved and the two pairs are not in 
        //                                    // the same direction 
        //                                    startflag = 1;
        //                                    break;
        //                                }
        //                            }// if
        //                        } // for
        //                        if (startflag) break;
        //                    } // if
        //                    else
        //                    {
        //                        if (pPCP->arrPair[j].uplen>pPCP->arrPair[j].downlen) 
        //                        { // the second pair should have the same direction as the first pair
        //                            startflag = 1;
        //                            break;
        //                        }						
        //                        int t;
        //                        CPair *pPair=&pPCP->arrPair[k];
        //                        for (t=1;t<=pPair->uplen-1;t++)
        //                        {
        //                            int length = pPair->downlen;
        //                            if (length>pPair->uplen-t) length = pPair->uplen-t;
        //                            if (!memcmp(pPair->up+t, pPair->down, length))
        //                            {
        //                                CPCPInstance PCP;
        //                                PCP.size = 2;
        //                                PCP.width = 3;
        //                                CConfig config(200);
        //                                int l;
        //                                ReversePair(&pPCP->arrPair[i], &PCP.arrPair[0]);
        //                                ReversePair(&pPCP->arrPair[j], &PCP.arrPair[1]);
        //                                PCP.CountOffset();
        //                                char str[MAXWIDTH];
        //                                for (l=0;l<t;l++)
        //                                    str[l] = pPair->up[t-l-1];
        //                                config.ConfigAssign(str, t, 1);
        //                                config.depth=1;
        //                                int ret = CSolver::BeginSolveConfig(&PCP, &config, 200, TEST_DEPTH);
        //                                if (ret>0) 
        //                                {
        //                                    startflag = 1;
        //                                    break;
        //                                }
        //                                if (ret==0 && !((PCP.arrPair[1].uplen=PCP.arrPair[1].downlen)))
        //                                {  
        //                                    // the config cannot be solved and the two pairs 
        //                                    // arenot in the same direction
        //                                    startflag = 1;
        //                                    break;
        //                                }
        //                            }// if
        //                        }// for		
        //                        if (startflag) break;
        //                    } // else
        //                } // for	
        //                if (startflag) continue;

        //                // now you can safely have two pairs
				
        //                // should be commented later
        //#ifdef USE_EXCLUSION
        //                pPCP->arrStartingPoint[i] = 0;
        //                continue;
        //#endif

        ///*
        //                int ret;
        //                CPCPInstance PCP;
        //                PCP.size = 2;
        //                PCP.width = pPCP->width;
        //                PCP.down_offset = pPCP->down_offset;
        //                PCP.up_offset = pPCP->up_offset;
        //                PCP.offset = PCP.down_offset;
        //                if (PCP.offset < PCP.up_offset) PCP.offset = PCP.up_offset;
        //                PCP.arrPair[0].Copy(&pPCP->arrPair[i]);
        //                PCP.arrPair[0].ID = 1;
        //                PCP.arrPair[1].Copy(&pPCP->arrPair[j]);
        //                PCP.arrPair[1].ID = 2;
        //                ret = SolveInstance(&PCP, iterative_flag);
        //                if (ret == 0) // unresolved
        //                {
        //                    fprintf(ofile, "Trivial case with pair %d and %d! Can not solve it to depth %d!\n",
        //                    i+1, j+1, iterative_depth_threshold);
        //                    return 0;
        //                }
        //                // find solution in trivial case, let the solution checking part to detect it, 
        //                // need improvement
        //                if (ret == 1) 
        //                    continue;
        //                else if (ret == -1)
        //                {
        //                    fprintf(ofile, "You can not start at pair %d and %d!\n", i+1, j+1);
        //                    pPCP->arrStartingPoint[i] = 0;
        //                }*/
        //            }
        //        }
        //        else pPCP->arrStartingPoint[i] = 0;
        //    }
        //    return 1;
        //}
        



    }
}
