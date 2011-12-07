using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindowsGame1
{
    class Config
    {
        public int len;
	    public int depth;
	    public int num0;  // #0 in the config, minus for bottom config
	    public char up;         // top or bottom
	    public char status;	

        public Config(int maxLen)
        {

        }
        public void Add_visited_node_number()
        {
            PCPSolver.node_num++;
        }
        public int MatchPair(Pair pPair, ref int[] arrSelection)
        {
	        int len;
            int num0=this.num0;
	        throw new NotImplementedException();
	        // update the string in config 
	        if (this.up!=null) // up is longer
	        {
		        if (this.len >= pPair.downlen) // down string will be all matched
		        {
			        // cut those matched to down string
			       // this.ConfigMove(pPair.downlen);
			        // add the up string to the config
		           // this.ConfigAppend(pPair.up, pPair.uplen);
		        }
		        else  // all letters in the config are matched
		        {
			        len = pPair.downlen - this.len; // length of left unmatched dwnn string 
			       // if (pPair.uplen>len) // up is longer
				       // this.ConfigAssign(pPair.up+len, pPair.uplen-len, 1);
			        //else  // down is longer, should change the direction of config
				      //  this.ConfigAssign(pPair.down+pPair.uplen+this.len, len-pPair.uplen, 0);

		        }
	        }
	        else 
	        {
		        if (this.len >= pPair.uplen) // up string will be all matched
		        {
			        // cut those matched to down string
			       // this.ConfigMove(pPair.uplen);
			        // add the up string to the config
			        //this.ConfigAppend(pPair.down, pPair.downlen);
                }
		        else  // config will be all matched
		        {
			        len = pPair.uplen - this.len; // length of left unmatched dwnn string 
			        //if (pPair.downlen>len) // down is longer
				    //   // this.ConfigAssign(pPair.down+len, pPair.downlen-len, 0);
			        //else  // up is longer, should change the direction of config
				    //   // this.ConfigAssign(pPair.up+pPair.downlen+this.len, len-pPair.downlen, 1);
		        }
	        }
            this.num0 = num0+pPair.diff0;
	
	        // update depth, selection, and number of visited node
            this.depth++;
	        arrSelection[this.depth]=pPair.ID;
	        Add_visited_node_number();

            return 1;
        }


        /*
        public void ConfigAssign(String arr,int len, int up){
            int i, j;
            int round = len / STRINGTYPEBITSIZE;
            int pos = len % STRINGTYPEBITSIZE;
            int index = 0;

            this->len = len;
            this->up = up;
            num0 = 0;

            // fully fill the new strings
            for (i = 0; i < round; i++)
            {
                this->arr[round] = 0;
                for (j = 0; j < STRINGTYPEBITSIZE; j++)
                    this->arr[round] |= (arr[index + j] << j);
                index += STRINGTYPEBITSIZE;
            }

            // fill the last one
            if (pos > 0)
            {
                this->arr[round] = 0;
                for (j = 0; j < pos; j++)
                    this->arr[round] |= (arr[index + j] << j);
            }

            for (i = 0; i < len; i++)
                if (arr[i] == 0) num0++;

            if (!up) num0 = -num0;
        }
         * */
    }
}
