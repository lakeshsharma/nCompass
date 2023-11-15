#include "stdafx.h"

#include "MulticastSocket.h"
#include "Logging/LogString.h"

MulticastSocket::MulticastSocket(void)
{
}

MulticastSocket::~MulticastSocket(void)
{
	if (_socket != INVALID_SOCKET)
    {
        int err = setsockopt(_socket, IPPROTO_IP,IP_DROP_MEMBERSHIP, (char *)&m_mreq, sizeof(m_mreq));
        if (err != 0)
        {
            LogString(ERROR_LOG, "MulticastSocket Drop Membership failed error %d", errno);
        }
    }
}

bool MulticastSocket::Connect(std::string ip, WORD Port)
{
    LogString(UDP_LOG, "MulticastSocket::Connect(%s,%d)", ip.c_str(), Port);

	try
	{
		std::string longIp(ip.data());

		if (UdpSocket::Connect(ip, Port))
		{

            LogString(UDP_LOG, "MulticastSocket::Connect - Connect succeeded - socket created");
			// socket created, make it a multicast socket
			m_mreq.imr_multiaddr.s_addr = inet_addr(ip.c_str());
			m_mreq.imr_interface.s_addr = INADDR_ANY;
			int err = setsockopt(_socket, IPPROTO_IP,IP_ADD_MEMBERSHIP, (char *)&m_mreq, sizeof(m_mreq));
			if (err != 0)
			{
                LogString(UDP_LOG,   "MulticastSocket: Failed to set as multicast %s failed with result %d", longIp.c_str() ,errno);
				LogString(ERROR_LOG, "MulticastSocket: Failed to set as multicast %s failed with result %d", longIp.c_str() ,errno);
				_connected = false;
				Close();
			}
            else
                LogString(UDP_LOG,   "MulticastSocket: Set as multicast %s succeeded", longIp.c_str());
        }
	}
	catch(...)
	{
		LogString(ERROR_LOG, "MulticastSocket: Caught exception setting multicast socket");
		_connected = false;
		Close();
	}

	return _connected; 
}

