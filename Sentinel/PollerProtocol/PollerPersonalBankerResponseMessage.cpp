#include "stdafx.h"
#include "PollerPersonalBankerResponseMessage.h"
#include "Logging/LogString.h"

CPollerPersonalBankerResponseMessage::CPollerPersonalBankerResponseMessage(const byte *response, WORD packetVersion, WORD DataLen)
{
    MessageHeader.DataLength = DataLen;
    byte offset = 0;

	m_pbInfo.SetCardNumber(response + offset);
	offset+= CARD_ID_LEN;

	byte tempByte;
	memcpy(&tempByte, response + offset, sizeof(tempByte));
	offset+= sizeof(tempByte);
	m_pbInfo.SetErrorCode((PBError)tempByte);

	DWORD tempDword;
	memcpy(&tempDword, response + offset , sizeof(tempDword));
	offset+= sizeof(tempDword);
	m_pbInfo.SetCashBalance(tempDword);

	memcpy(&tempDword, response + offset , sizeof(tempDword));
	offset+= sizeof(tempDword);
	m_pbInfo.SetPBFlags(tempDword);

	memcpy(&tempDword, response + offset , sizeof(tempDword));
	offset+= sizeof(tempDword);
	m_pbInfo.SetPromo2Balance(tempDword);

	memcpy(&tempDword, response + offset , sizeof(tempDword));
	offset+= sizeof(tempDword);
	m_pbInfo.SetPointsAvailable(tempDword);

	memcpy(&tempDword, response + offset , sizeof(tempDword));
	offset+= sizeof(tempDword);
	m_pbInfo.SetPointsPerDollar(tempDword);

	memcpy(&tempDword, response + offset , sizeof(tempDword));
	offset+= sizeof(tempDword);
	m_pbInfo.SetSequenceNumber(tempDword);

	memcpy(&tempDword, response + offset , sizeof(tempDword));
	offset+= sizeof(tempDword);
	m_pbInfo.SetMinimumPoints(tempDword);

	memcpy(&tempDword, response + offset , sizeof(tempDword));
	offset+= sizeof(tempDword);
	m_pbInfo.SetMaximumPoints(tempDword);

	memcpy(&tempDword, response + offset , sizeof(tempDword));
	offset+= sizeof(tempDword);
	m_pbInfo.SetMinimumCashDown(tempDword);

	memcpy(&tempDword, response + offset , sizeof(tempDword));
	offset+= sizeof(tempDword);
	m_pbInfo.SetMaximumCashDown(tempDword);

	memcpy(&tempDword, response + offset , sizeof(tempDword));
	offset+= sizeof(tempDword);
	m_pbInfo.SetMinimumCashUp(tempDword);

	memcpy(&tempDword, response + offset , sizeof(tempDword));
	offset+= sizeof(tempDword);
	m_pbInfo.SetMaximumCashUp(tempDword);

	memcpy(&tempDword, response + offset , sizeof(tempDword));
	offset+= sizeof(tempDword);
	m_pbInfo.SetMinimumPromoDown(tempDword);

	memcpy(&tempDword, response + offset , sizeof(tempDword));
	offset+= sizeof(tempDword);
	m_pbInfo.SetMaximumPromoDown(tempDword);

	memcpy(&tempByte, response + offset, sizeof(tempByte));
	offset+= sizeof(tempByte);

	int pinLength = tempByte > MAX_PBID_LENGTH ? MAX_PBID_LENGTH : tempByte;
	m_pbInfo.SetPersonalBankerID((char*)(response + offset), pinLength);
	offset+= pinLength;

	// if protocol version 3 or greater, set promo up values
	if (packetVersion >= PACKET_VERSION_NO_POINTS_FOR_PROMO)
	{
		memcpy(&tempDword, response + offset, sizeof (tempDword));
		offset += sizeof(tempDword);
		m_pbInfo.SetMinimumPromoUp(tempDword);
		
		memcpy(&tempDword, response + offset, sizeof (tempDword));
		offset += sizeof(tempDword);
		m_pbInfo.SetMaximumPromoUp(tempDword);
	}

	if (packetVersion >= PACKET_VERSION_CREDIT_TRANSFER)
	{
		memcpy(&tempByte, response + offset, sizeof(tempByte));
		offset+= sizeof(tempByte);
		m_pbInfo.SetExternalCreditProviderEnabled(tempByte != 0);

		memcpy(&tempDword, response + offset, sizeof (tempDword));
		offset += sizeof(tempDword);
		m_pbInfo.SetCreditFee(tempDword);

		memcpy(&tempByte, response + offset, sizeof(tempByte));
		offset += sizeof(tempByte);
		m_pbInfo.SetCreditFeeFixed(tempByte!=0);
	}

	//If the fixed data lenght is greater than PB_FIXED_DATE_LEN then FL supports external credit Type transfer to game
	if (MessageHeader.DataLength - pinLength > PB_FIXED_DATE_LEN)
	{

		memcpy(&tempByte, response + offset, sizeof(tempByte));
		m_pbInfo.SetExternalCreditTransferType((ExtCreditTransferType)tempByte);
        LogString(PBT_LOG, "%s : External credit to game enabled : %d  , External Credit Transfer Type : %d", __FUNCTION__, m_pbInfo.ExternalCreditProviderEnabled(), m_pbInfo.ExternalCreditTransferType());
		offset += sizeof(tempByte);
	}
}

CPollerPersonalBankerResponseMessage::~CPollerPersonalBankerResponseMessage(void)
{
}

CPersonalBankerInfo * CPollerPersonalBankerResponseMessage::GetPBInfo()
{
	CPersonalBankerInfo* pbRet = new CPersonalBankerInfo(m_pbInfo);
	return pbRet;
}
