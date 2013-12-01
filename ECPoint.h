#pragma once

#include "EllipticCurve.h"

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

