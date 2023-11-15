#pragma once

#include "UdpSocket.h"

class MulticastSocket :
	public UdpSocket
{
public:
	MulticastSocket(void);
	virtual ~MulticastSocket(void);
	virtual bool Connect(std::string HostName, WORD Port);

private:
	ip_mreq m_mreq;
};


