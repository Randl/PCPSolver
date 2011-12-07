using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindowsGame1
{
    public class PCPInstance
    {
        public int size;
        public int width;
        public int up_offset;
        public int down_offset;
        public int offset;
        public int downmask;
        public int upmask;
        public int turnover_downmask;
        public int turnover_upmask;
        public int gcd;

        public int[] arrStartingPoint = new int[17];

        public Pair[] arrPair = new Pair[17]; // the uplen of the last one is always 0 as the guard


        // max up & down strings (for lookahead heuristic)
        public int maxup;
        public int maxdown;

        public PCPInstance()
        {

        }
        public void Init()
        { 
            int i;
            for (i=0;i<16+1;i++)
            {
                arrPair[i].uplen=0;
                arrPair[i].downlen=0;
                arrPair[i].ID = i+1;
                arrStartingPoint[i] = 1;
            }
            //up_0drop = 0;
            //up_1drop = 0;
            //down_0drop = 0;
            //down_1drop = 0;
            size = 0;
            width = 0;
            offset = 0;
            up_offset = 0;
            down_offset = 0;
            downmask = 0;
            upmask = 0;
            turnover_downmask = 0;
            turnover_upmask = 0;
            gcd = 1;
	        //poi = 0;
        }



        public void CreateReversePCP(out PCPInstance reversedPCP)
        {

            //return newly created
            throw new NullReferenceException();
        }
    }
}
