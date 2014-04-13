#ifndef ASYMMEL_MPINT_INCLUDED
#define ASYMMEL_MPINT_INCLUDED

#include "mpuint.h"

namespace AsymmEL
{

class mpint : public mpuint
{
private:
public:
	bool isNegative;
	mpint(unsigned n) : isNegative(false), mpuint(n) {};
    mpint(const mpuint &n) : isNegative(false), mpuint(n) {};
    mpint(std::string hexStr, unsigned len=0) : isNegative(false), mpuint(hexStr,len) {};
	mpint() : isNegative(false), mpuint() {};
    void operator += (const mpint &n)
	{
		if(n.isNegative != isNegative && n > *this)
		{
			isNegative = !isNegative;
			mpuint temp(n);
			temp -= *this;
			*this = temp;
			return;
		}
		if(n.isNegative != isNegative)
		{
			mpuint::operator-=(n);
			return;
		}
		mpuint::operator+=(n);
	}
    void operator -= (const mpint &n)
	{
		if(n.isNegative == isNegative && n > *this)
		{
			isNegative = !isNegative;
			mpuint temp(n);
			temp -= *this;
			mpuint::operator=(temp);
			return;
		}
		if(n.isNegative != isNegative)
		{
			mpuint::operator+=(n);
			return;
		}
		mpuint::operator-=(n);
	}
    void operator *= (const mpint &n)
	{
		if(n.isNegative ^ isNegative)
		{
			isNegative = true;
		}
		else
		{
			isNegative = false;
		}
		mpuint::operator*=(n);
	}
	void operator =(CHUNK_DATA_TYPE n)
	{
		isNegative = false;
		mpuint::operator=(n);
	}
	const mpint & operator =(const mpint &n)
	{
		isNegative = n.isNegative;
		return mpuint::operator=(n);
	}
};

}

#endif