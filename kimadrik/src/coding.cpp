#include <EncryptedArray.h>
#include <Ctxt.h>
#include <FHE.h>
#include <FHEContext.h>
#include <NTL/ZZ.h>
#include <NTL/ZZX.h>
#include <PAlgebra.h>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include "utilities.h"

using namespace NTL;
using namespace std;

int main() {
	cout << "Homomorphic Integers Comparison Test Started...\n";

    srand(time(NULL));
    SetSeed(to_ZZ(time(NULL)));

    long bitLength = 10; // 10 20 30
	long logbits = (long)ceil(log2(bitLength));

    long p = 2;
    long r = 1;
    long security = 80;
    long L = 9;
    long c = 3;
    long d = 0;

    long m = FindM(security, L, c, p, d, 0, 0);

    FHEcontext context(m, p, r);
    buildModChain(context, L);
    ZZX F = context.alMod.getFactorsOverZZ()[0];

    cout << "Generating Keys... \n";
    FHESecKey secretKey(context);
    const FHEPubKey& publicKey = secretKey;
    secretKey.GenSecKey(security);

    cout << "Generating Frobenius and SIMD Matrices... \n";
    cout << endl;
    addFrbMatrices(secretKey);
    addSome1DMatrices(secretKey);
    EncryptedArray ea(context, F);

    long nslots = ea.size();
    cout << nslots << endl;
	ZZ Msg1 = RandomBits_ZZ(bitLength);
	ZZ Msg2 = RandomBits_ZZ(bitLength);

	cout << "Msg1 : " << Msg1 << endl;
	cout << "Msg2 : " << Msg2 << endl;

	cout << endl;


	vector<long> x(nslots, 0);
	vector<long> y(nslots, 0);

	for (long i = 0; i < bitLength; ++i) x[i] = bit(Msg1, bitLength - 1 - i);
	for (long i = 0; i < bitLength; ++i) y[i] = bit(Msg2, bitLength - 1 - i);

	Ctxt encX(publicKey);
	Ctxt encY(publicKey);

	ea.encrypt(encX, publicKey, x); // x
	ea.encrypt(encY, publicKey, y); // y


	// lessThan (0): evaluating greater than circuit
	// lessThan (1): evaluating less than circuit
	bool lessThan = false;
	string str = (lessThan == true) ? "Less Than" : "Greater Than";

	/*
		Comparison Test
	*/
	TIMER start;
	TIMER end;
	start = TIC;
	encX.addConstant(to_ZZ(1)); // x + 1
	Ctxt encD = encX;
	encD.addCtxt(encY); // x + y + 1
	encX.multiplyBy(encY); // (x + 1) * y

	//---------------------------------------------

	Ctxt encDshift = encD;

	for (long i = 0; i < logbits - 1; ++i) {
		long powi = (1 << i);
		ea.rotate(encDshift, powi);
		encD.multiplyBy(encDshift);
		encX.multiplyBy(encDshift);
		encDshift = encD;
	}
	ea.rotate(encDshift, (1 << (logbits - 1)));
	encX.multiplyBy(encDshift);

	Ctxt encXshift = encX;
	for (int i = 0; i < logbits - 1; ++i) {
		long powi = (1 << i);
		ea.rotate(encXshift, powi);
		encX.addCtxt(encXshift);
		encXshift = encX;
	}
	ea.rotate(encXshift, (1 << (logbits - 1)));
	encX.addCtxt(encXshift);

	if(!lessThan) encX.addConstant(to_ZZ(1));

	end = TOC;
	cout << "Time per integers comparison (" << str << ") test: " << get_time_us(start, end, 1) / 1000 / 1000 << " sec" << std::endl;
	cout << endl;
	vector<long> dx;
	ea.decrypt(encX, secretKey, dx);
	cout << "Comparison Result (encrypted): " << dx[bitLength - 1] << endl;
	cout << "Comparison Result (Plain): ";
	if(lessThan) cout << (Msg1 < Msg2) << endl;
	else cout << (Msg1 > Msg2) << endl;
	cout << endl;


	cout << "Homomorphic Integers Comparison Test Terminated...\n";
    return 0;
}
