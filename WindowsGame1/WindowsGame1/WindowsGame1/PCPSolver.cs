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
            int startingpoint_flag = 1;
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
		            if (!Convert.ToBoolean(instance.arrStartingPoint[i])) continue;

   	            // simply search from the start
                if (startingpoint_flag == 0) 
                {
                    pPair = instance.arrPair[i];
                    len = (pPair.uplen < pPair.downlen) ? 
				            pPair.uplen : pPair.downlen;
                    
                    if (pPair.up.Substring(0,len) == pPair.down.Substring(0,len)) continue;
                }

                // find one starting point
		        pNewConfig.MatchPair(instance.arrPair[i], ref arrSelection);        

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


        // Solve the instance with the given config
        // maxlen is the size of the maximum space allocated in pConfig
        // return value:
        //    -1: unsolvable
        //     0: unsolved till the depth threshold
        //   n>0: solved at depth of n
        static public int SolveConfig(PCPInstance instance,Config config, int maxLen){


            return 0;
        }
        // return value:
        //  1  -- solved, find solution
        //  -1 -- solved, no solution
        //  0  -- unsolved
        static public int SolvePCPInstance(PCPInstance instance, int iterative)
        {
            int ret = 0;
            PCPInstance ReversePCP;
            
            // create the inverse instance
            instance.CreateReversePCP(out ReversePCP);

            Mask.FindMask(instance, ReversePCP);
            //Convert.ToBoolean
            if (Convert.ToBoolean(instance.upmask) && Convert.ToBoolean(instance.downmask) ||
                    Convert.ToBoolean(ReversePCP.upmask) && Convert.ToBoolean(ReversePCP.downmask))
                return -1;
            int solved_flag = 0;
            // try original direction
            int original_visit_nodenum;
            node_num = 0;
            ret = SearchSolution(instance);
            if (solution_count >= 1) solved_flag = 1;
            else if (ret == -1) solved_flag = -1;

            original_visit_nodenum = node_num;
            

            //TODO IMPLEMENT HASH TABLE
            //ClearHashTable();

            // try reverse direction
            int reverse_visit_nodenum;
            node_num = 0;

            ret = SearchSolution(ReversePCP);
            if (solution_count >= 1) solved_flag = 1;
            else if (ret == -1) solved_flag = -1;
            
            reverse_visit_nodenum = node_num;
           

            //TODO: CONVERT TO C# HASHMAP
            //ClearHashTable();



            return 0;

        }

        public static int BeginSolveConfig(PCPInstance instance, int maxLen, int maxDepth)
        {
            return 0;
        }
    }
}
