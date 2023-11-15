#pragma once
#include "GlobalDefs.h"
#include "metersdef.h"
#include "TiltsDef.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class PollerCheckMessage; }

/// <summary>
/// Class for checking messages sent to the FloorLogix Poller Client.
/// Also see Tools\NvramParser\NvramParser.vcxproj for human-readable transaction parsing.
/// </summary>
class CPollerCheckMessage
{
public:
	/// <summary>
	/// Checks the transaction.
	/// </summary>
	/// <param name="data">IN - A pointer to a raw message transaction buffer.</param>
	/// <param name="transLength">IN - The length of the raw message transaction buffer.</param>
	/// <param name="nCompassMacAddress">IN - A pointer to the 6-byte MAC address for the nCompass unit. MAY be nullptr.</param>
	/// <param name="tiltCode">IN/OUT - Set to indicate if a problem was detected, otherwise unchanged.</param>
	/// <param name="reason">IN/OUT - Set to indicate the reason if a tiltCode was set, otherwise unchanged.</param>
	/// <param name="isNFDPoller">IN - Are we talking to NFD poller</param>
	/// <returns>false if the message is a transaction and there is something wrong with the transaction, otherwise true.</returns>
	/// <remarks>
	/// If the nCompass MAC address is not available yet, the MAC address MAY have all bytes set to zero.
	/// 2015-10-13 If checking messages when/after building the Poller resend list
	///    then Internals probably should be modified to send the the MAC address to the Poller Protocol before sending the NVRAM status,
	///    as the Poller Protocol currently builds the resend list when it receives the NVRAM status from Internals.
	/// </remarks>
	static void CheckMessage(const BYTE *msg,
		WORD msgLength,
		const BYTE *nCompassMacAddress,
		TiltCode &tiltCode,
		WORD messageID,
		std::string &reason,
		bool isNFDPoller = true);

private:
	// Theoretically, DLE encoding can almost double the length of a message.
	// So, any message length that is over half of the allocated buffer space is suspicious.
	static const DWORD MAXIMUM_MESSAGE_LENGTH = SIZE_OF_READ_BUFFER / 2;

	static const DWORD MINIMUM_UNIQUE_ID; // Zero is invalid.

	static const DWORD MINIMUM_METER_COUNT;
	static const DWORD MAXIMUM_METER_COUNT;

	static const DWORD MINIMUM_METER_NUMBER;
	static const DWORD MAXIMUM_METER_NUMBER;

	static const __int64 MINIMUM_METER_VALUE; // nCompass does not send INVALID_METER_VALUE (-1) to the Poller.
	static const __int64 MAXIMUM_METER_VALUE; // 18-digits should be the maximum value for meters obtained from SAS.

	static const __int64 MINIMUM_COIN_IN_METER_VALUE;
	static const __int64 MINIMUM_COIN_OUT_METER_VALUE;

	static const std::string MeterNameUnknown;

	static const BYTE m_macAddressNoInit[SIZE_OF_MAC_ADDRESS];
	static const BYTE m_macAddressBroadcast[SIZE_OF_MAC_ADDRESS];

	static bool CheckMessageLength(const BYTE *msg, DWORD &msgOffset, WORD msgLength, WORD messageID, std::string &reason);
	static bool CheckMessageHeader(const BYTE *msg,
		DWORD &msgOffset,
		WORD msgLength,
		const BYTE *nCompassMacAddress,
		WORD &messageId,
		std::string &reason);
	static bool CheckMessageHeader2(const BYTE *msg,
		DWORD &msgOffset,
		const BYTE *nCompassMacAddress,
		std::string &reason);
	static bool CheckTransactionFixedFields(const BYTE *msg,
		DWORD &msgOffset,
		WORD msgLength,
		BYTE &transactionCode,
		std::string &reason);
	static void GetMetersNeeded(bool metersNeeded[], unsigned metersNeededLength);
	static const std::string &GetMeterName(BYTE meterNumber);
	static bool CheckForMissingMeters(bool metersNeeded[], unsigned metersNeededLength, std::string &reason);
	static bool CheckTransactionMeterFields(const BYTE *msg,
		DWORD &msgOffset,
		WORD msgLength,
		BYTE transCode,
		std::string &reason,
		bool isNFDPoller);
	static bool CheckTransactionDataFields(const BYTE *msg,
		DWORD &msgOffset,
		WORD msgLength,
		BYTE transactionCode,
		std::string &reason);
	static bool CheckTransactionDataFieldsLoop(const BYTE *msg,
		DWORD &msgOffset,
		WORD msgLength,
		BYTE transactionCode,
		BYTE transDataCount,
		std::string &reason);
	static bool CheckTransactionDataFieldsEnd(
		DWORD &msgOffset,
		WORD msgLength,
		BYTE transDataCount,
		BYTE transDataCountParsed,
		std::string &reason);
	
	static bool CheckTransaction(
		const BYTE *msg, 
		DWORD &msgOffset, 
		WORD msgLength, 
		std::string &reason,
		bool isNFDPoller);

	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::PollerCheckMessage;

	FRIEND_TEST(PollerCheckMessage, PollerCheckMessage_Constants);
};
