#include "stdafx.h"
#include "FlashComServer.h"
#include "../Utilities.h"
#include "errno.h"

#include "Logging/LogString.h"

using namespace std;

//#define IP "127.0.0.1"
//#define IP "10.156.76.41"
#define IP "0.0.0.0"

CFlashComServer::CFlashComServer(unsigned short port, char packetTerminator) :
	m_buffer(new unsigned char[WEBSOCKET_MAX_LENGTH])
{
	LogString(SOCKETSERVER_LOG, "[CFlashComServer::CFlashComServer] Socket Server Object created with port %d", port);
	m_port = port;
	m_usesStartChar = false;
	m_endChar = packetTerminator;
}

CFlashComServer::~CFlashComServer(void)
{
	Stop();
}

int CFlashComServer::Initialize()
{
	struct sockaddr_in serverSocketAddress;
	int         iReturnValue = 0;
	char ipAddress[16] = IP;


	SetSystemReuse();

	ResetBufferToZero();
	m_startIndexfound = false;
	m_endIndexfound = false;

	m_gbClientConnectionFailed = false;

	m_gsClientSocket = INVALID_SOCKET;

	LogString(SOCKETSERVER_LOG, "Creating listening socket.");

	int bindFailCount = 0;
	do
	{
		m_gsServerSocket = socket(AF_INET, SOCK_STREAM, 0);
		LogString(SOCKETSERVER_LOG, "Server Socket %d Created.", m_gsServerSocket);
		if (m_gsServerSocket == INVALID_SOCKET)
		{
			iReturnValue = errno;

			LogString(SOCKETSERVER_LOG, "[CFlashComServer::Initialize]: Socket Create Failed! errno = %d", iReturnValue);
			LogString(ERROR_LOG, "[CFlashComServer::Initialize]: Socket Create Failed! errno = %d", iReturnValue);
			return iReturnValue;
		}

		int reuse = 1;
		setsockopt(m_gsServerSocket, SOL_SOCKET, SO_REUSEADDR, (const void*)&reuse, sizeof(reuse));

		serverSocketAddress.sin_family = AF_INET;
		serverSocketAddress.sin_port = htons(m_port);
		serverSocketAddress.sin_addr.s_addr = inet_addr(ipAddress);

		LogString(SOCKETSERVER_LOG, "Binding listening socket.");

		iReturnValue = ::bind(m_gsServerSocket, (sockaddr*)&serverSocketAddress, sizeof(serverSocketAddress));
		if (SOCKET_ERROR == iReturnValue)
		{
			iReturnValue = errno;

			LogString(SOCKETSERVER_LOG, "[CFlashComServer::Initialize]: Bind Failed! Try #: %d, Error: %d", 
				++bindFailCount, iReturnValue);
			LogString(ERROR_LOG, "[CFlashComServer::Initialize]: Bind Failed! Try #: %d, Error: %d", 
				++bindFailCount, iReturnValue);
			if (EADDRINUSE == iReturnValue)
			{
				close(m_gsServerSocket);
				CUtilities::Sleep(1000);
			}
		}
	} while (EADDRINUSE == iReturnValue);

	iReturnValue = Listen();

	return iReturnValue;
}

int CFlashComServer::Listen()
{
	int iReturnValue = 0;

	LogString(SOCKETSERVER_LOG, "Opening listening socket.");

	iReturnValue = listen(m_gsServerSocket, 5);
	if (SOCKET_ERROR == iReturnValue)
	{
		iReturnValue = errno;

		LogString(SOCKETSERVER_LOG, "[CFlashComServer::Listen]: Listen Failed! %d", iReturnValue);
		LogString(ERROR_LOG, "[CFlashComServer::Listen]: Listen Failed! %d", iReturnValue);
		return SOCKET_ERROR;
	}


	return iReturnValue;
}

int CFlashComServer::Accept()
{
	int iReturnValue = 0;
	sockaddr_in clientSocketAddress;
	unsigned long lArgp = 1;
	int iAddrLength = sizeof(clientSocketAddress);

	if (m_gsClientSocket == INVALID_SOCKET)
	{
		fd_set fdAccept;
		struct timeval tv;
		tv.tv_sec = 10;
		tv.tv_usec = 0;

		FD_ZERO(&fdAccept);
		FD_SET(m_gsServerSocket, &fdAccept);

		int selRes = select(/*0*/m_gsServerSocket + 1, &fdAccept, NULL, NULL, &tv);

		if (selRes == SOCKET_ERROR)
		{
			iReturnValue = errno;
			LogString(SOCKETSERVER_LOG, "[CFlashComServer::Accept] select() failed with error: %d", iReturnValue);
		} 
		else if (selRes == 0 )
		{
			iReturnValue = READ_TIMEOUT;
		}
		else
		{
			LogString(SOCKETSERVER_LOG, "[CFlashComServer::Accept] select() returned success (connection pending)");

			if (FD_ISSET(m_gsServerSocket, &fdAccept))
			{
				LogString(SOCKETSERVER_LOG, "[CFlashComServer::Accept] Server %d is accepting on listening port %d.", 
					m_gsServerSocket, m_port);
				m_gsClientSocket = accept(m_gsServerSocket, (struct sockaddr*)&clientSocketAddress, (socklen_t*)&iAddrLength);
				LogString(SOCKETSERVER_LOG, "[CFlashComServer::Accept] Client %d connected on listening port %d.", 
					m_gsClientSocket, m_port);

				iReturnValue = errno;

				//
				// WSAEINTR.  A blocking Windows Sockets call was canceled, 
				// or the socket was closed - MZ
				//
				if (iReturnValue == EINTR)
				{
					LogString(SOCKETSERVER_LOG, "****************************EINTR (Blocking Cancelled)****************************");
					LogString(ERROR_LOG, "****************************EINTR (Blocking Cancelled)****************************");
				}
			}
		}
	}

	if (iReturnValue != READ_TIMEOUT)
	{
		if (m_gsClientSocket == INVALID_SOCKET)
		{
			LogString(SOCKETSERVER_LOG, "[CFlashComServer::Accept]: Failed! %d", iReturnValue);
		}
		else
		{
			iReturnValue = ioctl(m_gsClientSocket, FIONBIO, (unsigned long*)&lArgp);
			LogString(SOCKETSERVER_LOG, "WebSocket client connected.");
		}
	}

	return iReturnValue;
}

void CFlashComServer::SetSystemReuse()
{
	const string CmdLineFile = "echo \"1\" > /proc/sys/net/ipv4/tcp_tw_reuse";

	if (system(CmdLineFile.c_str()) != 0)
	{
		LogString(SOCKETSERVER_LOG, "TCP reuse system call failed");
		LogString(ERROR_LOG, "TCP reuse system call failed");
	}
}

void CFlashComServer::CloseClientSocket()
{
	int error(close(m_gsClientSocket));
	if (SOCKET_ERROR == error)
	{
		LogString(SOCKETSERVER_LOG, "[CFlashComServer::CloseClientSocket] Closing client socket %d Failed!", m_gsClientSocket);
		LogString(ERROR_LOG, "[CFlashComServer::CloseClientSocket] Closing client socket %d Failed!", m_gsClientSocket);
	}
	m_gsClientSocket = INVALID_SOCKET;
}

void CFlashComServer::Stop()
{
	LogString(SOCKETSERVER_LOG, "[CFlashComServer::Stop] server %d client %d", m_gsServerSocket, m_gsClientSocket);

	if (m_gsClientSocket != INVALID_SOCKET)
	{
		int error(close(m_gsClientSocket));
		if (SOCKET_ERROR == error)
		{
			LogString(SOCKETSERVER_LOG, "Closing client socket %d Failed!", m_gsClientSocket);
			LogString(ERROR_LOG, "Closing client socket %d Failed!", m_gsClientSocket);
		}
		m_gsClientSocket = INVALID_SOCKET;
	}

	int error(close(m_gsServerSocket));
	if (SOCKET_ERROR == error)
	{
		LogString(SOCKETSERVER_LOG, "Closing server socket %d Failed!", m_gsServerSocket);
		LogString(ERROR_LOG, "Closing server socket %d Failed!", m_gsServerSocket);
	}
	m_gsServerSocket = INVALID_SOCKET;
}


// Start waiting for the UI to connect to us
int CFlashComServer::Start()
{
	return Initialize();
}

// Read data sent from the UI
ReadStatus CFlashComServer::Read(char* buftorecv, long maxlengthofdatatorecv, long& bytesrecv)
{
	//function to read from the TCP socket n char
	ReadStatus readStatus = READ_ERROR;
	bytesrecv = 0;

	fd_set fdRead;

	//Make read timeout to be 30 seconds
	struct timeval WaitingTimeInSelect = { 30, 0 };

	FD_ZERO(&fdRead);
	FD_SET(m_gsClientSocket, &fdRead);

	int selRes = select(m_gsClientSocket + 1, &fdRead, NULL, NULL, &WaitingTimeInSelect);

	if (selRes == SOCKET_ERROR)
	{
		LogString(SOCKETSERVER_LOG, "[CFlashComServer::Read] - select(): SOCKET_ERROR, READ_ERROR");
		readStatus = READ_ERROR;
	}
	else if (SOCKET_SUCCESS == selRes)
	{
		readStatus = READ_TIMEOUT;
	}
	else if(selRes > 0)
	{
		int iLengthOfDataRecv(0);
		{
			lock_guard<recursive_mutex> lock(m_criticalSection);
			iLengthOfDataRecv = recv(m_gsClientSocket, buftorecv, maxlengthofdatatorecv, 0);
		}
		if (iLengthOfDataRecv == SOCKET_ERROR)
		{
			int Error = errno;

			if ((Error == ECONNRESET) ||
				(Error == ENETDOWN) ||
				(Error == ENOTCONN) ||
				(Error == ENETRESET))
			{
				m_gbClientConnectionFailed = true;
			}
			else
			{
				LogString(SOCKETSERVER_LOG, "[CFlashComServer::Read] - recv(): SOCKET_ERROR, no data");
			}
			readStatus = READ_ERROR;
		}
		else
		{
			bytesrecv = iLengthOfDataRecv;
			readStatus = READ_SUCCESS;
		}
	}

	return readStatus;
}


//This is the public function used to push data on to buffer and true is returned if a packet
//is found. The packet is defined by the _stx and _etx passed in to SetStartNFinish or defaulted
//in construct
// if no StxEtx then add to outputpacket and return true
bool CFlashComServer::AddPacket(unsigned char* InputPacket, long InputPacketLength, unsigned char* OutputPacket, long OutputPacketLength, long* OutputPacketReturnLength)
{
	bool retval = false;
	bool goodPacket = false;

	m_startIndexfound = false;
	m_endIndexfound = false;
	m_startIndex = 0;
	m_endIndex = 0;
	try
	{
		//make sure what we are pushing doesnt exceed our boundaries.
		if (true == WillNewPacketExceedBufferMax(InputPacketLength, OutputPacketLength))
		{
			ResetBufferToZero();
		}

		//get the data on the buffer for parsing
		PushNewDataOnBuffer(InputPacket, InputPacketLength);

		if (0 < m_bufferLength)
		{
			if (true == FindStartCharacter())
			{
				std::cout << "Shouldn't be here" << std::endl;
				//we found a start character. Look for the charater that determines the end of the packet
				if (true == FindEndCharacter(m_startIndex))
				{

					do	//loop till we have a good formed packet
					{
						if (false == CheckIfSecondStartCharacter())
						{
							retval = true;		//we got our packet
							goodPacket = true;	//get out of loop
						}
						else
						{
							//we got two stx's then an etx Need to jump to next stx
							if (false == FindStartCharacter(m_startIndex))
							{
								//this should never happen. If it does it means something(thread) modified the input buffer
								//From the last CheckIfSecondSTX till this line of code.

								retval = false;		//we do not have our packet
								goodPacket = true;	//get out of loop anyways
								ResetBufferToZero();	//something is wrong clear the buffer
							}
						}
					} while (goodPacket == false);

				} //true == FindEndCharacter(_stxindex)

				else
				{
					//packet not complete yet
					retval = false;
				}

			}

			// We did not find a start character.
			else
			{
				//if there is no start character then the data is no good.
				retval = false;
				ResetBufferToZero();

			}
		}// if( 0 < _bufferlength)

		if (true == retval) //full packet found now add it to the outbuffer
		{
			long copyIndex = m_startIndex;
			long copyLength = m_endIndex - m_startIndex;

			if (m_usesStartChar)
			{
				copyIndex++;
				copyLength -= 2;
			}

			//calculate the size of packet
			//long tempsize = ( m_endIndex - m_startIndex + 1 );

			memcpy(OutputPacket, &m_buffer.get()[copyIndex], copyLength);

			*OutputPacketReturnLength = copyLength;

			CopyRemainingDataToFrontOfBuffer();

			if (*OutputPacketReturnLength == 0)
			{
				retval = false;
			}
		}
	}
	catch (...)
	{
		retval = false;
	}

	return retval;
}



//Puts the new data on the buffer
void CFlashComServer::PushNewDataOnBuffer(unsigned char* InputPacket, long InputPacketLength)
{
	if (0 < InputPacketLength)
	{
		//add new data to internal buffer
		memcpy(&m_buffer.get()[m_bufferLength], InputPacket, InputPacketLength);
		m_bufferLength += InputPacketLength;
	}
}

//Check to see if the size of the new packet will put us over the limited size
bool CFlashComServer::WillNewPacketExceedBufferMax(long newPacketLength, long outputPacketLength)
{
	bool retval = false;

	if (((m_bufferLength + newPacketLength) > WEBSOCKET_MAX_LENGTH) ||
		((m_bufferLength + newPacketLength) > outputPacketLength) ||
		((m_bufferLength + newPacketLength) <= 0))
	{
		retval = true;
	}

	return retval;
}

//loop through _buffer and find first occurence of stx
bool CFlashComServer::FindStartCharacter(long startindex)
{
	bool retval = false;

	// We are using a starting character.
	if (m_usesStartChar)
	{
		if (true == FindByte(m_buffer.get(), m_bufferLength, m_startChar, startindex, &m_startIndex))		//check the incoming for stx
		{
			//indicate we did find the stx in the packet
			m_startIndexfound = true;

			retval = true;
		}
	}

	// We are not using a starting character. Set to the beginning of th buffer.
	else
	{
		m_startIndex = 0;
		m_startIndexfound = true;
		retval = true;
	}

	return retval;
}


//loop through _buffer and find first occurence of the end character
bool CFlashComServer::FindEndCharacter(long startindex)
{
	bool retval = false;

	if (FindByte(m_buffer.get(), m_bufferLength, m_endChar, startindex, &m_endIndex))		//check the incoming for etx
	{
		//indicate we did find the stx in the packet
		m_endIndexfound = true;

		retval = true;
	}
	return retval;
}

//Check if there is another STX between the first STX and the First ETX
bool CFlashComServer::CheckIfSecondStartCharacter()
{
	bool retval = false;
	long tempstxindex = 0;

	//look for another stx byte in the buffer.
	if (m_usesStartChar && FindByte(m_buffer.get(), m_bufferLength - m_startIndex, m_startChar, m_startIndex + 1, &tempstxindex))		//make sure there isnt another stx before etx
	{
		if (m_endIndex < tempstxindex)
		{
			//did not find second stx
			retval = false;
		}
		else
		{
			retval = true;
		}
	}

	return retval;
}


//search a byte array for a certain byte from an offset
bool CFlashComServer::FindByte(unsigned char* PacketToCheck, long PacketToCheckLength, unsigned char byteToFind, long SearchStartIndex, long* ReturnIndex)
{
	bool retval = false;

	if (SearchStartIndex > PacketToCheckLength)
	{
		retval = false;
	}
	else
	{
		for (long i = SearchStartIndex; i < PacketToCheckLength; i++)
		{
			if (PacketToCheck[i] == byteToFind)
			{
				retval = true;
				*ReturnIndex = i;
				break;
			}
		}
	}
	return retval;
}

void CFlashComServer::ResetBufferToZero()
{
	m_bufferLength = 0;
}

//copy everything after the last packet to the beginning of buffer.
void CFlashComServer::CopyRemainingDataToFrontOfBuffer()
{
	if (m_endIndex < m_bufferLength)
	{
		// there is more data after packet

		//copy remaining data to the beginning of buffer
		//m_endIndex is zero based and when doing size math with it we need to subtract one
		memmove(m_buffer.get(), &m_buffer.get()[m_endIndex + 1], m_bufferLength - m_endIndex - 1);

		//set new buffer size
		m_bufferLength = m_bufferLength - m_endIndex - 1;
	}
	else
	{
		//there is no more data after this packet

		//set new buffer size
		m_bufferLength = 0;
	}
}

// Write a buffer to the UI
int CFlashComServer::Write(const string& psWriteBuffer)
{
	//function to read from the TCP socket n char
	char* psBuffer(const_cast<char*>(psWriteBuffer.c_str()));
	int iLengthOfWriteBuffer(psWriteBuffer.length()/* + 1*/);
	int   iWritten = 0, iToWrite = iLengthOfWriteBuffer;


	if (!psWriteBuffer.empty())
	{
		while (iToWrite)
		{
			iWritten = send(m_gsClientSocket, psBuffer, iToWrite, 0);

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
			psBuffer += iWritten;
		}
		return iLengthOfWriteBuffer;
	}
	else
		return 0;//ERROR_NULL_BUFFER;	
}
