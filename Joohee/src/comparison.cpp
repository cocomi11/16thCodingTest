#include "comparison.h"
#include<iostream>
#include <vector>
#include<cstdlib>
#include "FHEContext.h"
#include "FHE.h"
#include "Ctxt.h"
#include "DoubleCRT.h"
#include "EncryptedArray.h"
#include <NTL/ZZ.h>
#include <NTL/vec_ZZ.h>

	using namespace std;

void generateMessage(long *Msg, vector<long> *p, long N_bits){
    
    int i;
    long *ptrMsg = Msg;

    if (N_bits = 10){
        long g = rand()%(1024);
    
        *ptrMsg = g;
    
        for (i = 0; i < 10; i++)
        {
            (*p)[i] = g & 1;
            g = g >> 1;
        }
    }else if (N_bits = 20){
        long g = rand()%(1024);
        long h = rand()%(1024);
        
        *ptrMsg = g * 1024 + h;
        
        for (i = 0; i < 10; i++)
        {
            (*p)[i] = g & 1;
            g = g >> 1;
        }
        for (i = 10; i < 20; i++)
        {
            (*p)[i] = h & 1;
            h = h >> 1;
        }
    }else if (N_bits = 30){
        long g = rand()%(1024);
        long h = rand()%(1024);
        long k = rand()%(1024);
        
        *ptrMsg = k * 1048576 + g * 1024 + h;
        
        for (i = 0; i < 10; i++)
        {
            (*p)[i] = g & 1;
            g = g >> 1;
        }
        for (i = 10; i < 20; i++)
        {
            (*p)[i] = h & 1;
            h = h >> 1;
        }
        for (i = 20; i < 30; i++)
        {
            (*p)[i] = k & 1;
            k = k >> 1;
        }
    }

    
}

// Comparison Test over the Integer
// if lessThan = 1, lessThan circuit
// if lessThan = 0, greaterThan circuit
Ctxt comparisonTest(FHEcontext& context, const EncryptedArray& ea, Ctxt& c1, const Ctxt& c2, bool lessThan){
    
//    FHEcontext& context = (FHEcontext&) c1.getContext();
//    ea.getContext().alMod.restoreContext();
    
    int i,j;
    int nslots = ea.size();
    long d = ea.getDegree();
    Ctxt tmp1 = c1;
    Ctxt tmp2 = c2;

    cout << "Hello" << endl;
    vector< vector<long> > const1(10, vector<long>(nslots,0));
    
    vector<ZZX> const1_poly(10);
    
    for(i = 0; i < 10; i++){
        for(j = 0; j < i + 1; j++){
            const1[i][10 - j - 1] = 1;
        }
        
    }
    
    for(i = 0; i < 10; i++){
        ea.encode(const1_poly[i], const1[i]);
    }
    
    tmp1.addConstant(const1_poly[9]); // 1+xi;
    
    
    tmp2.addCtxt(tmp1); // 1+xi+yi;
    
    
    ea.shift(tmp2, -1); // shift 1+xi+yi;
    
    tmp1.multiplyBy(c2); // (1+xi)yi
    
    tmp2.addConstant(const1_poly[0]);
    
    Ctxt tmp3(tmp2);
    ea.shift(tmp3, -1);
    
    
    tmp3.addConstant(const1_poly[0]);
    
    tmp2.multiplyBy(tmp3);
    
    Ctxt tmp4(tmp3);
    ea.shift(tmp4, -1);
    
    
    
    tmp4.addConstant(const1_poly[0]);
    
    Ctxt tmp5(tmp4);
    ea.shift(tmp5, -1);
    
    
    
    tmp5.addConstant(const1_poly[0]);
    
    tmp4.multiplyBy(tmp5);
    
    
    tmp2.multiplyBy(tmp4);
    
    
    Ctxt tmp6(tmp5);
    ea.shift(tmp6, -1);
    tmp6.addConstant(const1_poly[0]);
    
    Ctxt tmp7(tmp6);
    ea.shift(tmp7, -1);
    tmp7.addConstant(const1_poly[0]);
    
    tmp6.multiplyBy(tmp7);
    
    Ctxt tmp8(tmp7);
    ea.shift(tmp8, -1);
    tmp8.addConstant(const1_poly[0]);
    
    Ctxt tmp9(tmp8);
    ea.shift(tmp9, -1);
    tmp9.addConstant(const1_poly[0]);
    
    tmp8.multiplyBy(tmp9);
    tmp6.multiplyBy(tmp8);
    tmp2.multiplyBy(tmp6);
    
    Ctxt tmp10(tmp9);
    ea.shift(tmp10,-1);
    tmp10.addConstant(const1_poly[0]);
    
    tmp2.multiplyBy(tmp10);
    
    
    
    
    /*        for (i = 1; i < 9; i++){
     ea.shift(tmp3, -1);
     tmp2.multiplyBy(tmp3);
     }*/
    
    
    tmp1.multiplyBy(tmp2);
    
    
    
    
    tmp3 = tmp1;
    ea.shift(tmp3, -9);
    
    
    for(i = 0; i < 9; i++){
        
        tmp2 = tmp1;
        ea.shift(tmp2, nslots - i - 1);
        ea.shift(tmp2, - nslots + 1);
        tmp3.addCtxt(tmp2);
    }
    
}
