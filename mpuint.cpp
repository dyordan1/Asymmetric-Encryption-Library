#include <stdio.h>
#include "mpuint.h" 
#include <iomanip>

#include "euclid.h"

namespace AsymmEL
{

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

mpuint::mpuint(std::string hexStr, unsigned len)
{
	int numChunks = (int)( hexStr.size()+(BITS_IN_CHUNK/8) )/(BITS_IN_CHUNK/4);
	if(len > 0)
	{
		length = len;
	}
	else
	{
		length = numChunks;
		if(length == 0)
			length = 1;
	}
	value = new CHUNK_DATA_TYPE[length];
	for(unsigned i=0;i<length;++i)
	{
		char chunk[BITS_IN_CHUNK/4+1];
		for(int j=0;j<BITS_IN_CHUNK/8;j++)
		{
			int first = (int)hexStr.size() - (i+1)*(BITS_IN_CHUNK/4)+2*j, second = (int)hexStr.size() - (i+1)*(BITS_IN_CHUNK/4)+2*j+1;
			char c1,c2;
			if(first < 0)
			{
				c1 = '0';
			}
			else
			{
				c1 = hexStr[first];
			}
			if(second < 0)
			{
				c2 = '0';
			}
			else
			{
				c2 = hexStr[second];
			}
			sprintf_s(&chunk[2*j],3,"%c%c",c1,c2);
		}
		sscanf_s(chunk, "%x", &value[i]);
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
	if(length != 0)
	{
		delete [] value;
	}
}

const mpuint & mpuint::operator = (const mpuint &n)
{
	if(length == 0)
		setSize(n.length);
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
	return n;
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
	if(this->length%2 == 0 && n.length%2 == 0)
	{
		mpuint_add_asm(this->value,n.value,length,n.length);
	}
	else
	{
#endif
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
#ifdef USE_ASSEMBLY_IMPLEMENTATIONS
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
	if(this->length%2 == 0 && n.length%2 == 0)
	{
		mpuint_sub_asm(this->value,n.value,length,n.length);
	}
	else
	{
#endif
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
#ifdef USE_ASSEMBLY_IMPLEMENTATIONS
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
				value[k] = (CHUNK_DATA_TYPE)product;
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
		value[i] = (CHUNK_DATA_TYPE)product;
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
		value[i] = (CHUNK_DATA_TYPE)(dividend / n);
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
		value[i] = (CHUNK_DATA_TYPE)x; 
		rolOver = x >> BITS_IN_CHUNK; 
	}
	if (rolOver != 0) 
		numeric_overflow();
} 

void mpuint::shiftRight(unsigned bit)
{
	unsigned fullChunks = bit/BITS_IN_CHUNK;
	bit %= BITS_IN_CHUNK;
	if(fullChunks != 0)
	{
		for (int i = fullChunks; i < length; ++i)
		{
			value[i-fullChunks] = value[i];
		}
		for(int i = length-fullChunks-1; i<length; i++)
		{
			value[i] = 0;
		}
	}
	if(bit != 0)
	{
		CHUNK_DATA_TYPE rolOver = 0;
		for (int i = length-1; i >= 0; --i)
		{
			CHUNK_DATA_TYPE x = (value[i] >> bit) | rolOver; 
			rolOver = value[i] << (BITS_IN_CHUNK-bit);
			value[i] = (CHUNK_DATA_TYPE)x;
		}
	}
} 

void mpuint::saveBits(unsigned bit)
{
	unsigned maxChunk = (bit+BITS_IN_CHUNK-1)/BITS_IN_CHUNK;
	for(int i=maxChunk;i<length;i++)
	{
		value[i] = 0;
	}
	unsigned numBitsinLast = bit%BITS_IN_CHUNK;
	CHUNK_DATA_TYPE bitmask = -1;
	bitmask >>= (BITS_IN_CHUNK-numBitsinLast);
	value[maxChunk-1] &= bitmask;
} 

void mpuint::Divide(const mpuint &dividend, const mpuint &divisor, mpuint &quotient,
  mpuint &remainder)
{ 
        if (divisor.IsZero())
                numeric_overflow();
		if (divisor.value[divisor.length-1] & MSB<<1)
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
                                remainder -= divisor;
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
	for(unsigned i=0;i<container.length; ++i)
	{
		if(i < divisor.length)
		{
			container.value[divisor.length-1-i] = dividend.value[dividend.length-1-i];
		}
		else
		{
			container.value[i] = 0;
		}
	}
	quotient = 0;
	DCHUNK_DATA_TYPE topChunk = (DCHUNK_DATA_TYPE)divisor.value[divisor.length-1]+1;
	mpuint temp(container.length);
	unsigned sizeDiff = dividend.length-divisor.length;
	for(unsigned i=0;i<sizeDiff;++i)
	{
		while(container > divisor)
		{
			container -= divisor;
			quotient += 1;
		}
		quotient.shift(BITS_IN_CHUNK);
		container.shift(BITS_IN_CHUNK);
		container += dividend.value[sizeDiff-1-i];
		DCHUNK_DATA_TYPE topTwo = container.value[container.length-2];
		topTwo <<= BITS_IN_CHUNK;
		topTwo += container.value[container.length-3];
		CHUNK_DATA_TYPE result = (CHUNK_DATA_TYPE)(topTwo/topChunk);
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
	if(*this > n)
	{
		mpuint quotient(length); 
		mpuint dividend(*this);
		Divide(dividend, n, quotient, *this);
	}
}

void xbinGCD(const mpuint & binPow, const mpuint & m, mpuint& pu, mpuint& pv) 
 { 
	mpuint alpha(binPow), u(binPow.length+2), v(binPow.length+2); 
 
	u = 1; v = 0; 
 
	/* The invariant maintained from here on is: 
	a = u*2*alpha - v*beta. */ 
 
	while ((alpha.value[0] & 1) == 0)
	{ 
		alpha.shiftRight(1); 
		if ((u.value[0] & 1) == 0)
		{
			u.shiftRight(1);
			v.shiftRight(1);
		}
		else
		{ 
			u += m;
			u.shiftRight(1);
			v += binPow; 
			v.shiftRight(1);
		} 
	}
	
	pu = u;
	pv = v;

	return; 
}

void multBits(mpuint a, mpuint b, int bitlength)
{
	unsigned i;
	DCHUNK_DATA_TYPE *multiplier = new DCHUNK_DATA_TYPE[a.length];
	for (i = 0; i < a.length; ++i)
	{
		multiplier[i] = a.value[i];
		a.value[i] = 0;
	}
	int numChunks = bitlength/BITS_IN_CHUNK+1;
	for (i = 0; i < numChunks; ++i)
	{
		unsigned j;
		for (j = 0; j < numChunks-i; ++j)
		{
			DCHUNK_DATA_TYPE product = multiplier[i] * b.value[j];
			unsigned k = i + j;
			while (product != 0)
			{
				if (k >= numChunks)
					break;
				product += a.value[k];
				a.value[k] = (CHUNK_DATA_TYPE)product;
				product >>= BITS_IN_CHUNK;
				++k;
			}
		}
	}
	unsigned numBitsinLast = bitlength%BITS_IN_CHUNK;
	CHUNK_DATA_TYPE bitmask = -1;
	bitmask >>= (BITS_IN_CHUNK-numBitsinLast);
	a.value[numChunks-1] &= bitmask;
	delete [] multiplier;
}

void multiplyMontgomery(const mpuint &aR, const mpuint &bR, mpuint &result, int bitlength, const mpuint &m, const mpuint &mP)
{
	mpuint temp(aR.length+bR.length);
	temp = aR;
	//temp = a' x b'
	temp *= bR;
	// u = (mP(temp mod R))mod R
	mpuint u(temp);
	u.saveBits(bitlength);
	multBits(u,mP,bitlength);
	// u = (temp+modulus*u)/R
	u *= m;
	u += temp;
	u.shiftRight(bitlength);
	if(u >= m)
	{
		u -= m;
	}

	result = u;
}

void Montgomery(const mpuint &base, const mpuint &exponent,
  const mpuint &modulus, mpuint &result)
{
	//find bitsize of modulus
	int bitlength = modulus.length;
	for(int i=modulus.length-1;i>=0;--i)
	{
		if(modulus.value[i] != 0)
			break;
		--bitlength;
	}
	CHUNK_DATA_TYPE chunk = modulus.value[bitlength-1];
	bitlength *= BITS_IN_CHUNK;
	CHUNK_DATA_TYPE bitmask = 1<<(BITS_IN_CHUNK-1);
	while((chunk & bitmask) == 0)
	{
		--bitlength;
		bitmask >>= 1;
	}

	//R = 2^(bitsize+1)
	int rSize = bitlength/BITS_IN_CHUNK+1;
	mpuint R;
	if(rSize%2 == 1)
		R.setSize(rSize+1);
	else
		R.setSize(rSize);
	R = 0;
	R.value[rSize-1] |= 1<<(bitlength%BITS_IN_CHUNK);
	mpuint rP(R.length),mP(R.length);
	mpuint g(R.length);

	//find R^-1 and k (rP,mP)
	EuclideanAlgorithm(R,modulus,rP,mP,g);

	//aR = a*R mod N (base*R mod modulus)
	mpuint temp(base.length+R.length);
	temp = base;
	temp *= R;
	temp %= modulus;
	mpuint aR(R.length),aRCopy(R.length);
	aR = aRCopy = temp;

	//start with the residue of a result of 1
	mpuint res(R.length);
	res = R;
	res %= modulus;

	unsigned chunkID = 0;
	while (chunkID < exponent.length)
	{
		CHUNK_DATA_TYPE bit = 1;
		do
		{
			if (exponent.value[chunkID] & bit)
			{
				multiplyMontgomery(res,aR,res,bitlength,modulus,mP);
			}
			bit <<= 1;
			multiplyMontgomery(aR,aR,aRCopy,bitlength,modulus,mP);
			aR = aRCopy;
		} while (bit != 0);
		++chunkID;
	}

	mpuint resCpy(res.length*2);
	resCpy = res;
	resCpy *= rP;
	resCpy %= modulus;
	result = resCpy;
	result = res;
}

void mpuint::Power(const mpuint &base, const mpuint &exponent,
  const mpuint &modulus, mpuint &result, bool mont)
{
	if(mont)
	{
		Montgomery(base,exponent,modulus,result);
	}
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

//end namespace
}
