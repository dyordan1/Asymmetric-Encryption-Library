#include "Message.h"
#include <windows.h>
#include <process.h>

namespace AsymmEL
{

Message::Message(unsigned len)
{
	chunkSize = len;
	d.setSize(len);
	e.setSize(len);
	n.setSize(len);
	length = 0;
	hasPublic = false;
	hasPrivate = false;
	isEncrypted = false;
}
Message::Message(unsigned len,const void* message, unsigned size)
{
	chunkSize = len;
	d.setSize(len);
	e.setSize(len);
	n.setSize(len);
	length = 0;
	hasPublic = false;
	hasPrivate = false;
	embedMessage(message, size);
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
	for(unsigned i=0;i<length;i++)
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
	unsigned pos = 0;
	int bytesInChunk = chunkSize*(BITS_IN_CHUNK/8);
	for(unsigned i=0;i < length;++i)
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

unsigned __stdcall Message::encryptPart(void *mes)
{
	messagePart &part = *((messagePart *)mes);
	Message &message = *part.theMessage;
	for(unsigned i=part.begin;i<part.end;++i)
	{
		mpuint result = mpuint(message.messageData[i].length);
		mpuint::Power(message.messageData[i],message.e,message.n,result);
		message.messageData[i] = result;
	}
	return 0;
}

unsigned __stdcall Message::decryptPart(void *mes)
{
	messagePart &part = *((messagePart *)mes);
	Message &message = *part.theMessage;
	for(unsigned i=part.begin;i<part.end;++i)
	{
		mpuint result = mpuint(message.messageData[i].length);
		mpuint::Power(message.messageData[i],message.d,message.n,result);
		message.messageData[i] = result;
	}
	return 0;
}

void Message::encryptMessage()
{
	if(length == 0 || isEncrypted || !hasPublic)
		return;
	int numThreads = length;
	if(numThreads > MAX_THREADS)
		numThreads = MAX_THREADS;
    unsigned threadID;
	HANDLE* threads = new HANDLE[numThreads];
	messagePart* parts = new messagePart[numThreads];
	for(int i=0;i<numThreads;i++)
	{
		parts[i].theMessage = this;
		parts[i].begin = (length*i)/numThreads;
		parts[i].end = (length*(i+1))/numThreads;
		threads[i] = (HANDLE)_beginthreadex( NULL, 0, Message::encryptPart, &parts[i], 0, &threadID );
	}
	WaitForMultipleObjects(numThreads,threads,true,INFINITE);
	delete[] threads;
	delete[] parts;
	isEncrypted = true;
}

void Message::decryptMessage()
{
	if(!isEncrypted || !hasPrivate)
		return;
	int numThreads = length;
	if(numThreads > MAX_THREADS)
		numThreads = MAX_THREADS;
    unsigned threadID;
	HANDLE* threads = new HANDLE[numThreads];
	messagePart* parts = new messagePart[numThreads];
	for(int i=0;i<numThreads;i++)
	{
		parts[i].theMessage = this;
		parts[i].begin = (length*i)/numThreads;
		parts[i].end = (length*(i+1))/numThreads;
		threads[i] = (HANDLE)_beginthreadex( NULL, 0, Message::decryptPart, &parts[i], 0, &threadID );
	}
	WaitForMultipleObjects(numThreads,threads,true,INFINITE);
	delete[] threads;
	delete[] parts;
	isEncrypted = false;
}

//end namespace
}
