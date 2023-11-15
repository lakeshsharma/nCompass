#include "stdafx.h"
#include "UdpSocket.h"
#include "Sentinel.h"
#include "sys/epoll.h"

const int READTIMEOUT = 10;

//This constructor is genarlly used for a non listening UdpSocket,
// or if someone doesnt want to set up there own socket.
UdpSocket::UdpSocket() : _socket(INVALID_SOCKET), _listener(INVALID_SOCKET), _connected(false), _cancelall(false), _port(0)
{
	FD_ZERO(&read_fds);
	FD_ZERO(&readerror_fds);

	_reading = false;
}

//This constructor is generally used by UdpSocketListener.
//The Socket is Listened for then UdpSocket is created with SOCKET in construct.
UdpSocket::UdpSocket(SOCKET sckt) : _socket(INVALID_SOCKET), _listener(INVALID_SOCKET), _connected(false), _cancelall(false), _port(0)
{
	tv.tv_sec = 0;
	tv.tv_usec = 1000000;
	FD_ZERO(&read_fds);
	FD_ZERO(&readerror_fds);

	if (INVALID_SOCKET == sckt)
	{
		_connected = false;
		return;
	}

	_socket = sckt;

	FD_SET(_socket, &readerror_fds);
	FD_SET(_socket, &read_fds);
	_connected = true;

	_reading = false;

	LogString(UDP_LOG,   "UdpSocket::UdpSocket() _connected = true");
}

//This destructor will close the socket, the handle to the cancel event and Clean up the WSA.
UdpSocket::~UdpSocket()
{
	try
	{
		Close();
	}
	catch (...)
	{/*Here just so a destructor does not throw an exception*/}
}


//Called to connect to a remote computer
bool UdpSocket::Connect(std::string HostName, WORD Port)
{
	LogString(UDP_LOG,   "UdpSocket::Connect(%s,%d), in", HostName.c_str(), Port);

	try
	{
		_connectioncritsec.lock();
		_cancelall = false;
		_port = Port;
		loc.sin_port	= htons(Port);
		loc.sin_family = AF_INET;
		loc.sin_addr.s_addr = INADDR_ANY; //*/inet_addr(HostName.c_str());   //

		remoteaddr.sin_port = 0;
		remoteaddr.sin_family = AF_INET;
		remoteaddr.sin_addr.s_addr = inet_addr(HostName.c_str());

		if (false == _connected)
		{
			LogString(UDP_LOG,   "UdpSocket::Connect(), not connected, creating socket");
			_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			if (INVALID_SOCKET == _socket)
			{
				LogString(UDP_LOG,   "UdpSocket::Connect(), not connected, creating socket FAILED, exiting");
				_connected = false;
				Close();
			}
			LogString(UDP_LOG,   "UdpSocket::Connect(), not connected, creating socket succeeded, binding");
			int reuse = 1;
			setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuse, sizeof(reuse));
			int broadcast = 1;
			setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, (const void *)&broadcast, sizeof(broadcast));

			struct timeval tv;
			tv.tv_sec = READTIMEOUT;  /* 10 Secs Timeout */
			tv.tv_usec = 0;
			setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (const void *)&tv, sizeof(struct timeval));
			
			int error = ::bind(_socket, (struct sockaddr *)&loc, sizeof(loc));
			if (0 != error)
			{
				LogString(ERROR_LOG, "UdpSocket::Connect BIND FAILED %d", (int)errno);
				LogString(UDP_LOG,   "UdpSocket::Connect(), bind failed, exiting");
				_connected = false;
				Close();
			}
			else
			{
				LogString(UDP_LOG,   "UdpSocket::Connect(), bind succeeded, setting read fds");
				FD_SET(_socket, &read_fds);
				FD_SET(_socket, &readerror_fds);
				_connected = true;
				_cancelall = false;
				//LogString(FLASHING, "UdpSocket::Connect CONNECTED");
				LogString(UDP_LOG,   "UdpSocket::Connect(), connected");
			}
		}
		_connectioncritsec.unlock();
	}
	catch (...)
	{
		_connectioncritsec.unlock();
		LogString(ERROR_LOG, "UdpSocket Caught Exception Trying to connect to socket");
		LogString(UDP_LOG,   "UdpSocket Caught Exception Trying to connect to socket");
		_connected = false;
		Close();
	}

	return _connected;
}


//called to close a connection
void UdpSocket::Close()
{
	LogString(UDP_LOG,  "UdpSocket::Close");
	_cancelall = true;
	if (_socket != INVALID_SOCKET)
	{
		FD_CLR(_socket, &read_fds);
		FD_CLR(_socket, &readerror_fds);
		int error = close(_socket);
		if (error == SOCKET_ERROR)
		{
			error = (int)errno;
			LogString(ERROR_LOG, "UdpSocket Failure Closing Socket - %d", error);
			LogString(UDP_LOG,   "UdpSocket Failure Closing Socket - %d", error);
		}

		_socket = INVALID_SOCKET;
	}
	_connected = false;
}




UdpReadStatus UdpSocket::Read(char *buftorecv, long maxlengthofdatatorecv, DWORD& bytesrecv, long timeout)
{
	LogString(UDP_LOG,  "UdpSocket::Read in - _socket %d", _socket);

	UdpReadStatus ireturn = UDP_READ_TIMEOUT;
	int nbytes = bytesrecv = 0;

	try
	{
		_reading = true; 
		_recvcritsec.lock();

		#define MAX_EVENTS 10
		struct epoll_event ev, events[MAX_EVENTS];
		int epollfd = epoll_create(1);
		if (epollfd == -1)
		{
			LogString(UDP_LOG, "Error creating epoll(1)");
			ireturn = UDP_READ_ERROR;
		}
		
		ev.events = EPOLLIN;
		ev.data.fd = _socket;
		if (ireturn != UDP_READ_ERROR && epoll_ctl(epollfd, EPOLL_CTL_ADD, _socket, &ev) == -1)
		{
			LogString(UDP_LOG, "Error adding epoll ctl");
			ireturn = UDP_READ_ERROR;
		}
		
		if (ireturn != UDP_READ_ERROR)
		{
			int result;
			result = epoll_wait(epollfd, events, 1, timeout);
			if (result == -1)
			{
	
				LogString(UDP_LOG,  "UdpSocket::Recv Failed on Select with -1 WSA error: %d", (int)errno);
				Close();
				_recvcritsec.unlock();
				_reading = false;
				ireturn = UDP_READ_ERROR;
			}
			else if (0 == result) //timeout
			{
				LogString(UDP_LOG,  "UdpSocket::Recv Failed on Select with 0-Timeout");
				bytesrecv = 0;
				_recvcritsec.unlock();
				_reading = false;
                close(epollfd);
				return UDP_READ_TIMEOUT;
			}
	
			if ((events[0].events & EPOLLIN) != 0)
			{
				unsigned int addrSize = sizeof(remoteaddr);
				if ((nbytes = recvfrom(
							_socket,
							(char *)buftorecv,
							maxlengthofdatatorecv,
							0,
							(sockaddr *)&remoteaddr,
							&addrSize)) <= 0)
				{
					LogString(UDP_LOG,  "UdpSocket::Recv Failed on Recv with errorcode %d", (int)errno);
					bytesrecv = 0;
					_recvcritsec.unlock();
					_reading = false;
                    close(epollfd);
					return UDP_READ_ERROR;
				}
				else
				{
					bytesrecv = nbytes;
					ireturn = UDP_READ_SUCCESS;
				}
			}
		}
		
		close(epollfd);


		_recvcritsec.unlock();
		_reading = false;
	}
	catch (...)
	{
		Close();
		_recvcritsec.unlock();
		_reading = false;
	}
	_reading = false;
	LogString(UDP_LOG,  "UdpSocket::Read out");
	return ireturn;
}


void UdpSocket::Cancel()
{
	_cancelall = true;
}

bool UdpSocket::Connected()
{
	return _connected;
}

bool UdpSocket::Reading()
{
	return _reading;
}


