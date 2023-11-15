#include "stdafx.h"
#include "PollerDisplayMessage.h"

CPollerDisplayMessage::CPollerDisplayMessage(int DataLength)
{
	m_lpBuffer = new byte[DataLength];
	m_bufferSize = DataLength;
}

CPollerDisplayMessage::~CPollerDisplayMessage(void)
{
}

std::string CPollerDisplayMessage::GetDisplayString( DisplayMessageType &displayMsgType )
{
	DWORD dwoffset = 0;
	displayMsgType = (DisplayMessageType)0;

	//display message type
	memcpy(&displayMsgType, m_lpBuffer + dwoffset, sizeof(byte));
	dwoffset+= sizeof(byte);

	//text length
	byte stlen;
	memcpy(&stlen, m_lpBuffer + dwoffset, sizeof(stlen));
	dwoffset+=sizeof(stlen);

	char * text = new char[stlen + 1];

	std::string retVal;

	//and the text itself
	memcpy(text, m_lpBuffer + dwoffset, stlen);
	text[stlen] = 0;

	retVal = text;

	delete [] text;
	return retVal;
}
