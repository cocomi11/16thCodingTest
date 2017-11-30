#include "comparison.h"

// Comparison Test over the Integer
// if lessThan = 1, lessThan circuit
// if lessThan = 0, greaterThan circuit
int comparisonTest(bool lessthan, Ctxt ct1, Ctxt ct2, Ctxt one, long bitlength, FHESecKey secretKey, EncryptedArray ea, long* level){

    vector<long> show;
    long sum = 0;

    if (lessthan == 1){
        
        
        ct1 += one;
        Ctxt res = ct1;
        res += ct2;
        Ctxt res2 = res;                            // preparation for comparison
        
        
        for (long i = 1; i < bitlength; i*=2){      // LT 함수의 xi + 1, yi, xi + yi + 1 등을 각각 하나의 벡터로 만든 후,
            ea.rotate(res2, ea.size() - i);         // xi + yi +1 벡터를 rotate 하면서 곱해줌으로써 LT 함수에 필요한 곱셈을
            res.multiplyBy(res2);                   // log(bitlength) + 2 번 만에 가능하도록 하였습니다.
            *level -= 1;
            res2 = res;
        }
        
        ea.shift(ct1, 1);
        ea.shift(ct2, 1);
        
        ct1.multiplyBy(ct2);
        *level -= 1;
        res.multiplyBy(ct1);
        *level -= 1;
        
        
        ea.decrypt(res, secretKey, show);

        for (int i = 0; i < ea.size(); i++)     // 이상하게 totalsums 함수가 이상한 값이 나와서 결과값을 직접 decrypt한 후
            sum+=show[i];                       // 각 element를 더해줌으로써 LT 함수의 덧셈 계산을 하였습니다.
        
        
        /*
        totalSums(ea, res);                   totalsums 작동을 안함...ㅠㅠ
        
        
        ZZX result;
        
        secretKey.Decrypt(result, res);
        */
        
        return sum%2;
    }
    
    else{                                           // GT 함수도 LT 함수와 거의 같습니다.
        
        
        ct2 += one;
        Ctxt res = ct2;
        res += ct1;
        Ctxt res2 = res;                            // preparation for comparison
        
        
        for (long i = 1; i < bitlength; i*=2){
            ea.rotate(res2, ea.size() - i);
            res.multiplyBy(res2);
            *level -= 1;
            res2 = res;
        }
        
        ea.shift(ct1, 1);
        ea.shift(ct2, 1);
        ct1.multiplyBy(ct2);
        *level -= 1;
        res.multiplyBy(ct1);
        *level -= 1;
        
        
        ea.decrypt(res, secretKey, show);
        
        for (int i = 0; i < ea.size(); i++)
            sum+=show[i];
        
        /*
        totalSums(ea, res);
        
        ZZX result;
        
        secretKey.Decrypt(result, res);
        */
        
        return sum%2;
    }
}
