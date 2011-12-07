using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindowsGame1
{
    class Config
    {
        public Config(int maxLen)
        {

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
