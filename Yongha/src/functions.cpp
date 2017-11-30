#include "functions.h"


/************************************************************************************************************

    Encode Msg1 & Msg 2 : 
    BitDecompose Msg1 & Msg 2 into bitstrings of length 'bitLength'

************************************************************************************************************* */ 

void BitDecomp(const long n, const long bitLength, vector<long>& BD){
    long temp = n;
    BD.resize(bitLength);
    for(int i=1; i<=bitLength; i++){
        if(temp % 2 == 1) BD[bitLength-i] = 1;
        else BD[bitLength-i] = 0;
        temp /= 2;
    }
}


void messageGen(const long Msg1, const long Msg2, const long bitLength, const long nslots, const EncryptedArray ea, 
                vector<long>& m1, vector<long>& m2){
    
    BitDecomp(Msg1, bitLength, m1);
    BitDecomp(Msg2, bitLength, m2);
    m1.resize(nslots);
    m2.resize(nslots);
}
