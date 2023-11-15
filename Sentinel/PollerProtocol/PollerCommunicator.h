#ifndef _POLLERCOMMUNICATOR_H_
#define _POLLERCOMMUNICATOR_H_

#include "TiltsDef.h"

#define MAXPACKETLENGTH 16384

#define DEFAULTDLEREPLACEMENT 0xE0
#define STX 0xEB
#define ETX 0xEE

#ifdef DEBUG
	#define DEBUGMEMFILLBYTE 0xFF
#endif


typedef enum
{
	SOCKET_READ_ERROR,
	SOCKET_READ_ZERO,
	SOCKET_READ_TIMEOUT,
	SOCKET_READ_SUCCESS,
} SocketReadStatus;

// Forward declare class to declare friend for unit test access.
namespace SentinelNativeUnitTests { class PollerProtocolUnitTests; }

class PollerCommunicator
{
	private:
		//friend class declarations for unit test access.
		friend class SentinelNativeUnitTests::PollerProtocolUnitTests;

		static const long SIZEOF_STX = sizeof(byte);
		static const long SIZEOF_ETX = sizeof(byte);
		static const long SIZEOF_EXTENDED_LENGTH = sizeof(DWORD);

		// The following const should be large enough to cover (most) valid messages.
		static const DWORD TO_HEX_STRING_MAXIMUM_DATA_LENGTH = 1024;

		static const DWORD CONNECT_TIMEOUT_SECONDS = 5;
		static const DWORD SEND_TIMEOUT_SECONDS = 30;

		static const int SOCKET_STATUS_CONNECT_IN_PROGRESS = 115;

		SOCKET _socket;
		SOCKET _listener;
		fd_set write_fds;
		fd_set writeerror_fds;
		fd_set read_fds;
		fd_set readerror_fds;
		fd_set listen_fds;
		bool _connected;        
        struct sockaddr_in rem;
        std::recursive_mutex _sendcritsec;
        std::recursive_mutex _recvcritsec;
        std::recursive_mutex _connectioncritsec;

		// This critical section is used to protect hSessionKey and pbSessionKeyBlob from asynchronous operations.
		// In particular, it is used prevent importing a new session key during calls to CryptEncrypt().
		std::recursive_mutex m_sessionKeyCriticalSection;

		// Even though the virtual key is expected to be used by only one thead, use a critical section to protect it.
		std::recursive_mutex m_sessionVerifyKeyCriticalSection;

		bool _cancelall;
		int _port;
		bool _reading;
		bool _sending;
		long _stxindex;
		long _etxindex;
		BYTE* _extendedpacket;
		BYTE* _buffer;
		BYTE* _tempbuffer;
		BYTE* _tempbufferforsend;
		BYTE* _tempbufferforrecv;
		BYTE* _tempbufferforCreate;
		long _bufferlength;
		DWORD m_pollerPacketSentVerifyErrors;
		bool FindByte(BYTE* PacketToCheck,long PacketToCheckLength,BYTE byteToFind, long SearchStartIndex, long* ReturnIndex);
		void ResetBufferToZero();
		bool WillNewPacketExceedBufferMax(long NewPacketLength);
		bool FindFirstPacket();
		bool FindSTX(long startindex = 0);
		bool FindETX(long startindex = 0);
		bool CheckIfSTXandETXMatch();
		bool CopyFoundPacketToOutputPacketAndDLEDecode(BYTE* extendedpacket, long extendedPacketBufferSize, long *extendedPacketReturnLength, BYTE* OutputPacket, long OutputPacketLength, long *OutputPacketReturnLength);
		bool CheckIfSecondSTX();
		void CopyRemainingDataToFrontOfBuffer();
		void PushNewDataOnBuffer(BYTE* InputPacket, long InputPacketLength);
		bool SetUpEncryption();
		void TearDownEncryption();
		void StripOffEncryptionPayload(BYTE* InputPacket, long InputPacketLength, BYTE* EncryptionPayload, long &EncryptionPayloadLength);

		// Functions related to verification of sent packets.
		void CheckForNewSessionVerifyKey();
		void LogBuffer(LPCTSTR prefix, LPCTSTR bufferDesc, const BYTE *buffer, long bufferLength, LPCTSTR callerDesc);
		void LogInfoBuffer(LPCTSTR bufferDesc, const BYTE *buffer, long bufferLength, LPCTSTR callerDesc);
		void LogInvalidBuffer(LPCTSTR bufferDesc, const BYTE *buffer, long bufferLength, LPCTSTR callerDesc);
		bool VerifyPacketSentInputs(const BYTE *massagedPacket, long massagedPacketLength, const BYTE *message, long messageLength);
		bool VerifyPacketSentStxEtx(const BYTE *massagedPacket, long massagedPacketLength);
		bool VerifyPacketSentExtendedLength(const BYTE *massagedPacket, long massagedPacketLength,
			const BYTE *packetSent, long packetSentLength, DWORD &extendedLength);
        void VerifyPacketSentDecrypt(BYTE *messageSent, DWORD messageSentLengthEncrypted);
		bool VerifyPacketSentMessage(const BYTE *massagedPacket, long massagedPacketLength, const BYTE *packetSent, long packetSentLength,
			const BYTE *messageSent, DWORD messageSentLength, const BYTE *message, long messageLength);
		
        RSA* hExchangePubKey;
        RC4_KEY* hSessionKey;
        RC4_KEY* hSessionVerifyKey;
		PBYTE pbExchangePubKeyBlob;
		DWORD dwExchangePubKeyBlobLength;
		bool bEncryptionSetup;
		bool bpublickeywassent;
		bool bsessionkeywassent;

	public:
		PollerCommunicator();
		virtual ~PollerCommunicator();

		/// <summary>
		/// Attempt to connect to a remote computer.
		/// </summary>
		/// <param name="HostName">IN - Name of the host.</param>
		/// <param name="Port">IN - The port.</param>
		/// <param name="tiltCode">IN/OUT - Set to indicate if a problem was detected, otherwise unchanged.</param>
		/// <returns>true if connected, false otherwise.</returns>
		bool Connect(std::string HostName, int Port, TiltCode &tiltCode);

		void Close();
		void Cancel();
		virtual bool Connected();
		bool Reading();
		bool Sending();
		void ResetCancel();
		virtual bool Send(char *datatosend, long lengthofdatatosend, long &bytessent);
		SocketReadStatus Read(char *buftorecv, long maxlengthofdatatorecv, long &bytesrecv, long timeout = 15000);

		/// <summary>
		/// Return the number of poller packet sent verify errors that have been detected since startup.
		/// </summary>
		/// <returns>The number of poller packet sent verify errors that have been detected since startup.</returns>
		DWORD GetPollerPacketSentVerifyErrors();

		/// <summary>
		/// Creates the packet to send.
		/// </summary>
		/// <param name="packettomassage">IN - The packet to massage.</param>
		/// <param name="massagedpacket">OUT - The massaged packet.</param>
		/// <param name="packetlength">IN - The length of the packet to massage.</param>
		/// <param name="massagedpacketlength">OUT - The length of the massaged packet.</param>
		/// <param name="tiltCode">IN/OUT - Set to indicate if a problem was detected, otherwise unchanged.</param>
		/// <param name="messageID">IN - Message ID passes for better logging and exceptional cases </param>
		/// <param name="newPacketSize">IN - New packet size passed for exceptional cases like for message 322 which could be bigger</param>
		/// <returns>true for success, false otherwise.</returns>
		/// <remarks>
		/// The caller is responsible for preventing buffer overruns.
		/// </remarks>
		bool CreatePacketToSend(const BYTE* packettomassage,
			BYTE* massagedpacket,
			long packetlength,
			long* massagedpacketlength,
			bool alreadySuspicious,
			const BYTE* nCompassMacAddress,
			TiltCode& tiltCode,
			WORD messageID,
			DWORD newPacketSize);

		/// <summary>
		/// Resizes the packet to send for exceptional cases like for message 322 which could be bigger.
		/// </summary>
		/// <param name="packet">IN - The packet buffer.</param>
		/// <param name="packetlength">IN - The current length of the packet.</param>
		/// <param name="messageID">IN - Message ID passes for better logging and exceptional cases </param>
		/// <param name="newPacketSize">IN - New packet size</param>
		/// <returns>true for success, false otherwise.</returns>
		/// <remarks>
		/// Returns the status of memory allocation.
		/// </remarks>
		bool ResizePacketToSendIfRequired(BYTE **packet,
			DWORD packetlength,
			WORD messageID,
			DWORD newPacketSize);

		/// <summary>
		/// Verifies the packet sent.  If the verfication fails then a diagnostic log message is generated.
		/// </summary>
		/// <param name="massagedPacket">IN - The massaged packet.</param>
		/// <param name="massagedPacketLength">IN - Length of the massaged packet.</param>
		/// <param name="message">IN - The message.</param>
		/// <param name="messageLength">IN - Length of the message.</param>
		/// <returns>True if the verification succeded, false otherwise (the verfication failed).</returns>
		/// <remarks>
		/// Input parameters are also verified.
		/// </remarks>
		virtual bool VerifyPacketSent(const BYTE *massagedPacket, long massagedPacketLength, const BYTE *message, long messageLength);

		/// <summary>
		/// DLE decodes the packet data.
		/// </summary>
		/// <param name="pabyPacketData">IN - The packet data.</param>
		/// <param name="pabyDecodeBuffer">OUT - The decoded packet data buffer.</param>
		/// <param name="lPacketLength">IN - Length of the packet data.</param>
		/// <returns>The length of the decoded packet data.</returns>
		/// <remarks>
		/// The caller is responsible for preventing buffer overruns.
		/// The caller is responsible for ensuring that the last BYTE of the packet data is not the first BYTE of a DLE escape/replacement sequence.
		/// </remarks>
		long DLEDecode(const BYTE *pabyPacketData, BYTE *pabyDecodeBuffer, long lPacketLength);

		/// <summary>
		/// DLE encodes the packet data.
		/// </summary>
		/// <param name="pabyPacketData">IN - The packet data.</param>
		/// <param name="pabyEncodeBuffer">OUT - The encoded packet data buffer.</param>
		/// <param name="lPacketLength">IN - Length of the packet data.</param>
		/// <returns>The length of the encoded packet data.</returns>
		/// <remarks>
		/// The caller is responsible for preventing buffer overruns.
		/// </remarks>
		long DLEEncode(const BYTE *pabyPacketData, BYTE *pabyEncodeBuffer, long lPacketLength);

		bool AddPacket(BYTE* InputPacket, long InputPacketLength, BYTE* OutputPacket, long OutputPacketLength, long *OutputPacketReturnLength);
};




#endif // _POLLERCOMMUNICATOR_H_
