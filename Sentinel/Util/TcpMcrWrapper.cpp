#include "stdafx.h"
#include "TcpMcrWrapper.h"
#include <sys/socket.h>

// Sentinel includes.
#include "Logging/LogString.h"

#include <openssl/sha.h>
#include <openssl/evp.h>
#include "RapidJsonHelper.h"

using namespace std;

uint64_t CTcpMcrWrapper::ntoh64(const uint64_t* input)
{
	uint64_t rval;
	uint8_t* data = (uint8_t*)&rval;

	data[0] = *input >> 56;
	data[1] = *input >> 48;
	data[2] = *input >> 40;
	data[3] = *input >> 32;
	data[4] = *input >> 24;
	data[5] = *input >> 16;
	data[6] = *input >> 8;
	data[7] = *input >> 0;

	return rval;
}

uint64_t CTcpMcrWrapper::hton64(const uint64_t* input)
{
	return (ntoh64(input));
}

CTcpMcrWrapper::CTcpMcrWrapper(const CConfig &config, int portNumber) :
	m_port(portNumber), 
	m_serverSocket(-1), 
	m_clientSocket(-1), 
	m_eventAvailable(false),
	m_eventConsumed(0)
{
	m_cardConfig.Setup(config);
	StartServer();
}

void CTcpMcrWrapper::RefreshConfig(const CConfig &config)
{
	m_cardConfig.Setup(config);
}

void CTcpMcrWrapper::StartServer()
{
	int retVal = 0;
	sockaddr_in serv_addr;
	m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_serverSocket < 0)
	{		 
		LogString(ERROR_LOG, "[CTcpMcrWrapper::StartServer] Creating socket object failed.");
		return;
	}

	int enable = 1;
	if (setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		LogString(GENERAL, "[CTcpMcrWrapper::StartServer] WARN: Setting SO_REUSEADDR socket opt failed.");
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(m_port);
	retVal = bind(m_serverSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (retVal < 0)
	{		 
		LogString(ERROR_LOG, "[CTcpMcrWrapper::StartServer] Binding server socket to port [%d] failed with error [%m].", m_port);
		return;
	}

	// We only want one client to connect to the server at a time and reject other connections
	retVal = listen(m_serverSocket, 1);
	if (retVal < 0)
	{		 
		LogString(ERROR_LOG, "[CTcpMcrWrapper::StartServer] Listening to server socket on port [%d] failed with error [%m].", m_port);
		return;
	}

	LogString(GENERAL, "[CTcpMcrWrapper::StartServer] Launching network read thread...");

	m_networkReadThread = unique_ptr<std::thread>(new std::thread(NetworkReadThread, this));
}

CTcpMcrWrapper::~CTcpMcrWrapper()
{
	StopServer();
}

void CTcpMcrWrapper::StopServer()
{
	if (m_serverSocket > 0)
	{
		close(m_clientSocket);
	}
	
	if (m_clientSocket > 0)
	{
		close(m_clientSocket);
	}
}

CCard *CTcpMcrWrapper::TcpMcrWaitForCardEvent(bool &bInserted, bool &bRemoved, bool &bTimeout)
{
	CCard* card = nullptr;
	if (m_eventAvailable)
	{
		if (!m_CardNumber.empty())
		{
			card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetUTCTime());
			card->SetCardID(m_CardNumber, &m_cardConfig);
		}
		
		bInserted = (card != nullptr);
		bRemoved = (card == nullptr);
		bTimeout = false;
		m_eventAvailable = false;
		m_eventConsumed.signal(false);
		LogString(GENERAL, "[CTcpMcrWrapper::TcpMcrWaitForCardEvent] Network event available, returning bInserted [%d] bRemoved [%d] bTimeout[%d]",
			bInserted, bRemoved, bTimeout);
	}
	else
	{
		// We do not have any new cardin/cardout event to be processed
		bInserted = false;
		bRemoved = false;
		bTimeout = true;
	}
	
	return card;
}

UINT CTcpMcrWrapper::NetworkReadThread(PVOID threadParam)
{
	CTcpMcrWrapper *tcpMcrWrapper = (CTcpMcrWrapper *)threadParam;

	// This loop forever
	tcpMcrWrapper->NetworkReadLoop();
	return 0;
}

BYTE* CTcpMcrWrapper::ParseFrame(const BYTE* frameData, size_t& payloadLength, bool& isPing, bool& isClose, size_t& headerUsed)
{
	size_t index = 0;
	bool isLastFrame = frameData[index] & 0x80;
	BYTE opcode = frameData[index] & 0x0f;
	isClose = false;

	if (opcode == 0x09)
	{
		isPing = true;
	}
	else
	{
		isPing = false;

		if (opcode == 0x08)
		{
			isClose = true;
		}

	}

	bool isMasked = frameData[++index] & 0x80;
	uint32_t length = frameData[index++] & 0x7f;
	uint32_t* mask;
	//examine length
	if (length == 126)
	{
		uint16_t* nLength = (uint16_t*)&frameData[index];
		length = ntohs(*nLength);
		index += 2;
	}
	else if (length == 127)
	{
		uint64_t* nLength = (uint64_t*)&frameData[index++];
		length = ntoh64(nLength);
	}

	if (isMasked)
	{
		mask = (uint32_t*)&frameData[index];
		*mask = ntohl(*mask);
		index += 4;
	}

	// index should point to beginning of payload data
	BYTE* payload = const_cast<BYTE*>(frameData) + index;
	//cout << hex << "frameData:0x" << frameData << ", payload:0x" << payload << endl;
	if (isMasked)
	{
		BYTE* bMask = (BYTE*)mask;
		for (int i = 0; i < length; i++)
		{
			BYTE b = payload[i];
			b = b ^ bMask[3 - (i % 4)];
			payload[i] = b;
		}
	}
	payload[length] = '\0';
	payloadLength = length;
	headerUsed = index;

	/* debug - to be removed?
	if(!isLastFrame)
	{
		LogString(ERROR_LOG, "ParseFrame isMasked=%u length=%u mask=0x%0x",
			isMasked, length, mask ? *mask : 0);
	}
	*/

	return payload;
}

bool CTcpMcrWrapper::BuildFrame(BYTE* frameBuffer, size_t frameBufferLength, bool isText, bool isClose, BYTE* payload, size_t payloadLength, size_t& outputFrameLength, bool isPong)
{
	LogString(ERROR_LOG, "[CTcpMcrWrapper::BuildFrame in");
	if (frameBuffer != nullptr)
	{
		// make sure frameBuffer is large enough to hold entire payload
		if ((payloadLength < 126 && frameBufferLength >= payloadLength + 2) ||
			(payloadLength >= 126 && frameBufferLength >= payloadLength + 4))
		{
			memset(frameBuffer, 0, frameBufferLength);
			size_t index = 0;

			// assume complete frame
			BYTE zero = 0;
			frameBuffer[index] = 0x80;
			if (isClose)
			{
				frameBuffer[index] |= 0x08;
			} else if (isText)
			{
				frameBuffer[index] |= 0x01;
			}
			else if (isPong)
			{
				frameBuffer[index] |= 0x09;
			}

			// outgoing frames don't need to be masked
			index++;
			if (payloadLength <= 125)
			{
				frameBuffer[index++] = payloadLength;
				outputFrameLength = payloadLength + 2;
			}
			else
			{
				frameBuffer[index++] = 126; // next 2 bytes will contain the actual payloadLength
				uint16_t nSize = htons(payloadLength);
				memcpy(frameBuffer + index, &nSize, sizeof(uint16_t));
				outputFrameLength = payloadLength + 4;
				index += 2;
			}
			//coutData("Frame before payload copy", frameBuffer, outputFrameLength);
			// copy actual data
			memcpy(frameBuffer + index, payload, payloadLength);
			//coutData("Frame after payload copy", frameBuffer, outputFrameLength);
			LogString(ERROR_LOG, "[CTcpMcrWrapper::BuildFrame success out");
			return true;
		}
		else
		{
			LogString(ERROR_LOG, "[CTcpMcrWrapper::BuildFrame - insufficient space");
			return false;
		}
	}
	else
	{
		LogString(ERROR_LOG, "[CTcpMcrWrapper::BuildFrame - no  buffer");
		return false;
	}
}


bool CTcpMcrWrapper::ProcessWebSocketFrame(char* buffer, long dataLength)
{
	BYTE* payload = nullptr;
	size_t payloadLength = 0;
	size_t outputLength = 0;
	size_t headerUsed = 0;
	size_t dataIndex = 0;
	bool isPing = false;
	bool isClose = false;

	while (dataLength > dataIndex)
	{
		LogString(ERROR_LOG, "[CTcpMcrWrapper::ProcessWebSocketFrame] calling ParseFrame");

		payload = ParseFrame((BYTE*)(buffer + dataIndex), payloadLength, isPing, isClose, headerUsed);

		// new dataIndex
		dataIndex += payloadLength + headerUsed;

		if (isClose)
		{
			LogString(ERROR_LOG, "[CTcpMcrWrapper::ProcessWebSocketFrame] isClose - generating close frame");

			BYTE* closeFrame = new BYTE[payloadLength + 4];

			// Need to return a corresponding close frame
			//payloadLength = 0;
			BuildFrame(closeFrame, 2/*payloadLength + 4*/, false, true, payload, 0/*payloadLength*/, outputLength, true);

			LogString(ERROR_LOG, "[CTcpMcrWrapper::ProcessWebSocketFrame] Sending close frame to client. Length=%d", outputLength);
			int written = write(m_clientSocket, (BYTE*)closeFrame, outputLength);
			int e = (written < 0 ? errno : 0);
			LogString(ERROR_LOG, "[CTcpMcrWrapper::ProcessWebSocketFrame] - bytes sent: %d error: %d", written, e);
			delete[] closeFrame;

			return true;
		}
		else if (isPing)
		{
			LogString(ERROR_LOG, "[CTcpMcrWrapper::ProcessWebSocketFrame] isPing - generating pong frame");

			BYTE* pongFrame = new BYTE[payloadLength + 4];

			// Need to return the pong frame
			BuildFrame(pongFrame, payloadLength + 4, false, false, payload, payloadLength, outputLength, true);
			LogString(ERROR_LOG, "[CTcpMcrWrapper::ProcessWebSocketFrame] Sending pong frame to client");
			write(m_clientSocket, (BYTE*)pongFrame, outputLength);
			delete[] pongFrame;
		}
		else
		{
			if (payloadLength > 0)
			{
				LogString(ERROR_LOG, "[CTcpMcrWrapper::ProcessWebSocketFrame] ordinary WS data frame. Processing...");

				string cardNumber;
				//buffer[charsRead] = '\0';
				TcpMcrCmdType cmdType = ParseTcpMcrCommandJSON((char*)payload, cardNumber);
				bool success = false;
				switch (cmdType)
				{
					case MCR_CARD_INSERTED:
					{
						LogString(ERROR_LOG, "[CTcpMcrWrapper::ProcessWebSocketFrame] MCR_CARD_INSERTED command received with cardNumber [%s]", cardNumber.c_str());
						if (m_CardNumber.empty())
						{
							m_CardNumber = cardNumber;
							m_eventAvailable = true;
							success = true;
						}
					}
					break;
					case MCR_CARD_REMOVED:
					{
						LogString(ERROR_LOG, "[CTcpMcrWrapper::ProcessWebSocketFrame] MCR_CARD_REMOVED command received.");

						if (!m_CardNumber.empty())
						{
							m_CardNumber = "";
							m_eventAvailable = true;
							success = true;
						}
					}
					break;
					default:
						break;
				}

				//LogString(GENERAL, "[CTcpMcrWrapper::ProcessWebSocketFrame] Command result: [%s].", success ? "ACK" : "NACK");

				// Write ACK/NACK to the network client
				//strcpy(buffer, success ? "ACK" : "NACK");
				//write(m_clientSocket, buffer, strlen(buffer));

				if (m_eventAvailable)
				{
					// Wait for the newly generated event to be consumed
					m_eventConsumed.wait();
				}
			}
		}
	}
	return false;
}

void CTcpMcrWrapper::NetworkReadLoop()
{
	sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	LogString(GENERAL, "[CTcpMcrWrapper::NetworkReadLoop] Waiting for incoming connection...");
	bool isWebSocketFrame = false;

	while ((m_clientSocket = accept(m_serverSocket, (struct sockaddr *) &client_addr, &client_addr_len)) > 0)
	{		
		isWebSocketFrame = false;
		LogString(GENERAL, "[CTcpMcrWrapper::NetworkReadLoop] Received remote Mcr connection...");
		// If accept returns valid socket it means we can proceed
		char buffer[500] = {0};
		int charsRead = 0;
		while ((charsRead = read(m_clientSocket, buffer, sizeof(buffer))) > 0)
		{
			if (isWebSocketFrame) {
				if (ProcessWebSocketFrame(buffer, charsRead))
					break;
			}
			else
			{
				string xmlString(reinterpret_cast<const char*>(buffer));
				if (xmlString.find(WEBSOCKET_HANDSHAKE_KEY, 0) != string::npos)
				{
					LogString(ERROR_LOG, "[CTcpMcrWrapper::NetworkReadLoop] not WS mode - handshake found in message");
					// Look for Sec-WebSocket-Key:
					// Sec-WebSocket-Key: 9zFs+hwVnrHn42gny7PcNg==
					auto begin = xmlString.find(WEBSOCKET_HANDSHAKE_KEY, 0) + 19;
					auto end = xmlString.find("==", begin) + 2;
					string key = xmlString.substr(begin, end - begin);
					string response = getHandshakeResponse(key);
					LogStringNoFormat(ERROR_LOG, "WS handshake key found in data received - returning response - switching to WS mode");
					// Send response
					WriteString(response);

					// Future comms consist of frame data and needs to be handled differently
					isWebSocketFrame = true;
				}
				else
				{
					string cardNumber;
					buffer[charsRead] = '\0';
					TcpMcrCmdType cmdType = ParseTcpMcrCommand(buffer, cardNumber);
					bool success = false;
					switch (cmdType)
					{
					case MCR_CARD_INSERTED:
					{
						LogString(GENERAL, "[CTcpMcrWrapper::NetworkReadLoop] MCR_CARD_INSERTED command received with cardNumber [%s]", cardNumber.c_str());
						if (m_CardNumber.empty())
						{
							m_CardNumber = cardNumber;
							m_eventAvailable = true;
							success = true;
						}
					}
					break;
					case MCR_CARD_REMOVED:
					{
						LogString(GENERAL, "[CTcpMcrWrapper::NetworkReadLoop] MCR_CARD_REMOVED command received.");

						if (!m_CardNumber.empty())
						{
							m_CardNumber = "";
							m_eventAvailable = true;
							success = true;
						}
					}
					break;
					default:
						break;
					}

					LogString(GENERAL, "[CTcpMcrWrapper::NetworkReadLoop] Command result: [%s].", success ? "ACK" : "NACK");

					// Write ACK/NACK to the network client
					strcpy(buffer, success ? "ACK" : "NACK");
					write(m_clientSocket, buffer, strlen(buffer));

					if (m_eventAvailable)
					{
						// Wait for the newly generated event to be consumed
						m_eventConsumed.wait();
					}
				}
			}
		}

		//wait for tcp to close the connection (based on https://stackoverflow.com/questions/29680922/make-the-server-wait-the-fin-ack-from-the-client-before-closing-the-socket)
		recv(m_clientSocket, buffer, sizeof(buffer), 0);
		shutdown(m_clientSocket, SHUT_RDWR);

		close(m_clientSocket);

		LogString(GENERAL, "[CTcpMcrWrapper::NetworkReadLoop] Remote Mcr connection disconnected, again waiting to accept new connection...");
	}
}

TcpMcrCmdType CTcpMcrWrapper::ParseTcpMcrCommandJSON(char* buffer, string& cardNumber)
{
	TcpMcrCmdType cmdType = UNKNOWN;
	rapidjson::Document document;
	string jsonString(buffer);
	try
	{
		if (document.Parse(jsonString.c_str()).HasParseError())
		{
			LogString(ERROR_LOG, "ParseTcpMcrCommandJSON parsing %s failed.", jsonString.c_str());
		}
		else
		{
			if (document.IsObject() && document.HasMember("COMMAND") && document["COMMAND"].IsString())
			{
				const char* cmd = document["COMMAND"].GetString();

				if (strcmp(cmd, "INSERT_CARD") == 0)
				{
					if (document.HasMember("CARD_NUMBER") && document["CARD_NUMBER"].IsString())
					{
						const char* operand = document["CARD_NUMBER"].GetString();

						cmdType = MCR_CARD_INSERTED;
						cardNumber = operand;
					}
				} 
				else if (strcmp(cmd, "REMOVE_CARD") == 0)
				{
					cmdType = MCR_CARD_REMOVED;
				}
			}
		}
	}
	catch (JsonException& e)
	{
		LogString(ERROR_LOG, "CTcpMcrWrapper::ParseTcpMcrCommandJSON JSON exception: %s: '%s'", e.codestring(), e.what());
	}
	catch (std::out_of_range& e)
	{
		LogString(ERROR_LOG, "CTcpMcrWrapper::ParseTcpMcrCommandJSON out_of_range exception: '%s'", e.what());
	}
	catch (std::invalid_argument& e)
	{
		LogString(ERROR_LOG, "CTcpMcrWrapper::ParseTcpMcrCommandJSON invalid_argument exception: '%s'", e.what());
	}

	return cmdType;
}

TcpMcrCmdType CTcpMcrWrapper::ParseTcpMcrCommand(char* buffer, string& cardNumber)
{
	TcpMcrCmdType cmdType = UNKNOWN;
	LogString(GENERAL, "[CTcpMcrWrapper::ParseTcpMcrCommand] Command: [%s]", buffer);

	// Packet format is simple:
	// INSERT_CARD xxxxx
	// REMOVE_CARD
	char cmd[50] = {0};
	char operand[200] = {0};
	sscanf(buffer, "%20s %200s", cmd, operand);
	if (!strcmp(cmd, "INSERT_CARD"))
	{
		cmdType = MCR_CARD_INSERTED;
		cardNumber = operand;
	}
	else if (!strcmp(cmd, "REMOVE_CARD"))
	{
		cmdType = MCR_CARD_REMOVED;
	}

	return cmdType;
}

char* base64(const unsigned char* input, int length) {
	const auto pl = 4 * ((length + 2) / 3);
	auto output = reinterpret_cast<char*>(calloc(pl + 1, 1)); //+1 for the terminating null that EVP_EncodeBlock adds on
	const auto ol = EVP_EncodeBlock(reinterpret_cast<unsigned char*>(output), input, length);
	if (pl != ol) { std::cerr << "Whoops, encode predicted " << pl << " but we got " << ol << "\n"; }
	return output;
}

string CTcpMcrWrapper::getHandshakeResponse(string key)
{
	//key = "LKF8lHGznbKGIgO1UzAOhg==";
	cout << "Input key:" << key << endl;
	// concantenate GUID
	string newKey = key + WEBSOCKET_HANDSHAKE_GUID;
	cout << "concantenated key:" << newKey << endl;

	unsigned char obuf[20];

	// Get sha-1 of new key
	SHA1((const unsigned char*)newKey.c_str(), newKey.length(), obuf);

	// Get base64 encoding of new hash
	char* encodedKey = base64(obuf, 20);
	cout << "encoded key:" << encodedKey << endl;

	// Build return string
	string response = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: " + string(encodedKey) + "\r\n\r\n";
	cout << "Handshake response:" << response << endl;

	// free base64 allocated memory
	free(encodedKey);
	return response;
}

int CTcpMcrWrapper::WriteString(const string& inString)
{
	int numberOfCharsWritten(0);

	if (!inString.empty())
	{
		numberOfCharsWritten = Write(inString);
	}

	return numberOfCharsWritten;
}


// Write a buffer to the MCR
int CTcpMcrWrapper::Write(const string& psWriteBuffer)
{
	//function to read from the TCP socket n char
	char* psBuffer(const_cast<char*>(psWriteBuffer.c_str()));
	int iLengthOfWriteBuffer(psWriteBuffer.length()/* + 1*/);
	int   iWritten = 0, iToWrite = iLengthOfWriteBuffer;


	if (!psWriteBuffer.empty())
	{
		while (iToWrite)
		{
			iWritten = send(m_clientSocket, psBuffer, iToWrite, 0);

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
						//m_gbClientConnectionFailed = true;
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
