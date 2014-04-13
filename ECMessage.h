#ifndef ASYMMEL_EC_MESSAGE_INCLUDED
#define ASYMMEL_EC_MESSAGE_INCLUDED

#include "finite_mpuint.h"
#include "ECPoint.h"

namespace AsymmEL
{

class ECMessage
{
private:
	unsigned length,chunkSize;
	mpuint* base;
	finite_mpuint* messageData;
	ECPoint* messagePoints;
	bool isEncrypted, hasPublic, hasPrivate;
	ECPoint P,Q;
	finite_mpuint d;
public:
    ECMessage(mpuint &base);
    ECMessage(mpuint &base, const void* message, unsigned len);
	void setPublicKey(const ECPoint &P, const ECPoint &Q)
	{
		this->P = P;
		this->Q = Q;
		hasPublic = true;
	}
	void setPrivateKey(const finite_mpuint &d)
	{
		this->d = d;
		hasPrivate = true;
	}
	void embedMessage(const void* message, unsigned len);
	int extractMessage(void* message, unsigned maxLen);
	static unsigned __stdcall encryptPart(void*);
	static unsigned __stdcall decryptPart(void*);
	void encryptMessage();
	void decryptMessage();
    ~ECMessage();
	struct messagePart
	{
		ECMessage* theMessage;
		unsigned begin;
		unsigned end;
	};
};

//end namespace
}

#endif
