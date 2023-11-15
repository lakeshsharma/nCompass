#include "stdafx.h"
#include "ProgressiveMulticastFrame.h"
#include "EnumMap.h"

using namespace std;


enum MessageHdrKey
{
	OneLinkSystemId,
	Type,
	Content_Type,
	Content_Length
};

static EnumMap<MessageHdrKey>::EnumPair messageHdrKeys[] =
{
	{ OneLinkSystemId, OneLinkHeader::pOneLinkSystemId	},
	{ Type			 , OneLinkHeader::pType				},
	{ Content_Type	 , Header::pContentType				}
};

static EnumMap<MessageHdrKey> messageHdrKeyMap(messageHdrKeys);

ProgressiveMulticastFrame::ProgressiveMulticastFrame(const std::string& id)
	: m_id(id)
	, m_receivedId(false)
	, m_type(FromOneLinkUdp::Invalid)
	, m_pTypeStr(nullptr)
	, m_encryptedBody(false)
{
}

ProgressiveMulticastFrame::~ProgressiveMulticastFrame()
{
}

void ProgressiveMulticastFrame::Reset()
{
	Frame::Reset();
	m_receivedId = false;
	m_type		 = FromOneLinkUdp::Invalid;
	m_pTypeStr	 = nullptr;
	m_encryptedBody = false;
}

Err ProgressiveMulticastFrame::Parse()
{
	Err err = Frame::Parse();
	if (err == Success)
	{
		if (m_type == FromOneLinkUdp::Invalid)
			err = Invalid_content_type;

		if (!m_receivedId)
			err = (Err) InvalidId;
	}
	return err;
}

FromOneLinkUdp::Type ProgressiveMulticastFrame::GetType() const
{
	return m_type;
}

const char* ProgressiveMulticastFrame::GetTypeStr() const
{
	return m_pTypeStr;
}

Err ProgressiveMulticastFrame::OnCommand(RecvCommand::Type command)
{
	return command == RecvCommand::Message ? Success : InvalidCommand;
}

bool ProgressiveMulticastFrame::HasEncryptedBody() const
{
	return m_encryptedBody;
}

Err ProgressiveMulticastFrame::OnHeader(char* pKey, size_t keylen, char* pVal, size_t valLen)
{
    UNREFERENCED_PARAMETER(&keylen); 
    UNREFERENCED_PARAMETER(&valLen); 

	Err result = Frame::SetHeaderValue(pKey, pVal, false);
	if (result == Success)
	{
		switch (messageHdrKeyMap.At(pKey))
		{
			case OneLinkSystemId:
				m_receivedId = true;
				if (strcmp(pVal, m_id.c_str()) != 0)
					result = (Err) InvalidId;
				break;

			case Type:
				m_pTypeStr = pVal;
				m_type = FromOneLinkUdp::emap.At(pVal);
				if (m_type == FromOneLinkUdp::Invalid)
					result = (Err) InvalidType;
				break;

			case Content_Type:
				if (strcmp(pVal, ContentType::pJson) == 0)
				{
					m_encryptedBody = false;
					break;
				}

				//If it is an encrypted message then let callers know body is encrypted.
				if (strcmp(pVal, ContentType::pJsonEnc) == 0)
				{
					m_encryptedBody = true;
					break;
				}
				
				result = Invalid_content_type;
				break;

        default:
            break;
		}
	}
	return result;
}

Err ProgressiveMulticastFrame::OnBody(char* pBody, size_t bodyLen)
{
	if (m_contentLen && m_contentLen.val != bodyLen)
		return Invalid_content_length;

	if (bodyLen == 0)
		return InvalidBodyLen;

	m_pBody   = pBody;
	m_bodyLen = bodyLen;
	return Success;
}