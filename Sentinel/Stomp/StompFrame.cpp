#include "stdafx.h"
#include "StompFrame.h"

using namespace std;
using namespace Stomp;

Frame::Frame(size_t reserve)
	: m_command(RecvCommand::Invalid)
	, m_pBody(nullptr)
	, m_bodyLen(0)
{
	if (reserve > 0)
		m_frame.reserve(reserve);
}

Frame::~Frame()
{
}

void Frame::Reset(bool resizeToZero)
{
	if (resizeToZero)
		m_frame.resize(0);

	m_command = RecvCommand::Invalid;
	m_pBody = nullptr;
	m_bodyLen = 0;
	m_contentLen.Reset();
	m_headers.clear();
}

void Frame::Empty()
{
	Reset();
	m_frame.clear();
}

size_t Frame::GetBufferLen() const
{
	return m_frame.size();
}

size_t Frame::GetBufferCapacity() const
{
	return m_frame.capacity();
}

char* Frame::GetBuffer(size_t len)
{
	Reset(false);

	if (len == 0 || len > MaxFrameLen)
		return nullptr;

	m_frame.resize(len);
	return &m_frame[0];
}

Err Frame::ReleaseBuffer(size_t len)
{
	Err err = Success;

	if (len == 0)
	{
		err = ZeroLength;
	}
	else if (len > MaxFrameLen || len > m_frame.capacity())
	{
		err = InvalidLength;
	}
	else if (m_frame[0] == 0 || m_frame[len - 1] != 0)
	{
		err = InvalidFrame;
	}
	else
	{
		m_frame.resize(len);
	}

	return err;
}

Err Frame::Parse(Version version)
{
	Reset(false);
	return m_frame.empty() ? ZeroLength :
		Parser::Parse(&m_frame[0], m_frame.size(), version);
}

RecvCommand::Type Frame::GetCommand() const
{
	return m_command;
}

size_t Frame::GetNumHeaders() const
{
	return m_headers.size();
}

const char* Frame::GetHeaderValue(const char* pKey, const char* pDefaultVal) const
{
	HeaderMap::const_iterator it = m_headers.find(pKey);
	return it == m_headers.end() ? pDefaultVal : it->second;
}

Err Frame::OnCommand(RecvCommand::Type command)
{
	m_command = command;
	return Success;
}

Err Frame::SetHeaderValue(const char* pKey, const char* pVal, bool save)
{
	if (save)
	{
		HeaderMap::const_iterator it = m_headers.find(pKey);
		if (it == m_headers.end())
			m_headers[pKey] = pVal;
	}

	if (strcmp(pKey, Header::pContentLength) == 0)
	{
		const int BaseTen = 10;

		size_t contentLen = 0;
		while (*pVal)
		{
			if (!isdigit(*pVal))
				return (Err) Invalid_content_length;

			contentLen *= BaseTen;
			contentLen += (int) (*pVal++ - '0');
		}

		if (contentLen > MaxBodyLen)
			return (Err) Invalid_content_length;

		m_contentLen = contentLen;
	}

	return Success;
}

Err Frame::OnHeader(char* pKey, size_t /*keyLen*/, char* pVal, size_t /*valueLen*/)
{
	return SetHeaderValue(pKey, pVal, true);
}

Err Frame::OnBody(char* pBody, size_t bodyLen)
{
	m_pBody = pBody;
	m_bodyLen = bodyLen;

	return Success;
}

size_t Frame::GetContentLen() const
{
	return m_contentLen.IsValid() ? m_contentLen.val : 0;
}

char* Frame::GetBody()
{
	return m_pBody;
}

size_t Frame::GetBodyLen() const
{
	return m_bodyLen;
}

string Frame::GetBodyAsString() const
{
	return m_pBody != nullptr && m_bodyLen != 0 ? string(m_pBody, m_bodyLen) : string();
}

bool Frame::Add(char ch)
{
	m_frame.push_back(ch);
	return ch == 0;
}



