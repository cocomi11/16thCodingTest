#include "comparison.h"

int main(){
	
	cout << "Homomorphic Integers Comparison Test Started...\n";
	
    srand(time(NULL));
    SetSeed(to_ZZ(time(NULL)));

    long p = 2;
    long r = 1;
    long security = 80;
    long L = 10;
    long c = 3;
    long d = 0;
    long bitLength = 20;

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
    
    generateMessage();
	
	cout << "Msg1 : " << Msg1 << endl;
	cout << "Msg2 : " << Msg2 << endl;

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
        Comparison Test
    */
	TIMER start;
	TIMER end;
	start = TIC;

    comparisonTest();
	
    end = TOC;
	cout << "Time per integers comparison (" << str << ") test: " << get_time_us(start, end, 1) * 1000 << " sec" << std::endl;


    cout << endl;
    cout << "Comparison Result (Plain): ";
    if(lessThan){
        cout << (Msg1 < Msg2) << endl;
    }
    else {
        cout << (Msg1 > Msg2) << endl;
    }
    cout << "Comparison Result (Encrypted): " <<  << endl;
    cout << "Homomorphic Levels Left: " <<  << endl;

	cout << "Homomorphic Integers Comparison Test Terminated...\n";
    return 0;
}