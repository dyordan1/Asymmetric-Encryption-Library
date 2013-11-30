#pragma once

#include "EllipticCurve.h"

class ECPoint
{
public:
	EllipticCurve ec;
	finite_mpuint x;
	finite_mpuint y;
	bool isInfinite;
	ECPoint(const EllipticCurve &_ec, const finite_mpuint &_x, const finite_mpuint &_y);
	ECPoint(const EllipticCurve &_ec, unsigned len, mpuint &base);
	void operator += (const ECPoint &point);
	void operator -= (const ECPoint &point);
	void operator *= (finite_mpuint &scalar);
};

