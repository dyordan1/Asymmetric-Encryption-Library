#pragma once
#include "finite_mpuint.h"

using namespace std;

class EllipticCurve
{
private:
	finite_mpuint a,b,c;
public:
	static char* names[4];
	static int sizes[4];
	static char* bases[4];
	static char* coefficients[4][3];
	static char* points[4][2];
	EllipticCurve(finite_mpuint &_a,finite_mpuint &_b,finite_mpuint &_c) :a(_a),b(_b),c(_c) {}
	EllipticCurve(const EllipticCurve &ec) :a(ec.a),b(ec.b),c(ec.c) {}
	friend class ECPoint;
};