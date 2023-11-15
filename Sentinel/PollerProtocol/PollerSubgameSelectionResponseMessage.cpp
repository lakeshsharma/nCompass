#include "stdafx.h"
#include "PollerSubgameSelectionResponseMessage.h"
#include "Config/Config.h"
#include "Logging/LogString.h"

const byte ACCOUNTING_CASHPLAYED = 3;
const byte ACCOUNTING_COINOUT = 5;
const byte ACCOUNTING_THEOWIN = 7;
const byte ACCOUNTING_CASHTHEOWIN = 8;

CPollerSubgameSelectionResponseMessage::CPollerSubgameSelectionResponseMessage(const byte *response, WORD packetVersion, WORD dataLength)
{
	SubgameSessionParameters &subgameSessionParams = m_subgameSelectionResponse.m_subgameSessionParams;
	DWORD offset = 0;

	memcpy(&subgameSessionParams.m_gameSessionSequenceNumber, response + offset, sizeof(subgameSessionParams.m_gameSessionSequenceNumber));
	offset+= sizeof(subgameSessionParams.m_gameSessionSequenceNumber);

	memcpy(&subgameSessionParams.m_playSessionID, response + offset, sizeof(subgameSessionParams.m_playSessionID));
	offset+= sizeof(subgameSessionParams.m_playSessionID);

	memcpy(&subgameSessionParams.m_multiplier, response + offset, sizeof(WORD));
	offset+= sizeof(WORD);

	memcpy(&subgameSessionParams.m_divider, response + offset, sizeof(WORD));
	offset+= sizeof(WORD);

	byte accountingType = 0;
	memcpy(&accountingType, response + offset, sizeof(byte));
	offset += sizeof(byte);
	switch (accountingType)
	{
	case ACCOUNTING_CASHPLAYED:
		subgameSessionParams.m_accountingType = ACCOUNTING_CASHABLE_PLAYED;
		break;
	case ACCOUNTING_COINOUT:
		subgameSessionParams.m_accountingType = ACCOUNTING_COINS_OUT;
		break;
	case ACCOUNTING_THEOWIN:
		subgameSessionParams.m_accountingType = ACCOUNTING_THEORETICAL;
		break;
	case ACCOUNTING_CASHTHEOWIN:
		subgameSessionParams.m_accountingType = ACCOUNTING_CASHABLE_THEORETICAL;
		break;
	default:
		subgameSessionParams.m_accountingType = ACCOUNTING_COINS_IN;
		break;
	}

	if ((packetVersion >= PACKET_VERSION_EXTENDED_BUCKETS) && (dataLength > offset))
	{
		// extended bucket countdown data count
		byte ebCountdownsCount = 0;
		memcpy(&ebCountdownsCount, response + offset, sizeof(byte));
		offset += sizeof(byte);

		// extended bucket countdown data
		for (int i = 1; i <= ebCountdownsCount; i++)
		{
			std::unique_ptr<ExtendedBucketsCountdownData> countdownData(new ExtendedBucketsCountdownData);

			// ID
			memcpy(&countdownData->id, response + offset, sizeof(countdownData->id));
			offset += sizeof(countdownData->id);

			// display name length
			byte length = 0;
			memcpy(&length, response + offset, sizeof(byte));
			offset += sizeof(byte);

			// display name
			countdownData->displayName = std::string((char*)(response + offset), length);
			offset += length;

			// bucket currency length
			byte currencyLength = 0;
			memcpy(&currencyLength, response + offset, sizeof(byte));
			offset += sizeof(byte);

			if (currencyLength > 0)
			{
				// bucket currency
				countdownData->bucketCurrency = std::string((char*)(response + offset), currencyLength);
				offset += currencyLength;
			}

			// countdown
			memcpy(&countdownData->countdown, response + offset, sizeof(countdownData->countdown));
			offset += sizeof(countdownData->countdown);

			// multiplier
			memcpy(&countdownData->multiplier, response + offset, sizeof(countdownData->multiplier));
			offset += sizeof(countdownData->multiplier);

			// divider
			memcpy(&countdownData->divider, response + offset, sizeof(countdownData->divider));
			offset += sizeof(countdownData->divider);

			m_subgameSelectionResponse.m_ebCountdowns.push_back(std::move(countdownData));
		}
	}
}

CPollerSubgameSelectionResponseMessage::~CPollerSubgameSelectionResponseMessage()
{
}

SubgameSelectionResponse* CPollerSubgameSelectionResponseMessage::GetSubgameSelectionResponse()
{
	SubgameSelectionResponse *response = new SubgameSelectionResponse;
	response->m_subgameSessionParams = m_subgameSelectionResponse.m_subgameSessionParams;
	for (auto &countdown : m_subgameSelectionResponse.m_ebCountdowns)
	{
		std::unique_ptr<ExtendedBucketsCountdownData> countdownData(new ExtendedBucketsCountdownData(*countdown));
		response->m_ebCountdowns.push_back(std::move(countdownData));
	}
	return response;
}
