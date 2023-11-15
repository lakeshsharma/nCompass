#include "stdafx.h"
#include "PollerPBTransactionMessage.h"
#include "../Utilities.h"

CPollerPBTransactionMessage::CPollerPBTransactionMessage(CMeters *meters,
														 CPersonalBankerTransfer * PBTrans,
														 byte transType, 
														 time_t transTime, 
														 DWORD uniqueId, 
														 const byte * cardID,
														 PersonalBankerType ammendedPBType)
{
    m_status = 0;
	m_meters = meters;
	m_meterCount = m_meters->GetMeterCount();
	if( m_meterCount > NUM_METERS )
		m_meterCount = NUM_METERS;

	m_uniqueId = uniqueId;
	m_transTime = transTime;
	m_cardID = new byte[CARD_ID_LEN];
	if (cardID)
	{
		memcpy(m_cardID, cardID, CARD_ID_LEN);
	}
	m_PBTransfer = PBTrans;
	m_transCode = transType;
	m_RWBufferIsSet = false;
	m_DataCount = 8;
	m_ammendedPBType = ammendedPBType;

	m_bufferSize = SIZE_OF_MESSAGE_HEADER + 
		NONVARIABLE_TRANSACTION_SIZE +	//
		sizeof(byte) +					//meter count
		(m_meterCount * (sizeof(PollerMeter().Number) + sizeof(PollerMeter().Value))) +
		sizeof(byte) + //data count
		sizeof(byte) + sizeof(byte) + m_PBTransfer->GetPBTIDLength() +	//PBT ID		(up to 20)
		sizeof(byte) + sizeof(byte) + sizeof(int) +		//Time Since Card In	(4)
		sizeof(byte) + sizeof(byte) + sizeof(__int64) + //PBT Original Credit Meter (8)
		sizeof(byte) + sizeof(byte) + sizeof(byte) +	//PBT Cancelled Type	(1)	
		sizeof(byte) + sizeof(byte) + sizeof(byte) +	//PBT Type				(1)
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) +	//PBT PIN (Encrytped)	(4)
		sizeof(byte) + sizeof(byte) + sizeof(__int64) +	//PBT Amount in Pennies(8)
		sizeof(byte) + sizeof(byte) + sizeof(WORD); 	//PBT SequenceNumber	(2)

	if (TaxWithheld == ammendedPBType ||
		CanceledTaxWithheld == ammendedPBType)
	{
		m_bufferSize += sizeof(byte) + sizeof(byte) + sizeof(int) +		 //Jackpot ID(4)
						sizeof(byte) + sizeof(byte) + sizeof(uint64_t) + //FederalTaxWithheldInPennies(8)
						sizeof(byte) + sizeof(byte) + sizeof(bool) +	 //FederalTaxOverridden(1)
						sizeof(byte) + sizeof(byte) + sizeof(bool);		 //StateTaxOverridden(1)
		m_DataCount = 12;
	}
	LogString(JACKPOT, "CPollerPBTransactionMessage ammendedPBType=%u m_bufferSize=%u", ammendedPBType, m_bufferSize);
	m_lpBuffer = new byte[m_bufferSize];
}

CPollerPBTransactionMessage::~CPollerPBTransactionMessage(void)
{
	if(m_cardID != NULL)
	{
		delete m_cardID;
		m_cardID = NULL;
	}
}

void CPollerPBTransactionMessage::SetStatus(DWORD status)
{
	m_status = status;
}

DWORD CPollerPBTransactionMessage::GetStatus()
{
	return m_status;
}

byte * CPollerPBTransactionMessage::GetReadWriteBuffer(DWORD &bufferSize)
{
	bufferSize = m_bufferSize;
	return m_lpBuffer;
}

void CPollerPBTransactionMessage::SetReadWriteBuffer()
{
	m_RWBufferIsSet = true;

	MessageHeader.MessageID = POLLER_MSG_TRANSACTION;
	MessageHeader.DataLength = m_bufferSize - SIZE_OF_MESSAGE_HEADER;

	int offset = 0;
	//Header (piece by piece to avoid missaligned data error)
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.PacketVersion), sizeof(MessageHeader.PacketVersion));
	offset+= sizeof(MessageHeader.PacketVersion);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.MessageID), sizeof(MessageHeader.MessageID));
	offset+= sizeof(MessageHeader.MessageID);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.DataLength), sizeof(MessageHeader.DataLength));
	offset+= sizeof(MessageHeader.DataLength);
	
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.MacAddress), sizeof(MessageHeader.MacAddress));
	offset+= sizeof(MessageHeader.MacAddress);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SequenceNumber), sizeof(MessageHeader.SequenceNumber));
	offset+= sizeof(MessageHeader.SequenceNumber);
	
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SlotMastID), sizeof(MessageHeader.SlotMastID));
	offset+= sizeof(MessageHeader.SlotMastID);
	
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SlotmastRevision), sizeof(MessageHeader.SlotmastRevision));
	offset+= sizeof(MessageHeader.SlotmastRevision);

	// Transaction body
	// Transaction code
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_transCode, sizeof(m_transCode));
	offset+= sizeof (m_transCode);

	// Transaction time
	SYSTEMTIME systime;
	systime = CUtilities::GetUTCSystemTime(m_transTime);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &systime, sizeof(SYSTEMTIME));
	offset += sizeof(SYSTEMTIME);

	// unique ID.
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_uniqueId, sizeof(m_uniqueId));
	offset += sizeof(m_uniqueId);

	// status.
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_status, sizeof(m_status));
	offset += sizeof(m_status);

	// card #
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, m_cardID, CARD_ID_LEN);
	offset += CARD_ID_LEN;

	//Meter Count
	memcpy(m_lpBuffer + offset, &m_meterCount, sizeof(byte));
	offset+= sizeof(byte);

	PollerMeter pollerMeter;
	CMeter *lpMeter;

	//Meters
	for(int n = 0; n < m_meterCount && n < NUM_METERS; n++)
	{
		lpMeter = m_meters->GetMeterByIndex(n);
		pollerMeter.Number = lpMeter->GetMeterNumber();
		pollerMeter.Value = lpMeter->GetValue();

		memcpy(m_lpBuffer + offset, &(pollerMeter.Number), sizeof(pollerMeter.Number));
		offset+=sizeof(pollerMeter.Number);

		memcpy(m_lpBuffer + offset, &(pollerMeter.Value), sizeof(pollerMeter.Value));
		offset+=sizeof(pollerMeter.Value);
	}

	// Transaction Data Count
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_DataCount, sizeof(byte));
	offset += sizeof(byte);


	// PBT ID
	byte transDataType = DATA_TYPE_PBT_ID;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	//length
	byte dataSize = m_PBTransfer->GetPBTIDLength();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);
	// Data
	const char * text = m_PBTransfer->GetID();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, text, dataSize);
	offset += dataSize;



	//Card In duration
	transDataType = DATA_TYPE_PBT_CARD_IN_DURATION;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	//length
	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);
	//Data
	DWORD nData = m_PBTransfer->GetSecondsSinceCardIn();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &nData, sizeof(nData));
	offset += dataSize;


	//Original Credit Meter
	transDataType = DATA_TYPE_PBT_ORIGINAL_CREDIT_METER;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	//length
	dataSize = sizeof(__int64);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);
	//Data
	__int64 n64Data = m_PBTransfer->GetOriginalCreditMeter();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &n64Data, sizeof(n64Data));
	offset += dataSize;


	//Cancelled Type
	transDataType = DATA_TYPE_PBT_CANCELLED_TYPE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	//length
	dataSize = sizeof(byte);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);
	//Data
	byte bData = m_PBTransfer->GetCancelledType();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &bData, sizeof(bData));
	offset += dataSize;


	// Type
	transDataType = DATA_TYPE_PBT_TYPE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	//length
	dataSize = sizeof(byte);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);
	//Data
	//If th eammended type is specified, change it.
	bData = m_ammendedPBType == Unspecified ? m_PBTransfer->GetPersonalBankerType() : m_ammendedPBType;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &bData, sizeof(bData));
	offset += dataSize;


	// PIN
	transDataType = DATA_TYPE_PBT_PIN;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	//length
	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);
	//Data
	nData = CUtilities::EncryptPBTPin(m_PBTransfer->GetPIN());
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &nData, sizeof(nData));
	offset += dataSize;


	// Amount
	transDataType = DATA_TYPE_PBT_AMOUNT_IN_PENNIES;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	//length
	dataSize = sizeof(__int64);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);
	//Data
	switch (m_ammendedPBType)
	{
	case PromoCreditsUpload:
	case PromoCreditsUploadAuto:
	case CanceledPromoCreditsUpload:
	case CanceledPromoCreditsUploadAuto:
	case PromoCreditsUploadCashout:
	case CanceledPromoCreditsUploadCashout:
		n64Data = m_PBTransfer->GetRestrictedAmount();
		break;
	case PointCashUpload:
	case PointCashUploadAuto:
	case PartialPointCashUpload:
	case CanceledPointCashUpload:
	case CanceledPartialPointCashUpload:
	case CanceledPointCashUploadAuto:
	case PointCashUploadCashout:
	case CanceledPointCashUploadCashout:
		n64Data = m_PBTransfer->GetNonrestrictedAmount();
		break;
	case Unspecified:
	case CashUpload:
	case CashUploadAuto:
	case PartialCashUpload:
	case TaxWithheld:
	case CanceledCashUpload:
	case CanceledPartialCashUpload:
	case CanceledCashUploadAuto:
	case CashUploadCashout:
	case CanceledCashUploadCashout:
	case CanceledTaxWithheld:
	default:
		n64Data = m_PBTransfer->GetAmount();
		break;
	}
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &n64Data, sizeof(n64Data));
	offset += dataSize;


	// Sequence Number
	transDataType = DATA_TYPE_PBT_SEQUENCE_NUMBER;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	//length
	dataSize = sizeof(WORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);
	//Data
	WORD wData;
	
	switch (m_ammendedPBType)
	{
	case PromoCreditsUpload:
	case PromoCreditsUploadAuto:
	case PromoCreditsUploadCashout:
		wData = m_PBTransfer->GetRestrictedSequenceNumber();
		break;
	case PointCashUpload:
	case PointCashUploadAuto:
	case PartialPointCashUpload:
	case PointCashUploadCashout:
		wData = m_PBTransfer->GetNonrestrictedSequenceNumber();
		break;
	case Unspecified:
	case CashUpload:
	case CashUploadAuto:
	case PartialCashUpload:
	case CashUploadCashout:
	case TaxWithheld:
	default:
		wData = m_PBTransfer->GetSequenceNumber();
		break;
	}
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &wData, sizeof(wData));
	offset += dataSize;

	if (TaxWithheld == m_ammendedPBType || CanceledTaxWithheld == m_ammendedPBType)
	{
		//****************DATA_TYPE_JACKPOT_ID****************//
		transDataType = DATA_TYPE_JACKPOT_ID;
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
		offset += sizeof(transDataType);		
		//length
		dataSize = sizeof(int);
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
		offset += sizeof(dataSize);
		//Data
		int intData = m_PBTransfer->GetJackpotId();
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &intData, sizeof(intData));
		offset += dataSize;

		//****************DATA_TYPE_FEDERAL_TAX_WITHHELD****************//
		transDataType = DATA_TYPE_FEDERAL_TAX_WITHHELD;
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
		offset += sizeof(transDataType);
		//length
		dataSize = sizeof(uint64_t);
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
		offset += sizeof(dataSize);
		//Data
		uint64_t uint64_tData = m_PBTransfer->GetFederalTaxDeductionInPennies();
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &uint64_tData, sizeof(uint64_tData));
		offset += dataSize;

		//****************DATA_TYPE_FEDERAL_TAX_OVERRIDDEN****************//
		transDataType = DATA_TYPE_FEDERAL_TAX_OVERRIDDEN;
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
		offset += sizeof(transDataType);
		//length
		dataSize = sizeof(bool);
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
		offset += sizeof(dataSize);
		//Data
		bool boolData = m_PBTransfer->IsFederalTaxOverridden();
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &boolData, sizeof(boolData));
		offset += dataSize;

		//****************DATA_TYPE_STATE_TAX_OVERRIDDEN****************//
		transDataType = DATA_TYPE_STATE_TAX_OVERRIDDEN;
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
		offset += sizeof(transDataType);
		//length
		dataSize = sizeof(bool);
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
		offset += sizeof(dataSize);
		//Data
		boolData = m_PBTransfer->IsStateTaxOverridden();
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &boolData, sizeof(boolData));
		offset += dataSize;

		LogString(JACKPOT, "CPollerPBTransactionMessage::SetReadWriteBuffer JackpotId=%d FederalTaxDeductionInPennies=%llu FederalTaxOverridden=%u StateTaxOverridden=%u", 
			m_PBTransfer->GetJackpotId(), m_PBTransfer->GetFederalTaxDeductionInPennies(), m_PBTransfer->IsFederalTaxOverridden(), m_PBTransfer->IsStateTaxOverridden());
	}
}


bool CPollerPBTransactionMessage::IsEligibleToResend()
{
	return SessionRequest != m_PBTransfer->GetPersonalBankerType();
}