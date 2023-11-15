#include "stdafx.h"

#include "ProgressiveProtocol/ProgressiveMulticastChannel.h"
#include "ProgressiveProtocol/ProgressiveMulticastFrame.h"
#include "ProgressiveProtocol/ProgressiveMulticastHost.h"
#include "Sentinel.h"
#include <pthread.h>
#include "Hardware/WatchDog.h"

using namespace std;

#define STOMP_STR			"stomp://"
#define STOMP_STR_LEN		(_countof(STOMP_STR) - 1)

#define UDPSTOMP_STR		"udpstomp://"
#define UDPSTOMP_STR_LEN	(_countof(UDPSTOMP_STR) - 1)

#define BASE_TEN			10

ProgressiveMulticastChannelSocket::ProgressiveMulticastChannelSocket(const string& url)
	: m_url(url)
{
	if (m_url.empty())
		throw SentinelExceptionT<ProgressiveChannelError>(InvalidParameter, "url is empty");

	const char* pUrl = url.c_str();

	if (strncasecmp(pUrl, UDPSTOMP_STR, UDPSTOMP_STR_LEN) == 0)
	{
#ifdef _DEBUG
		m_protocol = "updstomp";
#endif
		if (!ParseUrl(&pUrl[UDPSTOMP_STR_LEN], m_addr, m_port))
			throw SentinelExceptionT<ProgressiveChannelError>(InvalidParameter, (url + ": ParseUrl (UDP)").c_str());

		if (m_port == 0)
			throw SentinelExceptionT<ProgressiveChannelError>(InvalidPort, (url + "Invalid Port (UDP)").c_str());

		int result = Create(m_addr, m_port);
		if (result != 0)
			throw SentinelExceptionT<ProgressiveChannelError>((ProgressiveChannelError) result, (url + "Create (UDP)").c_str());
	}
	else if (strncasecmp(pUrl, STOMP_STR, STOMP_STR_LEN) == 0)
	{
#ifdef _DEBUG
		m_protocol = "stomp";
#endif
		if (!ParseUrl(&pUrl[STOMP_STR_LEN], m_addr, m_port))
			throw SentinelExceptionT<ProgressiveChannelError>(InvalidParameter, (url + "ParseUrl (Multicast)").c_str());

		if ((m_addr.s_addr & 0xe0) != 0xe0)
			throw SentinelExceptionT<ProgressiveChannelError>(InvalidMulticastAddress, (url + "Invalid Multicast Address").c_str());

		int result = Create();
		if (result != 0)
			throw SentinelExceptionT<ProgressiveChannelError>((ProgressiveChannelError) result, (url + "Create (Multicast)").c_str());

		result = Bind(nullptr, m_port);
		if (result != 0)
			throw SentinelExceptionT<ProgressiveChannelError>((ProgressiveChannelError) result, (url + "Bind (Multicast)").c_str());

		result = JoinMulticastGroup(m_addr.s_addr);
		if (result != 0)
			throw SentinelExceptionT<ProgressiveChannelError>((ProgressiveChannelError) result, (url + "JoinMulticastGroup").c_str());
	}
	else
	{
		throw SentinelExceptionT<ProgressiveChannelError>(InvalidProtocol, (url + "Invalid Protocol").c_str());
	}
}

const std::string& ProgressiveMulticastChannelSocket::GetUrl() const
{
	return m_url;
}

bool ProgressiveMulticastChannelSocket::ParseUrl(const char* pStr, in_addr& addr, WORD& port)
{
	const int maxBytes		  = 4;
	const int maxDigitsInWord = 6;

	union
	{
		in_addr	_addr;
		BYTE	b[maxBytes];
	};

	if (!pStr || !*pStr)
		return false;

	for (int part = 0;;)
	{
		int val = 0;
		int i = 0;
		while (i < maxBytes && val <= UCHAR_MAX && isdigit(pStr[i]))
		{
			val *= BASE_TEN;
			val += (int)(pStr[i++] - '0');
		}

		if (i == maxBytes || val > UCHAR_MAX)
			return false;

		pStr += i;

		b[part++] = (unsigned char) val;

		if (*pStr == '/' || *pStr == ':' || *pStr == 0)
		{
			if (part != maxBytes)
				return false;

			break;
		}

		if (*pStr++ != '.')
			return false;

		if (part == maxBytes)
			return false;
	}

	if (*pStr++ != ':')
		return false;

	int val = 0;
	int i = 0;
	while (i < maxDigitsInWord && val <= USHRT_MAX && isdigit(pStr[i]))
	{
		val *= BASE_TEN;
		val += (int)(pStr[i++] - '0');
	}

	if (!(pStr[i] == 0 || pStr[i] == '/') || i == 0 || i == maxDigitsInWord || val > USHRT_MAX)
		return false;

	addr = _addr;
	port = (WORD) val;
	return true;
}

///////////////////////////////////////////////////////////////////////////

unsigned  int ProgressiveMulticastChannel::_thread(ProgressiveMulticastChannel *pClass)
{
	try
	{
		pClass->ThreadMain();
	}
	catch (...) 
	{
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_MULTICAST_CHANNEL);
	}
	return 0;
}

ProgressiveMulticastChannel::ProgressiveMulticastChannel(Socket& socket, ProgressiveMulticastHost* pHost)
	:m_socket(move(socket))
	, m_pHost(pHost)
{

	// The thread will end when this is signaled
	m_hEndEvent.wait();
	
	m_thread =  std::unique_ptr<std::thread>(new std::thread(_thread, this));

}

void ProgressiveMulticastChannel::Close()
{
	m_hEndEvent.signal(false);
	m_socket->Close();
	CUtilities::Sleep(0);
}

const std::string& ProgressiveMulticastChannel::GetUrl() const
{
	return m_socket->GetUrl();
}

void ProgressiveMulticastChannel::ThreadMain()
{
	ProgressiveMulticastFrame frame(m_pHost->m_oneLinkSystemId);
	while (!m_hEndEvent.try_wait())
	{
		sockaddr from; // here for debugging purposes
		int bytesRead = 0;
		int result = m_socket->Read(
			frame.GetBuffer(ProgressiveMulticastFrame::FrameLen),
			ProgressiveMulticastFrame::FrameLen,
			bytesRead,
			/*INFINITE_DURATION,*/
			5000,
			&from);

		// These errors indicate that the socket has been closed; this is by design and not really an error.
		if (result == UdpSocket2::ErrReadCanceled || result == UdpSocket2::ErrInvalidSocket || result == EINTR)
		{
			LogString(PROGRESSIVEUDP, "ProgressiveMulticastChannel::ThreadMain: Socket closed: %d: %s", result, m_socket->GetUrl().c_str());
			LogString(ERROR_LOG, "ProgressiveMulticastChannel::ThreadMain: Socket closed: %d: %s", result, m_socket->GetUrl().c_str());
			CProgressiveConnection::m_multicastSocketFailed = true;
			break;
		}

		if (result == 0 && bytesRead > 0)
		{
			Err err = NullFrame;// To remove the warning of uninitilased variable 
			err = frame.ReleaseBuffer((size_t) bytesRead);
			if (err == Success)
			{
				err = frame.Parse();
				if (err == Success) {
					m_pHost->OnMulticastMessage(frame);
				}
			}
		}
	}
}

