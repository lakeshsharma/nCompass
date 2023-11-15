#include "stdafx.h"
#include "PollerPlayerResponseMessage.h"
#include "Logging/LogString.h"

const byte BIRTHDAY_MASK = 0x01;
const byte BANNED_FROM_PLAY_MASK = 0x02;
const byte BANNED_FROM_REDEMPTIONS_MASK = 0x04; // unused
const byte EXTENDED_BUCKETS_MASK = 0x08;
const byte WALLET_2_0_ENROLLED_MASK = 0x10;
const byte ACCOUNTING_CASHPLAYED = 3;
const byte ACCOUNTING_COINOUT = 5;
const byte ACCOUNTING_THEOWIN = 7;
const byte ACCOUNTING_CASHTHEOWIN = 8;

CPollerPlayerResponseMessage::CPollerPlayerResponseMessage(const byte *response, WORD packetVersion, WORD dataLength)
{
	bool ok = true;

	// preinitializing the variables that may be skipped due to packet version condition check
	m_playerResponse.ebPointsId = 0;
	m_playerResponse.ebCompsId = 0;
	m_playerResponse.ebPromosId = 0;
	m_playerResponse.ebTierPointsId = 0;

	// parse response, fill in m_playerResponse;
	size_t offset = 0;
	memcpy(m_playerResponse.card_id, response + offset, sizeof(m_playerResponse.card_id));
	offset += sizeof(m_playerResponse.card_id);

	memcpy(&m_playerResponse.multiplier, response + offset, sizeof(m_playerResponse.multiplier));
	offset += sizeof(m_playerResponse.multiplier);

	memcpy(&m_playerResponse.divider, response + offset, sizeof(m_playerResponse.divider));
	offset += sizeof(m_playerResponse.divider);

	byte playerOptionBitmask(0);
	memcpy(&playerOptionBitmask, response + offset, sizeof(playerOptionBitmask));
	m_playerResponse.birthdayIndicator = playerOptionBitmask & BIRTHDAY_MASK;
	m_playerResponse.bannedFromPlayIndicator = playerOptionBitmask & BANNED_FROM_PLAY_MASK;
	m_playerResponse.ebIndicator = playerOptionBitmask & EXTENDED_BUCKETS_MASK;
	m_playerResponse.enrolledInWallet_2_0 = playerOptionBitmask & WALLET_2_0_ENROLLED_MASK;
	offset += sizeof(playerOptionBitmask);

	memcpy(&m_playerResponse.anniversaryIndicator, response + offset, sizeof(m_playerResponse.anniversaryIndicator));
	offset += sizeof(m_playerResponse.anniversaryIndicator);

	memcpy(&m_playerResponse.totalPoints, response + offset, sizeof(m_playerResponse.totalPoints));
	offset += sizeof(m_playerResponse.totalPoints);

	memcpy(&m_playerResponse.countdown, response + offset, sizeof(m_playerResponse.countdown));
	offset += sizeof(m_playerResponse.countdown);

	memcpy(&m_playerResponse.comps, response + offset, sizeof(m_playerResponse.comps));
	offset += sizeof(m_playerResponse.comps);

	memcpy(&m_playerResponse.nameLength, response + offset, sizeof(m_playerResponse.nameLength));
	offset += sizeof(m_playerResponse.nameLength);

	if (m_playerResponse.nameLength > 0)
	{
		m_playerResponse.name = new byte[m_playerResponse.nameLength + 1];
		memcpy(m_playerResponse.name, response + offset, m_playerResponse.nameLength);
		m_playerResponse.name[m_playerResponse.nameLength] = '\0';
		offset += m_playerResponse.nameLength;
	}
	else
		m_playerResponse.name = NULL;

	memcpy(&m_playerResponse.respLength, response + offset, sizeof(m_playerResponse.respLength));
	offset += sizeof(m_playerResponse.respLength);

	if (m_playerResponse.respLength > 0)
	{
		m_playerResponse.response = new byte[m_playerResponse.respLength + 1];
		memcpy(m_playerResponse.response, response + offset, m_playerResponse.respLength);
		m_playerResponse.response[m_playerResponse.respLength] = '\0';
		offset += m_playerResponse.respLength;
	}
	else
		m_playerResponse.response = NULL;


	//version 2 includes loss limit
	if ((packetVersion >= PACKET_VERSION_LOSS_LIMIT) && (dataLength > offset))
	{
		memcpy(&m_playerResponse.lossLimitBalance, response + offset, sizeof(m_playerResponse.lossLimitBalance));
		offset += sizeof(m_playerResponse.lossLimitBalance);

		memcpy(&m_playerResponse.lossLimitStatus, response + offset, sizeof(m_playerResponse.lossLimitStatus));
		offset += sizeof(m_playerResponse.lossLimitStatus);
	}
	else
	{
		ok = false;
		m_playerResponse.lossLimitBalance = 0;
		m_playerResponse.lossLimitStatus = 0;
	}

	// Trackers 26925, 26984, 26928
	// version 4 includes highlander changes
	if ((packetVersion >= PACKET_VERSION_HIGHLANDER) && (dataLength > offset) && ok)
	{
		memcpy(&m_playerResponse.dailyPoints, response + offset, sizeof(m_playerResponse.dailyPoints));
		offset += sizeof(m_playerResponse.dailyPoints);

		memcpy(&m_playerResponse.maxCardLimitIndicator, response + offset, sizeof(m_playerResponse.maxCardLimitIndicator));
		offset += sizeof(m_playerResponse.maxCardLimitIndicator);

		memcpy(&m_playerResponse.pointsTillNextTier, response + offset, sizeof(m_playerResponse.pointsTillNextTier));
		offset += sizeof(m_playerResponse.pointsTillNextTier);

		int tierMultiplierInt(0);
		memcpy(&tierMultiplierInt, response + offset, sizeof(m_playerResponse.tierMultiplier));
		if (tierMultiplierInt >= 0)
		{
			memcpy(&m_playerResponse.tierMultiplier, response + offset, sizeof(m_playerResponse.tierMultiplier));
		}
		else
		{
			m_playerResponse.tierMultiplier = 0;
		}
		offset += sizeof(m_playerResponse.tierMultiplier);
	}
	else
	{
		ok = false;
		m_playerResponse.dailyPoints = -1;
		m_playerResponse.maxCardLimitIndicator = 0;
		m_playerResponse.pointsTillNextTier = -1;
		m_playerResponse.tierMultiplier = 0;
	}

	m_playerResponse.m_rankName = NULL;
	if ((packetVersion >= PACKET_VERSION_RANK) && (dataLength > offset) && ok)
	{
		CUtilities::MemCopyZero_FromOffset(&ok, &m_playerResponse.m_rankId, sizeof m_playerResponse.m_rankId,
			response, dataLength, &offset);
		CUtilities::MemCopyZero_FromOffset(&ok, &m_playerResponse.m_rankNameLength, sizeof m_playerResponse.m_rankNameLength,
			response, dataLength, &offset);
		if (m_playerResponse.m_rankNameLength > 0)
		{
			m_playerResponse.m_rankName = new byte[m_playerResponse.m_rankNameLength + 1];
			CUtilities::MemCopyZero_FromOffset(&ok, m_playerResponse.m_rankName, m_playerResponse.m_rankNameLength,
				response, dataLength, &offset);
			m_playerResponse.m_rankName[m_playerResponse.m_rankNameLength] = '\0';
		}
	}
	else
	{
		ok = false;
	}

	if (!ok) {
		m_playerResponse.m_rankId = DEFAULT_RANK_ID;
		m_playerResponse.m_rankNameLength = 0;
		delete m_playerResponse.m_rankName;
		m_playerResponse.m_rankName = NULL;
	}

	if ((packetVersion >= PACKET_TIER_POINTS) && (dataLength > offset) && ok)
	{
		memcpy(&m_playerResponse.m_playerId, response + offset, sizeof(m_playerResponse.m_playerId));
		offset += sizeof(m_playerResponse.m_playerId);

		memcpy(&m_playerResponse.m_tierPoints, response + offset, sizeof(m_playerResponse.m_tierPoints));
		offset += sizeof(m_playerResponse.m_tierPoints);
	}
	else
	{
		ok = false;
		m_playerResponse.m_playerId = DEFAULT_PLAYER_ID;
		m_playerResponse.m_tierPoints = DEFAULT_TIER_POINTS;
	}

	if ((packetVersion >= PACKET_VERSION_MULTI_GAME_MULTI_DENOM) && (dataLength > offset) && ok)
	{
		memcpy(&m_playerResponse.countdownHighPrecision, response + offset, sizeof(m_playerResponse.countdownHighPrecision));
		offset += sizeof(m_playerResponse.countdownHighPrecision);

		byte accountingType = 0;
		memcpy(&accountingType, response + offset, sizeof(byte));
		offset += sizeof(byte);
		switch (accountingType)
		{
		case ACCOUNTING_CASHPLAYED:
			m_playerResponse.accountingType = ACCOUNTING_CASHABLE_PLAYED;
			break;
		case ACCOUNTING_COINOUT:
			m_playerResponse.accountingType = ACCOUNTING_COINS_OUT;
			break;
		case ACCOUNTING_THEOWIN:
			m_playerResponse.accountingType = ACCOUNTING_THEORETICAL;
			break;
		case ACCOUNTING_CASHTHEOWIN:
			m_playerResponse.accountingType = ACCOUNTING_CASHABLE_THEORETICAL;
			break;
		default:
			m_playerResponse.accountingType = ACCOUNTING_COINS_IN;
			break;
		}

		if ((packetVersion >= PACKET_VERSION_EXTENDED_BUCKETS) && (dataLength > offset))
		{
			// extended bucket points ID
			memcpy(&m_playerResponse.ebPointsId, response + offset, sizeof(m_playerResponse.ebPointsId));
			offset += sizeof(m_playerResponse.ebPointsId);

			// extended bucket comps ID
			memcpy(&m_playerResponse.ebCompsId, response + offset, sizeof(m_playerResponse.ebCompsId));
			offset += sizeof(m_playerResponse.ebCompsId);

			// extended bucket promos ID
			memcpy(&m_playerResponse.ebPromosId, response + offset, sizeof(m_playerResponse.ebPromosId));
			offset += sizeof(m_playerResponse.ebPromosId);

			// extended bucket tier points ID
			memcpy(&m_playerResponse.ebTierPointsId, response + offset, sizeof(m_playerResponse.ebTierPointsId));
			offset += sizeof(m_playerResponse.ebTierPointsId);

			// extended bucket countdown data count
			byte ebCountdownsCount = 0;
			memcpy(&ebCountdownsCount, response + offset, sizeof(byte));
			offset += sizeof(byte);

			// extended bucket countdown data
			for (int i = 1; i <= ebCountdownsCount; i++)
			{
				ExtendedBucketsCountdownData *countdownData = new ExtendedBucketsCountdownData;

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

				m_playerResponse.ebCountdowns.push_back(countdownData);
			}

			// extended bucket data length
			DWORD ebDataLength = 0;
			memcpy(&ebDataLength, response + offset, sizeof(ebDataLength));
			offset += sizeof(ebDataLength);

			if (m_playerResponse.ebIndicator && ebDataLength > 0)
			{
				// extended buckets data
				m_playerResponse.ebJSONData = std::string((char*)(response + offset), ebDataLength);
				offset += ebDataLength;
			}
		}
	}
	else
	{
		ok = false;
		m_playerResponse.countdownHighPrecision = DEFAULT_COUNTDOWN_HIGH_PRECISION;
		m_playerResponse.accountingType = ACCOUNTING_COINS_IN;
	}

}

CPollerPlayerResponseMessage::~CPollerPlayerResponseMessage(void)
{
	if (m_playerResponse.name)
		delete [] m_playerResponse.name;
	if (m_playerResponse.response)
		delete [] m_playerResponse.response;
	delete[] m_playerResponse.m_rankName;

	for (auto itr : m_playerResponse.ebCountdowns)
	{
		delete itr;
	}
}

PlayerResponse CPollerPlayerResponseMessage::GetPlayerResponse()
{
	return m_playerResponse;
}
