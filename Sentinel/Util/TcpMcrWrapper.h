#pragma once
#include "../Card.h"
#include "../Utilities.h"
#include "../binsem.h"

const string WEBSOCKET_HANDSHAKE_KEY("Sec-WebSocket-Key:");
const string WEBSOCKET_HANDSHAKE_GUID("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

enum TcpMcrCmdType
{
	UNKNOWN,
	MCR_CARD_INSERTED,
	MCR_CARD_REMOVED
};

class CTcpMcrWrapper
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CTcpMcrWrapper(const CConfig &config, int portNumber);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CTcpMcrWrapper(void);	
	
	/// <summary>
	/// Tells Reader Processing Loop to look for card event
	/// </summary>
	/// <param name="bInserted"></param>
	/// <param name="bRemoved"></param>
	/// <param name="bTimeout"></param>
	/// <returns>Returns inserted card or NULL</returns>
	CCard *TcpMcrWaitForCardEvent(bool &bInserted, bool &bRemoved, bool &bTimeout);	
	
	void RefreshConfig(const CConfig &config);

private:

	void StartServer();
	void StopServer();
	TcpMcrCmdType ParseTcpMcrCommand(char* buffer, string& cardNumber);
	string getHandshakeResponse(string key);
	int WriteString(const string& inString);
	int Write(const string& psWriteBuffer);
	BYTE* ParseFrame(const BYTE* frameData, size_t& payloadLength, bool& isPing, bool& isClose, size_t& headerUsed);
	TcpMcrCmdType ParseTcpMcrCommandJSON(char* buffer, string& cardNumber);
	bool ProcessWebSocketFrame(char* buffer, long dataLength);
	bool BuildFrame(BYTE* frameBuffer, size_t frameBufferLength, bool isText, bool isClose, BYTE* payload, size_t payloadLength, size_t& outputFrameLength, bool isPong);
	uint64_t ntoh64(const uint64_t* input);
	uint64_t hton64(const uint64_t* input);

    int     		m_port;
	int             m_serverSocket;
	int 			m_clientSocket;
	std::string  	m_CardNumber;
	bool    		m_eventAvailable;
	binsem  		m_eventConsumed;
	CCardConfig 	m_cardConfig;

	std::unique_ptr<std::thread>      m_networkReadThread;

    static UINT NetworkReadThread(PVOID threadParam);
	
	void NetworkReadLoop();
};