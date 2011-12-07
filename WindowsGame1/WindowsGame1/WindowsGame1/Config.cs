using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindowsGame1
{
    class Config
    {
        String arr; 
        public int len;
	    public int depth;
	    public int num0;  // #0 in the config, minus for bottom config
	    public int up;         // up=1 down=0
	    public char status;	

        public Config(int maxLen)
        {
            //arr = new int[maxLen];
            this.len = maxLen;
        }
        public void Add_visited_node_number()
        {
            PCPSolver.node_num++;
        }


        //move string left by offset
        public void ConfigMove(int offset)
        {
            this.arr = arr.Remove(0, offset);
            //throw new NotImplementedException();
            //String x = Convert.ToString(this.arr).Remove(0,offset);
            //if (x != null)
            //{
            //    this.arr = Convert.ToInt32(x);
            //}
            //else
            //{
            //    //crap
            //}

        }

        public void ConfigAppend(String arr, int len)
        {
            //throw new NotImplementedException();
            this.arr = this.arr + arr;


        }

        public int MatchPair(Pair pPair)
        {
	        int len;
            int num0=this.num0;
	        //throw new NotImplementedException();
	        // update the string in config 
	        if (Convert.ToBoolean(this.up)) // up is longer
	        {
		        if (this.len >= pPair.downlen) // down string will be all matched
		        {
			        // cut those matched to down string
			        this.ConfigMove(pPair.downlen);
			        // add the up string to the config
		            this.ConfigAppend(pPair.up, pPair.uplen);
		        }
		        else  // all letters in the config are matched
		        {
			        len = pPair.downlen - this.len; // length of left unmatched dwnn string 
			        if (pPair.uplen>len) // up is longer
				        this.ConfigAssign(pPair.up.Substring(len), pPair.uplen-len, 1);
			        else  // down is longer, should change the direction of config
				        this.ConfigAssign(pPair.down.Substring(pPair.uplen+this.len), len-pPair.uplen, 0);

		        }
	        }
	        else 
	        {
		        if (this.len >= pPair.uplen) // up string will be all matched
		        {
			        // cut those matched to down string
			        this.ConfigMove(pPair.uplen);
			        // add the up string to the config
			        this.ConfigAppend(pPair.down, pPair.downlen);
                }
		        else  // config will be all matched
		        {
			        len = pPair.uplen - this.len; // length of left unmatched dwnn string 
			        if (pPair.downlen>len) // down is longer
                        this.ConfigAssign(pPair.down.Substring(len),pPair.downlen-len,0);
			        else  // up is longer, should change the direction of config
				        this.ConfigAssign(pPair.up.Substring(len), len-pPair.downlen, 1);
		        }
	        }
            this.num0 = num0+pPair.diff0;
	
	        // update depth, selection, and number of visited node
            this.depth++;
	        PCPSolver.arrSelection[this.depth]=pPair.ID;
	        Add_visited_node_number();

            return 1;
        }
        //compare letters in cofig with string arr
        public int ConfigCmp(String arr,int length)
        {
            if (arr == this.arr)
            {


                return 0;
            }
            else
            {
                return 1;
            }

        }


        // check the configuration can be matched with the pair
        // output:
        //   -1: cannot be matched
        //    0:  solved
        //    n:  n number left in the configuration
        public int TestMatchingPair(Pair pair)
        {
            String str;
            int len;
            int configlength;

            if (Convert.ToBoolean(this.up)) // up is longer
            {
                if (this.len >= pair.downlen)
                {
                    if (Convert.ToBoolean(this.ConfigCmp(pair.down, pair.downlen)))
                        return -1;
                }
                else
                {
                    if (Convert.ToBoolean(this.ConfigCmp(pair.down, this.len)))
                        return -1;
                    str = pair.down.Substring(this.len);
                    len = pair.downlen - this.len;
                    if (len > pair.uplen) len = pair.uplen;
                    if (str==pair.up)
                        return -1;
                }
                configlength = this.len + pair.uplen - pair.downlen;
            }
            else
            {
                if (this.len >= pair.uplen)
                {
                    if (Convert.ToBoolean(this.ConfigCmp(pair.up, pair.uplen)))
                        return -1;
                }
                else
                {
                    if (Convert.ToBoolean(this.ConfigCmp(pair.up, this.len)))
                        return -1;
                    str = pair.up + this.len;
                    len = pair.uplen - this.len;
                    if (len > pair.downlen) len = pair.downlen;
                    if (str==pair.down)
                        return -1;
                }
                configlength = this.len + pair.downlen - pair.uplen;
            }
            return Math.Abs(configlength);


        }

        
        public void ConfigAssign(String arr,int len, int up){
            this.arr = arr;
            this.len = len;
            this.up = up;
        }
         
    }
}
