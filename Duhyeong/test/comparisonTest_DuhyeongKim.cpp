#include <iostream>
#include <NTL/ZZ.h>
#include <NTL/ZZX.h>
#include "FHEContext.h"
#include "EncryptedArray.h"
#include "comparison.h"
#include "tools.h"
#include "utilities.h"
#include "gen_message.h"

using namespace std;
using namespace NTL;

int main(){
    
    cout << "Homomorphic Integers Comparison Test Started...\n";
    
    srand(time(NULL));
    SetSeed(to_ZZ(time(NULL)));

    long p = 2;
    long r = 1;
    long security = 80;  
    long c = 3;
    long d = 0;
    long L = 9, bitLength = 10;     //모두 L = 9에서 정상적으로 돌아감 
//    long L = 9, bitLength = 20;
//    long L = 9, bitLength = 30;


    long m = FindM(security, L, c, p, d, 0, 0);     
    FHEcontext context(m, p, r);                  
    buildModChain(context, L);                  
    ZZX F = context.alMod.getFactorsOverZZ()[0];  

    cout << "Generating Keys... \n";
    FHESecKey secretKey(context);
    const FHEPubKey& publicKey = secretKey;
    secretKey.GenSecKey(security);

    cout << "Generating Frobenius and SIMD Matrices... \n";
    addFrbMatrices(secretKey);
    addSome1DMatrices(secretKey);
    const EncryptedArray ea(context, F);

    // lessThan (0): evaluating greater than circuit
    // lessThan (1): evaluating less than circuit
    bool lessThan = 1; 
    string str;
    if (lessThan == 1){
        str = "Less Than";
    }
    else{
        str = "Greater Than";
    }

    /*
        Message Generation
    */
    ZZ Msg1, Msg2;
    vector<long> BD_Msg1, BD_Msg2; 
    generateMessage(Msg1, Msg2, BD_Msg1, BD_Msg2, bitLength, ea);
    cout << "Msg1 : " << Msg1 << endl;
    cout << "Msg2 : " << Msg2 << endl;

    /*
        HElib Ciphertext Generation
    */
    Ctxt Ctxt1(publicKey), Ctxt2(publicKey), Ctxt_comparison(publicKey);  
    ea.encrypt(Ctxt1, publicKey, BD_Msg1);
    ea.encrypt(Ctxt2, publicKey, BD_Msg2);

    /*
        Comparison Test
    */
    TIMER start;
    TIMER end;
    start = TIC;

    comparisonTest(Ctxt_comparison, Ctxt1, Ctxt2, lessThan, ea, bitLength);
    
    end = TOC;


    cout << "Time per integers comparison (" << str << ") test: " << get_time_us(start, end, 1) / 1000000 << " sec" << std::endl;
    cout << endl;
    cout << "Comparison Result (Plain): ";
    if(lessThan){
        cout << (Msg1 < Msg2) << endl;
    }
    else {
        cout << (Msg1 > Msg2) << endl;
    }
    vector<long> Ptxt_comparison;
    ea.decrypt(Ctxt_comparison, secretKey, Ptxt_comparison);
    cout << "Comparison Result (Encrypted): " << Ptxt_comparison.at(0) << endl;
    cout << "Homomorphic Levels Left: " << Ctxt_comparison.findBaseLevel() << endl;
    cout << "Homomorphic Integers Comparison Test Terminated...\n";
    return 0;
}
