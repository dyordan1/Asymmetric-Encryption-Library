#include "Message.h"
#include <sstream>

Message::Message(unsigned n)
{
	chunkSize = n;
	length = 0;
}
Message::Message(unsigned n,const string &message)
{
	chunkSize = n;
	length = 0;
	embedMessage(message);
}

Message::~Message()
{
	delete [] messageData;
}

void Message::embedMessage(const string &message)
{
	if(length != 0)
		delete [] messageData;
	size_t messageSize = message.size();
	int bytesInChunk = chunkSize*(BITS_IN_CHUNK/8);
	length = messageSize/(bytesInChunk-1)+1;
	messageData = new mpuint[length];
	unsigned char padSize = 0;
	for(int i=length-1;i>=0;i--)
	{
		messageData[i].setSize(chunkSize);
		messageData[i] = 0;
		for(int j=bytesInChunk-2;j>=0;j--)
		{
			unsigned short messageIndex = (length-1-i)*(bytesInChunk-1)+bytesInChunk-2-j;
			messageData[i].value[j/(BITS_IN_CHUNK/8)] <<= 8;
			unsigned char theChar;
			if(messageIndex >= messageSize)
			{
				if(padSize == 0)
					padSize = length*(bytesInChunk-1)-messageIndex;
				theChar = padSize;
			}
			else
			{
				theChar = message[messageIndex];
			}
			messageData[i].value[j/(BITS_IN_CHUNK/8)] |= theChar;
		}
	}
}

string Message::extractMessage()
{
	ostringstream extractedStream;
	int bytesInChunk = chunkSize*(BITS_IN_CHUNK/8);
	for(int i=length-1;i >= 0;i--)
	{
		for(int j=chunkSize-1;j >= 0;j--)
		{
			CHUNK_DATA_TYPE chunk = messageData[i].value[j];
			unsigned short currentByte = BITS_IN_CHUNK/8;
			while(currentByte--)
			{
				unsigned char theChar = chunk >> currentByte*8;
				if(theChar == 0 && j==chunkSize-1)
					continue;
				if(i==0 && (j*(BITS_IN_CHUNK/8)+currentByte+1)%0x100 == theChar)
				{
					goto afterPadding;
				}
				extractedStream << theChar;
			}
		}
	}
	afterPadding:
	return extractedStream.str();
}

void Message::encryptMessage(const mpuint &e,const mpuint &n)
{
	for(unsigned i=0;i<length;i++)
	{
		mpuint result = mpuint(messageData[i].length);
		mpuint::Power(messageData[i],e,n,result);
		messageData[i] = result;
	}
}

void Message::decryptMessage(const mpuint &d,const mpuint &n)
{
	for(unsigned i=0;i<length;i++)
	{
		mpuint result = mpuint(messageData[i].length);
		mpuint::Power(messageData[i],d,n,result);
		messageData[i] = result;
	}
}