#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <NTL/ZZ.h>
#include <vector>
#include "FHE.h"
#include "EncryptedArray.h"


NTL_CLIENT

void BitDecomp(const long, const long, vector<long>&);

void messageGen(const long, const long, const long, const long, const EncryptedArray,
				vector<long>&, vector<long>&);



#endif 