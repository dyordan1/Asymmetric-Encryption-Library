#include "euclid.h"

namespace AsymmEL
{

/*----------------------------------------------------------------------------
This function uses the Euclidean algorithm to find the greatest common divisor
g of the positive integers x and y and also two integers a and b such that
ax - by = g, 1 <= a <= y and 0 <= b < x.

Here mpuint is an unsigned integer type. All calculations use unsigned
arithmetic, and none produces any result larger than the maximum of x and y.
Numbers of type mpuint are accessed by reference because in some applications
(such as cryptography) they may be very large.

This function will fail in undefined ways if either x or y is zero.
----------------------------------------------------------------------------*/

void EuclideanAlgorithm(const mpuint &x, const mpuint &y, mpuint &a,
  mpuint &b, mpuint &g)
{
  unsigned length = x.length;
  if (y.length > length)
    length = y.length;
  if (x <= y)
  {
    mpuint q(length), r(length);
    mpuint::Divide(y, x, q, r);
    if (r == 0)
    {
      a = 1;
      b = 0;
      g = x;
    }
    else
    {
      mpuint ap(length);
      EuclideanAlgorithm(x, r, ap, b, g);
      // a = ap + b * q;
      a = b;
      a *= q;
      a += ap;
    }
  }
  else
  {
    mpuint ap(length), bp(length);
    EuclideanAlgorithm(y, x, bp, ap, g);
    // a = y - ap;
    a = y;
    a -= ap;
    // b = x - bp;
    b = x;
    b -= bp;
  }
}

/*----------------------------------------------------------------------------
This function uses the Euclidean algorithm to find the greatest common divisor
g of the positive integers x and y.

Here mpuint is an unsigned integer type. All calculations use unsigned
arithmetic, and none produces any result larger than the maximum of x and y.
Numbers of type mpuint are accessed by reference because in some applications
(such as cryptography) they may be very large.

This function will fail in undefined ways if either x or y is zero.
----------------------------------------------------------------------------*/

void GreatestCommonDivisor(const mpuint &x, const mpuint &y, mpuint &g)
{
  if (x <= y)
  {
    mpuint r(y);
    r %= x;
    if (r == 0)
      g = x;
    else
      GreatestCommonDivisor(x, r, g);
  }
  else
    GreatestCommonDivisor(y, x, g);
}

//end namespace
}
