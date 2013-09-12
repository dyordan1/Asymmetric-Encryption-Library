#include <iostream>
#include <sstream>
#include "rsa.h"

using namespace std;

mpuint* string_to_mpuint(string message)
{
	short messageLength = message.length();
	mpuint* m = new mpuint((messageLength+1)/2);
	for(int i=0;i < messageLength;i+=2)
	{
		char secondChar;
		if(i+1 == messageLength)
		{
			secondChar = 0x00;
		}
		else
		{
			secondChar = message[i+1];
		}
		m->value[i/2] = secondChar + message[i]*0x100;
	}
	return m;
}

string mpuint_to_string(mpuint encoded)
{
	short padSize = encoded.value[0];
	ostringstream oss;
	for(short i=padSize;i< encoded.length;i++)
	{
		short two = encoded.value[i];
		char char1 = two/0x100;
		char char2 = two%0x100;
		oss << char1;
		if(char2 != 0x00)
		{
			oss << char2;
		}
	}
	return oss.str();
}

mpuint pad_to_size(mpuint* small,short size)
{
	mpuint large = mpuint(size);
	short smallSize = small->length;
	short i=size;
	while(i>smallSize)
	{
		large.value[size-i] = size-smallSize;
		i--;
	}
	for(;i>0;i--)
	{
		large.value[size-i] = small->value[smallSize-i];
	}
	return large;
}

int main()
{
	mpuint d = mpuint(32);
	mpuint e = mpuint(32);
	mpuint n = mpuint(32);
	string originalMessage;
	cout << "Please enter a string:" << endl;
	getline(cin,originalMessage);
	mpuint original = pad_to_size(string_to_mpuint(originalMessage),32);
	mpuint encrypted = mpuint(32);
	mpuint decrypted = mpuint(32);
	GenerateKeys(d,e,n);
	EncryptDecrypt(encrypted,original,e,n);
	EncryptDecrypt(decrypted,encrypted,d,n);
	string decryptedMessage = mpuint_to_string(decrypted);
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