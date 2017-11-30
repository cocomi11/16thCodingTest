#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>
#include <vector>
#include <NTL/ZZ.h>

using namespace std;
using namespace NTL;

void BitDecomp(vector<long>& BD_Msg, const ZZ& Msg, const long bitLength);

#endif