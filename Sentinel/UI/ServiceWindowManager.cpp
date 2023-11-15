#include "stdafx.h"

#ifdef LOGGING
#include "Logging/LogString.h"
#endif

#include "Hardware/WatchDog.h"
#include "ServiceWindowManager.h"
#include "../Sentinel.h"
#include "UIProtocol.h"
#include "Config/ConfigExtData.h"
#include "DirectoryManager.h"
#include "DisplayManager.h"
#include "NotificationTextConfig.h"
#include "ServiceWindowDefs.h"
#include "Utilities.h"
#include "../ContentDeliverySystem/WebSocketServer.h"

using namespace BonusCounterState;
using namespace pugi;
using namespace std;

const string KILL_CHROME("killchrome.sh");

const WORD SERVICE_WINDOW_FLASH_PORT(7778);
const WORD CROSS_DOMAIN_POLICY_PORT(843);

uint64_t ntoh64(const uint64_t* input);
uint64_t hton64(const uint64_t* input);

//#define DEBUG_WEBSOCKET_FRAME
#define MIN_WEBSOCKET_CLIENT_HEADER_LENGTH 2
#define MAX_WEBSOCKET_CLIENT_HEADER_LENGTH 14

#define PING_FRAME_OPCODE 0x9
#define PONG_FRAME_OPCODE 0xA
#define TEXT_FRAME_OPCODE 0x1

static UINT AFX_CDECL ServiceWindowListenerThread(PVOID threadParam);

CServiceWindowManager::CServiceWindowManager()
{
	init();
}

void CServiceWindowManager::init()
{
	m_flashServer = nullptr;
	m_partialWebsocketFrame = nullptr;
	m_partialWebsocketFrameLen = 0;
    m_firstDataReceivedFromUI = false;
	m_serviceWindowListenerThreadExited = false;
}

CServiceWindowManager::~CServiceWindowManager(void)
{
	LogString(SOCKETSERVER_LOG, "CServiceWindowManager::~CServiceWindowManager");
}

void CServiceWindowManager::Start(CUIProtocol* uiProtocol)
{
	CUtilities::SetupDatabaseCertsForChrome();

	//Adds the default and custom certs
	vector<string>* listOfCerts(new vector<string>());
	CUtilities::AddCertsToCertificateDatabaseStore(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_DEFAULT_CERTIFICATE_STORE), *listOfCerts);
	CUtilities::AddCertsToCertificateDatabaseStore(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_ADDITIONAL_CERTIFICATE_STORE), *listOfCerts);
	if (listOfCerts->empty())
	{
		delete listOfCerts;
	}
	else
	{
		CUtilities::GetSystemResultString(FormatString(_T("cp %s/* /usr/local/share/ca-certificates"), CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_DEFAULT_CERTIFICATE_STORE).c_str()));
		CUtilities::GetSystemResultString(FormatString(_T("cp %s/* /usr/local/share/ca-certificates"), CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_ADDITIONAL_CERTIFICATE_STORE).c_str()));
		CUtilities::FlushVolume();
		CUtilities::GetSystemResultString("/usr/sbin/update-ca-certificates");
		sort(listOfCerts->begin(), listOfCerts->end());
		uiProtocol->SendMessageToInternals(new CInternalMessage(INT_MSG_LIST_OF_CERTS, listOfCerts), QUEUE_PRIORITY_HIGHEST);
	}

	DWORD startServiceWindowTickCount(CUtilities::GetTickCount());
	m_firstDataReceivedFromUI = false;
	m_serviceWindowListenerThreadExited = false;

	const DWORD WAIT_FOR_HEARTBEAT_SLEEP(1000);
	DWORD MAX_NUMBER_OF_TICKS_WAITING_FOR_HEARTBEAT(116000);
	DWORD startTickCountWaitingForHeartbeat(CUtilities::GetTickCount());

	LogString(SOCKETSERVER_LOG, _T("Starting ServiceWindowListenerThread"));

	m_serviceWindowListenerThread = unique_ptr<std::thread>(new std::thread(ServiceWindowListenerThread, uiProtocol));

	CWatchDog::Instance().RegisterThread(
		THREAD_SERVICEWINDOW_LISTENER,
		m_serviceWindowListenerThread->get_id(),
		SERVICE_WINDOW_LISTENER_THREAD_TIMEOUT,
		m_serviceWindowListenerThread->native_handle());

	LaunchChrome();

	CWatchDog::Instance().Checkin();
	while (!m_firstDataReceivedFromUI &&
		   !ServiceWindowListenerThreadExited() &&
		   (CUtilities::GetTickCount() - startTickCountWaitingForHeartbeat) < MAX_NUMBER_OF_TICKS_WAITING_FOR_HEARTBEAT)
	{
		CUtilities::Sleep(WAIT_FOR_HEARTBEAT_SLEEP);
		startTickCountWaitingForHeartbeat = CUtilities::GetTickCount();
		if (CUtilities::IsAutomationMode())
		{
			CWatchDog::Instance().Checkin();
		}
	}

	CWatchDog::Instance().Checkin();

	if (m_firstDataReceivedFromUI)
	{
		LogString(SOCKETSERVER_LOG, "UI Heartbeat Received. Waited %u", CUtilities::GetTickCount() - startServiceWindowTickCount);
	}
	else if (ServiceWindowListenerThreadExited())
	{
		LogString(SOCKETSERVER_LOG, "ServiceWindowListenerThread exited");
		LogString(ERROR_LOG, "ServiceWindowListenerThread exited");
	}
	else
	{
		LogString(SOCKETSERVER_LOG, "Waiting for heartbeat expired");
		LogString(ERROR_LOG, "Waiting for heartbeat expired");
		WORD shellUiRestart(1);
		uiProtocol->PerformShutdown(new CInternalMessage(INT_MSG_SHUTDOWN, shellUiRestart));
	}

	if (!ServiceWindowListenerThreadExited())
	{
		m_serviceWindowListenerThread->detach();
	}
}

void CServiceWindowManager::SendMessageToAngularUI(const std::string& message)
{
	if (m_firstDataReceivedFromUI)
	{
		SendMessage(const_cast<string&>(message));
	}
	else
	{
		LogString(ERROR_LOG, "[%s] Sending %s failed, heartbeat is not received yet.", __FUNCTION__, message.c_str());
	}
}

void CServiceWindowManager::ReadAndProcessFlashServer(void* parent)
{
	CUIProtocol* uiProtocol = (CUIProtocol*)parent;
	m_serviceWindowListenerThreadExited = false;
	LogString(SOCKETSERVER_LOG, "[CServiceWindowManager::ReadAndProcessFlashServer]");
	bool continueOuterLoop = false;

	// Pre load/start main CWebSocketServer in place of CFlashComServer
	m_flashServer = new CWebSocketServer(SERVICE_WINDOW_FLASH_PORT, NUL);

	int startRes = READ_SUCCESS;

	startRes = m_flashServer->Start();

	if (startRes == SOCKET_SUCCESS)
	{
		DWORD currentTickCount(CUtilities::GetTickCount());
		LogString(SOCKETSERVER_LOG, "[%s] Waiting for a connection.", __FUNCTION__);
		do
		{
			startRes = m_flashServer->Accept();
			if (startRes == READ_TIMEOUT)
			{
				//check in with the watchDog
				CWatchDog::Instance().Checkin();
			}
		} while (startRes != SOCKET_SUCCESS && !uiProtocol->IsTimeToClose());
		if (startRes == SOCKET_SUCCESS)
		{
			LogString(SOCKETSERVER_LOG, "[%s] It took %u to accept a connection.", __FUNCTION__, CUtilities::GetTickCount() - currentTickCount);
		}
		
	}

	do
	{
		continueOuterLoop = false;
		bool isWebSocketMode = false;
		bool clientClose = false;

		std::unique_ptr<char> readBuffer(new char[WEBSOCKET_MAX_LENGTH]);
		long bytesRead = 0;
		int maxPacketSize = WEBSOCKET_MAX_LENGTH;
		unique_ptr<BYTE> tempRecvPacket(new BYTE[WEBSOCKET_MAX_LENGTH + 1]);
		long packetLength = 0;

		// overloading crossDomainRequestReceived to also handle websocket handshake request
		bool crossDomainRequestReceived(false);
		while (!uiProtocol->IsTimeToClose() && !uiProtocol->IsTimeToRestartMWThread())
		{
			// Process any traffic received on the endpoint
			ReadStatus readStatus = m_flashServer->Read(readBuffer.get(), WEBSOCKET_MAX_LENGTH, bytesRead);

			// Check in with the watchDog
			CWatchDog::Instance().Checkin();

			if ((readStatus == READ_SUCCESS) && (0 < bytesRead))
			{
				if (isWebSocketMode)
				{
					ProcessWebSocketFrames(uiProtocol, (BYTE*)readBuffer.get(), bytesRead);
				}
				else
				{
					LogString(SOCKETSERVER_LOG, "[%s] Inner loop - not in WS mode yet", __FUNCTION__);

					string dataReceived(reinterpret_cast<const char*>(readBuffer.get()));
					LogString(SOCKETSERVER_LOG, "[%s] Received: %s", __FUNCTION__, readBuffer.get());
					if (dataReceived.find(WEBSOCKET_HANDSHAKE_KEY, 0) != string::npos)
					{
						// Look for Sec-WebSocket-Key:
						// Sec-WebSocket-Key: 9zFs+hwVnrHn42gny7PcNg==
						auto begin = dataReceived.find(WEBSOCKET_HANDSHAKE_KEY, 0) + 19;
						auto end = dataReceived.find("==", begin) + 2;
						string key = dataReceived.substr(begin, end - begin);
						string response = getHandshakeResponse(key);
						LogString(SOCKETSERVER_LOG, "[%s] WS handshake key found in data received. Returning response to switch to WS mode",
							__FUNCTION__);
						// Send response
						m_flashServer->Write(response);

						// Future comms consist of websocket frames and needs to be handled differently
						isWebSocketMode = true;
					}
					else
					{
						LogString(SOCKETSERVER_LOG, "[%s] WS handshake key not found in data received in request.",
							__FUNCTION__);
					}

					CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
				}
			}
			else if ((readStatus == READ_SUCCESS) && (bytesRead == 0))
			{
				// Other side closed the connection, closing here also.
				LogString(SOCKETSERVER_LOG, "[%s] Bytes Read zero", __FUNCTION__);
				LogString(ERROR_LOG, "[%s] Bytes Read zero", __FUNCTION__);
				clientClose = true;
				break;
			}

			CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
		}

		if (m_flashServer)
		{
			if (clientClose)
			{
				LogString(SOCKETSERVER_LOG, "[%s] Closing local client socket", __FUNCTION__);
				m_flashServer->CloseClientSocket();

				LogString(SOCKETSERVER_LOG, "[%s] Listening for new client", __FUNCTION__);

				if (SOCKET_SUCCESS == m_flashServer->Listen()) // accept new client
				{
					while (!uiProtocol->IsTimeToClose() && !uiProtocol->IsTimeToRestartMWThread())
					{
						LogString(SOCKETSERVER_LOG, "[%s] Accepting new client", __FUNCTION__);
						int accpt = m_flashServer->Accept();
						if (accpt == READ_TIMEOUT)
						{
							LogString(SOCKETSERVER_LOG, "[%s] Accepting new client - timed out ", __FUNCTION__);

							//check in with the watchDog
							CWatchDog::Instance().Checkin();
						}
						else if (accpt == SOCKET_SUCCESS)
						{
							LogString(SOCKETSERVER_LOG, "[%s] Restarting processing loop", __FUNCTION__);
							continueOuterLoop = true;
							break;
						}
						else
						{
							LogString(SOCKETSERVER_LOG, "[%s] (Re)accept client failed", __FUNCTION__);
							break;
						}
					}
				}
			}

			if (!continueOuterLoop)
			{
				delete m_flashServer;
				m_flashServer = nullptr;
			}
		}

	} while (continueOuterLoop);

	KillChrome();
	LogString(SOCKETSERVER_LOG, "[%s] Exit ReadAndProcessFlashServer", __FUNCTION__);
	m_serviceWindowListenerThreadExited = true;
}


// Consumer must free(ouput)
char* CServiceWindowManager::base64(const unsigned char* input, int length) {
	const auto pl = 4 * ((length + 2) / 3);
	auto output = reinterpret_cast<char*>(calloc(pl + 1, 1)); //+1 for the terminating null that EVP_EncodeBlock adds on
	const auto ol = EVP_EncodeBlock(reinterpret_cast<unsigned char*>(output), input, length);
	if (pl != ol) { std::cerr << "Whoops, encode predicted " << pl << " but we got " << ol << "\n"; }
	return output;
}

string CServiceWindowManager::getHandshakeResponse(string key)
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

void CServiceWindowManager::ProcessWebSocketFrames(CUIProtocol* uiProtocol, const BYTE* messageData, long dataLength)
{
	BYTE* payload = nullptr;
	size_t payloadLength = 0;
	size_t outputLength = 0;
	size_t headerUsed = 0;
	bool isPing = false;

	LogString(SOCKETSERVER_LOG, "[ProcessWebSocketFrames] Received data from UI, bytesRead: %ld", dataLength);

#ifdef DEBUG_WEBSOCKET_FRAME	
	int frameNum = 1;
#endif

	size_t parsedLength = 0;
	bool isFinalPartOfPayload = false;
	while (dataLength > 0)
	{
		payload = ParseFrame(messageData, dataLength, isPing, isFinalPartOfPayload, parsedLength, payloadLength);
		messageData += parsedLength;
		dataLength -= parsedLength;

#ifdef DEBUG_WEBSOCKET_FRAME	
		LogString(SOCKETSERVER_LOG, "[ProcessWebSocketFrames] FrameNum # %d, ParsedLength: %d, PayloadLength: %d, "
			"isFinalPartOfPayload: %d, isPing: %d",
			frameNum++, parsedLength, payloadLength, isFinalPartOfPayload, isPing);
#endif

		if (isPing)
		{
			// Need to return the pong frame
			BYTE* pongFrame = BuildFrame(PONG_FRAME_OPCODE, payload, payloadLength, outputLength);
			if (pongFrame)
			{
				CWebSocketServer* server = (CWebSocketServer*)m_flashServer;
				server->Write((BYTE*)pongFrame, outputLength);
				delete[] pongFrame;
			}
		}
		else if (payload != nullptr)
		{
			// Get string from payload
			string message((char*)payload, payloadLength);
			LogString(SOCKETSERVER_LOG, "[ProcessWebSocketFrames] Received websocket frame payload: [%s]", message.c_str());

			if (!isFinalPartOfPayload)
			{
				m_partialMessage += message;
				LogString(SOCKETSERVER_LOG, "[ProcessWebSocketFrames] Partial message detected, cached it to process later on.");
				delete[] payload;
				continue;
			}
			else if (!m_partialMessage.empty())
			{
				string restOfMessage(message);
				message = m_partialMessage + restOfMessage;
				m_partialMessage.clear();
				LogString(SOCKETSERVER_LOG, "[ProcessWebSocketFrames] Final message part detected, complete message: [%s]",
					message.c_str());
			}

			if (!m_firstDataReceivedFromUI)
			{
				m_firstDataReceivedFromUI = true;
				LogString(SOCKETSERVER_LOG, "First data received: %s", message.c_str());
			}

			if (message == "\"heartbeat\"")
			{
				// Send a heartbeat to internals
				uiProtocol->SendMessageToInternals(new CInternalMessage(INT_MSG_HEARTBEAT), QUEUE_PRIORITY_NORMAL);
			}
			else
			{
				uiProtocol->ProcessAngularUIMessage(message);
			}
		}

		delete[] payload;
	}
}

BYTE* CServiceWindowManager::ParseFrame(const BYTE* buffer, size_t bufferLength, bool& isPingFrame, bool& isFinalPartOfPayload, size_t& parsedLength, size_t& payloadLength)
{
	if (buffer == nullptr || bufferLength == 0)
	{
		LogString(SOCKETSERVER_LOG, "[ParseFrame] [ERROR] Unexpected, buffer is NULL or bufferLength is 0.");
		LogString(ERROR_LOG, "[ParseFrame] [ERROR] Unexpected, buffer is NULL or bufferLength is 0.");
		return nullptr;
	}

#ifdef DEBUG_WEBSOCKET_FRAME
	LogString(SOCKETSERVER_LOG, "[ParseFrame] [WEBSOCKET FRAME] data: [%x %x %x %x %x]",
		buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]
	);
#endif

	isPingFrame = false;
	parsedLength = 0;
	payloadLength = 0;
	isFinalPartOfPayload = false;
	BYTE* payloadData = nullptr;

	// Let's frame websocket header which would help us in calculating payload data
	BYTE* pWebsocketHeader = new BYTE[MAX_WEBSOCKET_CLIENT_HEADER_LENGTH];
	size_t websocketHeaderCurLength = 0;

	if (m_partialWebsocketFrame && m_partialWebsocketFrameLen > 0)
	{
		// partial frame can be either complete header and partial payload, or partial header.
		// we would be able to find payload length by appending max header length to partial frame.
		if (m_partialWebsocketFrameLen > MAX_WEBSOCKET_CLIENT_HEADER_LENGTH)
		{
			memcpy(pWebsocketHeader, m_partialWebsocketFrame, MAX_WEBSOCKET_CLIENT_HEADER_LENGTH);
			websocketHeaderCurLength = MAX_WEBSOCKET_CLIENT_HEADER_LENGTH;
		}
		else
		{
			memcpy(pWebsocketHeader, m_partialWebsocketFrame, m_partialWebsocketFrameLen);
			websocketHeaderCurLength = m_partialWebsocketFrameLen;
		}
	}

	// Copy leftover length of the header part from buffer passed, subjected to max of bufferLength
	memcpy(pWebsocketHeader + websocketHeaderCurLength, buffer,
		min(MAX_WEBSOCKET_CLIENT_HEADER_LENGTH - websocketHeaderCurLength, bufferLength));
	websocketHeaderCurLength += min(MAX_WEBSOCKET_CLIENT_HEADER_LENGTH - websocketHeaderCurLength, bufferLength);

	// Now let's see if we have enough data to calculate the payload length
	if (websocketHeaderCurLength < MIN_WEBSOCKET_CLIENT_HEADER_LENGTH)
	{
		// partial packet detected, store and bail out
		LogString(SOCKETSERVER_LOG, "[ParseFrame] [WEBSOCKET FRAME] Frame length < Minimum (2), storing as partial packet and bailing out");
		delete m_partialWebsocketFrame;
		m_partialWebsocketFrame = new BYTE[websocketHeaderCurLength + 1];
		memcpy(m_partialWebsocketFrame, pWebsocketHeader, websocketHeaderCurLength);
		m_partialWebsocketFrameLen = websocketHeaderCurLength;
		delete pWebsocketHeader;
		parsedLength = websocketHeaderCurLength;
		return nullptr;
	}

	size_t headerIndex = 0;

	// byte1 - mainly opcode
	isFinalPartOfPayload = pWebsocketHeader[headerIndex] & 0x80;
	BYTE opcode = pWebsocketHeader[headerIndex++] & 0x0f;
	if (opcode == PING_FRAME_OPCODE)
	{
		isPingFrame = true;
	}
	else
	{
		isPingFrame = false;
	}

	// byte2 - mainly payload length for payloads <= 125 bytes
	bool isPayloadMasked = pWebsocketHeader[headerIndex] & 0x80;
	uint32_t length = pWebsocketHeader[headerIndex++] & 0x7f;

#ifdef DEBUG_WEBSOCKET_FRAME
	LogString(SOCKETSERVER_LOG, "[ParseFrame] [WEBSOCKET FRAME] Opcode: %d, isFinalPartOfPayload: %d, Byte2 PayloadLength: %d",
		opcode, isFinalPartOfPayload, length);
#endif

	// interpret next bytes in header based on length mentioned in byte2
	if (length <= 125)
	{
		payloadLength = length;
	}
	else if ((length == 126) && (websocketHeaderCurLength >= MIN_WEBSOCKET_CLIENT_HEADER_LENGTH + sizeof(uint16_t)))
	{
		// payload length is > 125 and dictated by next 2 bytes
#ifdef DEBUG_WEBSOCKET_FRAME
		LogString(SOCKETSERVER_LOG, "[ParseFrame] [WEBSOCKET FRAME] Length = 126 detected, indicating > 125 bytes but < 64KB payload.");
#endif
		uint16_t* pLength = (uint16_t*)(pWebsocketHeader + headerIndex);
		payloadLength = ntohs(*pLength);
		headerIndex += sizeof(uint16_t);
	}
	else if ((length == 127) && (websocketHeaderCurLength >= MIN_WEBSOCKET_CLIENT_HEADER_LENGTH + sizeof(uint64_t)))
	{
		// payload length is > 64KB and dictated by next 8 bytes
		LogString(SOCKETSERVER_LOG, "[ParseFrame] [WEBSOCKET FRAME] Length = 127 detected, indicating > 64KB payload.");
		uint64_t* pLength = (uint64_t*)(pWebsocketHeader + headerIndex);
		payloadLength = ntoh64(pLength);
		headerIndex += sizeof(uint64_t);
	}
	else
	{
#ifdef DEBUG_WEBSOCKET_FRAME
		LogString(SOCKETSERVER_LOG, "[ParseFrame] [WEBSOCKET FRAME] Unable to find payload length, header is of less than expected length needed to calculate it.");
#endif
	}

	size_t websocketHeaderExpectedLength = headerIndex;
	if (isPayloadMasked)
	{
		websocketHeaderExpectedLength += sizeof(uint32_t);
	}

	if (payloadLength == 0 || websocketHeaderCurLength < websocketHeaderExpectedLength ||
		(m_partialWebsocketFrameLen + bufferLength < websocketHeaderExpectedLength + payloadLength))
	{
		// partial packet detected, store and bail out
		LogString(SOCKETSERVER_LOG, "[ParseFrame] [WEBSOCKET FRAME] Partial frame detected, incomplete header or payload."
			"[HeaderCurLength: %d HeaderExpectedLength: %d, ActualPayloadLength: %d, PartialFrameLength: %d, BufferLength: %d]",
			websocketHeaderCurLength, websocketHeaderExpectedLength, payloadLength, m_partialWebsocketFrameLen, bufferLength);

		BYTE* newPartialFrame = new BYTE[m_partialWebsocketFrameLen + bufferLength + 1];
		if (m_partialWebsocketFrameLen)
		{
			memcpy(newPartialFrame, m_partialWebsocketFrame, m_partialWebsocketFrameLen);
		}
		memcpy(newPartialFrame + m_partialWebsocketFrameLen, buffer, bufferLength);
		delete m_partialWebsocketFrame;
		m_partialWebsocketFrame = newPartialFrame;
		m_partialWebsocketFrameLen = m_partialWebsocketFrameLen + bufferLength;
		delete pWebsocketHeader;
		parsedLength = bufferLength;
		return nullptr;
	}

	uint32_t mask = 0;
	if (isPayloadMasked)
	{
		uint32_t* pMask = (uint32_t*)(pWebsocketHeader + headerIndex);
		mask = ntohl(*pMask);
	}

	delete pWebsocketHeader;
	pWebsocketHeader = nullptr;

	// Create the frame payload buffer
	payloadData = new BYTE[payloadLength + 1];

	size_t payloadLenFromPartialFrame = 0;
	if (m_partialWebsocketFrameLen > websocketHeaderExpectedLength)
	{
		// Partial frame had complete header in it, plus maybe payload part also.
		payloadLenFromPartialFrame = m_partialWebsocketFrameLen - websocketHeaderExpectedLength;
		memcpy(payloadData, m_partialWebsocketFrame + websocketHeaderExpectedLength,
			payloadLenFromPartialFrame);
		memcpy(payloadData + payloadLenFromPartialFrame, buffer, payloadLength - payloadLenFromPartialFrame);
		parsedLength = (payloadLength - payloadLenFromPartialFrame);
	}
	else
	{
		// Partial frame had partial header in it
		size_t payloadOffset = websocketHeaderExpectedLength - m_partialWebsocketFrameLen;
		memcpy(payloadData, buffer + payloadOffset, payloadLength);
		parsedLength = (payloadOffset + payloadLength);
	}

	// Delete partial frame
	delete m_partialWebsocketFrame;
	m_partialWebsocketFrame = nullptr;
	m_partialWebsocketFrameLen = 0;

	// Unmask the payload
	if (isPayloadMasked)
	{
		BYTE* pbMask = (BYTE*)&mask;
		for (int i = 0; i < payloadLength; i++)
		{
			BYTE b = payloadData[i];
			b = b ^ pbMask[3 - (i % 4)];
			payloadData[i] = b;
		}
	}

	return payloadData;
}

void CServiceWindowManager::SendMessage(string& message)
{
	size_t frameLength = 0; 
	BYTE* frameBuffer = BuildFrame(TEXT_FRAME_OPCODE, (BYTE*)message.c_str(), message.length(), frameLength);
	if (frameBuffer)
	{	
		CWebSocketServer* server = (CWebSocketServer*)m_flashServer;
		server->Write(frameBuffer, frameLength);
		LogString(ANGULARUILOG, "Sent: %s", message.c_str());
		delete[] frameBuffer;
	}
}

void CServiceWindowManager::GetAssetTagAndDuration(screenID screen, std::string& assetTag, DWORD& duration) const
{
	duration = DEFAULT_DURATION;
	switch (screen)
	{
	case screenPlayerAnniversary:
		assetTag = "Anniversary";
		duration = m_uiConfig.GetNotificationForPlayerTrackingTimeInMilliseconds();
		break;
	case screenPlayerBirthday:
		assetTag = "Birthday";
		duration = m_uiConfig.GetNotificationForPlayerTrackingTimeInMilliseconds();
		break;
	case screenPlayerBirthdayAndAnniversary:
		assetTag = "BirthdayAndAnniversary";
		duration = m_uiConfig.GetNotificationForPlayerTrackingTimeInMilliseconds() * 2;
		break;
	case screenPlayerCongratulations:
		assetTag = "Congrats";
		duration = m_uiConfig.GetNotificationForPlayerTrackingTimeInMilliseconds();
		break;
	case screenPlayerBadCard:
		assetTag = "BadCard";
		duration = INFINITE_DURATION;
		break;
	case screenBannedFromPlayMessage:
		assetTag = "BannedPlayer";
		duration = POPUPDISPLAYTIME;
		break;
	default:
		break;
	}
}

BYTE* CServiceWindowManager::BuildFrame(BYTE opCode, BYTE* payload, size_t payloadLength, size_t& outputFrameLength)
{
	outputFrameLength = 0;

	// Control frames can only have max payload length as 125
	if (((opCode == PING_FRAME_OPCODE) || (opCode == PONG_FRAME_OPCODE)) && (payloadLength > 125))
	{
		LogString(SOCKETSERVER_LOG, "[CServiceWindowManager::BuildFrame] Discarding building control web socket frame, "
		    "too big payload (> 125)");
		LogString(ERROR_LOG, "[CServiceWindowManager::BuildFrame] Discarding building text web socket frame, "
		    "too big payload (> 125)");
	}

	// We will only support data payload of < 64KB for messages sent from C++ to UI
	if ((opCode == TEXT_FRAME_OPCODE) &&
	    ((payload == nullptr) || (payloadLength == 0) || (payloadLength >= 64 * BYTES_PER_KB)))
	{
		LogString(SOCKETSERVER_LOG, "[CServiceWindowManager::BuildFrame] Discarding building text web socket frame, "
		    "invalid or too big payload (>= 64KB)");
		LogString(ERROR_LOG, "[CServiceWindowManager::BuildFrame] Discarding building text web socket frame, "
		    "invalid or too big payload (>= 64KB)");
		return nullptr;
	}

    // Max frame buffer length with payload <= 64KB and no masking is [payloadLength + 4]
    BYTE* frameBuffer = new BYTE[payloadLength + 4];
	memset(frameBuffer, 0, payloadLength + 4);

	size_t index = 0;

    // Add opcode with FIN marker (0x80)
	frameBuffer[index++] = 0x80 | opCode;

	if (payloadLength <= 125)
	{
		frameBuffer[index++] = payloadLength;
		outputFrameLength = payloadLength + 2;
	}
	else
	{
		frameBuffer[index++] = 126;
		// next 2 bytes will contain the actual payloadLength
		uint16_t nPayloadLength = htons(payloadLength);
		memcpy(frameBuffer + index, &nPayloadLength, sizeof(uint16_t));
		outputFrameLength = payloadLength + 4;
		index += 2;
	}

	if (payload && payloadLength)
	{
		memcpy(frameBuffer + index, payload, payloadLength);
	}

#ifdef DEBUG_WEBSOCKET_FRAME
	LogString(SOCKETSERVER_LOG, "[BuildFrame] [WEBSOCKET FRAME] data: [%x %x %x %x %x]",
		frameBuffer[0], frameBuffer[1], frameBuffer[2], frameBuffer[3], frameBuffer[4]
	);
#endif

	return frameBuffer;
}

uint64_t ntoh64(const uint64_t* input)
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

uint64_t hton64(const uint64_t* input)
{
	return (ntoh64(input));
}

bool CServiceWindowManager::IsServiceWindowListenerThreadClosed()
{
	LogString(SOCKETSERVER_LOG, "Waiting for serviceWindowListenerThread to close");
	try
	{
		m_serviceWindowListenerThread->join();
		m_serviceWindowListenerThread.release();
	}
	catch (std::system_error& error)
    {
		// If the tread is already joined, an exception will be thrown. Okay to cathch here.
    }
	
	return (true);
}

void CServiceWindowManager::SetUIConfig(CUIConfig &uiConfig)
{
	m_uiConfig = uiConfig;
}

static void ServiceWindowListenerThreadInner(CServiceWindowManager *serviceWindowManager, CUIProtocol *uiProtocol)
{
	CUtilities::ShowThreadId(_T(THREAD_SERVICEWINDOW_LISTENER));
	
	// This function will loop forever until the parent is shut down.
	serviceWindowManager->ReadAndProcessFlashServer(uiProtocol);
}

static UINT AFX_CDECL ServiceWindowListenerThread(PVOID threadParam)
{
	CUIProtocol *uiProtocol = (CUIProtocol *)threadParam;

	CUtilities::SetThreadName(CUtilities::GetCurrentThreadId(), THREAD_SERVICEWINDOW_LISTENER);

    CServiceWindowManager *serviceWindowManager = uiProtocol->GetServiceWindowManager();

	try
	{
		ServiceWindowListenerThreadInner(serviceWindowManager, uiProtocol);
	}
	catch (...) 
	{
        CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_SERVICEWINDOW_LISTENER);
	}

	CWatchDog::Instance().DeregisterThread();

	return 0;
}

bool CServiceWindowManager::ServiceWindowListenerThreadExited() const
{
	return m_serviceWindowListenerThreadExited;
}

void CServiceWindowManager::LaunchChrome()
{
	cout << CUtilities::GetTickCount() << " *******************************LaunchChrome************************************\n";
	string startPage = CUtilities::IsAutomationMode() ? "no_ui.html" : "index.html";

	/* Generate chrome debug logs when remote UI debugging is enabled.
	   **CAUTION**: 
	   Ensure that we use verbose level --v as -2 (only errors) or -1 (warnings, errors). Anything >= 0 
	   (0: +info, 1: +verbose1, ...) may generate too big log file over the time, especially when doing
	   certain activities like playing videos on chrome. Log file may go like beyond 1 GB over few hours 
	   in this case and if we end up creating this log file on RAMFS path (not physical card path), 
	   device will go out of memory and chrome will eventually crash.
	*/
	const string launchChrome(FormatString("google-chrome %s--kiosk --audio-buffer-size=4096 --disable-translate --disable-sync --disable-features=GoogleUpdate --disable-extensions %s/%s 2>/dev/null &",
		CUtilities::FileExists(CDirectoryManager::REMOTE_UI_FILE) ? "--remote-debugging-port=9222 --enable-logging --v=-2 --log-file=/opt/ncompass/logs/chrome_debug.log " : "",
		CDirectoryManager::BASEURL, startPage.c_str()));

	if (system(launchChrome.c_str()) == 0)
	{
		LogString(SOCKETSERVER_LOG, "Starting Chrome successful %s", launchChrome.c_str());
	}
	else
	{
		LogString(SOCKETSERVER_LOG, "Error starting chrome with command %s", launchChrome.c_str());
		LogString(ERROR_LOG, "Error starting chrome with command %s", launchChrome.c_str());
	}
}

void CServiceWindowManager::KillChrome()
{
	cout << "*******************************KillChrome************************************\n";
	const string killchrome(FormatString("%s/%s", CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_APPLICATIONS).c_str(), KILL_CHROME.c_str()));
	if (system(killchrome.c_str()) == 0)
	{
		LogString(SOCKETSERVER_LOG, "Killing Chrome successful %s", killchrome.c_str());
	}
	else
	{
		LogString(SOCKETSERVER_LOG, "Error killing chrome with command %s", killchrome.c_str());
		LogString(ERROR_LOG, "Error killing chrome with command %s", killchrome.c_str());
	}
}

CFullScreenAttractorCommand* CServiceWindowManager::CreateFullScreenAttractor(
	screenID screen, const string& dynamicContentPath, DWORD displayDurationOverride) const
{
	std::string assetTag;
	DWORD displayDuration(0);
	GetAssetTagAndDuration(screen, assetTag, displayDuration);
	if (displayDurationOverride)
	{
		displayDuration = displayDurationOverride;
	}
	return new CFullScreenAttractorCommand(displayDuration, assetTag, dynamicContentPath);
}