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

static bool IsPrime(mpuint &p)
{

	mpuint pminus1(p);
	pminus1 -= 1;
	checkPrimeParam params;
	params.candidate = &p;
	params.candidateminus1 = &pminus1;
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
	mpuint &p = *((mpuint*)point);
	p.value[p.length-1] |= MSB;
	p.value[0] |= 1;
	while (!IsPrime(p))
	p += 2;
	return 0;
}

void GenerateKeys(mpuint &d, mpuint &e, mpuint &n)
{
	mpuint p(d.length/2);
	mpuint q(d.length/2);
	unsigned threadID;
	HANDLE threads[2];
	Random(p);
	threads[0] = (HANDLE)_beginthreadex( NULL, 0, GeneratePrime, &p, 0, &threadID );
	Random(q);
	threads[1] = (HANDLE)_beginthreadex( NULL, 0, GeneratePrime, &q, 0, &threadID );
	Random(d);
	WaitForMultipleObjects(2,threads,true,INFINITE);
	mpuint pp(p);
	pp -= 1;
	mpuint qq(q);
	qq -= 1;
	mpuint pq(d.length);
	pq = pp;
	pq *= qq;
	n = p;
	n *= q;
	mpuint halfPhi(n.length);
	halfPhi = ((p-1)*(q-1))/2;
	d %= halfPhi;
	d += halfPhi;
	mpuint temp(d.length);
	mpuint g(d.length);
	while (true)
	{
	EuclideanAlgorithm(d, pq, e, temp, g);
	if (g == 1)
		break;
	d += 1;
	}
}

//end namespace
}
