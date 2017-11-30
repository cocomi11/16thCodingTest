#ifndef COMPARISON_H
#define COMPARISON_H

#include <NTL/ZZ.h>
#include <vector>
#include "FHE.h"
#include "EncryptedArray.h"
#include "functions.h"


NTL_CLIENT

void computeK(Ctxt&, const long, const EncryptedArray);

void comparisonTest(const Ctxt, const Ctxt, const long, const EncryptedArray, const bool lessThan,
					Ctxt&);

#endif 