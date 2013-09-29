#ifndef H__MPUINT
#define H__MPUINT

#include <iostream>

#define BITS_IN_CHUNK 32

#if BITS_IN_CHUNK==32
#define CHUNK_DATA_TYPE unsigned __int32
#define DCHUNK_DATA_TYPE unsigned __int64
#define MSB 0x80000000
#elif BITS_IN_CHUNK==16
#define CHUNK_DATA_TYPE unsigned __int16
#define DCHUNK_DATA_TYPE unsigned __int32
#define MSB 0x8000
#elif BITS_IN_CHUNK==8
#define CHUNK_DATA_TYPE unsigned __int8
#define DCHUNK_DATA_TYPE unsigned __int16
#define MSB 0x80
#endif

extern void numeric_overflow(void);

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

#endif