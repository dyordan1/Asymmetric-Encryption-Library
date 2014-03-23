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
void testArithmetic();
void testFiniteArithmetic();

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
		testArithmetic();
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

		char buff[1501];

		int bytesRead = theECMessage.extractMessage(buff,1500);

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
	cout << "Starting Benchmark..." << endl;

	//all mpuints
	mpuint d1(512/BITS_IN_CHUNK),e1(2),n1(512/BITS_IN_CHUNK);
	mpuint d2(512/BITS_IN_CHUNK),e2(2),n2(512/BITS_IN_CHUNK);
	mpuint d3(512/BITS_IN_CHUNK),e3(512/BITS_IN_CHUNK),n3(512/BITS_IN_CHUNK);
	mpuint d4(1024/BITS_IN_CHUNK),e4(2),n4(1024/BITS_IN_CHUNK);
	mpuint d5(1024/BITS_IN_CHUNK),e5(2),n5(1024/BITS_IN_CHUNK);
	mpuint d6(1024/BITS_IN_CHUNK),e6(1024/BITS_IN_CHUNK),n6(1024/BITS_IN_CHUNK);
	mpuint d7(2048/BITS_IN_CHUNK),e7(2),n7(2048/BITS_IN_CHUNK);
	mpuint d8(2048/BITS_IN_CHUNK),e8(2),n8(2048/BITS_IN_CHUNK);
	mpuint d9(2048/BITS_IN_CHUNK),e9(2048/BITS_IN_CHUNK),n9(2048/BITS_IN_CHUNK);
	mpuint m1(512/BITS_IN_CHUNK), enc1(512/BITS_IN_CHUNK);
	mpuint m2(1024/BITS_IN_CHUNK), enc2(1024/BITS_IN_CHUNK);
	mpuint m3(2048/BITS_IN_CHUNK), enc3(2048/BITS_IN_CHUNK);

	//all clocks
	clock_t rsakey1,rsakey2,rsakey3;
	clock_t rsaenc11,rsaenc12,rsaenc13,rsaenc21,rsaenc22,rsaenc23,rsaenc31,rsaenc32,rsaenc33;
	clock_t rsadec1,rsadec2,rsadec3;
	clock_t eccClocks[4];

	for(int choice=0;choice<4;++choice)
	{
		unsigned keySize = EllipticCurve::sizes[choice]/BITS_IN_CHUNK;

		mpuint prime(EllipticCurve::bases[choice],keySize);
		finite_mpuint a(EllipticCurve::coefficients[choice][0],prime,keySize+2),
						b(EllipticCurve::coefficients[choice][1],prime,keySize+2),
						c(EllipticCurve::coefficients[choice][2],prime,keySize+2),
						x(EllipticCurve::points[choice][0],prime,keySize+2),
						y(EllipticCurve::points[choice][1],prime,keySize+2);
		EllipticCurve ec(a,b,c);
		ECPoint P(ec,x,y);

		finite_mpuint d(keySize+2,prime);
		PseudoRandom(d);
		d %= prime;
		
		ECPoint Q(ec,x,y);

		//ECC Multiplication 192 bit
		//ECC Multiplication 256 bit
		//ECC Multiplication 384 bit
		//ECC Multiplication 512 bit
		eccClocks[choice] = clock();
		Q *= d;
		eccClocks[choice] = clock() - eccClocks[choice];
		cout << "ECC Done." << endl;
	}

	//random messages for RSA
	PseudoRandom(m1);
	PseudoRandom(m2);
	m1.value[512/BITS_IN_CHUNK-1] = 0;
	m2.value[1024/BITS_IN_CHUNK-1] = 0;

	//Key generation RSA 512-bit
	rsakey1 = clock();
	GenerateKeys(d1,e1,n1,3,true);
	GenerateKeys(d2,e2,n2,0xf4,true);
	GenerateKeys(d3,e3,n3,0,true);
	rsakey1 = clock() - rsakey1;
	rsakey1 /= 3;

	//Encrypt RSA 512-bit e=3
	rsaenc11 = clock();
	mpuint::Power(m1,e1,n1,enc1);
	mpuint::Power(m1,e1,n1,enc1);
	mpuint::Power(m1,e1,n1,enc1);
	rsaenc11 = clock() - rsaenc11;
	rsaenc11 /= 3;
	
	//Encrypt RSA 512-bit e=f4
	rsaenc12 = clock();
	mpuint::Power(m1,e2,n2,enc1);
	mpuint::Power(m1,e2,n2,enc1);
	mpuint::Power(m1,e2,n2,enc1);
	rsaenc12 = clock() - rsaenc12;
	rsaenc12 /= 3;

	//Encrypt RSA 512-bit arbitrary e
	rsaenc13 = clock();
	mpuint::Power(m1,e3,n3,enc1);
	mpuint::Power(m1,e3,n3,enc1);
	mpuint::Power(m1,e3,n3,enc1);
	rsaenc13 = clock() - rsaenc13;
	rsaenc13 /= 3;

	//Decrypt RSA 512-bit
	rsadec1 = clock();
	mpuint::Power(enc1,d1,n1,m1);
	mpuint::Power(enc1,d2,n2,m1);
	mpuint::Power(enc1,d3,n3,m1);
	rsadec1 = clock() - rsadec1;
	rsadec1 /= 3;

	cout << "RSA 512 Done." << endl;

	//Key generation RSA 1024-bit
	rsakey2 = clock();
	GenerateKeys(d4,e4,n4,3,true);
	GenerateKeys(d5,e5,n5,0xf4,true);
	GenerateKeys(d6,e6,n6,0,true);
	rsakey2 = clock() - rsakey2;
	rsakey2 /= 3;

	//Encrypt RSA 1024-bit e=3
	rsaenc21 = clock();
	mpuint::Power(m2,e4,n4,enc2);
	mpuint::Power(m2,e4,n4,enc2);
	mpuint::Power(m2,e4,n4,enc2);
	rsaenc21 = clock() - rsaenc21;
	rsaenc21 /= 3;
	
	//Encrypt RSA 1024-bit e=f4
	rsaenc22 = clock();
	mpuint::Power(m2,e5,n5,enc2);
	mpuint::Power(m2,e5,n5,enc2);
	mpuint::Power(m2,e5,n5,enc2);
	rsaenc22 = clock() - rsaenc22;
	rsaenc22 /= 3;

	//Encrypt RSA 1024-bit arbitrary e
	rsaenc23 = clock();
	mpuint::Power(m2,e6,n6,enc2);
	mpuint::Power(m2,e6,n6,enc2);
	mpuint::Power(m2,e6,n6,enc2);
	rsaenc23 = clock() - rsaenc23;
	rsaenc23 /= 3;

	//Decrypt RSA 1024-bit
	rsadec2 = clock();
	mpuint::Power(enc2,d4,n4,m2);
	mpuint::Power(enc2,d5,n5,m2);
	mpuint::Power(enc2,d6,n6,m2);
	rsadec2 = clock() - rsadec2;
	rsadec2 /= 3;

	cout << "RSA 1024 Done." << endl;

	//Key generation RSA 2048-bit
	rsakey3 = clock();
	GenerateKeys(d7,e7,n7,3,true);
	GenerateKeys(d8,e8,n8,0xf4,true);
	GenerateKeys(d9,e9,n9,0,true);
	rsakey3 = clock() - rsakey3;
	rsakey3 /= 3;

	//Encrypt RSA 2048-bit e=3
	rsaenc31 = clock();
	mpuint::Power(m3,e7,n7,enc3);
	mpuint::Power(m3,e7,n7,enc3);
	mpuint::Power(m3,e7,n7,enc3);
	rsaenc31 = clock() - rsaenc31;
	rsaenc31 /= 3;
	
	//Encrypt RSA 2048-bit e=f4
	rsaenc32 = clock();
	mpuint::Power(m3,e8,n8,enc3);
	mpuint::Power(m3,e8,n8,enc3);
	mpuint::Power(m3,e8,n8,enc3);
	rsaenc32 = clock() - rsaenc32;
	rsaenc32 /= 3;

	//Encrypt RSA 2048-bit arbitrary e
	rsaenc33 = clock();
	mpuint::Power(m3,e9,n9,enc3);
	mpuint::Power(m3,e9,n9,enc3);
	mpuint::Power(m3,e9,n9,enc3);
	rsaenc33 = clock() - rsaenc33;
	rsaenc33 /= 3;

	//Decrypt RSA 2048-bit
	rsadec3 = clock();
	mpuint::Power(enc3,d7,n7,m3);
	mpuint::Power(enc3,d8,n8,m3);
	mpuint::Power(enc3,d9,n9,m3);
	rsadec3 = clock() - rsadec3;
	rsadec3 /= 3;

	cout << "RSA 2048 Done." << endl;
	
	cout << "Benchmark results:" << endl << endl;
	cout << "+---------------+---------------+" << endl;
	cout << "|" << setw(15) << "Operation" << "|" << setw(15) << "Time(ms)" << "|" << endl;
	cout << "+---------------+---------------+" << endl;
	cout << "|" << setw(15) << "RSA 512 e=3" << "|" << setw(15) << rsaenc11/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 512 e=F4" << "|" << setw(15) << rsaenc12/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 1024 e=3" << "|" << setw(15) << rsaenc21/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 1024 e=F4" << "|" << setw(15) << rsaenc22/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 2048 e=3" << "|" << setw(15) << rsaenc31/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 2048 e=F4" << "|" << setw(15) << rsaenc32/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 512 d" << "|" << setw(15) << rsadec1/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 1024 d" << "|" << setw(15) << rsadec2/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 2048 d" << "|" << setw(15) << rsadec3/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 512 rand" << "|" << setw(15) << rsaenc13/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 1024 rand" << "|" << setw(15) << rsaenc23/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 2048 rand" << "|" << setw(15) << rsaenc33/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 512 keys" << "|" << setw(15) << rsakey1/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 1024 keys" << "|" << setw(15) << rsakey2/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "RSA 2048 keys" << "|" << setw(15) << rsakey3/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "ECC 192 mul" << "|" << setw(15) << eccClocks[0]/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "ECC 256 mul" << "|" << setw(15) << eccClocks[1]/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "ECC 384 mul" << "|" << setw(15) << eccClocks[2]/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "|" << setw(15) << "ECC 512 mul" << "|" << setw(15) << eccClocks[3]/((double)CLOCKS_PER_SEC/1000) << "|" << endl;
	cout << "+---------------+---------------+" << endl << endl;
}

void testArithmetic()
{
	int keySize = 16;
	mpuint d = mpuint(keySize),e = mpuint(keySize),n = mpuint(keySize);
	mpuint p = mpuint(keySize/2),q = mpuint(keySize/2);
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

void testFiniteArithmetic()
{
	mpuint p("499602D3",2);
	finite_mpuint a(4,p), b(4,p), c(4,p), d(4,p), e(4,p);
	int numTrials = 100;
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
				cout << "WRONG" << endl << "\t" << a << endl << "\t" << b << endl << "\t" << c << endl << "\t" << d << endl << "\t" << e << endl << "\t" << p << endl;
				return;
			}
		}
		averageClocks += clock() - tStart;
	}

	cout << "Operation took on average " << (double)averageClocks/numTrials << " clock cycles";
}
