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

         public int up_0drop = 0;
         public int   up_1drop = 0;
          public int  down_0drop = 0;
          public int  down_1drop = 0;

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
                if (arrPair[i] != null)
                {
                    arrPair[i].uplen = 0;
                    arrPair[i].downlen = 0;
                    arrPair[i].ID = i + 1;
                    arrStartingPoint[i] = 1;
                }
            }
            up_0drop = 0;
            up_1drop = 0;
            down_0drop = 0;
            down_1drop = 0;
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

        public void CountOffset()
        {
             int i, j;
            offset = 0;
            up_offset = 0;
            down_offset = 0;
            gcd = 1;
            up_0drop = 0;
            up_1drop = 0;
            down_0drop = 0;
            down_1drop = 0;

            int tmp;
            int[] arrOffset= new int[17];
            for (i=0;i<size;i++)
            {
                tmp = arrPair[i].uplen - arrPair[i].downlen;
                if (tmp>0 && up_offset<tmp)
                    up_offset = tmp;
                else if (tmp<0 && down_offset<-tmp)
                    down_offset = -tmp;
                arrOffset[i] = Math.Abs(tmp);
    
                arrPair[i].CountElementDiff();
                if (down_0drop<arrPair[i].diff0) down_0drop = arrPair[i].diff0;
                if (up_0drop<-arrPair[i].diff0) up_0drop = -arrPair[i].diff0;
                if (down_1drop<arrPair[i].diff1) down_1drop = arrPair[i].diff1;
                if (up_1drop<-arrPair[i].diff1) up_1drop = -arrPair[i].diff1;
            }
            offset = up_offset;
            if (offset<down_offset) offset = down_offset;

	        // count gcd
            int flag;
            for (j=offset;j>1;j--)
            {
                flag = 1;
                for (i=0;i<size;i++)
                    if (arrOffset[i]%j!=0) { flag=0; break;}
                if (Convert.ToBoolean(flag)) { gcd = j; break; }
            }


	        // count maxup and maxdown string
	        maxup = 0;
	        maxdown = 0;
            for (i=0;i<size;i++)
            {
		        if (arrPair[i].uplen>maxup) maxup = arrPair[i].uplen;
		        if (arrPair[i].downlen>maxdown) maxdown = arrPair[i].downlen;
	        }

        }

        public void CreateReversePCP(out PCPInstance reversedPCP)
        {
            reversedPCP = new PCPInstance();
            reversedPCP.Init();


            // copy member attributions
            reversedPCP.size = size;
            reversedPCP.width = width;
            reversedPCP.up_offset = up_offset;
            reversedPCP.down_offset = down_offset;
            reversedPCP.offset = offset;
            reversedPCP.gcd = gcd;
            
            int i = 0;
            foreach (Pair p in arrPair)
            {


                reversedPCP.arrPair[i] = p;

                Pair newPair = reversedPCP.arrPair[i];

                newPair.down = Reverse(newPair.down);
                newPair.up = Reverse(newPair.up);

                i++;
            }
            reversedPCP.CountOffset();
            //return newly created
            //throw new NullReferenceException();
        }

        public static string Reverse(string s)
        {
            char[] charArray = s.ToCharArray();
            Array.Reverse(charArray);
            return new string(charArray);
        }
    }
}
