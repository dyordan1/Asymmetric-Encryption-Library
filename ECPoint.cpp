#include "ECPoint.h"

namespace AsymmEL
{

ECPoint::ECPoint(EllipticCurve &_ec, const finite_mpuint &_x, const finite_mpuint &_y) : ec(&_ec), x(_x), y(_y)
{
	isInfinite = false;
}
ECPoint::ECPoint(EllipticCurve &_ec, unsigned len, mpuint &base): ec(&_ec), x(len,base), y(len,base)
{
	isInfinite = true;
}

void ECPoint::operator = (const ECPoint &point)
{
	ec = point.ec;
	isInfinite = point.isInfinite;
	if(!isInfinite)
	{
		x = point.x;
		y = point.y;
	}
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

//Sliding window method
void ECPoint::operator *= (const finite_mpuint &scalar)
{
	//determine window size
	const int windowSize = BITS_IN_CHUNK/4;

	//create dP for 2^{w-1}...2^{w}-1
	const CHUNK_DATA_TYPE numPowers = (MAX_CHUNK>>(BITS_IN_CHUNK-windowSize+1))+1;
	ECPoint* multiples = new ECPoint[numPowers];
	multiples[0] = *this;
	CHUNK_DATA_TYPE bit = 1;
	do
	{
		multiples[0] +=  multiples[0];
		bit <<= 1;
	} while(bit != numPowers);

	int i;
	for(i=1;i<numPowers;i++)
	{
		multiples[i] = multiples[i-1];
		multiples[i] += *this;
	}
	ECPoint temp(*ec,x.length,*(x.base));
	i = scalar.length-1;
	while (i >= 0)
	{
		CHUNK_DATA_TYPE bit = 1<<(BITS_IN_CHUNK-1);
		do
		{
			if(!(scalar.value[i] & bit))
			{
				temp += temp;
			}
			else
			{
				CHUNK_DATA_TYPE pow = 1;
				int w = windowSize-1;
				while(w > 0)
				{
					if(bit == 1)
					{
						if(i == 0)
						{
							ECPoint Q(*ec,x.length,*(x.base));
							CHUNK_DATA_TYPE bitmaskDoubleAdd = 1<<(windowSize-1);
							do
							{
								Q += Q;
								if(pow & bitmaskDoubleAdd)
								{
									Q += temp;
								}
								bitmaskDoubleAdd >>= 1;
							} while(bitmaskDoubleAdd != 0);
							temp = Q;
							x = temp.x;
							y = temp.y;
							delete [] multiples;
							return;
						}
						else
						{
							--i;
						}
					}
					else
					{
						bit >>= 1;
					}
					pow <<= 1;
					if(scalar.value[i] & bit)
						pow++;
					w--;
				}
				CHUNK_DATA_TYPE bitPow = 1;
				do
				{
					temp +=  temp;
					bitPow <<= 1;
				} while(bitPow != windowSize);
				temp += multiples[pow-numPowers];
			}
			bit >>= 1;
		} while (bit != 0);
		--i;
	}
	x = temp.x;
	y = temp.y;
	delete [] multiples;
}

//end namespace
}
