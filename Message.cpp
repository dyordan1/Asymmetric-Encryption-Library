#include "Message.h"

Message::Message(unsigned n)
{
	chunkSize = n;
	length = 0;
}
Message::Message(unsigned n,const void* message, unsigned len)
{
	chunkSize = n;
	length = 0;
	embedMessage(message, len);
}

Message::~Message()
{
	delete [] messageData;
}

void Message::embedMessage(const void* message, unsigned len)
{
	//Each chunk contains:
	//  00 F0..FF DATA..
	// |__|__..__|____..|
	// MSB=0 - Padding (to fill partial chunks and avoid weak sections) - Data
	if(length != 0)
		delete [] messageData;
	isEncrypted = false;
	int bytesInChunk = chunkSize*(BITS_IN_CHUNK/8);
	length = len/(bytesInChunk-2)+1;
	messageData = new mpuint[length];
	unsigned char* endOfMessage = ((unsigned char*)message)+len;
	for(int i=0;i<length;i++)
	{
		messageData[i].setSize(chunkSize);
		bool firstPad = true;
		unsigned char* loc = (unsigned char*) (messageData[i].value);
		for(int j=0;j<bytesInChunk-2;j++)
		{
			if(message != endOfMessage)
			{
				*(loc+j) = *(unsigned char*)message;
				message = ((unsigned char*)message)+1;
			}
			else
			{
				if(firstPad)
				{
					*(loc+j) = 0xff;
					firstPad = false;
				}
				else
				{
					*(loc+j) = 0xf0;
				}
			}
		}
		if(firstPad)
		{
			*(loc+bytesInChunk-2) = 0xff;
		}
		else
		{
			*(loc+bytesInChunk-2) = 0xf0;
		}
		*(loc+bytesInChunk-1) = 0x00;
	}
}

int Message::extractMessage(void* message, unsigned maxLen)
{
	if(isEncrypted)
	{
		return -1;
	}
	int pos = 0;
	int bytesInChunk = chunkSize*(BITS_IN_CHUNK/8);
	for(int i=0;i < length;++i)
	{
		unsigned char* loc = (unsigned char*) (messageData[i].value);
		for(int j=0;j<bytesInChunk-2;j++)
		{
			if(*(loc+j) == 0xff)
			{
				bool isPad = true;
				for(int k = j+1;k < bytesInChunk-1;++k)
				{
					if(*(loc+k) != 0xf0)
					{
						isPad = false;
					}
				}
				if(isPad)
				{
					break;
				}
			}
			if(pos >= maxLen)
			{
				return 0;
			}
			*((unsigned char*)message+pos) = *(loc+j);
			++pos;
		}
	}
	return pos;
}

void Message::encryptMessage(const mpuint &e,const mpuint &n)
{
	if(isEncrypted)
		return;
	for(unsigned i=0;i<length;++i)
	{
		mpuint result = mpuint(messageData[i].length);
		mpuint::Power(messageData[i],e,n,result);
		messageData[i] = result;
	}
	isEncrypted = true;
}

void Message::decryptMessage(const mpuint &d,const mpuint &n)
{
	if(!isEncrypted)
		return;
	for(unsigned i=0;i<length;++i)
	{
		mpuint result = mpuint(messageData[i].length);
		mpuint::Power(messageData[i],d,n,result);
		messageData[i] = result;
	}
	isEncrypted = false;
}