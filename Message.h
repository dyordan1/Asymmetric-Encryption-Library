#pragma once

#include "mpuint.h"

using namespace std;

class Message
{
private:
	unsigned length,chunkSize;
	mpuint* messageData;
	bool isEncrypted;
public:
    Message(unsigned);
    Message(unsigned n,const void* message, unsigned len);
	void embedMessage(const void* message, unsigned len);
	int extractMessage(void* message, unsigned maxLen);
	void encryptMessage(const mpuint &,const mpuint &);
	void decryptMessage(const mpuint &,const mpuint &);
    ~Message();
};