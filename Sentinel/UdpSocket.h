#ifndef _UDPSOCKET_H_
#define _UDPSOCKET_H_

typedef enum
{
	UDP_READ_ERROR = 0,
	UDP_READ_TIMEOUT,
	UDP_READ_SUCCESS,
} UdpReadStatus;


class UdpSocket
{
	protected:
		SOCKET _socket;
		SOCKET _listener;
		fd_set read_fds;
		fd_set readerror_fds;
		timeval tv;
		bool _connected;
		struct sockaddr_in loc;
		struct sockaddr_in remoteaddr;
		std::recursive_mutex _sendcritsec;
		std::recursive_mutex _recvcritsec;
		std::recursive_mutex _connectioncritsec;
		bool _cancelall;
		int _port;
		bool _reading;

	public:
		UdpSocket();
		UdpSocket(SOCKET sckt);
		virtual ~UdpSocket();
		virtual bool Connect(std::string HostName, WORD Port);
		void Close();
		void Cancel();
		bool Connected();
		bool Reading();
		void ResetCancel();
		UdpReadStatus Read(char *buftorecv, long maxlengthofdatatorecv, DWORD &bytesrecv, long timeout = 0);
};

#endif // _UDPSOCKET_H_





