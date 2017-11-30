#ifndef COMPARISON_H
#define COMPARISON_H
#include <NTL/ZZ.h>
#include <Ctxt.h>
#include <FHE.h>
#include <EncryptedArray.h>

using namespace NTL;

int comparisonTest(bool lessthan, Ctxt ct1, Ctxt ct2, Ctxt one, long bitlength, FHESecKey secretKey,EncryptedArray ea, long* level);

#endif
