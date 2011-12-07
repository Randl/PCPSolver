using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindowsGame1
{
    static class PCPSolver
    {
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
        // return value:
        //  1  -- solved, find solution
        //  -1 -- solved, no solution
        //  0  -- unsolved
        static public int SolvePCPInstance(PCPInstance instance, int iterative)
        {
            PCPInstance ReversePCP;
            
            // create the inverse instance
            instance.CreateReversePCP(out ReversePCP);

            Mask.FindMask(instance, ReversePCP);
            //Convert.ToBoolean
            if (Convert.ToBoolean(instance.upmask) && Convert.ToBoolean(instance.downmask) ||
                    Convert.ToBoolean(ReversePCP.upmask) && Convert.ToBoolean(ReversePCP.downmask))
                return -1;

            /*
            ret = CExclusion::Exclusion_Method(instance, iterative);
            if (ret == 0) return 0;
            ret = CExclusion::Exclusion_Method(ReversePCP, iterative);
            if (ret == 0) return 0;
            */

            return 0;

        }

        public static int BeginSolveConfig(PCPInstance instance, int maxLen, int maxDepth)
        {
            return 0;
        }
    }
}
