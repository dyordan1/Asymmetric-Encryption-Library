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
	unsigned messageSize = message.size();
	length = messageSize/(chunkSize-1)+1;
	messageData = new mpuint[length];
	for(int i=0;i<length;i++)
	{
		messageData[i].setSize(chunkSize);
		for(int j=0;j<chunkSize-1;j++)
		{
			if(i*(chunkSize-1)+j >= messageSize)
			{
				unsigned char padSize = chunkSize-j-1;
				for(int k=j;k<chunkSize-1;k++)
				{
					messageData[i].value[k] = padSize;
				}
				break;
			}
			messageData[i].value[j] = message[i*(chunkSize-1)+j];
		}
		messageData[i].value[chunkSize-1] = 0;
	}
}

string Message::extractMessage()
{
	ostringstream extractedStream;
	for(int i=0;i < length-1;i++)
	{
		for(int j=0;j<chunkSize-1;j++)
		{
			extractedStream << messageData[i].value[j];
		}
	}
	for(int j=0;j<chunkSize-1;j++)
	{
		unsigned char theChar = messageData[length-1].value[j];
		if(theChar == chunkSize-j-1)
		{
			bool isPad = true;
			for(int k=j;k<chunkSize-1;k++)
			{
				if(messageData[length-1].value[k] != chunkSize-j-1)
				{
					isPad = false;
				}
			}
			if(isPad)
				break;
		}
		extractedStream << theChar;
	}
	return extractedStream.str();
}

void Message::encryptMessage(const mpuint &e,const mpuint &n)
{
	for(int i=0;i<length;i++)
	{
		mpuint result = mpuint(messageData[i].length);
		mpuint::Power(messageData[i],e,n,result);
		messageData[i] = result;
	}
}

void Message::decryptMessage(const mpuint &d,const mpuint &n,const mpuint &p,const mpuint &q)
{
	mpuint phi = (p-1)*(q-1);
	mpuint pow = d%phi;
	for(int i=0;i<length;i++)
	{
		mpuint result = mpuint(messageData[i].length);
		mpuint::Power(messageData[i],d,n,result);
		messageData[i] = result;
	}
}