#include "gen_message.h"
#include "tools.h"

void generateMessage(ZZ& Msg1, ZZ& Msg2, vector<long>& BD_Msg1, vector<long>& BD_Msg2, const long bitLength, const EncryptedArray ea){
//generate two random messages Msg1, Msg2, and their bit decompostions applying BitDecomp alg in tools.h
	Msg1 = RandomBits_ZZ(bitLength);
	Msg2 = RandomBits_ZZ(bitLength);
	BitDecomp(BD_Msg1, Msg1, bitLength);
    BitDecomp(BD_Msg2, Msg2, bitLength);
    BD_Msg1.resize(ea.size());
    BD_Msg2.resize(ea.size());
}