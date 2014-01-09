#ifndef ASYMMEL_RSA_INCLUDED
#define ASYMMEL_RSA_INCLUDED

#include "mpuint.h"
#include <string>

namespace AsymmEL
{

void GenerateKeys(mpuint &d, mpuint &e, mpuint &n);

inline void EncryptDecrypt(mpuint &result, const mpuint &source,
  const mpuint &e, const mpuint &n)
{
  mpuint::Power(source, e, n, result);
}

//end namespace
}

#endif
