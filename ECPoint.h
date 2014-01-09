#ifndef ASYMMEL_EC_POINT_INCLUDED
#define ASYMMEL_EC_POINT_INCLUDED

#include "EllipticCurve.h"

namespace AsymmEL
{

class ECPoint
{
public:
	EllipticCurve* ec;
	finite_mpuint x;
	finite_mpuint y;
	bool isInfinite;
	ECPoint(EllipticCurve &_ec, const finite_mpuint &_x, const finite_mpuint &_y);
	ECPoint(EllipticCurve &_ec, unsigned len, mpuint &base);
	ECPoint() {};
	void operator += (const ECPoint &point);
	void operator -= (const ECPoint &point);
	void operator *= (const finite_mpuint &scalar);
};

//end namespace
}

#endif
