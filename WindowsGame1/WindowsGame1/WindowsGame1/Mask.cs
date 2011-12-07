using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindowsGame1
{
    static class Mask
    {
        static public int FindSideMask(PCPInstance instance, PCPInstance reverseInstance, int maskflag)
            {
	        int i, j, k, m, n, t;
	        Pair pair;
	        int ret;
	        int maxlen;
	        String longstr, shortstr;
            int pIntMask;
            int up = maskflag%2;

            switch (maskflag)
            {
                case 0: pIntMask = instance.downmask; break;
                case 1: pIntMask = instance.upmask; break;
                case 2: pIntMask = instance.turnover_downmask; break;
                case 3: pIntMask = instance.turnover_upmask; break;
                //default: assert(0);
                default:
                    break;
            }

	        for (i=0;i<instance.size;i++)
	        {
		        pair = instance.arrPair[i];

		        if (up==1)  // up
		        { 
			        n = pair.downlen;
			        t = pair.uplen;
			        longstr = pair.down;
			        shortstr = pair.up;
		        }
		        else // down
		        {
			        t = pair.downlen;
			        n = pair.uplen;
			        shortstr = pair.down;
			        longstr = pair.up;
		        }

		        if (n > t)
		        {
			        for (k=0; k<n-t;k++)
			        {
				        j = n-1-k;
				        m = j-t+1;

                        // they must be the multiple of gcd
                        if (m%instance.gcd!=0 || (n-1-j)%instance.gcd!=0)
                            continue;

				        
                        //TODO: PORT THIS LINE
                        // the string must be matched
				        //if (memcmp(shortstr, longstr+m, t)) continue;
				        if (shortstr == longstr.Substring(m,longstr.Length-m)) continue;
				        // the tail string in the bottom should possibly lead to the solution
				        if (j<n-1)
				        {
					        maxlen = n-j-1+10*instance.offset;
					        //Config config = new Config(maxlen);
					        //config.ConfigAssign(longstr+j+1, n-j-1, !up);
					        //config.depth = 1;
					        
                            //TODO: DECLARE STATIC FUNCT IN SOLVER
                            ret = PCPSolver.BeginSolveConfig(instance, maxlen, 100);
					        
                            if (ret == -1) continue;
				        }
                        else if (maskflag>=2) // find turnover
                            continue;

				        // the head string should be generated, so we use the reverse PCP to parse it
				        maxlen = m + 10*instance.offset;
				        Config config2 = new Config(maxlen);
				        int index;
				        
                        
                        //char str = new char[m];

                        //for (index=0;index<m;index++)
                        //    str[index] = longstr[m-index-1];

                        //config2.ConfigAssign(str, m, !up);
				        
                        //config2.depth = 1;

				        ret = PCPSolver.BeginSolveConfig(reverseInstance, maxlen, 100);
				        
                        if (ret == -1) continue;
				        else // no mask!
				        {
                            pIntMask = 0;
					        return 0;
				        }
			        }
		        }
	        }

            pIntMask = 1;

	        return 1;
        }

        static public int FindMask(PCPInstance instance, PCPInstance reverseInstance)
        {
            FindSideMask(instance, reverseInstance, 1);
            FindSideMask(instance, reverseInstance, 0);
            FindSideMask(reverseInstance, instance, 1);
            FindSideMask(reverseInstance, instance, 0);

            FindSideMask(instance, reverseInstance, 1);
            FindSideMask(instance, reverseInstance, 0);
            FindSideMask(reverseInstance, instance, 1);
            FindSideMask(reverseInstance, instance, 0);

            if (Convert.ToBoolean(instance.upmask)) reverseInstance.downmask = 1;
            if (Convert.ToBoolean(instance.downmask)) reverseInstance.upmask = 1;
            if (Convert.ToBoolean(reverseInstance.upmask)) instance.downmask = 1;
            if (Convert.ToBoolean(reverseInstance.downmask)) instance.upmask = 1;

            return 1;
        }

    }
}
