using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindowsGame1
{
    public class Filter
    {
        /// <summary>
        /// Identify the trivial case where some string has its downstring and upstring
        /// identical.
        /// </summary>
        /// <param name="?">Instance being checked.</param>
        /// <returns></returns>
        public static int Pass_Trivialcase_Filter(PCPInstance instance)
        {
	        int i;
	        Pair p;
	        for (i=0; i < instance.size; i++)
	        {
		        p = instance.arrPair[i];
		        if (p.downlen == p.uplen)
			        if (p.down == p.up)
				        return 0;
	        }
	        return 1;
        }

        /// <summary>
        /// Check whether there exists one pair whose one string is the prefix of the other.
        /// </summary>
        /// <param name="?">Instance being checked.</param>
        /// <returns></returns>
        public static int Pass_Prefix_Filter(PCPInstance instance)
        {
            int i;
            int len = 0;
	        Pair p;

	        for (i=0;i< instance.size; i++)
	        {
		        p = instance.arrPair[i];
		
		        if (p.downlen > p.uplen) 
			        len = p.uplen;
		        else if (p.downlen < p.uplen) 
			        len = p.downlen;
		
		        if (p.down.Substring(0, len) == p.up.Substring(0, len))
                    return 1;
	        }
	        return 0;
        }

        /// <summary>
        /// Check whether there exists one pair whose one string is the postfix of the other
        /// </summary>
        /// <param name="?">Instance being checked.</param>
        /// <returns></returns>
        public static int Pass_Postfix_Filter(PCPInstance instance)
        {
	        int i;
	        Pair p;
	        string str;
	        for (i=0;i<instance.size;i++)
	        {
		        p =  instance.arrPair[i];

		        if (p.downlen>p.uplen) 
		        {
			        str = p.down+(p.downlen - p.uplen);
			        if (!(str.Substring(0, p.uplen) == p.up.Substring(0, p.uplen)))
                        return 1;
		        }
		        else if (p.downlen<p.uplen)
		        {
			        str = p.up+(p.uplen-p.downlen);
                    if (!(str.Substring(0, p.downlen) == p.down.Substring(0, p.downlen)))
                        return 1;
		        }
	        }
	        return 0;
        }

        // Check whether there exists one pair with upstring longer 
        // than downstring and one pair with downstring longer than upstring
        public static int Pass_Length_Balance_Filter(PCPInstance instance)
        {
	        int i;
	        int upflag = 0;
	        int downflag = 0;
	        Pair p;

	        for (i=0;i<instance.size;i++)
	        {
		        p = instance.arrPair[i];
		        if (p.downlen>p.uplen)
			        downflag = 1;
		        else if (p.downlen<p.uplen) 
			        upflag = 1;
	        }

	        if ((downflag == 1) && (upflag == 1))
                return 1;
	        else return 0;
        }


        // Check whether there exists one pair whose upstring containing 
        // more 1's than downstring, and another pair whose downstring 
        // containing more 1's than upstring. The same situation for element 0
        public static int Pass_Element_Balance_Filter(PCPInstance instance)
        {
	        int i ,j;
	        int zero_upflag = 0;
	        int zero_downflag = 0;
	        int one_upflag = 0;
	        int one_downflag = 0;
	        int one_upcount;
	        int zero_upcount;
	        int one_downcount;
	        int zero_downcount;
	        Pair p;

	        for (i=0;i<instance.size;i++)
	        {
		        one_upcount = 0;
		        zero_upcount = 0;
		        one_downcount = 0;
		        zero_downcount = 0;
		        p = instance.arrPair[i];

		        for (j=0; j<p.uplen;j++)
		        {
			        if (p.up[j] == 0)
				        zero_upcount++;
			        else one_upcount++;
		        }

		        for (j=0; j<p.downlen;j++)
		        {
			        if (p.down[j] == 0)
				        zero_downcount++;
			        else one_downcount++;
		        }

		        if (zero_upcount>zero_downcount)
			        zero_upflag = 1;
		        else if (zero_upcount<zero_downcount)
			        zero_downflag = 1;

		        if (one_upcount>one_downcount)
			        one_upflag = 1;
		        else if (one_upcount<one_downcount)
			        one_downflag = 1;
	        }

	        if (zero_upflag == 1 && zero_downflag==0 || 
		        zero_upflag == 0 && zero_downflag==1 ) 
		        return 0; 

	        if (one_upflag == 1 && one_downflag==0 || 
		        one_upflag == 0 && one_downflag==1  ) 
		        return 0;

	        return 1;
        }


        // reture value:
        // -1: trivial case
        // -2: element balance violated  
        // 0: no solution
        // 1: pass all filters
        public static int PassAllFilters(PCPInstance instance)
        {
	        // has solution, but it is the trivial case
	        if (!(Pass_Trivialcase_Filter(instance) == 0)) 
	        {
		        Console.WriteLine("Trivial case: one pair contains the same strings\n"); 
		        return -1;
	        }

	        if (!(Pass_Prefix_Filter(instance) == 0))
	        {
		        Console.WriteLine("Cannot pass the prefix filter!\n");
		        return 0;
	        }

	        if (!(Pass_Postfix_Filter(instance) == 0))
	        {
		        Console.WriteLine("Cannot pass the postfix filter!\n");
		        return 0;
	        }

	        if (!(Pass_Length_Balance_Filter(instance) == 0))
	        {
		        Console.WriteLine("Cannot pass the length balance filter!\n");
		        return 0;
	        }

	        if (!(Pass_Element_Balance_Filter(instance) == 0))
	        {
		        Console.WriteLine("Cannot pass the element balance filter!\n");
		        Console.WriteLine("Some pair can not be used!\n");

		        if (instance.size<=2)
                    return 0;
                else return -2; //It may have solution. Need improvement
	        }

	        return 1;
        }


    }
}
