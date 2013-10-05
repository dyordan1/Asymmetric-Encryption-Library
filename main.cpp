#include <iostream>
#include <sstream>
#include <time.h>
#include "rsa.h"
#include "Message.h"
#include "random.h"
#include "mpuint.h"

using namespace std;

int main()
{
	//Get key size
	unsigned short keySize;
	cout << "Please choose key size in bits (will be rounded to nearest chunk):" << endl;
	cin >> keySize;
	keySize /= BITS_IN_CHUNK*4;
	keySize *= 4;

	//clear buffer for getline
	cin.clear();
	cin.sync();

	//create all multi-precision integers
	mpuint d = mpuint(keySize),e = mpuint(keySize),n = mpuint(keySize),p = mpuint(keySize/2),q = mpuint(keySize/2);

	//int numTrials = 1000;
	//int numTests = 100000;
	//clock_t averageClocks = 0;
	//
	//for(int j=0;j<numTrials;++j)
	//{
	//	clock_t tStart = clock();
	//	for(int i=0;i<numTests;++i)
	//	{
	//		PseudoRandom(p);
	//		PseudoRandom(q);
	//		/*n=p;
	//		n += q;
	//		e=n-q;
	//		d=n-p;
	//		if(e != p || d != q)
	//		{
	//			cout << "WRONG" << endl << "\t" << n << endl << "\t" << p << endl << "\t" << q << endl << "\t" << e << endl << "\t" << d << endl;
	//			return 1;
	//		}*/
	//		p %= q;
	//		n=p*q+q-p;
	//		e=n/q;
	//		d=n%q;
	//		if(e != p || d != q-p)
	//		{
	//			cout << "WRONG" << endl << "\t" << n << endl << "\t" << p << endl << "\t" << q << endl << "\t" << e << endl << "\t" << d << endl;
	//			return 1;
	//		}
	//	}
	//	averageClocks += clock() - tStart;
	//}

	//cout << "Operation took on average " << (double)averageClocks/numTrials << " clock cycles";

	//return 0;

	//get string for encryption
	string originalMessage;
	cout << "Please enter a string:" << endl;
	getline(cin,originalMessage);
	Message theMessage = Message(keySize,originalMessage);

	//Generate keys and do encryptions cycle
	GenerateKeys(d,e,n);
	theMessage.encryptMessage(e,n);
	theMessage.decryptMessage(d,n);

	//Get encryption cycle results
	string decryptedMessage = theMessage.extractMessage();
	cout << "The RSA encryption/decryption cycle came up with:" << endl << decryptedMessage << endl;
	if(decryptedMessage == originalMessage)
	{
		cout << "encryption/decryption cycle successful" << endl;
	}
	else
	{
		cout << "encryption/decryption cycle unsucessful" << endl;
	}
}