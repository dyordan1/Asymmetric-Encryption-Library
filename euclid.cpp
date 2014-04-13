#include "euclid.h"
#include "mpint.h"

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

bool EuclideanAlgorithm(const mpuint &_a, const mpuint &_b, mpuint &_x,
  mpuint &_y, mpuint &g)
{
	mpuint a(_a), b(_b);
	unsigned length = a.length;
	if (b.length > length)
		length = b.length;
	mpint x(length),y(length),u(length),v(length),q(length),r(length),m(length),n(length),temp(length);
	x = 0;
	y = 1;
	u = 1;
	v = 0;
	while(a != 0)
	{
		mpuint::Divide(b, a, q, r);
		m = x;
		temp = u;
		temp *= q;
		m -= temp;
		n = y;
		temp = v;
		temp *= q;
		n -= temp;
		b = a;
		a = r;
		x = u;
		y = v;
		u = m;
		v = n;
	}
	g = b;
	_x = x;
	_y = y;
	if(x.isNegative)
	{
		return true;
	}
	return false;
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
