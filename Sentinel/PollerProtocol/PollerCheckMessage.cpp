#include "stdafx.h"
#include "PollerCheckMessage.h"

// Sentinel includes.
#include "Diagnostics.h"
#include "Logging/LogString.h"
#include "PollerMessage.h"
#include "resource.h"
#include "UI/ScreenDefs.h"

using namespace std;

const DWORD CPollerCheckMessage::MINIMUM_UNIQUE_ID = 1;

const DWORD CPollerCheckMessage::MINIMUM_METER_COUNT = 52;
const DWORD CPollerCheckMessage::MAXIMUM_METER_COUNT = NUM_METERS;

const DWORD CPollerCheckMessage::MINIMUM_METER_NUMBER = mCD;
const DWORD CPollerCheckMessage::MAXIMUM_METER_NUMBER = NUM_METERS;

const __int64 CPollerCheckMessage::MINIMUM_METER_VALUE = 0;
const __int64 CPollerCheckMessage::MAXIMUM_METER_VALUE = 999999999999999999;

const __int64 CPollerCheckMessage::MINIMUM_COIN_IN_METER_VALUE = 1;
const __int64 CPollerCheckMessage::MINIMUM_COIN_OUT_METER_VALUE = 1;

const std::string CPollerCheckMessage::MeterNameUnknown("Unknown");
const BYTE CPollerCheckMessage::m_macAddressNoInit[SIZE_OF_MAC_ADDRESS] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const BYTE CPollerCheckMessage::m_macAddressBroadcast[SIZE_OF_MAC_ADDRESS] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

bool CPollerCheckMessage::CheckMessageLength(const BYTE *msg, DWORD &msgOffset, WORD msgLength, WORD messageID, std::string &reason)
{
	UNREFERENCED_PARAMETER(msg);
	UNREFERENCED_PARAMETER(msgOffset);
	bool ok(true);

	// A message length of zero is used for handshake messages.
	if (0 < msgLength && msgLength < SIZE_OF_MESSAGE_HEADER)
	{
		ok = false;
		reason = FormatString(IDS_FMT_MSG_LEN_HEADER,
			(unsigned)msgLength,
			(unsigned)SIZE_OF_MESSAGE_HEADER);
	}
	else if (msgLength > MAXIMUM_MESSAGE_LENGTH && messageID != POLLER_MSG_ALL_GAME_LIST)
	{
		// Theoretically, DLE encoding can almost double the length of a message.
		// So, any message length that is over half of the allocated buffer space is suspicious.
		ok = false;
		reason = FormatString(IDS_FMT_MSG_LEN_TOO_BIG,
			(unsigned)msgLength,
			(unsigned)MAXIMUM_MESSAGE_LENGTH,
			messageID);
	}
	// Should there be any other check for message length for this particular packet?
	else if (POLLER_MSG_ALL_GAME_LIST == messageID && msgLength > MAXIMUM_MESSAGE_LENGTH)
	{
		ok = true;
		LogString (POLLER_LOG, "%s: Message size too big (%u) for message id %u but will try to send.", 
			__FUNCTION__, msgLength, messageID);
	}

	return ok;
}

// msgLength should be >= SIZE_OF_MESSAGE_HEADER.
// msgOffset should be 0.
// If a transaction is recovered from NVRAM then the message header should be valid.
bool CPollerCheckMessage::CheckMessageHeader(const BYTE *msg,
	DWORD &msgOffset,
	WORD msgLength,
	const BYTE *nCompassMacAddress,
	WORD &messageId,
	std::string &reason)
{
	bool ok(true);
	messageId = (POLLER_MSG_NONE);
	PollerMessageHeader MessageHeader;

	// Copy the message header fields one at a time, in case they are not packed.
	memcpy(&MessageHeader.PacketVersion, msg + msgOffset, sizeof(MessageHeader.PacketVersion));
	msgOffset += sizeof(MessageHeader.PacketVersion);
	if (MessageHeader.PacketVersion < PACKET_VERSION_MINIMUM || PACKET_VERSION_MAXIMUM < MessageHeader.PacketVersion)
	{
		ok = false;
		reason = FormatString(IDS_FMT_INVALID_PACKET_VERSION,
			(unsigned)MessageHeader.PacketVersion);
	}

	if (ok)
	{
		memcpy(&MessageHeader.MessageID, msg + msgOffset, sizeof(MessageHeader.MessageID));
		msgOffset += sizeof(MessageHeader.MessageID);
		messageId = MessageHeader.MessageID;
		if (MessageHeader.MessageID < POLLER_MSG_MINIMUM || POLLER_MSG_MAXIMUM < MessageHeader.MessageID)
		{
			ok = false;
			reason = FormatString(IDS_FMT_INVALID_MESSAGE_ID,
				(unsigned)MessageHeader.MessageID);
		}
	}

	if (ok)
	{
		memcpy(&MessageHeader.DataLength, msg + msgOffset, sizeof(MessageHeader.DataLength));
		msgOffset += sizeof(MessageHeader.DataLength);

		// Only check the data length for transaction messages.
		// Otherwise, need exceptions for POLLER_MSG_REQUEST_CONFIG (301), POLLER_MSG_SENTINEL_STATUS (303) and perhaps others.
		if (POLLER_MSG_TRANSACTION == messageId)
		{
			if (MessageHeader.DataLength != msgLength - SIZE_OF_MESSAGE_HEADER)
			{
				ok = false;
				reason = FormatString(IDS_FMT_INCORRECT_DATA_LEN,
					(unsigned)MessageHeader.DataLength,
					(unsigned)msgLength,
					(unsigned)SIZE_OF_MESSAGE_HEADER);
			}
		}
	}

	if (ok)
	{
		ok = CheckMessageHeader2(msg, msgOffset, nCompassMacAddress, reason);

	}

	return ok;
}

bool CPollerCheckMessage::CheckMessageHeader2(const BYTE *msg,
	DWORD &msgOffset,
	const BYTE *nCompassMacAddress,
	std::string &reason)
{
	bool ok(true);
	PollerMessageHeader MessageHeader;

	memcpy(&MessageHeader.MacAddress, msg + msgOffset, sizeof(MessageHeader.MacAddress));
	msgOffset += sizeof(MessageHeader.MacAddress);
	if (0 == memcmp(m_macAddressNoInit, MessageHeader.MacAddress, sizeof(MessageHeader.MacAddress)) ||
		0 == memcmp(m_macAddressBroadcast, MessageHeader.MacAddress, sizeof(MessageHeader.MacAddress)))
	{
		ok = false;
		reason = IDS_INVALID_MAC_ADDRESS;
	}
	else if (nCompassMacAddress != nullptr &&
		0 != memcmp(nCompassMacAddress, m_macAddressNoInit, sizeof(m_macAddressNoInit)))
	{
		// If the Poller has received the nCompass MAC address from Internals then require a match.
		// We expect that valid MAC address(es) and NVRAM will remain with the same nCompass unit.
		if (0 != memcmp(nCompassMacAddress, MessageHeader.MacAddress, sizeof(MessageHeader.MacAddress)))
		{
			ok = false;
			reason = IDS_WRONG_MAC_ADDRESS;
		}
	}

	if (ok)
	{
		memcpy(&MessageHeader.SequenceNumber, msg + msgOffset, sizeof(MessageHeader.SequenceNumber));
		msgOffset += sizeof(MessageHeader.SequenceNumber);
		// All sequence numbers could be valid.
	}

	if (ok)
	{
		memcpy(&MessageHeader.SlotMastID, msg + msgOffset, sizeof(MessageHeader.SlotMastID));
		msgOffset += sizeof(MessageHeader.SlotMastID);
		if (MessageHeader.SlotMastID > INT32_MAX)
		{
			// Bart and/or Oasis may be more restrictive about allowed SlotMastID values, but allow any non-negative INT32 values.
			ok = false;
			reason = FormatString(IDS_FMT_INVALID_SLOTMAST_ID,
				(unsigned)MessageHeader.SlotMastID);
		}
	}

	if (ok)
	{
		memcpy(&(MessageHeader.SlotmastRevision), msg + msgOffset, sizeof(MessageHeader.SlotmastRevision));
		msgOffset += sizeof(MessageHeader.SlotmastRevision);
		// All slot master revision values could be valid.
	}

	return ok;
}

bool CPollerCheckMessage::CheckTransactionFixedFields(const BYTE *msg, DWORD &msgOffset, WORD msgLength, BYTE &transactionCode, std::string &reason)
{
	bool ok(true);

	// Check for minimum required length.
	if (msgOffset + NONVARIABLE_TRANSACTION_SIZE > msgLength)
	{
		ok = false;
		reason = FormatString(IDS_FMT_MSG_LEN_FIXED_DATA,
			(unsigned)msgLength,
			(unsigned)(SIZE_OF_MESSAGE_HEADER + NONVARIABLE_TRANSACTION_SIZE));
	}

	if (ok)
	{
		// Transaction code.
		memcpy(&transactionCode, msg + msgOffset, sizeof(transactionCode));
		msgOffset += sizeof(transactionCode);
		if (transactionCode <= NONE_SPECIFIED || TRANSACTION_CODE_MAXIMUM < transactionCode)
		{
			ok = false;
			reason = FormatString(IDS_FMT_INVALID_TRANS_CODE,
				(unsigned)transactionCode);
		}
	}

	if (ok)
	{
		// Transaction time.
		SYSTEMTIME transTime;
		memcpy(&transTime, msg + msgOffset, sizeof(transTime));
		msgOffset += sizeof(transTime);
		// Leap seconds are not supported by Microsoft SYSTEMTIME, per https://msdn.microsoft.com/en-us/library/ms724950(VS.85).aspx.
		if (transTime.wYear < 1970 || 2099 < transTime.wYear ||
			transTime.wMonth < 1 || 12 < transTime.wMonth ||
			transTime.wDay < 1 || 31 < transTime.wDay ||
			// Skip the day of the week, as it is generally not critical.
			23 < transTime.wHour ||
			59 < transTime.wMinute ||
			59 < transTime.wSecond ||
			999 < transTime.wMilliseconds)
		{
			ok = false;
			reason = FormatString(IDS_FMT_INVALID_TRANS_TIME,
				transTime.wYear,
				transTime.wMonth,
				transTime.wDay,
				transTime.wHour,
				transTime.wMinute,
				transTime.wSecond,
				transTime.wMilliseconds);
		}
	}

	if (ok)
	{
		// Unique ID.
		DWORD uniqueId;
		memcpy(&uniqueId, msg + msgOffset, sizeof(uniqueId));
		msgOffset += sizeof(uniqueId);
		if (uniqueId < MINIMUM_UNIQUE_ID)
		{
			ok = false;
			reason = FormatString(IDS_FMT_INVALID_UNIQUE_ID,
				(unsigned)uniqueId);
		}
	}

	if (ok)
	{
		// Status.
		DWORD transStatus;
		memcpy(&transStatus, msg + msgOffset, sizeof(transStatus));
		msgOffset += sizeof(transStatus);
		// Allow any value for the transaction status bit flags.
	}

	if (ok)
	{
		// Card Number.
		byte cardID[CARD_ID_LEN];
		memcpy(cardID, msg + msgOffset, sizeof(cardID));
		msgOffset += sizeof(cardID);
		// For compatibility with card numbers for non-Oasis cards, allow any card number.
	}

	return ok;
}

// Clear all the values in metersNeeded, then set the indexes that correspond to meters needed for a transaction.
// "metersNeededLength" MUST be at least the value of the highest meter number needed.
void CPollerCheckMessage::GetMetersNeeded(bool metersNeeded[], unsigned metersNeededLength)
{
	for (unsigned meterNumber = 0; meterNumber < metersNeededLength; ++meterNumber)
	{
		metersNeeded[meterNumber] = false;
	}

	metersNeeded[mCD] = true;
	metersNeeded[mSDr] = true;
	metersNeeded[mDDr] = true;
	metersNeeded[mLDr] = true;
	metersNeeded[mCDr] = true;
	metersNeeded[mFDr] = true;
	metersNeeded[mD1] = true;
	metersNeeded[mD2] = true;
	metersNeeded[mD5] = true;
	metersNeeded[mD10] = true;
	metersNeeded[mD20] = true;
	metersNeeded[mD50] = true;
	metersNeeded[mD100] = true;
	metersNeeded[mCoupV] = true;
	metersNeeded[mTVal] = true;
	metersNeeded[mDr] = true;
	metersNeeded[mCrd] = true;
	metersNeeded[mTCi] = true;
	metersNeeded[mTCo] = true;
	metersNeeded[mBD] = true;
	metersNeeded[mTHP] = true;
	metersNeeded[mCTktPV] = true;
	metersNeeded[mHPWinRcptVal] = true;
	metersNeeded[mActualCD] = true;
	metersNeeded[mCPromoOutVal] = true;
	metersNeeded[gCI] = true;
	metersNeeded[gCO] = true;
	metersNeeded[gGS] = true;
	metersNeeded[gGW] = true;
	metersNeeded[gMC] = true;
	metersNeeded[gCC] = true;
	// The gBW meter is not required for older transactions.
	metersNeeded[gJP] = true;
	metersNeeded[gPI] = true;
	metersNeeded[mHCC] = true;
	metersNeeded[mActualSlotDr] = true;
	metersNeeded[mPbAC2G] = true;
	metersNeeded[mPbAR2G] = true;
	metersNeeded[mPbAN2G] = true;
	metersNeeded[mPbAC2H] = true;
	metersNeeded[mPbAR2H] = true;
	metersNeeded[mPbAN2H] = true;
	metersNeeded[mPbAD2G] = true;
	metersNeeded[mBonusMach] = true;
	metersNeeded[mBonusAtt] = true;
	metersNeeded[mPrgPayAtt] = true;
	metersNeeded[mPrgPayMac] = true;
	metersNeeded[mTValueCI] = true;
	metersNeeded[mTValueCO] = true;
	metersNeeded[mRstrPlayd] = true;
	metersNeeded[mGameWTPP] = true;
	metersNeeded[mTotalDrop] = true;
	metersNeeded[mVchrTktDrop] = true;
}

const std::string &CPollerCheckMessage::GetMeterName(BYTE meterNumber)
{
	if (meterNumber < _countof(TXT_METER_NAMES))
	{
		return TXT_METER_NAMES[meterNumber];
	}

	return MeterNameUnknown;
}

bool CPollerCheckMessage::CheckForMissingMeters(bool metersNeeded[], unsigned metersNeededLength, std::string &reason)
{
	bool ok(true);

	for (unsigned meterNumber = 0; meterNumber < metersNeededLength && ok; ++meterNumber)
	{
		if (metersNeeded[meterNumber])
		{
			// Meter number is still needed after processing all the meters in the transaction.
			ok = false;
			reason = FormatString(IDS_FMT_METER_NUMBER_MISSING,
				(unsigned)meterNumber,
				static_cast<LPCTSTR>(GetMeterName(meterNumber).c_str()));
		}
	}

	return ok;
}

bool CPollerCheckMessage::CheckTransactionMeterFields(const BYTE *msg, 
	DWORD &msgOffset, 
	WORD msgLength, 
	BYTE transCode, 
	std::string &reason,
	bool isNFDPoller)
{
	bool ok(true);
	bool metersNeeded[BYTE_MAX + 1];
	GetMetersNeeded(metersNeeded, _countof(metersNeeded));

	// Meter Count.
	BYTE meterCount = 0;
	if (msgOffset + sizeof(meterCount) > msgLength)
	{
		ok = false;
		reason = FormatString(IDS_FMT_MSG_LEN_METER_COUNT,
			(unsigned)msgLength,
			(unsigned)(SIZE_OF_MESSAGE_HEADER + NONVARIABLE_TRANSACTION_SIZE + sizeof(meterCount)));
	}
	if (ok)
	{
		memcpy(&meterCount, msg + msgOffset, sizeof(meterCount));
		msgOffset += sizeof(meterCount);
		if ((!isNFDPoller && meterCount < MINIMUM_METER_COUNT) || 
			(MAXIMUM_METER_COUNT < meterCount))
		{
			ok = false;
			reason = FormatString(IDS_FMT_INVALID_METER_COUNT,
				(unsigned)meterCount);
		}
	}

	// Meters.
	PollerMeter meter;
	if (msgOffset + meterCount * (sizeof(meter.Number) + sizeof(meter.Value)) > msgLength)
	{
		ok = false;
		reason = FormatString(IDS_FMT_MSG_LEN_METERS,
			(unsigned)msgLength,
			(unsigned)(SIZE_OF_MESSAGE_HEADER + NONVARIABLE_TRANSACTION_SIZE + meterCount * (sizeof(meter.Number) + sizeof(meter.Value))),
			(unsigned)meterCount);
	}

	if (ok)
	{
		for (int count = 0; count < meterCount && count < NUM_METERS && ok; count++)
		{
			memcpy(&meter.Number, msg + msgOffset, sizeof(meter.Number));
			msgOffset += sizeof(meter.Number);
			memcpy(&meter.Value, msg + msgOffset, sizeof(meter.Value));
			msgOffset += sizeof(meter.Value);
			if (meter.Number < MINIMUM_METER_NUMBER || MAXIMUM_METER_NUMBER < meter.Number ||
				meter.Value < MINIMUM_METER_VALUE || MAXIMUM_METER_VALUE < meter.Value)
			{
				ok = false;
				reason = FormatString(IDS_FMT_INVALID_METER,
					(unsigned)meter.Number,
					GetMeterName(meter.Number).c_str(),
					(long long)meter.Value);
			}
			else if (PLAYER_CARD_IN_CODE == transCode || PLAYER_CARD_OUT_CODE == transCode || PLAYER_UPDATE_CODE == transCode)
			{
				if ((gCI == meter.Number && meter.Value < MINIMUM_COIN_IN_METER_VALUE) ||
					(gCO == meter.Number && meter.Value < MINIMUM_COIN_OUT_METER_VALUE))
				{
					ok = false;
					reason = FormatString(IDS_FMT_INVALID_METER_PLAYER,
						(unsigned)meter.Number,
						static_cast<LPCTSTR>(GetMeterName(meter.Number).c_str()),
						(long long)meter.Value);
				}
			}

			metersNeeded[meter.Number] = false;
		}
	}

	if (ok && !isNFDPoller)
	{
		// Check for missing meter numbers. Non-NFD pollers  
		// require a minimum fixed set of 52 meters.
		ok = CheckForMissingMeters(metersNeeded, _countof(metersNeeded), reason);
	}

	return ok;
}

// For now, don't verify values.
bool CPollerCheckMessage::CheckTransactionDataFields(const BYTE *msg,
	DWORD &msgOffset,
	WORD msgLength,
	BYTE transactionCode,
	std::string &reason)
{
	bool ok(true);

	// It is normal for a transaction to omit the Transaction Data Count field and all following fields.
	if (msgOffset < msgLength)
	{
		// Transaction Data count.
		BYTE transDataCount(0);
		if (msgOffset + sizeof(transDataCount) > msgLength)
		{
			ok = false;
			reason = FormatString(IDS_FMT_MSG_LEN_DATA_COUNT,
				(unsigned)msgLength,
				(unsigned)(msgOffset + sizeof(transDataCount)));
		}
		if (ok)
		{
			if (msgOffset + sizeof(transDataCount) <= msgLength)
			{
				memcpy(&transDataCount, msg + msgOffset, sizeof(transDataCount));
				msgOffset += sizeof(transDataCount);
				if (transDataCount > DATA_TYPE_MAXIMUM) // Really should be much less than this.
				{
					ok = false;
					reason = FormatString(IDS_FMT_INVALID_DATA_COUNT,
						(unsigned)transDataCount);
				}
			}
		}

		if (ok)
		{
			ok = CheckTransactionDataFieldsLoop(msg, msgOffset, msgLength, transactionCode, transDataCount, reason);
		}
	}

	return ok;
}

bool CPollerCheckMessage::CheckTransactionDataFieldsLoop(const BYTE *msg,
	DWORD &msgOffset,
	WORD msgLength,
	BYTE transactionCode,
	BYTE transDataCount,
	std::string &reason)
{
	bool ok(true);
	BYTE transDataCountParsed(0);

	for (int count = 0; count < transDataCount && count < DATA_TYPE_MAXIMUM && ok; count++)
	{
		// Transaction Data Type.
		byte transDataType;
		if (msgOffset + sizeof(transDataType) > msgLength)
		{
			ok = false;
			reason = FormatString(IDS_FMT_MSG_LEN_DATA_TYPE,
				(unsigned)msgLength,
				(unsigned)(msgOffset + sizeof(transDataType)),
				(unsigned)transDataCountParsed,
				(unsigned)transDataCount);
		}
		if (ok)
		{
			memcpy(&transDataType, msg + msgOffset, sizeof(transDataType));
			msgOffset += sizeof(transDataType);
			if (transDataType < DATA_TYPE_MINIMUM || DATA_TYPE_MAXIMUM < transDataType)
			{
				ok = false;
				reason = FormatString(IDS_FMT_INVALID_DATA_TYPE,
					(unsigned)transDataType,
					(unsigned)transDataCountParsed,
					(unsigned)transDataCount);
			}
		}

		// Transaction Data Size.
		byte transDataSize; // How much data is actually present for the transDataType.
		if (msgOffset + sizeof(transDataSize) > msgLength)
		{
			ok = false;
			reason = FormatString(IDS_FMT_MSG_LEN_DATA_SIZE,
				(unsigned)msgLength,
				(unsigned)(msgOffset + sizeof(transDataSize)),
				(unsigned)sizeof(transDataSize),
				(unsigned)transDataCountParsed,
				(unsigned)transDataCount);
		}
		if (ok)
		{
			memcpy(&transDataSize, msg + msgOffset, sizeof(transDataSize));
			msgOffset += sizeof(transDataSize);

			// 2014-03-01 Workaround(s) for Sentinel III and/or nCompass transaction format issues.
			// FloorLogix and the Poller Data Parser tool could have similar workaround code.
			if (transDataType == DATA_TYPE_TOTAL_COIN_OUT)
			{
				if (transactionCode == ONE_TIME_BONUS_EVENT_CODE || transactionCode == MULTIPLE_BONUS_EVENT_CODE)
				{
					if (transDataSize == sizeof(DWORD))
					{
						transDataSize = sizeof(__int64);
					}
				}
			}
		}

		// Don't bother validating dataSize vs. dataType for now.  An incorrect data size will probably cause subsequent tests to fail.
		if (ok)
		{
			if (msgOffset + transDataSize > msgLength)
			{
				ok = false;
				reason = FormatString(IDS_FMT_MSG_LEN_DATA_SIZE,
					(unsigned)msgLength,
					(unsigned)(msgOffset + transDataSize),
					(unsigned)transDataSize,
					(unsigned)transDataCountParsed,
					(unsigned)transDataCount);
			}
		}
		if (ok)
		{
			// Skip reading and validating the transaction data value.  Just increment the offset past the value.
			msgOffset += transDataSize;
		}

		++transDataCountParsed;
	}

	if (ok)
	{
		if ( (transactionCode >= MOBILE_CONNECT_PAIRING_SETTINGS_REQUEST)
			&& (transactionCode <= PROXIMITY_STATUS) )
		{
			WORD blobDataSize;
			memcpy(&blobDataSize, msg + msgOffset, sizeof(blobDataSize));

			// Skip reading and validating the blob. Just increment the offset past the value.
			msgOffset += sizeof(blobDataSize);
			msgOffset += blobDataSize;
		}

		ok = CheckTransactionDataFieldsEnd(msgOffset, msgLength, transDataCount, transDataCountParsed, reason);
	}

	return ok;
}

bool CPollerCheckMessage::CheckTransactionDataFieldsEnd(
	DWORD &msgOffset,
	WORD msgLength,
	BYTE transDataCount,
	BYTE transDataCountParsed,
	std::string &reason)
{
	bool ok(true);

	// Check for unused/unparsed message length.
	if (msgOffset < msgLength)
	{
		ok = false;
		reason = FormatString(IDS_FMT_MSG_LEN_TRANS_MSG,
			(unsigned)msgOffset,
			(unsigned)msgLength,
			(unsigned)transDataCountParsed,
			(unsigned)transDataCount);

	}

	if (ok)
	{
		// Internal checks.
		if (msgOffset != msgLength || transDataCountParsed != transDataCount)
		{
			ok = false;
			reason = FormatString(IDS_FMT_MSG_LEN_INCONSISTENT,
				(unsigned)msgOffset,
				(unsigned)msgLength,
				(unsigned)transDataCountParsed,
				(unsigned)transDataCount);

		}
	}

	return ok;
}

bool CPollerCheckMessage::CheckTransaction(const BYTE *msg, 
	DWORD &msgOffset, 
	WORD msgLength, 
	std::string &reason,
	bool isNFDPoller)
{
	bool ok(true);
	byte transactionCode(NONE_SPECIFIED);

	ok = CheckTransactionFixedFields(msg, msgOffset, msgLength, transactionCode, reason);

	if (ok)
	{
		ok = CheckTransactionMeterFields(msg, msgOffset, msgLength, transactionCode, reason, isNFDPoller);
	}

	if (ok)
	{
		ok = CheckTransactionDataFields(msg, msgOffset, msgLength, transactionCode, reason);
	}

	return ok;
}

// In general, checks mostly for issues with transaction messages.
void CPollerCheckMessage::CheckMessage(const BYTE *msg,
	WORD msgLength,
	const BYTE nCompassMacAddress[],
	TiltCode &tiltCode,
	WORD messageID,
	std::string &reason,
	bool isNFDPoller)
{
	DWORD msgOffset(0);
	WORD messageId = (POLLER_MSG_NONE);

	bool ok = CheckMessageLength(msg, msgOffset, msgLength, messageID, reason);
	if (ok && msgLength >= SIZE_OF_MESSAGE_HEADER)
	{
		ok = CheckMessageHeader(msg, msgOffset, msgLength, nCompassMacAddress, messageId, reason);
	}

	if (ok)
	{
		switch (messageId)
		{
		case POLLER_MSG_TRANSACTION:
			ok = CheckTransaction(msg, msgOffset, msgLength, reason, isNFDPoller);
			break;
		default:
			break;
		}
	}

	if (!ok)
	{
		tiltCode = tltPollerProtocolCheckPacket;
	}
}
