#include "ECMessage.h"
#include "random.h"

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
	delete [] messageData;
	delete [] messagePoints;
}

void ECMessage::embedMessage(const void* message, unsigned len)
{
	//Each chunk contains:
	//  00..01 DATA..
	// |__..__|____..|
	// Padding (to prevent data collision) - Data
	if(length != 0)
		delete [] messageData;
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
	for(int i=0;i<length;i++)
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
		for(int j=bytesInChunk;j<2*bytesInChunk;++j)
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
	int pos = 0;
	int bytesInChunk = chunkSize*(BITS_IN_CHUNK/8)*2;
	for(int i=0;i < length;++i)
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

void ECMessage::encryptMessage(const ECPoint &P, const ECPoint &Q)
{
	if(isEncrypted)
		return;
	finite_mpuint k(2*base->length,*base);
	for(unsigned i=0;i<length;++i)
	{
		PseudoRandom(k);
		k %= *base;
		
		messagePoints[i].ec = P.ec;
		messagePoints[i].x.setBase(*base);
		messagePoints[i].x = P.x;
		messagePoints[i].y.setBase(*base);
		messagePoints[i].y = P.y;
		messagePoints[i].isInfinite = false;
		messagePoints[i] *= k;

		ECPoint C2(*Q.ec,Q.x,Q.y);
		C2 *= k;

		messageData[i] += C2.x;
	}
	isEncrypted = true;
}

void ECMessage::decryptMessage(const finite_mpuint d)
{
	if(!isEncrypted)
		return;
	for(unsigned i=0;i<length;++i)
	{
		messagePoints[i] *= d;

		messageData[i] -= messagePoints[i].x;
	}
	isEncrypted = false;
}