#include "comparison.h"


// Comparison Test over the Integer
// if lessThan = 1, lessThan circuit
// if lessThan = 0, greaterThan circuit
void comparisonTest(const EncryptedArray &ea, Ctxt &c1, Ctxt &c2, ZZX &const_1poly, ZZX &const_2poly, long bitLength, bool lessThan){
   
//    Ctxt Temp0(c1), Temp1(c2);

    Ctxt Temp1(c2);

    c1.multByConstant(const_1poly); // Temp0 = (X, 0, X_{-1}, ...)
    Temp1.multByConstant(const_2poly); // Temp1 = (0, Y, Y_{-1}, ...)
    
    if (lessThan==1)
    {
        c1.addConstant(const_1poly); // Temp0 = (X+1, 0, X_{-1}+1, ...)
        c1 += Temp1 ; // Temp0 = (X+1, Y, (X+Y)_{-1}+1, ...)
    }
    else
    { 
        Temp1.addConstant(const_2poly); // Temp0 = (0, Y+1, Y_{-1}+1, ...)
        c1 += Temp1 ; // Temp0 = (X, Y+1, (X+Y)_{-1}+1, ...)
    }

    long Iter = ceil(log2(bitLength+1)); //(10, 4), (20, 5), (30, 5)
    for (int i = Iter-1; i > -1 ; --i)
    {
        Ctxt Temp3(c1) ;     
        ea.shift(Temp3, -bitLength*(1<<i));
        c1.multiplyBy(Temp3);
    }

    Iter = ceil(log2(bitLength));
    ea.shift(c1, (1<<Iter)-bitLength);
    
    for (int i = Iter-1; i > -1; --i)
    {
        Ctxt Temp4(c1);
        ea.shift(Temp4, -(1<<i));
        c1 += Temp4;
    }

}