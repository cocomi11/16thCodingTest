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

    // lessThan 입력 작성

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

//    ZZ TempMsg1 = Msg1, TempMsg2 = Msg2 ;

    vector<long> p1(nslots, 0); 
    vector<long> p2(nslots, 0);
/*
    for (int i = 0; i < bitLength; ++i)
    {        
        p1[i] = bit(Msg1, i);
        p2[i] = bit(Msg2, i);
    }
*/
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

//    std::cout << p1 <<std::endl;
//    std::cout << p2 <<std::endl;
    

/*
    Ctxt temp1(c1);

    temp1.addConstant(const_1poly);
    temp1 += c2;

    ea.shift(c1, -1);
    ea.shift(c2, -1);

    Ctxt temp2(c1);

    temp2.addConstant(const_1poly);
    temp2 += c2;

    temp1.multiplyBy(temp2);
*/


/*
    NewPlaintextArray pp1(ea);
    NewPlaintextArray pp2(ea);

    ea.decrypt(c1, secretKey, pp1);
    ea.decrypt(c2, secretKey, pp2);

    std::cout << pp1 <<std::endl;
    std::cout << pp2 <<std::endl;
*/


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
/*
    Ctxt Temp0(c1), Temp1(c2);

    Temp0.multByConstant(const_1poly); // Temp0 = (X, 0, X_{-1}, ...)
    Temp1.multByConstant(const_2poly); // Temp1 = (0, Y, Y_{-1}, ...)
    
    if (lessThan==1)
    {
        Temp0.addConstant(const_1poly); // Temp0 = (X+1, 0, X_{-1}+1, ...)
        Temp0 += Temp1 ; // Temp0 = (X+1, Y, (X+Y)_{-1}+1, ...)
    }
    else
    { 
        Temp1.addConstant(const_2poly); // Temp0 = (0, Y+1, Y_{-1}+1, ...)
        Temp0 += Temp1 ; // Temp0 = (X, Y+1, (X+Y)_{-1}+1, ...)
    }
*/
/*
    NewPlaintextArray pTemp0(ea);
    ea.decrypt(Temp0, secretKey, pTemp0);
    std::cout << pTemp0 <<std::endl;
*/


//    std::cout << "leftLevel of Temp0 = " << Temp0.findBaseLevel() << std::endl;
/*
    long Iter = ceil(log2(bitLength+1)); //(10, 4), (20, 5), (30, 5)
    for (int i = Iter-1; i > -1 ; --i)
    {
        Ctxt Temp3(Temp0) ;     
        ea.shift(Temp3, -bitLength*(1<<i));
        Temp0.multiplyBy(Temp3);

//        ea.decrypt(Temp0, secretKey, pTemp0);
//        std::cout << pTemp0 <<std::endl;
//        std::cout << "Iter = " << i << std::endl;
//        std::cout << "leftLevel of Temp0 = " << Temp0.findBaseLevel() << std::endl;
    }
*/
/*
    Iter = ceil(log2(bitLength));
    ea.shift(Temp0, (1<<Iter)-bitLength);
    
    for (int i = Iter-1; i > -1; --i)
    {
        Ctxt Temp4(Temp0);
        ea.shift(Temp4, -(1<<i));
        Temp0 += Temp4;
    }
*/
/*

    c1.addConstant(const_1poly); // c1 = (x + 1, 1, ... , 1)
                                 // c2 = (y, 0, ... , 0)

//    std::cout << "leftLevel of x+1 = " << c1.findBaseLevel() << std::endl;
//    std::cout << "leftLevel of y = " << c2.findBaseLevel() << std::endl;

    Ctxt Temp0(c1), Temp1(c1);
    Temp0.multiplyBy(c2); // Temp0 = (y*(x+1), 0, ... , 0)
    Temp1 += c2;          // Temp1 = (y+x+1, 1, ... , 1)

    Ctxt Temp2(Temp1); 
    Ctxt Temp3(Temp1);

//    std::cout << "leftLevel of Temp0 = " << Temp0.findBaseLevel() << std::endl;
//    std::cout << "leftLevel of Temp1 = " << Temp1.findBaseLevel() << std::endl;

    ea.shift(Temp1, -1); // count = 1
    Ctxt Prod12(Temp1);
    ea.shift(Temp1, -1); // count = 2
    Prod12.multiplyBy(Temp1); 

//    std::cout << "leftLevel of Prod12 = " << Prod12.findBaseLevel() << std::endl;
//    std::cout << "leftLevel of Temp1 = " << Temp1.findBaseLevel() << std::endl;

    ea.shift(Temp1, -1); // count = 3
    Ctxt Prod34(Temp1);
    ea.shift(Temp2, -4); // count = 4
    Prod34.multiplyBy(Temp2);

//    std::cout << "leftLevel of Temp1 = " << Temp1.findBaseLevel() << std::endl;
//    std::cout << "leftLevel of Temp2 = " << Temp2.findBaseLevel() << std::endl;

    ea.shift(Temp2, -1); // count = 5
    Ctxt Prod56(Temp2);
    ea.shift(Temp2, -1); // count = 6
    Prod56.multiplyBy(Temp2);

//    std::cout << "leftLevel of Temp2 = " << Temp2.findBaseLevel() << std::endl;

    ea.shift(Temp3, -7); // count = 7
    Ctxt Prod789(Temp3);
    ea.shift(Temp3, -1); // count = 8
    Prod789.multiplyBy(Temp3);
    ea.shift(Temp3, -1); // count = 9
    Prod789.multiplyBy(Temp3);

//    std::cout << "leftLevel of Prod789 = " << Prod789.findBaseLevel() << std::endl;
//    std::cout << "leftLevel of Temp3 = " << Temp3.findBaseLevel() << std::endl;

    Temp0.multiplyBy(Prod12);
    Prod34.multiplyBy(Prod56);

    Temp0.multiplyBy(Prod34);

    Temp0.multiplyBy(Prod789); // Temp0 = (*, 0, ..., 0);

    // LT = sum_0^n-1 of Temp0
*/

//    Ctxt CLT(Temp0);
/*
    for (int i = 1; i < bitLength; ++i)
    {
        ea.shift(Temp0, -1);
        CLT += Temp0;
    }
*/
    end = TOC;
    cout << "Time per integers comparison (" << str << ") test: " << get_time_us(start, end, 1) * 1000 << " sec" << std::endl;

   /*
        Decryption
    */
    

    Ctxt CMP(c1);
//    NewPlaintextArray LT(ea);
    vector<long> Res(nslots);

    ea.decrypt(CMP, secretKey, Res);
//    std::cout << Res <<std::endl;




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
