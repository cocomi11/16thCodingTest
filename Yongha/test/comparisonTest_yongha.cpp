#include "comparison.h"
#include "utilities.h"
#include "functions.h"

using namespace std;

int main(){
    cout << "Homomorphic Integers Comparison Test Started...\n";
	
    srand(time(NULL));
    SetSeed(to_ZZ(time(NULL)));

    long p = 2;
    long r = 1;
    long security = 80;
    long L = 9;
    long c = 3;
    long d = 0;
    long bitLength = 30;

    /*  In fact, L >= 7 can supports all bitLengths = 10, 20, 30,
        But for every bitLength, L = 9 is the optimal choice 
		왜인지는 전혀 모르겠다. */

    long m = FindM(security, L, c, p, d, 0, 0);
    
    FHEcontext context(m, p, r);
    buildModChain(context, L);
    ZZX F = context.alMod.getFactorsOverZZ()[0];

    cout << "Generating Keys... \n" ;
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

    long nslots = ea.size();

    long iterate;

    cout << "Iterations: " ;
    cin >> iterate;

	for (int i = 0; i < iterate; i++) {
		long Msg1, Msg2;

		Msg1 = RandomBits_long(bitLength);
		Msg2 = RandomBits_long(bitLength);

		cout << "Msg1 : " << Msg1 << endl;
		cout << "Msg2 : " << Msg2 << endl;

		vector<long> m1;
		vector<long> m2;

		/* 최적화를 위한 노력: HElib 설명서를 보면 NewPlaintextArray class 를 만들도록 하는데,
			HElib의 헤더를 보니 long vector로도 충분히 코드가 작동함을 확인할 수 있었다.
			하지만 이 변화가 유의미한 속도개선을 주지는 않았다.
			(어차피 input을 long vector로 줘도 HElib 안에서 다시 encode하기 때문인 것 같다.) */

		messageGen(Msg1, Msg2, bitLength, nslots, ea, m1, m2);

		Ctxt c1(publicKey), c2(publicKey);
		ea.encrypt(c1, publicKey, m1);
		ea.encrypt(c2, publicKey, m2);

		// lessThan (0): evaluating greater than circuit
		// lessThan (1): evaluating less than circuit
		bool lessThan = 0;

		string str;
		if (lessThan == 1) {
			str = "Less Than";
		}
		else {
			str = "Greater Than";
		}

		/*
			Comparison Test
		*/

		Ctxt com(publicKey);

		TIMER start;
		TIMER end;
		start = TIC;

		comparisonTest(c1, c2, bitLength, ea, lessThan, com);

		end = TOC;

		cout << "Time per integers comparison (" << str << ") test: " << get_time_us(start, end, 1) / 1000000 << " sec" << endl;

		cout << "Comparison Result (Plain): ";
		if (lessThan) {
			cout << (Msg1 < Msg2) << endl;
		}
		else {
			cout << (Msg1 > Msg2) << endl;
		}

		vector<long> dec;
		ea.decrypt(com, secretKey, dec);

		bool compare = 0;
		for (int j = 0; j < bitLength; j++) {
			compare += dec[j];
		}

		cout << "Comparison Result (Encrypted): " << compare << endl;
		cout << "Homomorphic Levels Left: " << com.findBaseLevel() << endl;
		cout << endl;
	}
	cout << "Homomorphic Integers Comparison Test Terminated...\n";

    return 0;
}