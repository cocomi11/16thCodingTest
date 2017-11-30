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

	- ����ȭ�� ���� ��� 1
	
	�ʱ� �ڵ�� D�� 1ĭ rotate�ϰ� ���ϴ� ���� bitLengthȸ �ݺ��ؼ� bitLength��° slot ���� �״�� comparison ���� �ǰ� �ߴ�.
	: com = D; for(i=1; i<bitLength; i++) ea.rotate(D,1); com += D;    . . . (*)
	�׷��� (������ ��Ȯ�� �𸣰�����) rotate�� ������ �ʿ��� ������ ������, (rotate Ƚ���� �þ�� Depth�� Ŀ����� ��)
	�� ���¿��� bitLength�� �ø��� rotate�� �׸�ŭ �þ Depth�� Ŀ����� �ߴ�. ��, bitLength�� �þ�� �ӵ��� �ſ� ��������.

	�׷��� ( * ) �κ��� �������� �ʰ� D�� �״�� output�ϵ��� �ٲٰ�,
	main code���� decryption�� ���� 1 ~ bitLength ��° slot�� ��Ʈ���� ��� ���ϵ��� �����ߴ�. (110 - 113 Lines)

	���⼭ ������ ���� D�� �״�� output�ϴ°Ͱ� rotate & sum �� D�� output �� ���� ������ ���̰� ���ٴ� ���̴�.
	�ٽ� ���� D�� �޽����� ����Ǹ� rotate & sum �� D �� �޽����� ��� ������ �� �ְ�,
	�ݴ�� rotate & sum �� D �� �޽����� ����Ǹ� D�� �޽����� ������ �� �ִ�. ( D �� �޺κ� slot���� ��� 1�� ä���������� �˰� �ֱ� ���� )
	���� ���� ���� ����� ������ ������ ���� ������ �Ǵܵȴ�.

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

	- ����ȭ�� ���� ��� 2

	ó������ rotate ������ ���ſ� ������ �𸣰� ���� bitLength�� Ctxt vector�� ����� Z�� ��� rotate�� �����صΰ� (bitLengthȸ rotate),
	�� rotated Z���� ���ϴ� �κи� log(bitLength)�� ó���ߴ�.
	
	������ �������� ����ߵ� rotate ������ �ξ� �� ���̴ٴ� ���� �˰� �Ǿ���,
	���� ���� �����ϴ� ����� �����Ͽ� rotate Ƚ���� log(bitLength)�� ���̰� �Ǿ���.
	Ư�� Ctxt vector�� �ʿ���� �Ǿ���, �޸� ���� �̵��� ����..����?

    ************************************************************************************************************* */    

   
    }

}