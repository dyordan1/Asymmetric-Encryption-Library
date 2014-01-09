#ifndef ASYMMEL_MPUINT_INCLUDED
#define ASYMMEL_MPUINT_INCLUDED

#include <iostream>

#define MAX_THREADS 4

#ifdef _WIN64
#define BITS_IN_CHUNK 32
#elif _WIN32
#define BITS_IN_CHUNK 16
#endif

#if BITS_IN_CHUNK==32
#define USE_ASSEMBLY_IMPLEMENTATIONS
#define CHUNK_DATA_TYPE unsigned __int32
#define DCHUNK_DATA_TYPE unsigned __int64
#define MSB 0xC0000000
#define MAX_CHUNK 0xFFFFFFFF
#elif BITS_IN_CHUNK==16
#define CHUNK_DATA_TYPE unsigned __int16
#define DCHUNK_DATA_TYPE unsigned __int32
#define MSB 0xC000
#define MAX_CHUNK 0xFFFF
#elif BITS_IN_CHUNK==8
#define CHUNK_DATA_TYPE unsigned __int8
#define DCHUNK_DATA_TYPE unsigned __int16
#define MSB 0xC0
#define MAX_CHUNK 0xFF
#endif

namespace AsymmEL
{

extern "C" void numeric_overflow(void);

class mpuint
{
  private:
    CHUNK_DATA_TYPE remainder(CHUNK_DATA_TYPE);
  public:
    void shift(unsigned);
    CHUNK_DATA_TYPE *value;
    bool IsZero(void) const;
    int Compare(const mpuint &) const;
    int Compare(CHUNK_DATA_TYPE) const;
    unsigned length;
    mpuint(unsigned);
    mpuint(const mpuint &);
    mpuint(std::string hexStr, unsigned len=0);
	mpuint() {length=0;};
	void setSize(unsigned);
    ~mpuint();
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
    void operator %= (const mpuint &);
    void operator %= (CHUNK_DATA_TYPE);
    static void Divide(const mpuint &, const mpuint &, mpuint &, mpuint &);
    static void SmartDivide(const mpuint &, const mpuint &, mpuint &, mpuint &);
    CHUNK_DATA_TYPE *edit(CHUNK_DATA_TYPE *) const;
    bool scan(const CHUNK_DATA_TYPE *&);
    void dump() const;
    bool mpuint::operator == (const mpuint &n) const {return Compare(n) == 0;}
    bool mpuint::operator != (const mpuint &n) const {return Compare(n) != 0;}
    bool mpuint::operator >  (const mpuint &n) const {return Compare(n) >  0;}
    bool mpuint::operator >= (const mpuint &n) const {return Compare(n) >= 0;}
    bool mpuint::operator <  (const mpuint &n) const {return Compare(n) <  0;}
    bool mpuint::operator <= (const mpuint &n) const {return Compare(n) <= 0;}
    bool mpuint::operator == (CHUNK_DATA_TYPE n) const {return Compare(n) == 0;}
    bool mpuint::operator != (CHUNK_DATA_TYPE n) const {return Compare(n) != 0;}
    bool mpuint::operator >  (CHUNK_DATA_TYPE n) const {return Compare(n) >  0;}
    bool mpuint::operator >= (CHUNK_DATA_TYPE n) const {return Compare(n) >= 0;}
    bool mpuint::operator <  (CHUNK_DATA_TYPE n) const {return Compare(n) <  0;}
    bool mpuint::operator <= (CHUNK_DATA_TYPE n) const {return Compare(n) <= 0;}
	friend std::ostream& operator<<(std::ostream& out,mpuint h);
    static void Power(const mpuint &, const mpuint &, const mpuint &,
      mpuint &);
};

mpuint operator +(const mpuint &, const mpuint &);
mpuint operator +(const mpuint &, CHUNK_DATA_TYPE);
mpuint operator -(const mpuint &, const mpuint &);
mpuint operator -(const mpuint &, CHUNK_DATA_TYPE);
mpuint operator *(const mpuint &, const mpuint &);
mpuint operator *(const mpuint &, CHUNK_DATA_TYPE);
mpuint operator /(const mpuint &, const mpuint &);
mpuint operator /(const mpuint &, CHUNK_DATA_TYPE);
mpuint operator %(const mpuint &, const mpuint &);
mpuint operator %(const mpuint &, CHUNK_DATA_TYPE);

#ifdef USE_ASSEMBLY_IMPLEMENTATIONS
extern "C"
{
	void mpuint_add_asm(unsigned __int32* a, unsigned __int32* b, size_t sizeA, size_t sizeB);
	void mpuint_sub_asm(unsigned __int32* a, unsigned __int32* b, size_t sizeA, size_t sizeB);
};
#endif

//end namespace
}

#endif
