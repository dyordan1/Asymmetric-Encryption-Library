#ifndef ASYMMEL_FINITE_MPUINT_INCLUDED
#define ASYMMEL_FINITE_MPUINT_INCLUDED

#include "mpuint.h"
#include "euclid.h"

namespace AsymmEL
{

class finite_mpuint: public mpuint
{
	private:
	mpuint a, b, g;
	public:
	mpuint* base;
    void shift(unsigned);
    finite_mpuint() {length=0;};
    finite_mpuint(unsigned, mpuint &);
    finite_mpuint(const finite_mpuint &);
    finite_mpuint(const mpuint &, mpuint &);
    finite_mpuint(std::string hexStr, mpuint &, unsigned len=0);
	void setBase(mpuint &);
    void operator = (const mpuint &);
    void operator = (CHUNK_DATA_TYPE);
    void operator += (const mpuint &);
    void operator += (CHUNK_DATA_TYPE);
    void operator -= (const mpuint &);
    void operator -= (CHUNK_DATA_TYPE);
    void operator *= (const mpuint &);
    void operator *= (CHUNK_DATA_TYPE);
    void operator /= (const mpuint &);
    void operator /= (CHUNK_DATA_TYPE);
};

//end namespace
}

#endif
