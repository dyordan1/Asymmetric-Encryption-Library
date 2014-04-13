#include "ECMessage.h"
#include "random.h"
#include <windows.h>
#include <process.h>

namespace AsymmEL
{

ECMessage::ECMessage(mpuint &_base)
{
	base = &_base;
	chunkSize = base->length;
	length = 0;
}

ECMessage::ECMessage(mpuint &_base, const void* message, unsigned len)
{
	base = &_base;
	chunkSize = base->length;
	length = 0;
	embedMessage(message, len);
}

ECMessage::~ECMessage()
{
	if(length != 0)
	{
		delete [] messageData;
		delete [] messagePoints;
	}
}

void ECMessage::embedMessage(const void* message, unsigned len)
{
	//Each chunk contains:
	//  00..01 DATA..
	// |__..__|____..|
	// Padding (to prevent data collision) - Data
	if(length != 0)
	{
		delete [] messageData;
		delete [] messagePoints;
	}
	isEncrypted = false;
	int bytesInChunk = chunkSize*(BITS_IN_CHUNK/8);
	int usableBytes = bytesInChunk;
	for(int i=base->length-1;i>=0;--i)
	{
		unsigned char * loc = (unsigned char *)&(base->value[i]);
		for(int j=(BITS_IN_CHUNK/8)-1;j>=0;--j)
		{
			if(loc+j != 0)
			{
				usableBytes = i*(BITS_IN_CHUNK/8)+j;
				goto foundBase;
			}
		}
	}
	foundBase:
	length = len/(usableBytes)+1;
	messageData = new finite_mpuint[length];
	messagePoints = new ECPoint[length];
	unsigned char* endOfMessage = ((unsigned char*)message)+len;
	for(unsigned i=0;i<length;i++)
	{
		messageData[i].setBase(*base);
		bool firstPad = true;
		unsigned char* loc = (unsigned char*) (messageData[i].value);
		for(int j=0;j<usableBytes;j++)
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
					*(loc+j) = 0x01;
					firstPad = false;
				}
				else
				{
					*(loc+j) = 0x00;
				}
			}
		}
		for(int j=usableBytes;j<bytesInChunk;++j)
		{
			if(firstPad)
			{
				*(loc+j) = 0x01;
				firstPad = false;
			}
			else
			{
				*(loc+j) = 0x00;
			}
		}
		for(int j=bytesInChunk;j<bytesInChunk+2*(BITS_IN_CHUNK/8);++j)
		{
			*(loc+j) = 0x00;
		}
	}
}

int ECMessage::extractMessage(void* message, unsigned maxLen)
{
	if(isEncrypted)
	{
		return -1;
	}
	unsigned pos = 0;
	int bytesInChunk = (chunkSize+2)*(BITS_IN_CHUNK/8);
	for(unsigned i=0;i < length;++i)
	{
		unsigned char* loc = (unsigned char*) (messageData[i].value);
		for(int j=0;j<bytesInChunk-2;j++)
		{
			if(*(loc+j) == 0x01)
			{
				bool isPad = true;
				for(int k = j+1;k < bytesInChunk-1;++k)
				{
					if(*(loc+k) != 0x00)
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

unsigned __stdcall ECMessage::encryptPart(void *mes)
{
	messagePart &part = *((messagePart *)mes);
	ECMessage &message = *part.theMessage;
	finite_mpuint k(message.base->length+2,*message.base);
	for(unsigned i=part.begin;i<part.end;++i)
	{
		PseudoRandom(k);
		k %= *message.base;
		
		message.messagePoints[i].ec = message.P.ec;
		message.messagePoints[i].x.setBase(*message.base);
		message.messagePoints[i].x = message.P.x;
		message.messagePoints[i].y.setBase(*message.base);
		message.messagePoints[i].y = message.P.y;
		message.messagePoints[i].isInfinite = false;
		message.messagePoints[i] *= k;

		ECPoint C2(*message.Q.ec,message.Q.x,message.Q.y);
		C2 *= k;

		message.messageData[i] += C2.x;
	}
	return 0;
}

unsigned __stdcall ECMessage::decryptPart(void *mes)
{
	messagePart &part = *((messagePart *)mes);
	ECMessage &message = *part.theMessage;
	for(unsigned i=part.begin;i<part.end;++i)
	{
		message.messagePoints[i] *= message.d;
		message.messageData[i] -= message.messagePoints[i].x;
	}
	return 0;
}

void ECMessage::encryptMessage()
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
		threads[i] = (HANDLE)_beginthreadex( NULL, 0, ECMessage::encryptPart, &parts[i], 0, &threadID );
	}
	WaitForMultipleObjects(numThreads,threads,true,INFINITE);
	delete[] threads;
	delete[] parts;
	isEncrypted = true;
}

void ECMessage::decryptMessage()
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
		threads[i] = (HANDLE)_beginthreadex( NULL, 0, ECMessage::decryptPart, &parts[i], 0, &threadID );
	}
	WaitForMultipleObjects(numThreads,threads,true,INFINITE);
	delete[] threads;
	delete[] parts;
	isEncrypted = false;
}

//end namespace
}
