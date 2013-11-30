#pragma once

#include "mpuint.h"
#include "euclid.h"

class finite_mpuint: public mpuint
{
	private:
	mpuint a, b, g;
	public:
	mpuint* base;
    void shift(unsigned);
    finite_mpuint(unsigned, mpuint &);
    finite_mpuint(const finite_mpuint &);
    finite_mpuint(const mpuint &, mpuint &);
    finite_mpuint(std::string hexStr, mpuint &, unsigned len=0);
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

