#pragma once

#include "mpuint.h"

using namespace std;

class Message
{
private:
	unsigned length,chunkSize;
	mpuint* messageData;
public:
    Message(unsigned);
    Message(unsigned,const string &);
	void embedMessage(const string &);
	string extractMessage();
	void encryptMessage(const mpuint &,const mpuint &);
	void decryptMessage(const mpuint &,const mpuint &,const mpuint &,const mpuint &);
    ~Message();
};