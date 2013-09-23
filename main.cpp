#include <iostream>
#include <sstream>
#include "rsa.h"
#include "Message.h"
#include "random.h"
#include "mpuint.h"

using namespace std;

//extern "C" {int addFunc(int a, int b);};

mpuint string_to_mpuint(string message)
{
	short messageLength = message.length();
	mpuint m = mpuint(messageLength);
	for(int i=0;i < messageLength;i++)
	{
		m.value[i] = message[i];
	}
	return m;
}

string mpuint_to_string(mpuint encoded)
{
	unsigned char padSize = encoded.value[0];
	ostringstream oss;
	for(unsigned char i=padSize;i< encoded.length-1;i++)
	{
		unsigned char theChar = encoded.value[i];
		oss << theChar;
	}
	return oss.str();
}

mpuint pad_to_size(const mpuint &small,unsigned char size)
{
	mpuint large = mpuint(size);
	unsigned char smallSize = small.length;
	unsigned char i=size;
	while(i>smallSize)
	{
		large.value[size-i] = size-smallSize;
		i--;
	}
	for(;i>0;i--)
	{
		large.value[size-i] = small.value[smallSize-i];
	}
	return large;
}

int main()
{
	//Get key size
	short keySize;
	cout << "Please choose key size in bits (will be rounded to nearest byte):" << endl;
	cin >> keySize;
	keySize /= 8;

	//clear buffer for getline
	cin.clear();
	cin.sync();

	//create all multi-precision integers
	mpuint d = mpuint(keySize),e = mpuint(keySize),n = mpuint(keySize),p = mpuint(keySize/2),q = mpuint(keySize/2);

	//get string for encryption
	string originalMessage;
	cout << "Please enter a string:" << endl;
	getline(cin,originalMessage);
	Message theMessage = Message(keySize,originalMessage);

	//Generate keys and do encryptions cycle
	GenerateKeys(d,e,n,p,q);
	theMessage.encryptMessage(e,n);
	theMessage.decryptMessage(d,n,p,q);

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