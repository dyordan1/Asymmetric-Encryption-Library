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
	bool isEncrypted;
public:
    ECMessage(mpuint &base);
    ECMessage(mpuint &base, const void* message, unsigned len);
	void embedMessage(const void* message, unsigned len);
	int extractMessage(void* message, unsigned maxLen);
	void encryptMessage(const ECPoint &P, const ECPoint &Q);
	void decryptMessage(const finite_mpuint &d);
    ~ECMessage();
};

//end namespace
}

#endif
