#include "comparison.h"

// Comparison Test over the Integer
// if lessThan = 1, lessThan circuit
// if lessThan = 0, greaterThan circuit

    /************************************************************************************************************

    Let Msg 1 is encoded to m1 = X = [ x_l | x_(l-1) | . . . | x_1 | 0 | . . . | 0 ]
        Msg 2 is encoded to m2 = Y = [ y_l | y_(l-1) | . . . | y_1 | 0 | . . . | 0 ].

    ************************************************************************************************************* */

void comparisonTest(const Ctxt c1, const Ctxt c2, const long bitLength,
                    const EncryptedArray ea, const bool lessThan, Ctxt& com){

    if(lessThan == 1){ // Less Than

    /************************************************************************************************************

    We will compute the comparison circuit from     D = (X + 1) * Y = [ d_(l-i) | 0s ]  and 
                                                    Z = (X + 1 + Y) = [ z_(l-i) | 0s ]

    * the comparison circuit : d_l + d_(l-1) * k_(l-1) + d_(l-2) * k_(l-2) + . . . d_1 * k_1
										where k_(l-i) = z_l * . . . * z_(l-i+1).

    Observe that Z is of the form [ z_l | z_(l-1) | z_(l-2) | . . . | z_2 | z_1 |  1  | . . . | 1 ],
    and its rotate will be        [  1  |   z_l   | z_(l-1) | . . . | z_3 | z_2 | z_1 | . . . | 1 ],
                                  [  1  |    1    |   z_l   | . . . | z_4 | z_3 | z_2 | . . . | 1 ], and so on.


    ************************************************************************************************************* */

        Ctxt D(c1);
        D.addConstant(to_ZZ(1));

        Ctxt Z(D);
        D.multiplyBy(c2);
        Z.addCtxt(c2); 

    /************************************************************************************************************

    The function "computeK" computes some rotates of Z and multiply them
    to convert Z into K = [ k_(l-1) | k_(l-2) | k_(l-3) | . . . | k_1 |   *   | . . . ].

    ************************************************************************************************************* */

        computeK(Z, bitLength, ea);

    /************************************************************************************************************

    Rotate K to obtain K = [   1   | k_(l-1)  | k_(l-2)  | . . . |  k_2 |  k_1  | . . . ],
    and the output would be product of K and D.
    
    * The sum of 1st ~ lth slot would give the comparison result, which is in the main cpp code.

    ************************************************************************************************************ */

        ea.rotate(Z, 1);

        D.multiplyBy(Z);

        com = D;   

	/******************************************************************************************************

	- 최적화를 위한 노력 1
	
	초기 코드는 D를 1칸 rotate하고 더하는 것을 bitLength회 반복해서 bitLength번째 slot 값이 그대로 comparison 값이 되게 했다.
	: com = D; for(i=1; i<bitLength; i++) ea.rotate(D,1); com += D;    . . . (*)
	그러나 (이유는 정확히 모르겠지만) rotate에 곱셈이 필요한 것으로 보였고, (rotate 횟수가 늘어나면 Depth가 커졌어야 함)
	이 상태에서 bitLength를 늘리면 rotate도 그만큼 늘어나 Depth도 커졌어야 했다. 즉, bitLength가 늘어나면 속도가 매우 느려졌다.

	그래서 ( * ) 부분을 수행하지 않고 D를 그대로 output하도록 바꾸고,
	main code에서 decryption한 이후 1 ~ bitLength 번째 slot의 비트들을 모두 더하도록 변경했다. (110 - 113 Lines)

	여기서 관찰할 것은 D를 그대로 output하는것과 rotate & sum 된 D를 output 한 것의 안전성 차이가 없다는 점이다.
	다시 말해 D의 메시지가 누출되면 rotate & sum 된 D 의 메시지도 모두 복구할 수 있고,
	반대로 rotate & sum 된 D 의 메시지가 누출되면 D의 메시지도 복구할 수 있다. ( D 의 뒷부분 slot들은 모두 1로 채워져있음을 알고 있기 때문 )
	따라서 위와 같은 방법에 안전성 문제는 없는 것으로 판단된다.

	************************************************************************************************************* */
    
	}

    else{ // Greater Than 

    /************************************************************************************************************

    Almost similar to the Less than case..

    ************************************************************************************************************* */

        Ctxt D(c2);
        D.addConstant(to_ZZ(1));

        Ctxt Z(D);
        D.multiplyBy(c1);
        Z.addCtxt(c1);

        computeK(Z, bitLength, ea);

        ea.rotate(Z, 1);

        D.multiplyBy(Z);

		com = D;

        
    }    
}

void computeK(Ctxt& Z, const long bitLength, const EncryptedArray ea){

    long log = log2(bitLength) + 1;

	for (int i = 0; i < log; i++) {
		Ctxt tmp(Z);
		ea.rotate(tmp, pow(2, i));
		Z.multiplyBy(tmp);

    /************************************************************************************************************

    Starting from       Z = [ z_l | z_(l-1) | z_(l-2) | . . . | z_2 | z_1 |   1   | . . . | 1 ],
    rotate it by 1 gives    [  1  |   z_l   | z_(l-1) | . . . | z_3 | z_2 |  z_1  | . . . | 1 ],
    We multiply them, and then Z becomes
                        Z = [   z_l   | z_(l-1) * z_l | z_(l-2) * z_(l-1) | . . . | z_2 * z_3 | * | * | * ]
                          = [ k_(l-1) |    k_(l-2)    | z_(l-2) * z_(l-1) | . . . | z_2 * z_3 | * | * | * ]


    In the next loop, we rotate Z by 2, and then we have in the end of this loop 
                        Z = [   z_l   | z_(l-1) * z_l | z_(l-2) * z_(l-1) * z_l | . . . | z_2 * z_3 * z_4 * z_5 | * | * | * ]
                          = [ k_(l-1) |    k_(l-2)    |         k_(l-3)         | . . . | z_2 * z_3 * z_4 * z_5 | * | * | * ],
    that is, the first 4 slots would be k_i s.

    To obtain every k, log(bitLength) number of iterations are sufficient.

	*************************************************************************************************************

	- 최적화를 위한 노력 2

	처음에는 rotate 연산이 무거운 것인줄 모르고 길이 bitLength인 Ctxt vector를 만들어 Z의 모든 rotate를 저장해두고 (bitLength회 rotate),
	이 rotated Z들을 곱하는 부분만 log(bitLength)로 처리했다.
	
	하지만 위에서도 언급했듯 rotate 연산이 훨씬 더 무겁다는 것을 알게 되었고,
	위와 같이 연산하는 방식을 생각하여 rotate 횟수도 log(bitLength)로 줄이게 되었다.
	특히 Ctxt vector도 필요없게 되었고, 메모리 상의 이득이 있을..지도?

    ************************************************************************************************************* */    

   
    }

}