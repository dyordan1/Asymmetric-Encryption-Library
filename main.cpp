#include <iostream>
#include <iomanip>
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
using namespace AsymmEL;

void benchmarkSpeed();
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
	
	srand((unsigned)time(NULL));
	string originalMessage = gen_random(1500);

	string decryptedMessage;

	if(method == 'r')
	{
		//Get key size
		unsigned short keySize;
		cout << "Please choose key size in bits (will be rounded to nearest chunk):" << endl;
		cin >> keySize;
		keySize /= BITS_IN_CHUNK*4;
		keySize *= 4;

		Message theMessage = Message(keySize,originalMessage.c_str(),(unsigned)originalMessage.size());

		/*
		//test arithmetic
		mpuint d = mpuint(keySize),e = mpuint(keySize),n = mpuint(keySize);
		mpuint p = mpuint(keySize/2),q = mpuint(keySize/2);
		testArithmetic(d,e,n,p,q);
		return 0;
		*/

		//Generate keys and do encryptions cycle
		theMessage.generateKeys();
		theMessage.encryptMessage();
		theMessage.decryptMessage();

		char* buff = new char[1501];

		int bytesRead = theMessage.extractMessage(buff,1500);

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

		ECMessage theECMessage(prime,originalMessage.c_str(),(unsigned)originalMessage.size());
		
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

void benchmarkSpeed()
{
	//all mpuints
	mpuint d1(512/BITS_IN_CHUNK),e1(512/BITS_IN_CHUNK),n1(512/BITS_IN_CHUNK);
	mpuint d2(512/BITS_IN_CHUNK),e2(512/BITS_IN_CHUNK),n2(512/BITS_IN_CHUNK);
	mpuint d3(512/BITS_IN_CHUNK),e3(512/BITS_IN_CHUNK),n3(512/BITS_IN_CHUNK);
	mpuint d4(1024/BITS_IN_CHUNK),e4(1024/BITS_IN_CHUNK),n4(1024/BITS_IN_CHUNK);
	mpuint d5(1024/BITS_IN_CHUNK),e5(1024/BITS_IN_CHUNK),n5(1024/BITS_IN_CHUNK);
	mpuint d6(1024/BITS_IN_CHUNK),e6(1024/BITS_IN_CHUNK),n6(1024/BITS_IN_CHUNK);
	mpuint m1(512/BITS_IN_CHUNK), enc1(512/BITS_IN_CHUNK);
	mpuint m2(1024/BITS_IN_CHUNK), enc2(1024/BITS_IN_CHUNK);

	//all clocks
	clock_t rsakey1,rsakey2;
	clock_t rsaenc11,rsaenc12,rsaenc13,rsaenc21,rsaenc22,rsaenc23;
	clock_t rsadec1,rsadec2;

	//random messages for RSA
	PseudoRandom(m1);
	PseudoRandom(m2);
	m1.value[512/BITS_IN_CHUNK-1] = 0;
	m2.value[1024/BITS_IN_CHUNK-1] = 0;

	//Key generation RSA 512-bit
	rsakey1 = clock();
	GenerateKeys(d1,e1,n1,3);
	GenerateKeys(d2,e2,n2,0xf4);
	GenerateKeys(d3,e3,n3);
	rsakey1 = clock() - rsakey1;

	//Encrypt RSA 512-bit e=3
	rsaenc11 = clock();
	mpuint::Power(m1,e1,n1,enc1);
	mpuint::Power(m1,e1,n1,enc1);
	mpuint::Power(m1,e1,n1,enc1);
	rsaenc11 = clock() - rsaenc11;
	
	//Encrypt RSA 512-bit e=f4
	rsaenc12 = clock();
	mpuint::Power(m1,e2,n2,enc1);
	mpuint::Power(m1,e2,n2,enc1);
	mpuint::Power(m1,e2,n2,enc1);
	rsaenc12 = clock() - rsaenc12;

	//Encrypt RSA 512-bit arbitrary e
	rsaenc13 = clock();
	mpuint::Power(m1,e3,n3,enc1);
	mpuint::Power(m1,e3,n3,enc1);
	mpuint::Power(m1,e3,n3,enc1);
	rsaenc13 = clock() - rsaenc13;

	//Decrypt RSA 512-bit
	rsadec1 = clock();
	mpuint::Power(enc1,d1,n1,m1);
	mpuint::Power(enc1,d2,n2,m1);
	mpuint::Power(enc1,d3,n3,m1);
	rsadec1 = clock() - rsadec1;

	//Key generation RSA 1024-bit
	rsakey2 = clock();
	GenerateKeys(d4,e4,n4,3);
	GenerateKeys(d5,e5,n5,0xf4);
	GenerateKeys(d6,e6,n6);
	rsakey2 = clock() - rsakey2;

	//Encrypt RSA 1024-bit e=3
	rsaenc21 = clock();
	mpuint::Power(m2,e4,n4,enc2);
	mpuint::Power(m2,e4,n4,enc2);
	mpuint::Power(m2,e4,n4,enc2);
	rsaenc21 = clock() - rsaenc21;
	
	//Encrypt RSA 1024-bit e=f4
	rsaenc22 = clock();
	mpuint::Power(m2,e5,n5,enc2);
	mpuint::Power(m2,e5,n5,enc2);
	mpuint::Power(m2,e5,n5,enc2);
	rsaenc22 = clock() - rsaenc22;

	//Encrypt RSA 1024-bit arbitrary e
	rsaenc23 = clock();
	mpuint::Power(m2,e6,n6,enc2);
	mpuint::Power(m2,e6,n6,enc2);
	mpuint::Power(m2,e6,n6,enc2);
	rsaenc23 = clock() - rsaenc23;

	//Decrypt RSA 1024-bit
	rsadec2 = clock();
	mpuint::Power(enc2,d4,n4,m2);
	mpuint::Power(enc2,d5,n5,m2);
	mpuint::Power(enc2,d6,n6,m2);
	rsadec2 = clock() - rsadec2;

	//ECC Multiplication 192 bit

	//ECC Multiplication 256 bit

	//ECC Multiplication 384 bit

	//ECC Multiplication 512 bit
	
	cout << "Benchmark results:" << endl << endl;
	cout << "+---------------+---------------+" << endl;
	cout << "|" << setw(15) << "Operation" << "|" << setw(15) << "Time(ms)" << "|" << endl;
	cout << "+---------------+---------------+" << endl;
	cout << "|" << setw(15) << "RSA 512 e=3" << "|" << setw(15) << rsaenc11/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 512 e=F4" << "|" << setw(15) << rsaenc12/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 1024 e=3" << "|" << setw(15) << rsaenc21/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 1024 e=F4" << "|" << setw(15) << rsaenc22/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 512 d" << "|" << setw(15) << rsadec1/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 1024 d" << "|" << setw(15) << rsadec2/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 512 rand" << "|" << setw(15) << rsaenc13/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 1024 rand" << "|" << setw(15) << rsaenc23/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 512 keys" << "|" << setw(15) << rsakey1/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 1024 keys" << "|" << setw(15) << rsakey2/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "+---------------+---------------+" << endl << endl;
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