#include <iostream>

#include "comparison.h"
#include "FHEContext.h"
#include "FHE.h"
#include "EncryptedArray.h"
#include "Ctxt.h"
#include "utilities.h"

int main(){
    
    std::cout << "Homomorphic Integers Comparison Test Started...\n";
    
    srand(time(NULL));
    SetSeed(to_ZZ(time(NULL)));

    long p = 2;
    long r = 1;
    long security = 80;
    long L = 7;
    long c = 3;
    long d = 0;
    long bitLength = 10;
    long s = 160;

    /*
    Parameter Setting (s = bitLength * (1 << ceil(log2(bitLength+1))))
        bitLength        L        s
        10               7        160
        20               9        640
        30               17       960
    %L 은 exprimental choice.
    */

    long m = FindM(security, L, c, p, d, s, 0);
    
    FHEcontext context(m, p, r);
    buildModChain(context, L);
    ZZX F = context.alMod.getFactorsOverZZ()[0];

    std::cout << "Generating Keys... \n";
    FHESecKey secretKey(context);
    const FHEPubKey& publicKey = secretKey;
    secretKey.GenSecKey(security);

    std::cout << "Generating Frobenius and SIMD Matrices... \n";
    addFrbMatrices(secretKey);
    addSome1DMatrices(secretKey);

    const EncryptedArray ea(context, F); 

    /*
        Message Generate
    */
    ZZ Msg1 = RandomBits_ZZ(bitLength) , Msg2 = RandomBits_ZZ(bitLength) ;
    
    std::cout << "Msg1 : " << Msg1 << std::endl;
    std::cout << "Msg2 : " << Msg2 << std::endl;

    // lessThan (0): evaluating greater than circuit
    // lessThan (1): evaluating less than circuit
    bool lessThan = 1; 
    string str;

    std::cout << "Comparison" << std::endl;
    std::cout << "lessThan(1), greaterThan(0) : ";
    std::cin >> lessThan;

    if (lessThan == 1){
        str = "Less Than";
    }
    else{
        str = "Greater Than";
    }


    /*
        Plaintext Setting
    */
    
    long nslots = ea.size();

    std::cout << "nslots = " << nslots << "\n";

    vector<long> p1(nslots, 0); 
    vector<long> p2(nslots, 0);

    long Tempbit1 = bit(Msg1, 0), Tempbit2 = bit(Msg2, 0);
    p1[0] = Tempbit1;
    p1[bitLength] = Tempbit1;
    p2[0] = Tempbit2;
    p2[bitLength] = Tempbit2;

    for (int i = 1; i < bitLength; ++i)
    { 
        int Li = bitLength + i;
        Tempbit1 = bit(Msg1, i), Tempbit2 = bit(Msg2, i);
        p1[i] = Tempbit1, p1[Li] = Tempbit1;
        p2[i] = Tempbit2, p2[Li] = Tempbit2;
        int I = i + 2;
        for (int j = 2; j < I; ++j)
        {
            int J = bitLength*j + i+1-j;
            p1[J] = Tempbit1;
            p2[J] = Tempbit2;
        }
    }
    // Plaintext of X = (X, X, X_{-1}, X{-2}, ... , X_{-bitLength+1}, 0, ..., 0)



    /*
        Encryption
    */
    
    Ctxt c1(publicKey), c2(publicKey);

    ea.encrypt(c1, publicKey, p1); 
    ea.encrypt(c2, publicKey, p2);
// c1 = (X, X, X_{-1}, X_{-2}, ... , X_{-bitLength+1}, 0 , ..., 0)
// c2 = (Y, Y, Y_{-1}, Y_{-2}, ... , Y_{-bitLength+1}, 0 , ..., 0)

    ZZX const_1poly, const_2poly;
    std::vector<long> con1(nslots, 1), con2(nslots, 1);
    for (int i = 0; i < bitLength; ++i)
    {
        con1[bitLength+i] = 0 ;
        con2[i] = 0; 
    }

    ea.encode(const_1poly, con1); 
    ea.encode(const_2poly, con2);

//const_1poly = (1, 0, 1, 1, ... , 1)
//const_2poly = (0, 1, 1, 1, ... , 1)

    /*
        Comparison Test
    */

    TIMER start;
    TIMER end;
    start = TIC;

    comparisonTest(ea, c1, c2, const_1poly, const_2poly, bitLength, lessThan);

    end = TOC;
    cout << "Time per integers comparison (" << str << ") test: " << get_time_us(start, end, 1) * 1000 << " sec" << std::endl;


    /*
        Decryption
    */
    

    Ctxt CMP(c1);
    vector<long> Res(nslots);
    ea.decrypt(CMP, secretKey, Res);



    cout << endl;
    cout << "Comparison Result (Plain): ";
    if(lessThan){
        cout << (Msg1 < Msg2) << endl;
    }
    else {
        cout << (Msg1 > Msg2) << endl;
    }
    cout << "Comparison Result (Encrypted): " << Res[0] << endl;
    cout << "Homomorphic Levels Left: " << CMP.findBaseLevel() << endl;

    cout << "Homomorphic Integers Comparison Test Terminated...\n";

    return 0;
}
