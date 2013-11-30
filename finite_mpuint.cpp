#include "finite_mpuint.h"

void finite_mpuint::shift(unsigned bit)
{
	mpuint::shift(bit);
	mpuint::operator%=(*base);
}
finite_mpuint::finite_mpuint(unsigned len, mpuint &_base) : mpuint(len), a(_base.length), b(_base.length), g(_base.length)
{
	base = &_base;
	mpuint::operator%=(*base);
}

finite_mpuint::finite_mpuint(const finite_mpuint &n) : mpuint(n), a(n.base->length), b(n.base->length), g(n.base->length)
{
	base = n.base;
}

finite_mpuint::finite_mpuint(const mpuint &n, mpuint &_base) : mpuint(n), a(_base.length), b(_base.length), g(_base.length)
{
	base = &_base;
	mpuint::operator%=(*base);
}
finite_mpuint::finite_mpuint(std::string hexStr, mpuint &_base, unsigned len) : mpuint(hexStr,len), a(_base.length), b(_base.length), g(_base.length)
{
	base = &_base;
	mpuint::operator%=(*base);
}
void finite_mpuint::operator = (const mpuint &n)
{
	mpuint::operator=(n);
}
void finite_mpuint::operator = (CHUNK_DATA_TYPE n)
{
	mpuint::operator=(n);
}
void finite_mpuint::operator += (const mpuint &n)
{
	mpuint::operator+=(n);
	mpuint::operator%=(*base);
}
void finite_mpuint::operator += (CHUNK_DATA_TYPE n)
{
	mpuint::operator+=(n);
	mpuint::operator%=(*base);
}
void finite_mpuint::operator -= (const mpuint &n)
{
	if(*this < n)
	{
		mpuint::operator+=(*base);
	}
	mpuint::operator-=(n);
	mpuint::operator%=(*base);
}
void finite_mpuint::operator -= (CHUNK_DATA_TYPE n)
{
	if(*this < n)
	{
		mpuint::operator+=(*base);
	}
	mpuint::operator-=(n);
	mpuint::operator%=(*base);
}
void finite_mpuint::operator *= (const mpuint &n)
{
	mpuint::operator*=(n);
	mpuint::operator%=(*base);
}
void finite_mpuint::operator *= (CHUNK_DATA_TYPE n)
{
	mpuint::operator*=(n);
	mpuint::operator%=(*base);
}
void finite_mpuint::operator /= (const mpuint &n)
{
	EuclideanAlgorithm(n,*base,a,b,g);
	mpuint::operator*=(a);
	mpuint::operator%=(*base);
}
void finite_mpuint::operator /= (CHUNK_DATA_TYPE n)
{
	mpuint num(base->length);
	num = n;
	EuclideanAlgorithm(num,*base,a,b,g);
	mpuint::operator*=(a);
	mpuint::operator%=(*base);
}