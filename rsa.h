#ifndef ASYMMEL_RSA_INCLUDED
#define ASYMMEL_RSA_INCLUDED

#include "mpuint.h"
#include <string>

namespace AsymmEL
{

void GenerateKeys(mpuint &d, mpuint &e, mpuint &n, CHUNK_DATA_TYPE customE = 0, bool pseudoKeys = false);

inline void EncryptDecrypt(mpuint &result, const mpuint &source,
  const mpuint &e, const mpuint &n)
{
  mpuint::Power(source, e, n, result);
}

//end namespace
}

#endif
