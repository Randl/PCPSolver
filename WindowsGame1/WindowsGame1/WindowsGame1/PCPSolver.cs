using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindowsGame1
{
    static class PCPSolver
    {
        static public int node_num = 0;
        static public int solution_count = 0;
        static public int iterative_depth_threshold = 0;
       
        
        static public int[] arrSelection= new int[2000];
        //public PCPSolver(PCPInstance instance){
        //    int ret;
        //    int succ;
        //    //clock_t time_start,time_end;
        //    int time_tips;
        //    int count = 0, nosol_count = 0, sol_count = 0, unsolved_count=0;
        //    PCPInstance PCP, ReversePCP;

        //    //time_start = clock();
        //    ret = SolvePCPInstance(instance, 0);
        //    //time_end = clock();

        //}
        static public int TryFind(Config arr, Config arr2, Config arr3)
        {
            throw new NotImplementedException();
        }

        static public int SearchSolution(PCPInstance instance)
        {
            
            
            //start at beginning.
            int startingpoint_flag = 0;
	        
            
            
            int i, len, succ;    
            int ret = -1, solveret = -1;
            Pair pPair;
	        int maxlen = instance.offset*(100+1);
            Config pCacheConfig;
  
	        //CConfig pNewconfig(maxlen);
            Config pNewConfig;

        
            pNewConfig = new Config(maxlen);
        

            for (i=0;i<instance.size;i++)
	        {
 		        // initialize config
		        pNewConfig.depth = 0;
		        pNewConfig.len = 0;
		
		        // if use exclusion method, startingpoints are meaningful
                if (startingpoint_flag == 1)
                {
                    


                    if (!Convert.ToBoolean(instance.arrStartingPoint[i])) continue;


                }

   	            // simply search from the start
                if (startingpoint_flag == 0) 
                {
                    pPair = instance.arrPair[i];
                    len = (pPair.uplen < pPair.downlen) ? 
				            pPair.uplen : pPair.downlen;
                    
                    if (pPair.up.Substring(len) == pPair.down.Substring(len)) continue;
                }
                

                // find one starting point
		        pNewConfig.MatchPair(instance.arrPair[i]);        

                // check masks 
		        if (Convert.ToBoolean(pNewConfig.up))
			        if (Convert.ToBoolean(instance.upmask)) continue;
		        if (!Convert.ToBoolean(pNewConfig.up))
			        if (Convert.ToBoolean(instance.downmask)) continue;

                // add to the cache
                //succ = TryFind(hashTable, pNewConfig, &pCacheConfig);

		        // try to solve
		        solveret=SolveConfig(instance, pNewConfig, maxlen);

		        if (solveret>0) // solve it
			        return solveret; 
		        if (solveret==0) // don't solve it to the iterative_depth_threshold
                    ret = 0;
	        }

            return ret;
        }
        static public int Solution_Found(PCPInstance instance,Config config){
            int k;


            

	        // for test, may not be solution, so need not record length and count	
	        //if (globalStatus == FIND_MASK || globalStatus == EXCLUSION_METHOD)			
		    //    return pConfig->depth;

	        // just to find solution
	        if (true) 
	        {
		       // solution_length = pConfig->depth;
		        solution_count = 1;
		        return config.depth;
	        }


        }

        // Solve the instance with the given config
        // maxlen is the size of the maximum space allocated in pConfig
        // return value:
        //    -1: unsolvable
        //     0: unsolved till the depth threshold
        //   n>0: solved at depth of n
        static public int SolveConfig(PCPInstance instance,Config config, int maxLen){
            int i, j, k;
	
	        // store the matched pairs and the lengths of resulting configs
	        int[] arrRetValue = new int[17];
	        int[] arrMatchedPair = new int[17];
            int matchedPairNum = 0;  

	        // ret value
	        int matchret=0;
	        int solveret;
	        int ret = -1;
	        int succ;
            int newlen;

	        // store the location of the config in the cache
	        Config pCacheConfig;
            Config pNewConfig;

            for (i = 0; i < instance.size; i++)
            {
                matchret = config.TestMatchingPair(instance.arrPair[i]);

               
                if (matchret == 0) // find one solution
                {
                    config.depth++;
                    PCPSolver.arrSelection[config.depth] = instance.arrPair[i].ID;
                    succ = Solution_Found(instance, config);
                    if (succ > 0) { return config.depth; }
                    else ret = 0;
                    config.depth--;
                }
                else if (matchret > 0) // find one match
                {
                    //// having exceeded the threshold, prune it
                    //if (config.depth == iterative_depth_threshold - 1)
                    //    ret = 0;
                    //else  // record the pair
                    //{
                        arrRetValue[matchedPairNum] = matchret;
                        arrMatchedPair[matchedPairNum++] = i;
                    //}
                }
            }

            //// sort the matched pair array according to its matchret value	
            for (i = 0; i < matchedPairNum - 1; i++)
                for (j = i + 1; j < matchedPairNum; j++)
                {
                    if (arrRetValue[i] > arrRetValue[j])
                    {
                        k = arrRetValue[i];
                        arrRetValue[i] = arrRetValue[j];
                        arrRetValue[j] = k;

                        k = arrMatchedPair[i];
                        arrMatchedPair[i] = arrMatchedPair[j];
                        arrMatchedPair[j] = k;
                    }
                }

             // try all matched pairs
            for (i = 0; i < matchedPairNum; i++)
            {
                // the last matched pair and the config can be reused? Jump back!
                if (i + 1 == matchedPairNum && arrRetValue[i] <= maxLen)
                {

                    config.MatchPair(instance.arrPair[arrMatchedPair[i]]);

                }
            }

            return matchret;
        }
        // return value:
        //  1  -- solved, find solution
        //  -1 -- solved, no solution
        //  0  -- unsolved
        static public int SolvePCPInstance(PCPInstance instance, int iterative)
        {

            instance.CountOffset();

            int ret = 0;
            
            //PCPInstance ReversePCP;
            //// create the inverse instance
            //instance.CreateReversePCP(out ReversePCP);

            //Console.WriteLine("After reversal PCP Instance Is:");
            //instance.Print();



            //Mask.FindMask(instance, ReversePCP);

            //if (Convert.ToBoolean(instance.upmask) && Convert.ToBoolean(instance.downmask) ||
            //        Convert.ToBoolean(ReversePCP.upmask) && Convert.ToBoolean(ReversePCP.downmask))
            //    return -1;


            int solved_flag = 0;
            // try original direction
            int original_visit_nodenum;
            node_num = 0;
            ret = SearchSolution(instance);
            if (solution_count >= 1) solved_flag = 1;
            else if (ret == -1) solved_flag = -1;

            original_visit_nodenum = node_num;
            

            ////TODO IMPLEMENT HASH TABLE
            ////ClearHashTable();

            //// try reverse direction
            //int reverse_visit_nodenum;
            //node_num = 0;

            //ret = SearchSolution(ReversePCP);
            //if (solution_count >= 1) solved_flag = 1;
            //else if (ret == -1) solved_flag = -1;
            
            //reverse_visit_nodenum = node_num;
           

            ////TODO: CONVERT TO C# HASHMAP
            ////ClearHashTable();


            if (solved_flag == 1)
            {
                Console.WriteLine("SOLUTION FOUND OR FALSE POSITIVE:");
                Console.WriteLine(PCPSolver.arrSelection);
            }


            return 0;

        }

        public static int BeginSolveConfig(PCPInstance instance,Config config, int maxLen, int maxDepth)
        {
            iterative_depth_threshold = maxDepth;
            int ret = SolveConfig(instance, config, maxLen);
            ClearHashTable();	

            return ret;
        }

        public static void ClearHashTable()
        {

        }
    }
}
