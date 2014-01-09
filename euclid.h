#ifndef ASYMMEL_EUCLID_INCLUDED
#define ASYMMEL_EUCLID_INCLUDED

#include "mpuint.h"

namespace AsymmEL
{

void EuclideanAlgorithm(const mpuint &x, const mpuint &y, mpuint &a,
  mpuint &b, mpuint &g);

void GreatestCommonDivisor(const mpuint &x, const mpuint &y, mpuint &g);

}

#endif
