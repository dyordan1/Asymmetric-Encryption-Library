#ifndef ASYMMEL_ELLIPTIC_CURVE_INCLUDED
#define ASYMMEL_ELLIPTIC_CURVE_INCLUDED

#include "finite_mpuint.h"

namespace AsymmEL
{

class EllipticCurve
{
private:
	finite_mpuint a,b,c;
public:
	static char* names[6];
	static int sizes[6];
	static char* bases[6];
	static char* coefficients[6][3];
	static char* points[6][2];
	EllipticCurve(finite_mpuint &_a,finite_mpuint &_b,finite_mpuint &_c) :a(_a),b(_b),c(_c) {}
	EllipticCurve(const EllipticCurve &ec) :a(ec.a),b(ec.b),c(ec.c) {}
	friend class ECPoint;
};

//end namespace
}

#endif
