#include "stdafx.h"

#include "Game.h"

#ifdef LOGGING
#include "Logging/LogString.h"
#endif

const int PAYTABLELENGTH_PREMEMGD = 9;

CGame::CGame(MEMORY_TYPE memType,bool bIsEGM)
: m_Meters(memType)
{
	m_bIsEGM = bIsEGM;
	m_type = memType;
	m_dwAllowedSize = bIsEGM? NVRAM_EGM_GAME_SIZE : NVRAM_SUBGAME_SIZE;
	if (bIsEGM)
	{
		m_Meters.SetAllowedSize(NVRAM_NUMBER_OF_EGM_METERS * NVRAM_METER_SIZE);
	}
	else
	{
		m_Meters.SetAllowedSize(NVRAM_NUMBER_OF_SUBGAME_METERS * NVRAM_METER_SIZE);
	}
	SetPaytableIDLength(PAY_TABLE_ID_LEN);
	memset( m_lpPaytableID, 0, PAY_TABLE_ID_LEN );
	memset( m_lpMfgID, 0, MFG_ID_LEN );
	m_bIsEnabled = true;
	m_bIsCurrentGame = false;
	m_dwGameNumber = DEFAULT_GAME_NUMBER;
	m_DenomType = US_DOLLARS;
	m_nDenomMultiplier = DEFAULT_DENOMINATION_MULTIPLIER;
	m_gamenameLength = 0;
	memset( m_lpAdditionalID, 0, ADDITIONAL_ID_LEN );
	SetGamenameLength(GAME_NAME_LENGTH);
	memset(m_gameName, 0, GAME_NAME_LENGTH);
	memset(m_gameAdditionalInfo, 0, ADDITIONAL_INFO_LENGTH);
	m_wTheoreticalPayback = 0;
	if (m_bIsEGM)
	{
		this->m_dwGameNumber = EGM_GAME_NUMBER;
	}
	m_maxBet = 0;
	m_progressiveGroup = DEFAULT_PROGRESSIVE_GROUP;
	m_infoIsFetched = false;
	m_levelIds = 0;
}

// Copy constrctor
// NOTE: This will NOT always make a non-persistant player object.
CGame::CGame(const CGame &gameRHS)
: m_Meters(MEMORY_NONE)
{
	*this = gameRHS;
}

CGame::~CGame(void)
{
}

CGame & CGame::operator=( const CGame &gameRHS )
{
	if (this != &gameRHS)
	{
		// Make the recipient object non-persistant memory.
		m_type = MEMORY_NONE;

		this->SetGameNumber(gameRHS.GetGameNumber());
		this->SetDenomInfo(gameRHS.GetDenomCode(), gameRHS.GetDenomMultiplier(), gameRHS.GetDenomType());
		this->SetMfgID(gameRHS.GetMfgID());
		gameRHS.IsEnabled() ? this->Enable() : this->Disable();
		this->ChangeCurrentGameStatus(gameRHS.IsCurrentGame());
		this->SetEGMIndicator(gameRHS.IsEGM());
		this->SetAdditionalID(gameRHS.GetAdditionalID());
		this->SetAdditionalInfo(gameRHS.GetAdditionalInfo());
		this->SetTheoreticalPayback(gameRHS.GetTheoreticalPayback());
		this->SetMaxBet(gameRHS.GetMaxBet());
		this->SetProgressiveGroup(gameRHS.GetProgressiveGroup());
		this->SetGamenameLength(gameRHS.GetGamenameLength());
		this->SetPaytableIDLength(gameRHS.GetPaytableIDLength());
		this->SetGameName(gameRHS.GetGameName());
		this->SetPaytableID(gameRHS.GetPaytableID());
		this->m_Meters = gameRHS.m_Meters;
		this->SetInfoFetchedFlag(gameRHS.IsInfoFetched());
		this->m_levelIds = gameRHS.m_levelIds;
	}

	return *this;
}

//S.Fairbairn 3/12/2007 Added so Games object could have both NVRAM and Non NVRAM games
void CGame::SetMemoryType(MEMORY_TYPE memType)
{
	m_type = memType;
	m_Meters.SetMemoryType(memType);
}

MEMORY_TYPE CGame::GetMemoryType()
{
	return m_type;
}

void CGame::SetOffset(DWORD dwOffset, bool preMGMD)
{
	m_bOffsetIsSet = true;
	m_dwOffset = dwOffset;
	SetMemberOffsets(preMGMD);
}

void CGame::SetGameData(const CGame &sourceGame)
{
	// Set the data for this game.
	SetGameNumber(sourceGame.GetGameNumber());
	sourceGame.IsEnabled() ? Enable() : Disable();
	ChangeCurrentGameStatus(sourceGame.IsCurrentGame());
	SetDenomInfo(sourceGame.GetDenomCode(), sourceGame.GetDenomMultiplier(), sourceGame.GetDenomType());
	SetMfgID(sourceGame.GetMfgID());
	SetAdditionalID(sourceGame.GetAdditionalID());
	SetTheoreticalPayback(sourceGame.GetTheoreticalPayback());
	SetMaxBet(sourceGame.GetMaxBet());
	SetProgressiveGroup(sourceGame.GetProgressiveGroup());
	SetAdditionalInfo(sourceGame.GetAdditionalInfo());
	SetGamenameLength(sourceGame.GetGamenameLength());
	SetPaytableIDLength(sourceGame.GetPaytableIDLength());
	SetGameName(sourceGame.GetGameName());
	SetPaytableID(sourceGame.GetPaytableID());
	SetLevelIds(sourceGame.GetLevelIds());
	SetInfoFetchedFlag(sourceGame.IsInfoFetched());
}

void CGame::SetNonProgressiveGameData(const CGame& sourceGame)
{
	// Set the data for this game that wasn't captured in B0B5 poll.
	SetGameNumber(sourceGame.GetGameNumber());
	sourceGame.IsEnabled() ? Enable() : Disable();
	ChangeCurrentGameStatus(sourceGame.IsCurrentGame());
	SetMfgID(sourceGame.GetMfgID());
	SetAdditionalID(sourceGame.GetAdditionalID());
	SetTheoreticalPayback(sourceGame.GetTheoreticalPayback());
	SetAdditionalInfo(sourceGame.GetAdditionalInfo());
	SetInfoFetchedFlag(sourceGame.IsInfoFetched());
}

void CGame::SetGameMeters(const CMeters &sourceMeters)
{
	m_Meters = sourceMeters;
	m_Meters.SetMemoryType(MEMORY_NVRAM);
	m_Meters.InitialWriteToPermanentStore(IsEGM());
}

void CGame::SetGameNumber(DWORD dwGameNumber)
{
	m_dwGameNumber = dwGameNumber;
	Write(m_dwGameNumberOffset, (BYTE*)&m_dwGameNumber, sizeof(m_dwGameNumber));
}

void CGame::SetDenom(int nDenomMultiplier, DenomCurrencyType DenomType)
{
	m_DenomType = DenomType;
	m_nDenomMultiplier = nDenomMultiplier;
	Write(m_dwDenomTypeOffset, (BYTE*)&m_DenomType, sizeof(m_DenomType),
		  m_dwDenomMultiplierOffset, (BYTE*)&m_nDenomMultiplier, sizeof(m_nDenomMultiplier));
}

void CGame::SetDenomInfo(int code, int nDenomMultiplier, DenomCurrencyType DenomType)
{
	// denom code does not need to be persisted
	m_denomCode = code;
	SetDenom(nDenomMultiplier, DenomType);
}

void CGame::SetPaytableID(const char *lpPaytableID)
{
	memset(m_lpPaytableID, 0, PAY_TABLE_ID_LEN);
	strncpy(m_lpPaytableID, lpPaytableID, m_paytableIDLength);
	Write(m_dwPaytableIDOffset, (BYTE*)m_lpPaytableID, sizeof(m_lpPaytableID));
}

void CGame::SetPaytableID(char * lpPaytableID)
{
	memset(m_lpPaytableID, 0, PAY_TABLE_ID_LEN);
	for (int i = 0; i < m_paytableIDLength; i++)
	{
		m_lpPaytableID[i] = lpPaytableID[i];
	}
	m_lpPaytableID[m_paytableIDLength] = 0;
	Write(m_dwPaytableIDOffset, (BYTE*)m_lpPaytableID, sizeof(m_lpPaytableID));
}

void CGame::SetPaytableIDLength(byte paytableIDLength)
{
	m_paytableIDLength = paytableIDLength > PAY_TABLE_ID_LEN - 1? PAY_TABLE_ID_LEN - 1: paytableIDLength;
}

byte CGame::GetPaytableIDLength() const
{
	return m_paytableIDLength;
}

void CGame::SetMfgID(const char *lpMfgID)
{
	SetMfgID(const_cast<char*>(lpMfgID));
}

void CGame::SetMfgID(char* lpMfgID)
{
	strncpy(m_lpMfgID, lpMfgID, MFG_ID_SAS_LEN);
	m_lpMfgID[MFG_ID_SAS_LEN] = '\0';
	Write(m_dwMfgIDOffset, (BYTE*)m_lpMfgID, sizeof(m_lpMfgID));
}

void CGame::SetAdditionalID(const char *lpcAdditionalID)
{
	strncpy(m_lpAdditionalID, lpcAdditionalID, ADDITIONAL_ID_LEN - 1);
	Write(m_dwAdditionalIDOffset, (BYTE*)m_lpAdditionalID, sizeof(m_lpAdditionalID));
}

void CGame::SetAdditionalID(char *  lpcAdditionalID)
{
	for (int i = 0; i < ADDITIONAL_ID_LEN; i++)
	{
		m_lpAdditionalID[i] = lpcAdditionalID[i];
	}
	m_lpAdditionalID[ADDITIONAL_ID_LEN - 1] = 0;
	Write(m_dwAdditionalIDOffset, (BYTE*)m_lpAdditionalID, sizeof(m_lpAdditionalID));
}

void CGame::SetTheoreticalPayback(WORD wTheoreticalPayback)
{
	m_wTheoreticalPayback = wTheoreticalPayback;
	Write(m_dwTheoreticalPaybackOffset, (BYTE*)&m_wTheoreticalPayback, sizeof(m_wTheoreticalPayback));
}

void CGame::SetTheoreticalPayback(char *lpcTheoreticalPayback)
{
	WORD multiplier = 1;
	m_wTheoreticalPayback = 0;
	for ( int i = strlen(lpcTheoreticalPayback) - 1; i >= 0; i--, multiplier *= 10 )
	{
		m_wTheoreticalPayback += (lpcTheoreticalPayback[i] - '0') * multiplier;
	}

	// If the theoretical payback percentage is less than 40%, assume there is an implied 100% to be added. (tracker 17202)
	// i.e. 76 is really 10076 (which is 100.76%)
	if ( m_wTheoreticalPayback < 4000 )
		m_wTheoreticalPayback += 10000;

	Write(m_dwTheoreticalPaybackOffset, (BYTE*)&m_wTheoreticalPayback, sizeof(m_wTheoreticalPayback));
}

void CGame::Enable()
{
	m_bIsEnabled = true;
	Write(m_dwIsEnabledOffset, (BYTE*)&m_bIsEnabled, sizeof(m_bIsEnabled));
}

void CGame::Disable()
{
	m_bIsEnabled = false;
	Write(m_dwIsEnabledOffset, (BYTE*)&m_bIsEnabled, sizeof(m_bIsEnabled));
}

void CGame::ChangeCurrentGameStatus(bool bCurrentGame)
{
	m_bIsCurrentGame = bCurrentGame;
	Write(m_dwIsCurrentGameOffset, (BYTE*)&m_bIsCurrentGame, sizeof(m_bIsCurrentGame));
}

void CGame::SetEGMIndicator( bool bIsEGM )
{
	m_bIsEGM = bIsEGM;
}

void CGame::SetMeter(byte meterNumber, __int64 meterValue)
{
	//Check range of meter.
	//if it's less than the number of meters for the entire egm
	//or if its between the alowd values for a subgame
	if((IsEGM() && (meterNumber <= NVRAM_NUMBER_OF_EGM_METERS)) || 
		(!IsEGM() && (meterNumber < SUBGAME_METER_LAST)))
	{
		m_Meters.SetMeter(meterNumber, meterValue, IsEGM());
	}
	else
	{
#ifdef LOGGING
		LogString(ERROR_LOG, "ERROR: The %s meter %d is out of range.",  IsEGM() ? "EGM" : "SubGame", meterNumber);
#endif
//		_ASSERT(FALSE);
	}
}

void CGame::AddToMeter(byte meterNumber, __int64 additionalMeterValue)
{
	//Check range of meter.
	//if it's less than the number of meters for the entire egm
	//or if its between the alowd values for a subgame
	if((IsEGM() && (meterNumber <= NVRAM_NUMBER_OF_EGM_METERS)) || 
		(!IsEGM() && (meterNumber < SUBGAME_METER_LAST)))
	{
		m_Meters.AddToMeter(meterNumber, additionalMeterValue, IsEGM());
	}
	else
	{
#ifdef LOGGING
		LogString(ERROR_LOG, "ERROR: The %s meter %d is out of range.",  IsEGM() ? "EGM" : "SubGame", meterNumber);
#endif
//		_ASSERT(FALSE);
	}
}

__int64 CGame::GetMeter(byte meterNumber) const
{
	__int64 meterValue(INVALID_METER_VALUE);

	CMeter * lpTempMeter = m_Meters.GetMeterByNumber(meterNumber);
	if (lpTempMeter != NULL)
	{
		meterValue = lpTempMeter->GetValue();
	}

	return meterValue;
}


__int64 CGame::CompareMeter( CMeter *meter )
{
	return m_Meters.CompareMeter( meter );
}

void CGame::InitialWriteToPermanentStore()
{
	//This should only happen when first added to Games.
	Write(m_dwGameNumberOffset, (BYTE*)&m_dwGameNumber, sizeof(m_dwGameNumber));
	Write(m_dwDenomTypeOffset, (BYTE*)&m_DenomType, sizeof(m_DenomType));
	Write(m_dwDenomMultiplierOffset, (BYTE*)&m_nDenomMultiplier, sizeof(m_nDenomMultiplier));
	Write(m_dwPaytableIDOffset, (BYTE*)m_lpPaytableID, sizeof(m_lpPaytableID));
	Write(m_dwMfgIDOffset, (BYTE*)m_lpMfgID, sizeof(m_lpMfgID));
	Write(m_dwIsEnabledOffset, (BYTE*)&m_bIsEnabled, sizeof(m_bIsEnabled));
	Write(m_dwIsCurrentGameOffset, (BYTE*)&m_bIsCurrentGame, sizeof(m_bIsCurrentGame));
	Write(m_dwAdditionalIDOffset, (BYTE*)m_lpAdditionalID, sizeof(m_lpAdditionalID));
	Write(m_dwTheoreticalPaybackOffset, (BYTE*)&m_wTheoreticalPayback, sizeof(m_wTheoreticalPayback));

	//initialize the meters in permanent store
	int nNumberOfMeters = IsEGM() ? NVRAM_NUMBER_OF_EGM_METERS : NVRAM_NUMBER_OF_SUBGAME_METERS;
	BYTE byInactive = INACTIVE_LOCATION;

	DWORD dwMetersOffset = m_dwMetersOffset + NVRAM_LOCATION_IS_ACTIVE + sizeof(DWORD); //jump over the meters isactive and the count
	long lMeterVal = INVALID_METER_VALUE;

	for (int i = 0; i < nNumberOfMeters; i++)
	{
		Write(dwMetersOffset, (BYTE*)&byInactive, sizeof(byInactive),
			dwMetersOffset + NVRAM_LOCATION_IS_ACTIVE + sizeof(byte)/*the meter number*/, (BYTE*)&lMeterVal, sizeof(lMeterVal));
		dwMetersOffset += NVRAM_METER_SIZE;
	}
	//and then write any that exist
	m_Meters.InitialWriteToPermanentStore(IsEGM()); 
}

DWORD CGame::GetGameNumber() const
{
	return m_dwGameNumber;
}

bool CGame::IsEGM() const
{
	return m_dwGameNumber == EGM_GAME_NUMBER;
}

bool CGame::IsEnabled() const
{
	return m_bIsEnabled;
}

bool CGame::IsCurrentGame() const
{
	return m_bIsCurrentGame;
}

DenomCurrencyType CGame::GetDenomType() const
{
	return m_DenomType;
}

int CGame::GetDenomMultiplier() const
{
	return m_nDenomMultiplier;
}

int CGame::GetDenomCode() const
{
	return m_denomCode;
}

const char *CGame::GetPaytableID() const
{
	return m_lpPaytableID;
}

const char *CGame::GetMfgID() const
{
	return m_lpMfgID;
}

const char *CGame::GetAdditionalID() const
{
	return m_lpAdditionalID;
}

WORD CGame::GetTheoreticalPayback() const
{
	return m_wTheoreticalPayback;
}

bool CGame::IsDoorOpen(DoorType door) const
{
	byte meterNum = mNone;
	switch (door)
	{
	case SlotDoor:
		meterNum = mSDr;
		break;
	case DropDoor:
		meterNum = mDDr;
		break;
	case LogicDoor:
		meterNum = mLDr;
		break;
	case CashDoor:
		meterNum = mCDr;
		break;
	case AuxFillDoor:
		meterNum = mFDr;
		break;
	case ExtraDoor:
		meterNum = mDr;
		break;
	case BellyDoor:
		meterNum = mBDr;
		break;
	default:
		meterNum = mNone;
		break;
	}

	bool doorOpen = false;
	if (m_bIsEGM && mNone != meterNum)
	{
		__int64 meterValue = GetMeter(meterNum);
		if (meterValue != INVALID_METER_VALUE && !CUtilities::IsEven(meterValue))
		{
			doorOpen = true;	// door is open if meter value is odd.
		}
	}

	return doorOpen;
}


void CGame::BuildYourself()
{
	if (m_type != MEMORY_NONE)
	{
		Read(m_dwGameNumberOffset, (BYTE*)&m_dwGameNumber, sizeof(m_dwGameNumber));
		Read(m_dwDenomTypeOffset, (BYTE*)&m_DenomType, sizeof(m_DenomType));
		Read(m_dwDenomMultiplierOffset, (BYTE*)&m_nDenomMultiplier, sizeof(m_nDenomMultiplier));
		Read(m_dwPaytableIDOffset, (BYTE*)m_lpPaytableID, sizeof(m_lpPaytableID));
		Read(m_dwMfgIDOffset, (BYTE*)m_lpMfgID, sizeof(m_lpMfgID));
		Read(m_dwIsEnabledOffset, (BYTE*)&m_bIsEnabled, sizeof(m_bIsEnabled));
		Read(m_dwIsCurrentGameOffset, (BYTE*)&m_bIsCurrentGame, sizeof(m_bIsCurrentGame));
		Read(m_dwAdditionalIDOffset, (BYTE*)m_lpAdditionalID, sizeof(m_lpAdditionalID));
		Read(m_dwTheoreticalPaybackOffset, (BYTE*)&m_wTheoreticalPayback, sizeof(m_wTheoreticalPayback));
		m_maxBet = 0;
		m_progressiveGroup = DEFAULT_PROGRESSIVE_GROUP;
		m_levelIds = 0;
		m_infoIsFetched = false;
		SetEGMIndicator(m_dwGameNumber == EGM_GAME_NUMBER);

		m_Meters.BuildYourself();
	}
}

void CGame::SetMemberOffsets(bool preMGMD)
{
	m_dwGameNumberOffset = m_dwOffset + NVRAM_LOCATION_IS_ACTIVE;
	m_dwDenomTypeOffset = m_dwGameNumberOffset + sizeof(m_dwGameNumber);
	m_dwDenomMultiplierOffset = m_dwDenomTypeOffset + sizeof(m_DenomType);
	m_dwPaytableIDOffset = m_dwDenomMultiplierOffset+ sizeof(m_nDenomMultiplier);
	if (preMGMD)
	{
		// Set up offsets for pre MGMD CGame and zero out array
		m_dwMfgIDOffset = m_dwPaytableIDOffset + PAYTABLELENGTH_PREMEMGD;
		memset(m_lpPaytableID, 0, PAY_TABLE_ID_LEN);
	}
	else
	{
		m_dwMfgIDOffset = m_dwPaytableIDOffset + sizeof(m_lpPaytableID);
	}
	m_dwIsEnabledOffset = m_dwMfgIDOffset + sizeof(m_lpMfgID);
	m_dwIsCurrentGameOffset = m_dwIsEnabledOffset + sizeof(m_bIsEnabled);
	m_dwAdditionalIDOffset = m_dwIsCurrentGameOffset + sizeof(m_bIsCurrentGame);
	m_dwTheoreticalPaybackOffset = m_dwAdditionalIDOffset + sizeof(m_lpAdditionalID);
	m_dwMetersOffset = m_dwTheoreticalPaybackOffset + sizeof(m_wTheoreticalPayback);

	m_Meters.SetOffset(m_dwMetersOffset);
}

void CGame::RemoveFromPermanentStore()
{
	//S.Fairbairn 3/12/07 Added to ensure that writes do not happen to NVRAM from Non NVRAM games
	if (m_type == MEMORY_NVRAM)
	{
		SetInactiveLocation(m_dwOffset);
	}
	//S.Fairbairn 3/12/07 End
}

__int64 CGame::GetCreditsWon() const
{
	__int64 creditsWon(0);

	__int64 meterValue = GetMeter(gCO);
	if (meterValue != INVALID_METER_VALUE)
		creditsWon += meterValue;

	meterValue = GetMeter(gJP);
	if (meterValue != INVALID_METER_VALUE)
		creditsWon += meterValue;

	meterValue = GetMeter(mBonusMach);
	if (meterValue != INVALID_METER_VALUE)
		creditsWon += meterValue;

	meterValue = GetMeter(mPrgPayAtt);
	if (meterValue != INVALID_METER_VALUE)
		creditsWon += meterValue;

	meterValue = GetMeter(mPrgPayMac);
	if (meterValue != INVALID_METER_VALUE)
		creditsWon += meterValue;

	return creditsWon;
}

__int64 CGame::GetMaxBet() const
{
	return m_maxBet;
}

void CGame::SetMaxBet(__int64 maxBet)
{
	m_maxBet = maxBet;
}

byte CGame::GetProgressiveGroup() const
{
	return m_progressiveGroup;
}

void CGame::SetProgressiveGroup(byte progressiveGroup)
{
	m_progressiveGroup = progressiveGroup;
}

byte CGame::GetGamenameLength() const
{
	return m_gamenameLength;
}

void CGame::SetGamenameLength(byte gamenameLength)
{
	m_gamenameLength = gamenameLength > GAME_NAME_LENGTH - 1? GAME_NAME_LENGTH - 1: gamenameLength;
}

const char* CGame::GetGameName() const
{
	return m_gameName;
}

void CGame::SetGameName(char* gameName)
{
	memset(m_gameName, 0, GAME_NAME_LENGTH);
	for (int i = 0; i < m_gamenameLength; i++)
	{
		m_gameName[i] = gameName[i];
	}
	m_gameName[m_gamenameLength] = 0;
}

string CGame::GetDefaultGameName() const
{
	return string(GetMfgID()) + "#" + std::to_string(GetGameNumber());
}

const char* CGame::GetAdditionalInfo() const
{
	return m_gameAdditionalInfo;
}

void CGame::SetAdditionalInfo(char* additionalInfo)
{
	for (int i = 0; i < ADDITIONAL_INFO_LENGTH; i++)
	{
		m_gameAdditionalInfo[i] = additionalInfo[i];
	}
	m_gameAdditionalInfo[ADDITIONAL_INFO_LENGTH - 1] = 0;
}

void CGame::SetAdditionalInfo(const char *lpcAdditionalInfo)
{
	strncpy(m_gameAdditionalInfo, lpcAdditionalInfo, ADDITIONAL_INFO_LENGTH - 1);
}

void CGame::SetGameName(const char *gameName)
{
	memset(m_gameName, 0, GAME_NAME_LENGTH);
	strncpy(m_gameName, gameName, m_gamenameLength);
}

void CGame::SetInfoFetchedFlag(bool value)
{
	m_infoIsFetched = value;
}

bool CGame::IsInfoFetched() const
{
	return m_infoIsFetched;
}

string CGame::GetLevelIdsString() const
{
	const int BITSINDWORD = 32;
	stringstream ss;
	DWORD value = m_levelIds;
	if (m_levelIds > 0)
	{
		bool firstNumber = true;
		for (int i = 1; i <= BITSINDWORD; i++, value >>= 1)
		{
			if ((value & 1))
			{
				if (!firstNumber)
				{
					ss << ",";
				}
				else
				{
					firstNumber = false;
				}

				ss << (i);
			}
		}
	}

	return ss.str();
}

void CGame::SetLevelIds(DWORD levelIds)
{
	m_levelIds = levelIds;
}

DWORD CGame::GetLevelIds() const
{
	return m_levelIds;
}