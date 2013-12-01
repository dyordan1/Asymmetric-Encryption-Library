#include "ECPoint.h"

ECPoint::ECPoint(EllipticCurve &_ec, const finite_mpuint &_x, const finite_mpuint &_y) : ec(&_ec), x(_x), y(_y)
{
	isInfinite = false;
}
ECPoint::ECPoint(EllipticCurve &_ec, unsigned len, mpuint &base): ec(&_ec), x(len,base), y(len,base)
{
	isInfinite = true;
}
void ECPoint::operator += (const ECPoint &point)
{
	if(point.isInfinite)
	{
		return;
	}
	else if(isInfinite)
	{
		x = point.x;
		y = point.y;
		isInfinite = false;
		return;
	}
	finite_mpuint m(point.y);
	finite_mpuint temp(x);
	if(x == point.x)
	{
		if(y == point.y)
		{
			m = x;
			m *= x;
			m *= 3;
			temp = ec->a;
			temp *= 2;
			temp *= x;
			m += temp;
			m += ec->b;
			temp = y;
			temp *= 2;
			m /= temp;
		}
		else
		{
			isInfinite = true;
			return;
		}
	}
	else
	{
		finite_mpuint dx(point.x);
		dx -= x;
		m -= y;
		m /= dx;
	}
	finite_mpuint x3(m),y3(m);
	x3 *= m;
	x3 -= ec->a;
	x3 -= x;
	x3 -= point.x;
	y3 *= x;
	temp = x3;
	temp *= m;
	y3 -= temp;
	y3 -= y;
	x = x3;
	y = y3;
}
void ECPoint::operator -= (const ECPoint &point)
{
	ECPoint copy(point);
	copy.y = 0;
	copy.y -= point.y;
	ECPoint::operator+=(copy);
}
void ECPoint::operator *= (const finite_mpuint &scalar)
{
	ECPoint multiple(*this),multipleCopy(*this),temp(*ec,x.length,*(x.base));
	unsigned i = 0;
	while (i < scalar.length)
	{
		CHUNK_DATA_TYPE bit = 1;
		do
		{
			if (scalar.value[i] & bit)
			{
				temp += multiple;
			}
			bit <<= 1;
			multipleCopy += multiple;
			multiple = multipleCopy;
		} while (bit != 0);
		++i;
	}
	x = temp.x;
	y = temp.y;
}