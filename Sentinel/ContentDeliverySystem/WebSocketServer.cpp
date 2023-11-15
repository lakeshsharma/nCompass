#include "stdafx.h"
#include "WebSocketServer.h"
#include <netinet/in.h>


CWebSocketServer::CWebSocketServer(unsigned short port, char packetTerminator) : CFlashComServer(port, packetTerminator)
{

}

int CWebSocketServer::Write(BYTE* data, size_t dataLength)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	//function to read from the TCP socket n char
	int iLengthOfWriteBuffer(dataLength);
	int   iWritten = 0, iToWrite = iLengthOfWriteBuffer;
	if (dataLength > 0)
	{
		while (iToWrite)
		{				
			iWritten = send(m_gsClientSocket, data, iToWrite, 0);

			if (SOCKET_ERROR == iWritten)
			{
				int Error = errno;

				if (Error == EWOULDBLOCK)
				{
					iWritten = 0;
				}
				else
				{
					if ((Error == ECONNRESET) ||
						(Error == ENETDOWN) ||
						(Error == ENOTCONN) ||
						(Error == ENETRESET))
					{
						m_gbClientConnectionFailed = true;
					}
					return -1;
				}
			}
			iToWrite -= iWritten;
			data += iWritten;
		}
		return iLengthOfWriteBuffer;
	}
	else
		return 0;//ERROR_NULL_BUFFER;	

}
