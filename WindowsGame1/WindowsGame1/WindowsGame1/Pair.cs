using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindowsGame1
{
    public class Pair
    {
        public String up; // the last char is always SENTINEL
	    public String down;
	    public int uplen;
	    public int downlen;
        public int diff0, diff1;
	    public int ID;


        public void CountElementDiff()
        {
            int j;
            int one_upcount = 0, zero_upcount = 0, one_downcount = 0, zero_downcount = 0;

            for (j = 0; j < uplen; j++)
            {
                if (up[j] == 0)
                    zero_upcount++;
                else one_upcount++;
            }
            for (j = 0; j < downlen; j++)
            {
                if (down[j] == 0)
                    zero_downcount++;
                else one_downcount++;
            }
            diff1 = one_upcount - one_downcount;
            diff0 = zero_upcount - zero_downcount;
        }    

    }

    

}
