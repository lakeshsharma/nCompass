#include "stdafx.h"
#include "CardConfig.h"


CCardConfig::CCardConfig() :
m_commID(0), m_rcclCardsEnabled(false), m_nclCardsEnabled(false),
m_globalCardPrefix(INVALID_CARD_PREFIX),
m_mechanicCardPrefix(INVALID_CARD_PREFIX),
m_floorCardPrefix(INVALID_CARD_PREFIX),
m_tempCardPrefix(INVALID_CARD_PREFIX),
m_universeEnabled(false),
m_mixedModeEnabled(false),
m_cardReaderTrackNumber(2),
m_useSDSCardFormat(false)
{
	ClearPlayerCardPrefixes();
	ClearUniverseCardPrefixes();
	ClearUniversePropCodes();
}


CCardConfig::~CCardConfig()
{
}

void CCardConfig::Setup(const CConfig &config)
{
	SetCommID(config.GetCommID());
	SetRcclCardsEnabled(config.RcclCardsEnabled());
	SetNclCardsEnabled(config.NclCardsEnabled());
    SetPlayerCardPrefixes(config.GetPlayerCardPrefixes());
	SetUniversePrefixes(config.GetUniverseCardPrefixes());
	SetGlobalCardPrefix(config.GetGlobalCardPrefix());
	SetMechanicCardPrefix(config.GetMechanicCardPrefix());
	SetFloorCardPrefix(config.GetFloorCardPrefix());
	SetTempCardPrefix(config.GetTempCardPrefix());
	SetUniverseEnabled(config.GetUniverseEnabled());
	SetMixedModeEnabled(config.GetMixedModeEnabled());
	SetUniversePropCodes(config.GetUniversePropCodes());
	SetCardReaderTrackNumber(config.CardReaderTrackNumber());
	SetUseSDSCardFormat(config.GetUseSDSCardFormat());
}

byte CCardConfig::SetCommID(byte commID)
{
	byte bOldVal = m_commID;

	if (m_commID != commID)
	{
		m_commID = commID;
	}

	return bOldVal;
}

byte CCardConfig::GetCommID() const
{
	return (m_commID);
}

bool CCardConfig::SetRcclCardsEnabled(bool rcclCardsEnabled)
{
	bool bOldVal = m_rcclCardsEnabled;

	m_rcclCardsEnabled = rcclCardsEnabled;

	return bOldVal;
}

bool CCardConfig::RcclCardsEnabled() const
{
	return m_rcclCardsEnabled;;
}

bool CCardConfig::SetNclCardsEnabled(bool nclCardsEnabled)
{
	bool bOldVal = m_nclCardsEnabled;

	m_nclCardsEnabled = nclCardsEnabled;

	return bOldVal;
}

bool CCardConfig::NclCardsEnabled() const
{
	return m_nclCardsEnabled;
}

void CCardConfig::SetPlayerCardPrefixes(const std::vector<BYTE> &playerCardsPrefix)
{
    m_playerCardPrefixes = playerCardsPrefix;
}

void CCardConfig::ClearPlayerCardPrefixes()
{
    m_playerCardPrefixes.clear();
}

const std::vector<BYTE>CCardConfig::GetPlayerCardPrefixes() const
{
	return(m_playerCardPrefixes);
}

BYTE CCardConfig::SetGlobalCardPrefix(BYTE globalCardPrefix)
{
	BYTE bOldVal = m_globalCardPrefix;

	m_globalCardPrefix = globalCardPrefix;

	return bOldVal;
}

BYTE CCardConfig::GetGlobalCardPrefix() const
{
	return(m_globalCardPrefix);
}

BYTE CCardConfig::SetMechanicCardPrefix(BYTE mechanicCardPrefix)
{
	BYTE bOldVal = m_mechanicCardPrefix;

	m_mechanicCardPrefix = mechanicCardPrefix;

	return bOldVal;
}

BYTE CCardConfig::GetMechanicCardPrefix() const
{
	return(m_mechanicCardPrefix);
}

BYTE CCardConfig::SetFloorCardPrefix(BYTE floorCardPrefix)
{
	BYTE bOldVal = m_floorCardPrefix;

	m_floorCardPrefix = floorCardPrefix;

	return bOldVal;
}

BYTE CCardConfig::GetFloorCardPrefix() const
{
	return(m_floorCardPrefix);
}

BYTE CCardConfig::SetTempCardPrefix(BYTE tempCardPrefix)
{
	BYTE bOldVal = m_tempCardPrefix;

	m_tempCardPrefix = tempCardPrefix;

	return bOldVal;
}

BYTE CCardConfig::GetTempCardPrefix() const
{
	return(m_tempCardPrefix);
}

bool CCardConfig::SetUniverseEnabled(bool universeEnabled)
{
	bool bOldVal = m_universeEnabled;

	m_universeEnabled = universeEnabled;

	return bOldVal;
}

bool CCardConfig::GetUniverseEnabled(void) const
{
	return m_universeEnabled;
}

bool CCardConfig::SetMixedModeEnabled(bool mixedModeEnabled)
{
	bool bOldVal = m_mixedModeEnabled;

	m_mixedModeEnabled = mixedModeEnabled;

	return bOldVal;
}

bool CCardConfig::GetMixedModeEnabled(void) const
{
	return m_mixedModeEnabled;
}

bool CCardConfig::IsUniversePlayerCardPrefix(byte prefix) const
{
	bool bFound = false;

	if (m_universeEnabled)
	{
        for (auto &universePrefix : m_universePlayerCardPrefixes)
        {
			if (universePrefix == prefix)
            {
				bFound = true;
                break;
            }
        }
	}

	return bFound;
}

void CCardConfig::SetUniversePrefixes(const std::vector<BYTE> &universePrefixes)
{
    m_universePlayerCardPrefixes = universePrefixes;
}

void CCardConfig::ClearUniverseCardPrefixes()
{
    m_universePlayerCardPrefixes.clear();
}

const std::vector<BYTE> CCardConfig::GetUniverseCardPrefix() const
{
	return (m_universePlayerCardPrefixes);
}

void CCardConfig::SetUniversePropCodes(const std::vector<WORD> &universePropCodes)
{
    m_universePropCodes = universePropCodes;
}

void CCardConfig::ClearUniversePropCodes()
{
    m_universePropCodes.clear();
}

const std::vector<WORD> CCardConfig::GetUniversePropCodes() const
{
	return (m_universePropCodes);
}

BYTE CCardConfig::SetCardReaderTrackNumber(BYTE cardReaderTrackNumber)
{
	BYTE bOldVal = m_cardReaderTrackNumber;

	m_cardReaderTrackNumber = cardReaderTrackNumber;

	return bOldVal;
}

BYTE CCardConfig::CardReaderTrackNumber() const
{
	return m_cardReaderTrackNumber;
}

byte CCardConfig::GetRcclCardPlayerPrefix() const
{
	byte playerCardPrefix = INVALID_CARD_PREFIX;

    if (!m_playerCardPrefixes.empty())
	{
		playerCardPrefix = m_playerCardPrefixes[0];
	}

	return playerCardPrefix;
}

bool CCardConfig::SetUseSDSCardFormat(bool value)
{
	bool oldVal(m_useSDSCardFormat);

	m_useSDSCardFormat = value;

	return oldVal;
}

bool CCardConfig::GetUseSDSCardFormat() const
{
	return m_useSDSCardFormat;
}