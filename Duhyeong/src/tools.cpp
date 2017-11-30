#include <algorithm>
#include "tools.h"

void BitDecomp(vector<long>& BD_Msg, const ZZ& Msg, const long bitLength){
//A function mapping from Msg of the type ZZ to its bit decomposition of the length bitLength
//e.g. 10 -> 0000001010
	ZZ Msg_tmp = Msg;
	long tmp = bitLength;
  	while(tmp != 0) {
	    if (Msg_tmp % 2 == 1)
	      BD_Msg.push_back(1);
	    else
	      BD_Msg.push_back(0);
	    Msg_tmp >>= 1;
	    tmp -= 1;  
  	}
 	reverse(BD_Msg.begin(), BD_Msg.end());
}
