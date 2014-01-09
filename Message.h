#ifndef ASYMMEL_MESSAGE_INCLUDED
#define ASYMMEL_MESSAGE_INCLUDED

#include "mpuint.h"
#include "rsa.h"

namespace AsymmEL
{

class Message
{
private:
	unsigned length,chunkSize;
	mpuint* messageData;
	mpuint d,e,n;
	bool isEncrypted, hasPublic, hasPrivate;
public:
	Message(unsigned);
	Message(unsigned n,const void* message, unsigned len);
	void generateKeys()
	{
		GenerateKeys(d,e,n);
		hasPublic = true;
		hasPrivate = true;
	}
	void setPublicKey(const mpuint &_e,const mpuint &_n)
	{
		e = _e;
		n = _n;
		hasPublic = true;
	}
	void setPrivateKey(const mpuint &_d,const mpuint &_n)
	{
		d = _d;
		n = _n;
		hasPrivate = true;
	}
	void embedMessage(const void* message, unsigned len);
	int extractMessage(void* message, unsigned maxLen);
	void encryptMessage();
	void decryptMessage();
	~Message();
	
	static unsigned __stdcall encryptPart(void*);
	static unsigned __stdcall decryptPart(void*);
	struct messagePart
	{
		Message* theMessage;
		unsigned begin;
		unsigned end;
	};
};

//end namespace
}

#endif
