#include <stdlib.h>
#include "rsa.h"
#include "euclid.h"
#include "random.h"
#include <windows.h>
#include <process.h>

namespace AsymmEL
{

/*----------------------------------------------------------------------------
This function uses Fermat's Theorem 100 times to test the primeness of a
(large) positive integer.
----------------------------------------------------------------------------*/

struct checkPrimeParam
{
	mpuint* candidate;
	mpuint* candidateminus1;
	volatile boolean isPrime;
};

struct generatePrimeParam
{
	mpuint* candidate;
	mpuint* candidateminus1;
	CHUNK_DATA_TYPE* e;
};

unsigned __stdcall checkPrime(void* _params)
{
	checkPrimeParam &params = *((checkPrimeParam*)_params);
	mpuint &p = *params.candidate;
	mpuint &pminus1 = *params.candidateminus1;
	mpuint r(pminus1.length);
	mpuint x(pminus1.length);
	unsigned count = 500/MAX_THREADS;
	while (--count != 0)
	{
		if(params.isPrime == false)
		{
			_endthreadex(0);
		}
		PseudoRandom(x);
		x %= p;
		if (x != 0)
		{
			mpuint::Power(x, pminus1, p, r);
			if (r != 1)
			{
				params.isPrime = false;
				return 0;
			}
		}
	}
	return 0;
}

static bool IsPrime(mpuint &p, mpuint &pp)
{

	checkPrimeParam params;
	params.candidate = &p;
	params.candidateminus1 = &pp;
	params.isPrime = true;
	unsigned threadID;
#if MAX_THREADS>1
	const unsigned MAX_THREADS_HALF = MAX_THREADS/2;
#else
	const unsigned MAX_THREADS_HALF = 1;
#endif
	HANDLE threads[MAX_THREADS_HALF];
	for(int i=0;i<MAX_THREADS_HALF;i++)
	{
		threads[i] = (HANDLE)_beginthreadex( NULL, 0, checkPrime, &params, 0, &threadID );
	}
	WaitForMultipleObjects(MAX_THREADS_HALF,threads,true,INFINITE);
	if(params.isPrime == false)
	{
		return false;
	}
	return true;
}

/*----------------------------------------------------------------------------
This function generates a (large) prime.
----------------------------------------------------------------------------*/

unsigned __stdcall GeneratePrime(void* point)
{
	generatePrimeParam &params = *((generatePrimeParam*)point);
	mpuint &p = *params.candidate;
	mpuint &pp = *params.candidateminus1;
	CHUNK_DATA_TYPE &e = *params.e;
	p.value[p.length-1] |= MSB;
	p.value[0] |= 1;
	pp = p;
	pp -= 1;
	while (!IsPrime(p,pp) || (e!= 0 && (p-1)%e == 0) )
	{
		p += 2;
		pp += 2;
	}
	return 0;
}

void GenerateKeys(mpuint &d, mpuint &e, mpuint &n, CHUNK_DATA_TYPE customE, bool pseudoKeys)
{
	mpuint p(d.length/2);
	mpuint q(d.length/2);
	mpuint pp(p.length);
	mpuint qq(q.length);
	mpuint pq(d.length);
	if(pseudoKeys)
	{
		PseudoRandom(p);
		PseudoRandom(q);
	}
	else
	{
		Random(p);
		Random(q);
	}
	e = customE;
	unsigned threadID;
	HANDLE threads[2];
	generatePrimeParam paramP;
	paramP.candidate = &p;
	paramP.candidateminus1 = &pp;
	paramP.e = &customE;
	threads[0] = (HANDLE)_beginthreadex( NULL, 0, GeneratePrime, &paramP, 0, &threadID );
	generatePrimeParam paramQ;
	paramQ.candidate = &q;
	paramQ.candidateminus1 = &qq;
	paramQ.e = &customE;
	threads[1] = (HANDLE)_beginthreadex( NULL, 0, GeneratePrime, &paramQ, 0, &threadID );
	WaitForMultipleObjects(2,threads,true,INFINITE);
	pq = pp;
	pq *= qq;
	n = p;
	n *= q;
	mpuint temp(d.length);
	mpuint g(d.length);
	if(customE == 0)
	{
		if(pseudoKeys)
			PseudoRandom(d);
		else
			Random(d);
		while (true)
		{
			EuclideanAlgorithm(d, pq, e, temp, g);
			if (g == 1)
				break;
			d += 1;
		}
	}
	else
	{
		EuclideanAlgorithm(e, pq, d, temp, g);
	}
}

//end namespace
}
