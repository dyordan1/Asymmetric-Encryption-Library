#include <iostream>
#include <sstream>
#include "rsa.h"
#include "Message.h"
#include "random.h"
#include "mpuint.h"

using namespace std;

//extern "C" {int addFunc(int a, int b);};

int main()
{
	//Get key size
	short keySize;
	cout << "Please choose key size in bits (will be rounded to nearest chunk):" << endl;
	cin >> keySize;
	keySize /= BITS_IN_CHUNK;

	//clear buffer for getline
	cin.clear();
	cin.sync();

	//create all multi-precision integers
	mpuint d = mpuint(keySize),e = mpuint(keySize),n = mpuint(keySize),p = mpuint(keySize/2),q = mpuint(keySize/2);

	/*
	for(int i=0;i<1000;i++)
	{
		Random(p);
		Random(q);
		p %= q;
		n=p*q+q-p;
		e=n/q;
		d=n%q;
		if(e != p || d != q-p)
			cout << "\t\t\tWRONG" << endl << "\t\t\t" << n << endl << "\t\t\t" << p << endl << "\t\t\t" << q << endl << "\t\t\t" << e << endl << "\t\t\t" << d << endl;
	}
	*/

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