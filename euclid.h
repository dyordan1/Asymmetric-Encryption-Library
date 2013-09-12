#ifndef H__EUCLID
#define H__EUCLID

#include "mpuint.h"

typedef mpuint UITYPE;

void EuclideanAlgorithm(const UITYPE &x, const UITYPE &y, UITYPE &a,
  UITYPE &b, UITYPE &g);

void GreatestCommonDivisor(const UITYPE &x, const UITYPE &y, UITYPE &g);

#endif