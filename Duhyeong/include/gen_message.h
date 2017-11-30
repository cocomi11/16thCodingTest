#ifndef GEN_MESSAGE_H
#define GEN_MESSAGE_H

#include <vector>
#include <NTL/ZZ.h>
#include "EncryptedArray.h"

using namespace std;
using namespace NTL;

void generateMessage(ZZ& Msg1, ZZ& Msg2, vector<long>& BD_Msg1, vector<long>& BD_Msg2, const long bitLength, const EncryptedArray ea);

#endif