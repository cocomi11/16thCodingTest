#include "comparison.h"
#include "utilities.h"
#include <NTL/ZZ.h>
#include <NTL/vector.h>
#include <NTL/tools.h>
#include "FHE.h"
#include "EncryptedArray.h"


#include <iostream>

using namespace std;
using namespace NTL;

int main(){
	
	cout << "Homomorphic Integers Comparison Test Started...\n";
	
    srand(time(NULL));
    SetSeed(to_ZZ(time(NULL)));

    long p = 2;
    long r = 1;
    long security = 80;
    long L = 7;
    long c = 3;
    long d = 0;
    long bitLength = 30;

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

    /*
        Message Generate
    */
    ZZ Msg1, Msg2;
    
    RandomLen(Msg1, bitLength);
    RandomLen(Msg2, bitLength);
    
	cout << "Msg1 : " << Msg1 << endl;
	cout << "Msg2 : " << Msg2 << endl;
    
    
    ZZ temp1 = Msg1;
    ZZ temp2 = Msg2;
    
    vector<long> v1;                            // generate v1, &  decimal to binary
    for(int i = 0 ; i < ea.size(); i++) {
        v1.push_back(temp1%2);
        temp1 = temp1/2;
    }
    Ctxt ct1(publicKey);
    ea.encrypt(ct1, publicKey, v1);             // encrypt v1 to c1
    
    vector<long> v2;                            // generate v2, &  decimal to binary
    for(int i = 0 ; i < ea.size(); i++) {
        v2.push_back(temp2%2);
        temp2 = temp2/2;
    }
    Ctxt ct2(publicKey);
    ea.encrypt(ct2, publicKey, v2);             // encrypt v2 to c2
    
    vector<long> identity(ea.size(), 1);        // generate 1 vector
    Ctxt one(publicKey);
    ea.encrypt(one, publicKey, identity);       // encrypt 1 vector to one
    
    
	// lessThan (0): evaluating greater than circuit
	// lessThan (1): evaluating less than circuit
    
    bool lessThan = 1;
    cout<<"Choose between lessThan(1), GreaterThan(0)!"<<endl;
    cin>>lessThan;
    
	string str;
	if (lessThan == 1){
		str = "Less Than";
    }
	else{
		str = "Greater Than";
    }
    
    
	/*
        Comparison Test
    */
	TIMER start;
	TIMER end;
	start = TIC;

    
    int answer = comparisonTest(lessThan, ct1, ct2, one, bitLength, secretKey, ea, &L);
    
    
    end = TOC;
    
    
	cout << "Time per integers comparison (" << str << ") test: " << get_time_us(start, end, 1) * 0.000001 << " sec" << std::endl;
    cout << endl;
    cout << "Comparison Result (Plain): ";
    if(lessThan){
        cout << (Msg1 < Msg2) << endl;
    }
    else {
        cout << (Msg1 > Msg2) << endl;
    }
    cout << "Comparison Result (Encrypted): "  << answer << endl;
    cout << "Homomorphic Levels Left: " << L << endl;

	cout << "Homomorphic Integers Comparison Test Terminated...\n";
    return 0;
}
