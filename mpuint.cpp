#include <stdio.h>
#include "mpuint.h" 
#include <iomanip>

void numeric_overflow(void)
{
	printf("Numeric Overflow");
}

std::ostream& operator<<(std::ostream& out,mpuint h)
{
	out << "0x";
	for(int i= h.length-1;i>=0;i--)
	{
		out << std::setw(BITS_IN_CHUNK/4) << std::setfill('0') << std::hex << (int)h.value[i];
	}
	return out;
}

mpuint::mpuint(unsigned len)
{
	length = len;
	value = new CHUNK_DATA_TYPE[len]; 
}

mpuint::mpuint(std::string hexStr)
{
	length = hexStr.size()/(BITS_IN_CHUNK/4);
	value = new CHUNK_DATA_TYPE[length];
	for(int i=length-1;i>=0;--i)
	{
		char chunk[BITS_IN_CHUNK/4+1];
		for(int j=0;j<BITS_IN_CHUNK/8;j++)
		{
			sprintf(&chunk[2*j],"%c%c",hexStr[i*(BITS_IN_CHUNK/4)+2*j],hexStr[i*(BITS_IN_CHUNK/4)+2*j+1]);
		}
		sscanf_s(chunk, "%x", &value[length-i-1]);
	}
}

void mpuint::setSize(unsigned len)
{
	if(length != 0)
		delete [] value;
	length = len;
	value = new CHUNK_DATA_TYPE[len]; 
}

mpuint::mpuint(const mpuint &n) 
{
	length = n.length;
	value = new CHUNK_DATA_TYPE[length]; 
	unsigned i; 
	for (i = 0; i < length; ++i) 
		value[i] = n.value[i]; 
} 

mpuint::~mpuint()
{
	delete [] value;
}

void mpuint::operator = (const mpuint &n)
{
	unsigned i;
	for (i = 0; i < length && i < n.length; ++i)
		value[i] = n.value[i];
	if(i < length)
		memset (&value[i],0,(length-i)*(BITS_IN_CHUNK/8));
	for (; i < n.length; ++i)
	{
		if (n.value[i] != 0)
			numeric_overflow();
	}
}

void mpuint::operator = (CHUNK_DATA_TYPE n)
{
	value[0] = n;
	if(1 < length)
		memset (&value[1],0,(length-1)*(BITS_IN_CHUNK/8));
}

void mpuint::operator += (const mpuint &n)
{
#ifdef USE_ASSEMBLY_IMPLEMENTATIONS
	mpuint_add_asm(this->value,n.value,length,n.length);
#else
	unsigned i;
	DCHUNK_DATA_TYPE carry = 0;
	for (i = 0; i < length; ++i)
	{
		DCHUNK_DATA_TYPE sum = carry + value[i] + (i < n.length ? n.value[i] : 0);
		value[i] = sum;
		carry = sum >> BITS_IN_CHUNK;
	}
	if (carry != 0)
		numeric_overflow();
	for (; i < n.length; ++i)
	{
		if (n.value[i] != 0)
			numeric_overflow();
	}
#endif
}

void mpuint::operator += (CHUNK_DATA_TYPE n)
{
	value[0] += n;
	if (value[0] < n)
	{
		unsigned i;
		for (i = 1; i < length; ++i)
		{
			if (++value[i] != 0)
			return;
		}
		numeric_overflow();
	}
}

void mpuint::operator -= (const mpuint &n)
{
#ifdef USE_ASSEMBLY_IMPLEMENTATIONS
	mpuint_sub_asm(this->value,n.value,length,n.length);
#else
	unsigned i;
	DCHUNK_DATA_TYPE borrow = 0;
	for (i = 0; i < length; ++i)
	{
		DCHUNK_DATA_TYPE subtrahend = (i < n.length ? n.value[i] : 0) + borrow;
		borrow = subtrahend > value[i];
		value[i] -= subtrahend;
	}
	if (borrow != 0)
		numeric_overflow();
	for (; i < n.length; ++i)
	{
		if (n.value[i] != 0)
			numeric_overflow();
	}
#endif
}

void mpuint::operator -= (CHUNK_DATA_TYPE n)
{
	if (value[0] >= n)
		value[0] -= n;
	else
	{
		value[0] -= n;
		for (unsigned i = 1; i < length; ++i)
		{
			if (--value[i] != 0xFF)
			return;
		}
		numeric_overflow();
	}
}

void mpuint::operator *= (const mpuint &n)
{
	unsigned i;
	DCHUNK_DATA_TYPE *multiplier = new DCHUNK_DATA_TYPE[length];
	for (i = 0; i < length; ++i)
	{
		multiplier[i] = value[i];
		value[i] = 0;
	}
	for (i = 0; i < length; ++i)
	{
		unsigned j;
		for (j = 0; j < n.length; ++j)
		{
			DCHUNK_DATA_TYPE product = multiplier[i] * n.value[j];
			unsigned k = i + j;
			while (product != 0)
			{
				if (k >= length)
					numeric_overflow();
				product += value[k];
				value[k] = product;
				product >>= BITS_IN_CHUNK;
				++k;
			}
		}
	}
	delete [] multiplier;
}

void mpuint::operator *= (CHUNK_DATA_TYPE n)
{
	unsigned i;
	DCHUNK_DATA_TYPE product = 0;
	for (i = 0; i < length; ++i)
	{
		product += n * (DCHUNK_DATA_TYPE)value[i];
		value[i] = product;
		product >>= BITS_IN_CHUNK;
	}
	if (product != 0)
		numeric_overflow();
}

CHUNK_DATA_TYPE mpuint::remainder(CHUNK_DATA_TYPE n)
{
	unsigned i = length;
	unsigned rem = 0;
	while (i-- != 0)
	{
		DCHUNK_DATA_TYPE dividend = (DCHUNK_DATA_TYPE) rem << BITS_IN_CHUNK | (DCHUNK_DATA_TYPE) value[i];
		value[i] = dividend / n;
		rem = dividend % n;
	}
	return rem;
}

void mpuint::operator /= (CHUNK_DATA_TYPE n)
{
	(void) remainder(n);
}

void mpuint::operator %= (CHUNK_DATA_TYPE n)
{
	*this = remainder(n);
}

int mpuint::Compare(const mpuint &n) const
{
	unsigned i;
	if (length > n.length)
	{
		for (i = length-1; i >= n.length; i--)
		{
			if (value[i] != 0)
			return 1;
		}
	}
	else if (n.length > length)
	{
		for (i = n.length-1; i >= length; i--)
		{
			if (n.value[i] != 0)
			return -1;
		}
	}
	else
		i = length-1;
	while (true)
	{
		if (value[i] > n.value[i])
			return 1;
		if (value[i] < n.value[i])
			return -1;
		if (i == 0)
			return 0;
		i--;
	}
}

int mpuint::Compare(CHUNK_DATA_TYPE n) const
{
	unsigned i;
	for (i = length-1; i >= 1; i--)
	{
		if (value[i] != 0)
			return 1;
	}
	return value[0] > n ? 1 : value[0] < n ? -1 : 0;
}


bool mpuint::IsZero(void) const
{
	unsigned i;
	for (i = 0; i < length; ++i)
	{
		if (value[i] != 0)
			return false;
	}
	return true;
}

CHUNK_DATA_TYPE *mpuint::edit(CHUNK_DATA_TYPE *s) const
{
	mpuint n(*this);
	unsigned i = 0; 
	do 
		s[i++] = n.remainder(10) + '0'; 
	while (!n.IsZero());
	s[i] = 0; 
	unsigned j; 
	for (j = 0; --i > j; ++j) 
	{ 
		CHUNK_DATA_TYPE c = s[i]; 
		s[i] = s[j];
		s[j] = c; 
	}
	return s; 
}

bool mpuint::scan(const CHUNK_DATA_TYPE *&s) 
{ 
	const CHUNK_DATA_TYPE *t = s; 
	bool found = false; 
	while (*t == ' ' || *t == '\t') 
		++t;
	*this = 0; 
	while ('0' <= *t && *t <= '9') 
	{ 
		found = true;
		*this *= 10; 
		*this += (CHUNK_DATA_TYPE) (*t++ - '0'); 
	} 
	s = t; 
	return found; 
}

void mpuint::shift(unsigned bit)
{
	CHUNK_DATA_TYPE rolOver = 0;
	for (unsigned i = 0; i < length; ++i)
	{ 
		DCHUNK_DATA_TYPE x = ((DCHUNK_DATA_TYPE)value[i] << bit) | rolOver; 
		value[i] = x; 
		rolOver = x >> BITS_IN_CHUNK; 
	}
	if (rolOver != 0) 
		numeric_overflow();
} 

void mpuint::Divide(const mpuint &dividend, const mpuint &divisor, mpuint &quotient,
  mpuint &remainder)
{ 
        if (divisor.IsZero())
                numeric_overflow();
		if (divisor.value[divisor.length-1] != MAX_CHUNK  && (divisor.value[divisor.length-1] & MSB<<1))
		{
			SmartDivide(dividend,divisor,quotient,remainder);
			return;
		}
        remainder = 0;
        quotient = 0;
        unsigned i = dividend.length;
        while (i-- > 0)
        {
                CHUNK_DATA_TYPE bit = 1 << (BITS_IN_CHUNK-1);
                do
                {
                        remainder.shift(1);
                        remainder.value[0] |= ((dividend.value[i] & bit)!= 0);
                        if(remainder >= divisor)
                        {
                                remainder = remainder-divisor;
                                quotient.value[i] |= bit;
                        }
                } while(bit >>= 1);
        }
} 


void mpuint::SmartDivide(const mpuint &dividend, const mpuint &divisor, mpuint &quotient,
  mpuint &remainder)
{
	mpuint container(divisor.length+2);
	//AND with 0
	container = 0;
	for(int i=0;i<divisor.length; ++i)
	{
		container.shift(BITS_IN_CHUNK);
		container += dividend.value[dividend.length-1-i];
	}
	quotient = 0;
	CHUNK_DATA_TYPE topChunk = divisor.value[divisor.length-1]+1;
	mpuint temp(container.length);
	for(int i=0;i<dividend.length-divisor.length;++i)
	{
		while(container > divisor)
		{
			container -= divisor;
			quotient += 1;
		}
		quotient.shift(BITS_IN_CHUNK);
		container.shift(BITS_IN_CHUNK);
		container += dividend.value[dividend.length-divisor.length-1-i];
		DCHUNK_DATA_TYPE topTwo = container.value[container.length-2];
		topTwo <<= BITS_IN_CHUNK;
		topTwo += container.value[container.length-3];
		CHUNK_DATA_TYPE result = topTwo/topChunk;
		quotient += result;
		temp = divisor;
		temp *= result;
		container -= temp;
	}
	while(container > divisor)
	{
		container -= divisor;
		quotient += 1;
	}
	remainder = container;
} 

void mpuint::operator /= (const mpuint &n) 
{ 
	mpuint remainder(length); 
	mpuint dividend(*this); 
	Divide(dividend, n, *this, remainder);
} 

void mpuint::operator %= (const mpuint &n) 
{ 
	mpuint quotient(length); 
	mpuint dividend(*this);
	Divide(dividend, n, quotient, *this);
}

void mpuint::Power(const mpuint &base, const mpuint &exponent,
  const mpuint &modulus, mpuint &result)
{
	mpuint r(2*modulus.length+1);
	mpuint binPow(2*base.length);
	mpuint binPowCopy(2*base.length);
	r=1;
	binPow = base;
	binPowCopy = binPow;
	unsigned i = 0;
	while (i < exponent.length)
	{
		CHUNK_DATA_TYPE bit = 1;
		do
		{
			if (exponent.value[i] & bit)
			{
				r *= binPow;
				r %= modulus;
			}
			bit <<= 1;
			binPowCopy *= binPow;
			binPowCopy %= modulus;
			binPow = binPowCopy;
		} while (bit != 0);
		++i;
	}
	result = r;
}


void mpuint::dump() const
{
	unsigned i;
	for (i = 0; i < length; ++i)
		printf(" %x", value[i]);
		putchar('\n');
}


mpuint operator +(const mpuint &m, const mpuint &n)
{
	mpuint c = mpuint(m);
	c += n;
	return c;
}

mpuint operator +(const mpuint &m, CHUNK_DATA_TYPE n)
{
	mpuint c = mpuint(m);
	c += n;
	return c;
}

mpuint operator -(const mpuint &m, const mpuint &n)
{
	mpuint c = mpuint(m);
	c -= n;
	return c;
}

mpuint operator -(const mpuint &m, CHUNK_DATA_TYPE n)
{
	mpuint c = mpuint(m);
	c -= n;
	return c;
}

mpuint operator *(const mpuint &m, const mpuint &n)
{
	mpuint c = mpuint(m.length+n.length);
	c = m;
	c *= n;
	return c;
}

mpuint operator *(const mpuint &m, CHUNK_DATA_TYPE n)
{
	mpuint c = mpuint(m.length+1);
	c = m;
	c *= n;
	return c;
}

mpuint operator /(const mpuint &m, const mpuint &n)
{
	mpuint c = mpuint(m);
	c /= n;
	return c;
}

mpuint operator /(const mpuint &m, CHUNK_DATA_TYPE n)
{
	mpuint c = mpuint(m);
	c /= n;
	return c;
}

mpuint operator %(const mpuint &m, const mpuint &n)
{
	mpuint c = mpuint(m);
	c %= n;
	return c;
}

mpuint operator %(const mpuint &m, CHUNK_DATA_TYPE n)
{
	mpuint c = mpuint(m);
	c %= n;
	return c;
}