#pragma once

typedef enum
{
	READ_ERROR = 0,
	READ_TIMEOUT,
	READ_SUCCESS,
} ReadStatus;

class CFlashComServer
{
public:
	CFlashComServer(unsigned short port, char packetTerminator);
	~CFlashComServer(void);
	int Initialize();
	int Start();
	void Stop();
	ReadStatus Read(char* buftorecv, long maxlengthofdatatorecv, long& bytesrecv);
	bool AddPacket(unsigned char* InputPacket, long InputPacketLength, unsigned char* OutputPacket, long OutputPacketLength, long* OutputPacketReturnLength);

	void CloseClientSocket();
	int Listen();
	int Accept();
	int Write(const std::string& psWriteBuffer);
protected:
	bool m_gbClientConnectionFailed;	
	void CopyRemainingDataToFrontOfBuffer();
	void ResetBufferToZero();
	bool FindStartCharacter(long startindex = 0);
	bool FindEndCharacter(long startindex = 0);
	bool FindByte(unsigned char* PacketToCheck, long PacketToCheckLength, unsigned char byteToFind, long SearchStartIndex, long* ReturnIndex);
	bool CheckIfSecondStartCharacter();
	bool WillNewPacketExceedBufferMax(long newPacketLength, long outputPacketLength);
	void PushNewDataOnBuffer(unsigned char* InputPacket, long InputPacketLength);
	
	void SetSystemReuse();

	std::unique_ptr<unsigned char> m_buffer;

	unsigned short m_port;
	SOCKET m_gsClientSocket;
	SOCKET m_gsServerSocket;
	long m_bufferLength;

	bool m_usesStartChar;

	unsigned char m_startChar;
	unsigned char m_endChar;

	bool m_startIndexfound;
	bool m_endIndexfound;

	long m_startIndex;
	long m_endIndex;

	std::recursive_mutex m_criticalSection;
};
