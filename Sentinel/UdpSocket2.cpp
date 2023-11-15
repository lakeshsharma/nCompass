#include "stdafx.h"
#include "sys/socket.h"
#include "Utilities.h"
#include "UdpSocket2.h"
#include "sys/epoll.h"
#include "Sentinel.h"
#include "iostream"
using namespace std;

const int READTIMEOUT = 10;

UdpSocket2::UdpSocket2(bool create)
	: m_socket(INVALID_SOCKET)
	, m_isCanceled(false)
{
	if (create)
		Create();
}

UdpSocket2::~UdpSocket2()
{
	Close();
}

int UdpSocket2::Create(const sockaddr_in& addr, bool reuse, bool allowBroadcast, DWORD timeoutMillseconds)
{
	int result = Create(reuse, allowBroadcast, timeoutMillseconds);
	return result == 0 ? Bind(addr) : result;
}

int UdpSocket2::Create(const char *pAddr, unsigned short port, bool reuse, bool allowBroadcast, DWORD timeoutMillseconds)
{
	int result = Create(reuse, allowBroadcast, timeoutMillseconds);
	return result == 0 ? Bind(pAddr, port) : result;
}

int UdpSocket2::Create(in_addr addr, unsigned short port, bool reuse, bool allowBroadcast, DWORD timeoutMillseconds)
{
	int result = Create(reuse, allowBroadcast, timeoutMillseconds);
	return result == 0 ? Bind(addr, port) : result;
}

int UdpSocket2::Create(bool reuse, bool allowBroadcast, DWORD timeoutMillseconds)
{
	if (m_socket != INVALID_SOCKET)
	{
		return (int)ErrConnected;
	}

	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_socket == INVALID_SOCKET)
	{
		return errno;
	}

	int on = 0; //DONT
	setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char *)&on, sizeof(on));

	if (reuse)
	{
		on = 1;
		// "reuse address" must be done before bind
		setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	}

	if (allowBroadcast)
	{
		on = 1;
		setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char *)&on, sizeof(on));
	}

	if (timeoutMillseconds > 0)
	{
		setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeoutMillseconds, sizeof(timeoutMillseconds));
	}

	struct timeval tv;
	tv.tv_sec = READTIMEOUT;  /* 10 Secs Timeout */
	tv.tv_usec = 0;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const void *)&tv, sizeof(struct timeval));

	return 0;
}

int UdpSocket2::Bind(const char *pAddr, unsigned short port)
{
	sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = pAddr ? inet_addr(pAddr) : INADDR_ANY;
	addr.sin_port = htons(port);

	return Bind(addr);
}

int UdpSocket2::Bind(in_addr inaddr, unsigned short port)
{
	sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inaddr.s_addr;
	addr.sin_port = htons(port);

	return Bind(addr);
}

int UdpSocket2::Bind(const sockaddr_in& addr)
{
	if (m_socket == INVALID_SOCKET)
	{
		return (int)ErrInvalidSocket;
	}

	int error = ::bind(m_socket, (struct sockaddr *)&addr, sizeof(addr));
	if (error != 0)
	{
		return errno;
	}

	return 0;
}

int UdpSocket2::SetMulticastInterface(ULONG interfaceAddr)
{
	if (m_socket == INVALID_SOCKET)
		return (int)ErrInvalidSocket;

	int result = setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_IF, (char *)&interfaceAddr, sizeof(interfaceAddr));
	return result ? errno : 0;
}

int UdpSocket2::JoinMulticastGroup(ULONG groupAddr, ULONG interfaceAddr)
{
	ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = groupAddr;
	mreq.imr_interface.s_addr = interfaceAddr;

	int result = setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq));
	if (result != 0)
	{
		result = errno;
	}
	return result;
}

int UdpSocket2::LeaveMulticastGroup(ULONG groupAddr, ULONG interfaceAddr)
{
	ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = groupAddr;
	mreq.imr_interface.s_addr = interfaceAddr;

	int result = setsockopt(m_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq, sizeof(mreq));
	if (result != 0)
		result = errno;

	return result;
}

int UdpSocket2::Close()
{
	if (m_socket == INVALID_SOCKET)
		return (int)ErrInvalidSocket;

	Cancel();
	CUtilities::Sleep(0);

	int result = close(m_socket);
	m_socket = INVALID_SOCKET;
	if (result)
	{
		result = errno;
	}
	return result;
}

int UdpSocket2::Cancel()
{
	m_isCanceled = true;
	return 0;
}

int UdpSocket2::ResetCancel()
{
	m_isCanceled = false;
	return 0;
}

bool UdpSocket2::IsCanceled() const
{
	return m_isCanceled;
}

bool UdpSocket2::IsConnected() const
{
	return m_socket != INVALID_SOCKET;
}

int UdpSocket2::Select(long timeout)
{
	m_readCS.lock();

	fd_set  readSet;
	//readSet.fd_count = 1;
	//readSet.fd_array[0] = m_socket;

	fd_set errorSet;
	//errorSet.fd_count = 1;
	//errorSet.fd_array[0] = m_socket;

	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = timeout <= 0 ? 0 : timeout * 1000;

	int result = select(0, &readSet, nullptr, &errorSet, &tv);
	if (result < 0)
	{
		result = errno;
	}
	else if (result == 0) // timeout
	{
		result = ErrReadTimeout;
	}
	else
	{
		if (FD_ISSET(m_socket, &errorSet))
		{
			result = ErrReadError;
		}
		else if (FD_ISSET(m_socket, &readSet))
		{
			result = 0;
		}
	}

	m_readCS.unlock();

	return m_isCanceled ? ErrReadCanceled : result;
}

int UdpSocket2::Read(char *pBuffer, int bufferLen, int& bytesRead, long timeout, sockaddr *pFrom)
{
	bytesRead = 0;
	int result = 0;
	m_readCS.lock();

	if (m_socket == INVALID_SOCKET)
		return (int)ErrInvalidSocket;

	if (!pBuffer || bufferLen <= 0)
		return (int)ErrReadInvalid;

	if (m_isCanceled)
		return (int)ErrReadCanceled;

#define MAX_EVENTS 10
	struct epoll_event ev, events[MAX_EVENTS];
	int epollfd = epoll_create(1);
	if (epollfd == -1)
	{
		LogString(UDP_LOG, "Error creating epoll(1)");
		result = ErrReadInvalid;
	}

	ev.events = EPOLLIN;
	ev.data.fd = m_socket;
	if (result != ErrReadInvalid && epoll_ctl(epollfd, EPOLL_CTL_ADD, m_socket, &ev) == -1)
	{
		LogString(UDP_LOG, "Error adding epoll ctl");
		result = ErrReadInvalid;
	}

	if (result != ErrReadInvalid)
	{
		result = epoll_wait(epollfd, events, 1, timeout);
		if (result == -1)
		{
			LogString(UDP_LOG,  "UdpSocket::Recv Failed on Select with -1 WSA error: %d", (int)errno);
			Close();
			result = ErrReadInvalid;
		}
		else if (0 == result) //timeout
		{
			LogString(UDP_LOG,  "UdpSocket::Recv Failed on Select with 0-Timeout");
			bytesRead = 0;
			result = ErrReadInvalid;
		}

		if ((events[0].events & EPOLLIN) != 0)
		{
			unsigned int addrSize = pFrom == nullptr ? 0 : sizeof(sockaddr_in);
			ssize_t nbytes;
			if ((nbytes = recvfrom(
						m_socket,
						pBuffer,
						bufferLen,
						0,
						pFrom,
						&addrSize)) <= 0)
			{
				LogString(UDP_LOG,  "UdpSocket::Recv Failed on Recv with errorcode %d", (int)errno);
				bytesRead = 0;
				result =  ErrReadInvalid;
			}
			else
			{
				bytesRead = nbytes;
				result = 0;
			}
		}
	}

	close(epollfd);

	m_readCS.unlock();
	return result;
}

int UdpSocket2::SendTo(sockaddr_in& addr, const void *pData, int dataLen, int flags)
{
	if (m_socket == INVALID_SOCKET)
		return (int)ErrInvalidSocket;

	int result = sendto(m_socket, (char *)pData, dataLen, flags, (struct sockaddr *)&addr, sizeof(addr));
	return result == SOCKET_ERROR ? errno : 0;
}

int UdpSocket2::SendTo(const char *pAddr, unsigned short port, const void *pData, int dataLen, int flags)
{
	if (m_socket == INVALID_SOCKET)
		return (int)ErrInvalidSocket;

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(pAddr);
	addr.sin_port = htons(port);

	return SendTo(addr, pData, dataLen, flags);
}

int UdpSocket2::SendTo(in_addr inaddr, unsigned short port, const void *pData, int dataLen, int flags)
{
	sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inaddr.s_addr;
	addr.sin_port = htons(port);

	return SendTo(addr, pData, dataLen, flags);
}

bool UdpSocket2::IsAddrMulticast(in_addr addr)
{
	return (addr.s_addr & 0x000000f0) == 0x000000e0;
}


