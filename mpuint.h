#ifndef H__MPUINT
#define H__MPUINT

#include <iostream>

extern void numeric_overflow(void);

class mpuint
{
  private:
    unsigned char remainder(unsigned char);
    void shift(unsigned);
  public:
    unsigned char *value;
    bool IsZero(void) const;
    int Compare(const mpuint &) const;
    int Compare(unsigned char) const;
    unsigned length;
    mpuint(unsigned);
    mpuint(const mpuint &);
	mpuint() {length=0;};
	void setSize(unsigned);
    ~mpuint();
    void operator = (const mpuint &);
    void operator = (unsigned char);
    void operator += (const mpuint &);
    void operator += (unsigned char);
    void operator -= (const mpuint &);
    void operator -= (unsigned char);
    void operator *= (const mpuint &);
    void operator *= (unsigned char);
    void operator /= (const mpuint &);
    void operator /= (unsigned char);
    void operator %= (const mpuint &);
    void operator %= (unsigned char);
    static void Divide(const mpuint &, const mpuint &, mpuint &, mpuint &);
    unsigned char *edit(unsigned char *) const;
    bool scan(const unsigned char *&);
    void dump() const;
    bool mpuint::operator == (const mpuint &n) const {return Compare(n) == 0;}
    bool mpuint::operator != (const mpuint &n) const {return Compare(n) != 0;}
    bool mpuint::operator >  (const mpuint &n) const {return Compare(n) >  0;}
    bool mpuint::operator >= (const mpuint &n) const {return Compare(n) >= 0;}
    bool mpuint::operator <  (const mpuint &n) const {return Compare(n) <  0;}
    bool mpuint::operator <= (const mpuint &n) const {return Compare(n) <= 0;}
    bool mpuint::operator == (unsigned char n) const {return Compare(n) == 0;}
    bool mpuint::operator != (unsigned char n) const {return Compare(n) != 0;}
    bool mpuint::operator >  (unsigned char n) const {return Compare(n) >  0;}
    bool mpuint::operator >= (unsigned char n) const {return Compare(n) >= 0;}
    bool mpuint::operator <  (unsigned char n) const {return Compare(n) <  0;}
    bool mpuint::operator <= (unsigned char n) const {return Compare(n) <= 0;}
	friend std::ostream& operator<<(std::ostream& out,mpuint h);
    static void Power(const mpuint &, const mpuint &, const mpuint &,
      mpuint &);
};

mpuint operator +(const mpuint &, const mpuint &);
mpuint operator +(const mpuint &, unsigned char);
mpuint operator -(const mpuint &, const mpuint &);
mpuint operator -(const mpuint &, unsigned char);
mpuint operator *(const mpuint &, const mpuint &);
mpuint operator *(const mpuint &, unsigned char);
mpuint operator /(const mpuint &, const mpuint &);
mpuint operator /(const mpuint &, unsigned char);
mpuint operator %(const mpuint &, const mpuint &);
mpuint operator %(const mpuint &, unsigned char);

#endif