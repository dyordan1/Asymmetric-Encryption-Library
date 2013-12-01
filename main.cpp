#include <iostream>
#include <sstream>
#include <time.h>
#include "rsa.h"
#include "Message.h"
#include "ECMessage.h"
#include "random.h"
#include "mpuint.h"
#include "finite_mpuint.h"
#include "ECPoint.h"
#include <string>

using namespace std;

void testArithmetic(mpuint &d,mpuint &e,mpuint &n,mpuint &p,mpuint &q);
void testFiniteArithmetic(finite_mpuint &a,finite_mpuint &b,finite_mpuint &c,finite_mpuint &d,finite_mpuint &e,mpuint &p);

string gen_random(const int len)
{
	ostringstream stream;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i)
	{
        stream << alphanum[rand() % (sizeof(alphanum) - 1)];
    }

	return stream.str();
}

int main()
{
	//Get encryption method
	char method;
	do
	{
		cout << "Would you like to use ECC(E/e) or RSA(R/r)?" << endl;
		cin >> method;
		method = tolower(method);
	} while(method != 'e' && method != 'r');

	//clear buffer for getline
	cin.clear();
	cin.sync();

	////get string for encryption
	//string originalMessage;
	//cout << "Please enter a string:" << endl;
	//getline(cin,originalMessage);
	
	srand(time(NULL));
	string originalMessage = gen_random(10);

	string decryptedMessage;

	if(method == 'r')
	{
		//Get key size
		unsigned short keySize;
		cout << "Please choose key size in bits (will be rounded to nearest chunk):" << endl;
		cin >> keySize;
		keySize /= BITS_IN_CHUNK*4;
		keySize *= 4;

		Message theMessage = Message(keySize,originalMessage.c_str(),originalMessage.size());

		//create all multi-precision integers
		mpuint d = mpuint(keySize),e = mpuint(keySize),n = mpuint(keySize);

		/*
		//test arithmetic
		mpuint p = mpuint(keySize/2),q = mpuint(keySize/2);
		testArithmetic(d,e,n,p,q);
		return 0;
		*/

		//Generate keys and do encryptions cycle
		GenerateKeys(d,e,n);
		theMessage.encryptMessage(e,n);
		theMessage.decryptMessage(d,n);

		char buff[501];

		int bytesRead = theMessage.extractMessage(buff,500);

		buff[bytesRead] = 0;

		decryptedMessage = string(buff);
	}
	else
	{
		/*
		//test arithmetic
		finite_mpuint a(keySize,p),b(keySize,p),c(keySize,p),d(keySize,p),e(keySize,p);
		testFiniteArithmetic(a,b,c,d,e,p);
		return 0;
		*/
		
		unsigned choice;
		do
		{
			cout << "Currently, only NIST pre-determined curves are available." << endl
				<< "Please select one of the below:" << endl;
			for(int i=0;i<4;i++)
			{
				cout << "(" << i << ")" << EllipticCurve::names[i] << endl;
			}
			cin >> choice;
		} while(choice > 3);

		unsigned keySize = EllipticCurve::sizes[choice]/BITS_IN_CHUNK;

		mpuint prime(EllipticCurve::bases[choice],keySize);
		finite_mpuint a(EllipticCurve::coefficients[choice][0],prime,2*keySize),
					  b(EllipticCurve::coefficients[choice][1],prime,2*keySize),
					  c(EllipticCurve::coefficients[choice][2],prime,2*keySize),
					  x(EllipticCurve::points[choice][0],prime,2*keySize),
					  y(EllipticCurve::points[choice][1],prime,2*keySize);
		EllipticCurve ec(a,b,c);
		ECPoint P(ec,x,y);
		
		finite_mpuint d(2*keySize,prime);
		Random(d);
		d %= prime;
		
		ECPoint Q(ec,x,y);
		Q *= d;

		cout << P.x << ":" << P.y << endl
			 << d << endl
			 << Q.x << ":" << Q.y << endl;

		ECMessage theECMessage(prime,originalMessage.c_str(),originalMessage.size());
		
		theECMessage.encryptMessage(P,Q);
		theECMessage.decryptMessage(d);

		char buff[501];

		int bytesRead = theECMessage.extractMessage(buff,500);

		buff[bytesRead] = 0;

		decryptedMessage = string(buff);
	}
	
	//Get encryption cycle results
	cout << "The original string was:" << endl << originalMessage << endl << endl;
	cout << "The encryption/decryption cycle came up with:" << endl << decryptedMessage << endl;
	if(decryptedMessage == originalMessage)
	{
		cout << "encryption/decryption cycle successful" << endl;
	}
	else
	{
		cout << "encryption/decryption cycle unsucessful" << endl;
	}
}

void testArithmetic(mpuint &d,mpuint &e,mpuint &n,mpuint &p,mpuint &q)
{
	int numTrials = 1000;
	int numTests = 1000;
	clock_t averageClocks = 0;
	
	for(int j=0;j<numTrials;++j)
	{
		clock_t tStart = clock();
		for(int i=0;i<numTests;++i)
		{
			PseudoRandom(p);
			PseudoRandom(q);
			//n=p;
			//n += q;
			//e=n-q;
			//d=n-p;
			//if(e != p || d != q)
			//{
			//	cout << "WRONG" << endl << "\t" << n << endl << "\t" << p << endl << "\t" << q << endl << "\t" << e << endl << "\t" << d << endl;
			//	return 1;
			//}
			p %= q;
			n=p*q+q-p;
			e=n/q;
			d=n%q;
			if(e != p || d != q-p)
			{
				cout << "WRONG" << endl << "\t" << n << endl << "\t" << p << endl << "\t" << q << endl << "\t" << e << endl << "\t" << d << endl;
				return;
			}
		}
		averageClocks += clock() - tStart;
	}

	cout << "Operation took on average " << (double)averageClocks/numTrials << " clock cycles";
}

void testFiniteArithmetic(finite_mpuint &a,finite_mpuint &b,finite_mpuint &c,finite_mpuint &d,finite_mpuint &e,mpuint &p)
{
	int numTrials = 1000;
	int numTests = 1000;
	clock_t averageClocks = 0;
	
	for(int j=0;j<numTrials;++j)
	{
		clock_t tStart = clock();
		for(int i=0;i<numTests;++i)
		{
			PseudoRandom(a);
			PseudoRandom(b);
			a%=p;
			b%=p;
			c=a;
			c *= b;
			e=c;
			e/=b;
			d=c;
			d/=a;
			if(e != a || d != b)
			{
				cout << "WRONG" << endl << "\t" << a << endl << "\t" << b << endl << "\t" << c << endl << "\t" << d << endl << "\t" << e << endl;
				return;
			}
		}
		averageClocks += clock() - tStart;
	}

	cout << "Operation took on average " << (double)averageClocks/numTrials << " clock cycles";
}