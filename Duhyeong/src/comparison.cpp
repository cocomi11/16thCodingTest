// Comparison Test over the Integer
// if lessThan = 1, lessThan circuit
// if lessThan = 0, greaterThan circuit

#include "comparison.h"

using namespace std;
using namespace NTL;


void comparisonTest(Ctxt& Ctxt_comparison, const Ctxt& Ctxt1, const Ctxt& Ctxt2, const bool lessThan, const EncryptedArray& ea, const long bitLength){
//The explanation of the algorithm is specified in case of bitLength = 10
//It works for any bitLength with just a small modification on the last homomorphic multiplication part.

////////Homomorphic Comparision Algorithm at a high level////////

//Default setting : Ctxt1 = Enc(x_9,x_8,...,x_0,0,...,0), Ctxt2 = Enc(y_9,y_8,...,y_0,0,...,0)
//In case of lessthan test, we make following ciphertexts:
//Ctxt_add = Ctxt1 + Ctxt2 + Enc(1,1,....1) = Enc(x_9+y_9+1,x_8+y_8+1,...,x_0+y_0+1,1,1,...,1)
//Ctxt_mult = (Ctxt1 + Enc(1,1,...,1)) * Ctxt2 = Enc((x_9+1)y_9,(x_8+1)y_8,...,(x_0+1)y_0,1,1,,,,1)
//We compute Ctxt_comparison = Ctxt_mult * rotate(Ctxt_add,1) * rotate(Ctxt_add,2) * ... * rotate(Ctxt_add,9)
//For the homomorphic multiplication, we can make a tree graph to compute efficiently. (This is the only part depends on the value of bitLength)
//The first 10 slots of the plaintext corresponds to Ctxt_comparison are exactly (x_i+1)y_i * Prod_{j > i}(x_j+y_j+1).
//So when summing these slots, we get the comparison equation described in [CKK15]
//To get this, we finally compute and output rotate_{i = 0}^{9}(Sum Ctxt_comparison, -i)


//============================================================"Summarization in a table"=========================================================================
//===============================================================================================================================================================																															
//1st slot: (x_9+1)y_9				1					1					1 		........		1						  (x_9+1)y_9
//2nd slot:	(x_8+1)y_8			x_9+y_9+1				1					1		........		1       	 		 (x_8+1)y_8(x_9+y_9+1)
//3rd slot:	(x_7+1)y_7			x_8+y_8+1			x_9+y_9+1				1		........		1			     (x_7+1)y_7(x_8+y_8+1)(x_9+y_9+1)
// 			   .                    .					.					.		........		.							   .
// 			   .                    .					.					.		........		.							   .
// 			   .                    .					.					.		........		.							   .																				
// 			   .                    .					.					.		........		.							   .																					
// 			   .                    .					.					.		........		.							   .																					
//9th slot:	(x_0+1)y_0			x_1+y_1+1			x_2+y_2+1			x_3+y_3+1   ........	x_9+y_9+1   	   (x_0+1)y_0 * Prod_{j > 0}(x_j+y_j+1)
//===============================================================================================================================================================
//	Ctxt  :	 Ctxt_mult      rotate(Ctxt_add,1)  rotate(Ctxt_add,2)  rotate(Ctxt_add,3) ..... rotate(Ctxt_add,9) 	  		Ctxt_comparison
//===============================================================================================================================================================	
//  Ctxt_tmp[0] * Ctxt_tmp[1]	 Ctxt_tmp[2]		Ctxt_tmp[3]			Ctxt_tmp[4]	   .....  	Ctxt_tmp[10]				Ctxt_comparison
//===============================================================================================================================================================	

	vector<long> one(ea.size(), 1);
	ZZX poly_one;
	ea.encode(poly_one, one);
    vector<Ctxt> Ctxt_tmp;
    if(lessThan){
    	Ctxt_tmp.push_back(Ctxt1);
    	Ctxt_tmp.push_back(Ctxt2);
	}
	else{
    	Ctxt_tmp.push_back(Ctxt2);
    	Ctxt_tmp.push_back(Ctxt1);
	}
    Ctxt_tmp[0].addConstant(poly_one);
    Ctxt Ctxt_add(Ctxt_tmp[0]);
    Ctxt_add += Ctxt_tmp[1];
    for(int i = 0; i < bitLength - 1; i++){
        Ctxt_tmp.push_back(Ctxt_add);
        ea.rotate(Ctxt_tmp[i + 2], i + 1);
    }
//Now, we multiply from Ctxt_tmp[0] to Ctxt_tmp[bitLength]    
//1. Optimization for bitLength = 10
//A bit faster in average than when using the General algorithm below
    if(bitLength == 10){      
    Ctxt_tmp[0].multiplyBy2(Ctxt_tmp[1], Ctxt_tmp[2]);
    Ctxt_tmp[3].multiplyBy2(Ctxt_tmp[4], Ctxt_tmp[5]);
    Ctxt_tmp[6].multiplyBy2(Ctxt_tmp[7], Ctxt_tmp[8]);
    Ctxt_tmp[6].multiplyBy2(Ctxt_tmp[9], Ctxt_tmp[10]);
    Ctxt_tmp[0].multiplyBy2(Ctxt_tmp[3], Ctxt_tmp[6]);
    }
//2. General algorithm : making pairs recursively   
//Of course, the following algorithm also works for bitLength = 10      
    else{
        int i = 1, k = 1;
        while(i <= bitLength){
            for(int j = 0; j <= ((bitLength - i) >> k); j++){
                Ctxt_tmp[2 * i * j].multiplyBy(Ctxt_tmp[2 * i * j + i]);
            }
            i *= 2;
            k += 1;
        }
    }

    Ctxt_comparison = Ctxt_tmp[0];
    for(int i = 1; i < bitLength; i++){
        ea.rotate(Ctxt_tmp[0], -1);
        Ctxt_comparison += Ctxt_tmp[0]; 
    }
}