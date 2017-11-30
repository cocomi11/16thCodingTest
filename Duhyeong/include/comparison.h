#ifndef COMPARISON_H
#define COMPARISON_H

#include <NTL/ZZ.h>
#include "FHEContext.h"
#include "EncryptedArray.h"

using namespace NTL;

void comparisonTest(Ctxt&, const Ctxt&, const Ctxt&, const bool, const EncryptedArray&, const long);

#endif