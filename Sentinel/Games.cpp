#include "stdafx.h"
#include "Games.h"
#include "Logging/LogString.h"
#include "RapidJsonHelper.h"
#include "UI/ScreenDefs.h"
#include <algorithm>

using namespace std;

CGames::CGames(MEMORY_TYPE memType, IOperatingSystem &operatingSystem) 
: CDataSubscriptionProvider("Games"), 
  m_operatingSystem(operatingSystem)
{
	m_type = memType;
	m_memoryReset = false;
	m_NVRAMGameCount = 0;

	m_SASVersion = "";
	m_gameId = "";
	m_progressiveGroup = CGame::DEFAULT_PROGRESSIVE_GROUP;
	m_address = (byte)1;
	m_serialNumber = "";
	m_assetNumber = "";
	m_accountingDenom = 0;
	m_multiDenomEnabled = false;
	m_extendedMetersEnabled = false;
	m_aftEnabled = false;
	m_aftBonusEnabled = false;
	m_aftInHouseEnabled = false;
	m_legacyBonusEnabled = false;
	m_egmTransferMode = EGMTransferMode_PENDING;
	m_eftTransferToEnabled = false;
	m_eftTransferFromEnabled = false;
	m_validationExtensionsEnabled = false;
	m_lastTotalCoinOutMeterValue = INVALID_METER_VALUE;
	m_currentDenomMultiplier = 0;
	m_lastGamelistTimestamp = 0;
	m_completeGamelistFetched = false;
	m_implementedGames = 0;
	m_numberOfEnabledGames = 0;
	
	m_lastEgmMetersSendTick = 0;
	m_egmMetersSendFrequency = 0;
	m_lastSubgameMetersSendTick = 0;
	m_subgameMetersSendFrequency = 0;
	m_forceEgmMetersUpdate = false;

	//if this is active, build it 
	//otherwise, write zero count and set active
	if (m_type == MEMORY_NVRAM)
	{
		m_dwOffset = NVRAM_GAMES_OFFSET;
		m_bOffsetIsSet = true;
		m_dwAllowedSize = NVRAM_GAMES_SIZE;

		if( !IntegrityCheck() )
		{
			ResetGames();
			m_memoryReset = true;
		}
		else
		{
			if (IsActiveLocation(m_dwOffset))
			{
				BuildYourself();
			}
			else
			{
				//First we check that it's not just a bad ActiveLocationByte
				if (!GameCountsMatch())
				{
					ResetGames();
					m_memoryReset = true;
				}
				else
				{
					//if it is we'll set it as active and move on.
					this->SetActiveLocation(m_dwOffset);
					BuildYourself();
				}
			}
		}
	}
}

CGames::CGames(const CGames &gRHS) 
: CDataSubscriptionProvider("Games"), 
  m_operatingSystem(COperatingSystem::m_operatingSystem)
{
	*this = gRHS;
}

CGames::CGames(bool, IOperatingSystem &operatingSystem) : m_operatingSystem(operatingSystem)
{
	m_type = MEMORY_NVRAM;
	m_memoryReset = false;
	m_NVRAMGameCount = 0;

	m_SASVersion = "";
	m_gameId = "";
	m_progressiveGroup = CGame::DEFAULT_PROGRESSIVE_GROUP;
	m_address = (byte)1;
	m_serialNumber = "";
	m_assetNumber = "";
	m_accountingDenom = 0;
	m_multiDenomEnabled = false;
	m_extendedMetersEnabled = false;
	m_aftEnabled = false;
	m_aftBonusEnabled = false;
	m_aftInHouseEnabled = false;
	m_legacyBonusEnabled = false;
	m_egmTransferMode = EGMTransferMode_PENDING;
	m_eftTransferToEnabled = false;
	m_eftTransferFromEnabled = false;
	m_validationExtensionsEnabled = false;
	m_lastTotalCoinOutMeterValue = INVALID_METER_VALUE;
	m_currentDenomMultiplier = 0;
	m_lastGamelistTimestamp = 0;
	m_completeGamelistFetched = false;
	m_implementedGames = 0;
	m_numberOfEnabledGames = 0;
	m_lastEgmMetersSendTick = 0;
	m_egmMetersSendFrequency = 0;
	m_lastSubgameMetersSendTick = 0;
	m_subgameMetersSendFrequency = 0;
	m_forceEgmMetersUpdate = false;

	//if this is active, build it 
	//otherwise, write zero count and set active
	if (m_type == MEMORY_NVRAM)
	{
		m_dwOffset = NVRAM_GAMES_OFFSET;
		m_bOffsetIsSet = true;
		m_dwAllowedSize = NVRAM_GAMES_SIZE;

		if (IntegrityCheck())
		{
			if (IsActiveLocation(m_dwOffset))
			{
				BuildYourselfPreMGMD();
			}
			else
			{
				//First we check that it's not just a bad ActiveLocationByte
				if (GameCountsMatch())
				{
					//if it is we'll set it as active and move on.
					this->SetActiveLocation(m_dwOffset);
					BuildYourselfPreMGMD();
				}
			}
		}
	}
}

CGames::~CGames(void)
{
	for_each(m_gamesArray.begin(), m_gamesArray.end(), [](CGame *game) { delete game; });
	m_gamesArray.clear();
}

CGames & CGames::operator=(const CGames &gRHS)
{
	m_criticalSection.lock();
	if (this != &gRHS)
	{
		this->m_type = MEMORY_NONE;
		this->m_memoryReset = false;
		this->m_NVRAMGameCount = 0;

		for (int i = 0; i < gRHS.GetGameCount(); i++)
		{
			CGame *lpGame = new CGame(*gRHS.GetGameByIndex(i));
			this->AddGame(lpGame);
		}

		ShallowCopy(gRHS);
	}
	m_criticalSection.unlock();

	return *this;
}

string CGames::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId)
{
    UNREFERENCED_PARAMETER(&topic); 
    UNREFERENCED_PARAMETER(&whereId); 

	CGame *game(GetEGM());
	CMeters meters(MEMORY_NONE);

	if (game != nullptr)
	{
		meters = *game->GetMeters();
	}
	else
	{
		for (int mi = mCD; mi <= mVchrTktDrop; mi++)
		{
			meters.SetMeter((byte)mi);
		}
	}

	string meterVals[mMeterLast];

	for (int index = 0; index < mMeterLast; index++)
	{
		meterVals[index] = "-1";
	}

	for (int mi = 0; mi < meters.GetMeterCount(); mi++)
	{
		// Get the source meter.
		CMeter *meter(meters.GetMeterByIndex(mi));

		if (meter->GetMeterNumber() < mMeterLast)
		{
			if (meter->GetValue() != INVALID_METER_VALUE)
			{
				//load our local variable with meter value
				meterVals[meter->GetMeterNumber()] = FormatString("%lld", (long long)meter->GetValue());
			}
		}
	}

	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	writer.StartObject();
	writer.Key("Meters");
	{
		writer.StartArray();
		for (int index = mCD; index < mMeterLast; ++index)
		{
			writer.StartObject();
			writer.Key("Label");	writer.String(TXT_METER_NAMES[index]);
			writer.Key("Value");	writer.String(meterVals[index] == "-1" ? "N/A" : meterVals[index]);
			writer.EndObject();
		}
		writer.EndArray();
	}
	writer.EndObject();

	return writeStream.m_str;
}

int CGames::GetGameCount() const
{
	return m_gamesArray.size();
}

const CGame * CGames::GetGame( DWORD gameNumber ) const
{
	const CGame *retVal = NULL;

	m_criticalSection.lock();
	for(const auto& item : m_gamesArray)
	{
		if (item->GetGameNumber() == gameNumber)
		{
			retVal = item;
			break;
		}
	}
	
	m_criticalSection.unlock();

	return retVal;
}

CGame *CGames::GetGame(DWORD gameNumber)
{
	// Use the const method to do the work then cast the result.
	return const_cast<CGame *>(static_cast<const CGames &>(*this).GetGame(gameNumber));
}

const CGame *CGames::GetCurrentGame( void ) const
{
	CGame *currentGame = NULL;

	m_criticalSection.lock();
	for(const auto& item : m_gamesArray)
	{
		if (item->IsCurrentGame())
		{
			currentGame = item;
			break;
		}
	}
	m_criticalSection.unlock();

	return currentGame;
}

CGame *CGames::GetCurrentGame()
{
	// Use the const method to do the work then cast the result.
	return const_cast<CGame *>(static_cast<const CGames &>(*this).GetCurrentGame());
}

CGame *CGames::GetCurrentSessionGame( ) 
{
	CGame *currentGame = GetCurrentGame();
	CGame *retGame = NULL;
	
	if (currentGame != NULL)
	{
		DWORD currentGameNumber = currentGame->GetGameNumber();
		
		if (currentGameNumber > EGM_GAME_NUMBER || ( EGM_GAME_NUMBER == currentGameNumber && GetGameCount() == 1))
		{
			retGame = currentGame;
		}
        else
        {
            LogString(GENERAL, _T("GetCurrentSessionGame: current game number: %d. Game count: %d"), currentGameNumber, GetGameCount());
        }
	}
    else
    {
        LogString(GENERAL, _T("GetCurrentSessionGame: current game could not be found"));
    }

	return retGame;
}

CGame *CGames::GetAdjustedCurrentGameCopy() const
{
	const CGame *currentGame = NULL;

	unique_ptr<CGames> enabledGames(GetEnabledGames());
	if (enabledGames->GetGameCount() == ONLY_ONE_GAME_COUNT)
	{
		currentGame = enabledGames->GetGameByIndex(0);
	}
	else if (enabledGames->GetGameCount() == HAS_ONE_SUBGAME_COUNT)
	{
		currentGame = enabledGames->GetGameByIndex(1);
	}
	else if (enabledGames->GetGameCount() > 0)
	{
		currentGame = GetCurrentGame();
	}
	else
	{
		LogString(PROGRESSIVE, "No enabled game");
	}

	CGame* currentGameCopy(NULL);
	if (currentGame != NULL)
	{
		currentGameCopy = new CGame(*currentGame);
		LogString(PROGRESSIVE, "The current game is %d", currentGame->GetGameNumber());
	}
	else
	{
		LogString(PROGRESSIVE, "No current game. Send EGM_GAME_NUMBER");

		const CGame *egmGame(GetGame(EGM_GAME_NUMBER));
		if (egmGame != nullptr)
		{
			// No current game. Send EGM_GAME_NUMBER
			currentGameCopy = new CGame(*egmGame);
		}
	}

	if ((enabledGames->GetGameCount() == ONLY_ONE_GAME_COUNT || GetGameCount() == ONLY_ONE_GAME_COUNT) &&
		currentGameCopy && currentGameCopy->GetGameNumber() == 0)
	{
		LogString(PROGRESSIVE, "SetGameNumber SINGLE_GAME_NUMBER");
		currentGameCopy->SetGameNumber(SINGLE_GAME_NUMBER);
	}


	return currentGameCopy;
}

CGame * CGames::GetEGM()
{
	if (m_gamesArray.size() > 0)
	{
		return m_gamesArray[0];
	}
	else
	{
		return NULL;
	}
}

__int64 CGames::GetEgmMeterValue(byte meterNumber)
{
	__int64 meterValue(INVALID_METER_VALUE);

	CGame *egmGame(GetEGM());
	if (egmGame != nullptr)
	{
		meterValue = egmGame->GetMeter(meterNumber);
	}

	return meterValue;
}

const CGame *CGames::GetGameByIndex(int gameIndex) const
{
	CGame *retVal = NULL;

	if (gameIndex < (int)m_gamesArray.size())
		retVal = m_gamesArray[gameIndex];

	return retVal;
}

CGame *CGames::GetGameByIndex(int gameIndex)
{
	// Use the const method to do the work then cast the result.
	return const_cast<CGame *>(static_cast<const CGames &>(*this).GetGameByIndex(gameIndex));
}

void CGames::AddGame(CGame* lpGame)
{
	m_criticalSection.lock();
	if (lpGame->IsEGM() && (m_gamesArray.size() > 0))
	{
		LogString(GAMES, "EGM %u already exists.", m_gamesArray[0]->GetGameNumber());
	}
	else
	{
		m_gamesArray.push_back(lpGame);
		//Spencer Fairbairn 4/21/2009
		//Tracker 23786 - The sentinel should not be storing subgames in NVRAM, since it does not remove them
		//when they are removed from the game, and we do not have the capacity to store them all anyway.
		//Tedd Vernaz 7/9/2020 - Modifying to store "enabled" games in NVRAM.
		if (m_type == MEMORY_NVRAM && lpGame->IsEnabled())
		{
			DWORD dwGameOffset = GetNewGameOffset();
			BYTE byActive = ACTIVE_LOCATION;

			if (dwGameOffset != OUT_OF_SPACE_FAILURE)
			{
				//S.Fairbairn 3/12/07 Added so only 256 subgames get stored
				// Now storing enabled games, disregard next comment.
				// S.Fairbairn 4/21/2009... but now we should only be storing one. Sometime in the future we should
				// Reclaim this NVRAM

				lpGame->SetMemoryType(MEMORY_NVRAM);

				lpGame->SetOffset(dwGameOffset);
				lpGame->InitialWriteToPermanentStore();
				m_NVRAMGameCount++;

				//Atomically update the game count and set the game active in NVRAM
				Write(m_dwOffset + NVRAM_LOCATION_IS_ACTIVE, (BYTE*)&m_NVRAMGameCount, sizeof(m_NVRAMGameCount),
					dwGameOffset, (BYTE*)&byActive, sizeof(byActive)); 

				LogString(GAMES, "AddGame GameNumber=%u IsEnabled=%u dwGameOffset=%u m_NVRAMGameCount=%u",
					lpGame->GetGameNumber(), lpGame->IsEnabled(), dwGameOffset, m_NVRAMGameCount);
			}
			else
			{
				LogString(ERROR_LOG, "AddGame - no available offset GameNumber=%u IsEnabled=%u", lpGame->GetGameNumber(), lpGame->IsEnabled());
				LogString(GAMES, "AddGame - no available offset GameNumber=%u IsEnabled=%u", lpGame->GetGameNumber(), lpGame->IsEnabled());
			}
		}
	}
	m_criticalSection.unlock();

}

void CGames::RemoveGame(DWORD dwGameNumber)
{
	CGame *tempGame;

	m_criticalSection.lock();
	for (auto it = m_gamesArray.begin(); it != m_gamesArray.end(); it++)
	{
		if ((*it)->GetGameNumber() == dwGameNumber)
		{
			tempGame = *it;
			m_gamesArray.erase(it);
			if (tempGame->GetMemoryType() == MEMORY_NVRAM)
			{
				if (tempGame->IsEnabled())
				{
					// Mark location inactive
					tempGame->RemoveFromPermanentStore();
					DecrementEnabledNVRAMGameCount();
				}

			}
			
			delete tempGame;
			
			break;
		}
	}

	m_criticalSection.unlock();
}

bool CGames::SetGame(const CGame *sourceGame)
{
	bool subgameAdded = false;

	CGame *targetGame(nullptr);

	if (sourceGame != nullptr)
	{
		if (sourceGame->IsCurrentGame())
		{
			CGame *currentGame = GetCurrentGame();
			if (currentGame != NULL && currentGame->GetGameNumber() != sourceGame->GetGameNumber())
			{
				currentGame->ChangeCurrentGameStatus(false);
			}
		}

		// See if we can find this game in our list.
		targetGame = GetGame(sourceGame->GetGameNumber());

		// If we did not find a game we have to add a new one.
		if (targetGame == nullptr)
		{
			//The EGM already exists, but may not have its number set yet
			if (sourceGame->IsEGM())
			{
				targetGame = GetEGM();			
			}			
			if (targetGame)
			{
				targetGame->SetGameNumber(sourceGame->GetGameNumber());
			}
			else
			{
				targetGame = new CGame(MEMORY_NONE, false);
				subgameAdded = true;
			}
		}
		else if (m_type == MEMORY_NVRAM)
		{
			// Found existing game and games are stored in nvram
			// If game was enabled and new game is disabled, it needs to be removed from NVRAM
			if (targetGame->IsEnabled() && !sourceGame->IsEnabled())
			{
				LogString(GAMES, "Removing game number %u from NVRAM.", targetGame->GetGameNumber());
				targetGame->RemoveFromPermanentStore();
				DecrementEnabledNVRAMGameCount();
			}
		}

		// Set the data for this game.
		targetGame->SetGameData(*sourceGame);
		if (subgameAdded)
		{
			AddGame(targetGame);
		}		
	}
	
	if (targetGame)
	{
		LogString(GAMES, "SetGame GameNumber=%u Enabled=%u IsEGM=%u Added=%u",
			targetGame->GetGameNumber(), targetGame->IsEnabled(), targetGame->IsEGM(), subgameAdded);
	}

	return subgameAdded;
}

void CGames::SetCurrentGame(WORD newCurrentGameNumber)
{
	LogString(GENERAL, _T("SetCurrentGame to %d"), newCurrentGameNumber);
	CGame *currentGame = GetCurrentGame();
	CGame *newCurrentGame = GetGame(newCurrentGameNumber);

    if (currentGame == NULL) 
    {
        LogString(GENERAL, _T("SetCurrentGame: Could not find previous current game in list"));
    }
    else
    {
        LogString(GENERAL, _T("SetCurrentGame: Previous current game number: %d"), currentGame->GetGameNumber());
    }

    if (newCurrentGame == NULL) 
    {
        LogString(GENERAL, _T("SetCurrentGame: Could not find new current game in list"));
    }
    else
    {
        LogString(GENERAL, _T("SetCurrentGame: new current game number is to be %d"), newCurrentGame->GetGameNumber());
    }

	if ( currentGame != newCurrentGame )
	{
		if (currentGame != NULL)
		{
			currentGame->ChangeCurrentGameStatus(false);
            LogString(GENERAL, _T("SetCurrentGame: Game number %d set as NOT current game"), currentGame->GetGameNumber());
		}

		if (newCurrentGame != NULL)
		{
			newCurrentGame->ChangeCurrentGameStatus(true);
            LogString(GENERAL, _T("SetCurrentGame: Game number %d set as current game"), newCurrentGame->GetGameNumber());
		}
	}
    else
    {
        LogString(GENERAL, _T("SetCurrentGame: New current game not set as prev/new selected game objects are the same."));
    }
}

void CGames::SetCurrentDenom(int currentDenomMultiplier)
{
	m_currentDenomMultiplier = currentDenomMultiplier;
}

DWORD CGames::GetNewGameOffset()
{
	// Start from the beginning and returned first unused spot.
	DWORD dwCurrentOffset = m_dwOffset + NVRAM_LOCATION_IS_ACTIVE + sizeof(DWORD);
	DWORD dwCount = m_gamesArray.size();
	int nvramItemCount = 1; // default EGM game

	if (dwCount != 0)
	{
		//Special case for the egm game;
		if( IsActiveLocation(dwCurrentOffset))
		{
			dwCurrentOffset+= NVRAM_EGM_GAME_SIZE;
			while (((dwCurrentOffset + NVRAM_SUBGAME_SIZE) <= (m_dwAllowedSize + m_dwOffset)) && IsActiveLocation(dwCurrentOffset))
			{
				nvramItemCount++;
				dwCurrentOffset+= NVRAM_SUBGAME_SIZE;
			}
		}
	}

	if ((dwCurrentOffset + NVRAM_SUBGAME_SIZE) > (m_dwAllowedSize + m_dwOffset))
	{
		return OUT_OF_SPACE_FAILURE;
	}
	else
	{
		return dwCurrentOffset;
	}
}

void CGames::ResetGames()
{
	m_criticalSection.lock();

	//empty the games array
	for_each(m_gamesArray.begin(), m_gamesArray.end(), [](CGame *game){ delete game;});
	m_gamesArray.clear();

	//Go through the Games NVRAM and set the 
	//Active markers on all Games to Inactive
	DWORD dwCurrentOffset = m_dwOffset + NVRAM_LOCATION_IS_ACTIVE + sizeof (DWORD);

	//EGM game;
	SetInactiveLocation(dwCurrentOffset);
	dwCurrentOffset+= NVRAM_EGM_GAME_SIZE;
	SetInactiveLocation(dwCurrentOffset);
	dwCurrentOffset+= NVRAM_SUBGAME_SIZE;

	//And All Subgames
	while (dwCurrentOffset < (m_dwAllowedSize + m_dwOffset))
	{
		SetInactiveLocation(dwCurrentOffset);
		dwCurrentOffset+= NVRAM_SUBGAME_SIZE;
	}

	m_NVRAMGameCount = 0;
	BYTE byActive = ACTIVE_LOCATION;

	Write(m_dwOffset, (BYTE*)&byActive, sizeof(byActive),
		m_dwOffset + NVRAM_LOCATION_IS_ACTIVE, (BYTE*)&m_NVRAMGameCount, sizeof(m_NVRAMGameCount));

	//Create default egm game
	CGame * lpgame = new CGame(MEMORY_NONE, true);
	AddGame(lpgame);

	//Initialize calculated meters
	lpgame->SetMeter(mPtR, 0);
	lpgame->SetMeter(mPBn, 0);
	lpgame->SetMeter(mNonNcepPlayed, 0);

	UpdateGameCount();

	m_criticalSection.unlock();
}

void CGames::BuildYourself()
{
	if (m_type != MEMORY_NONE)
	{
		DWORD dwCurrentOffset = m_dwOffset + NVRAM_LOCATION_IS_ACTIVE;
		m_NVRAMGameCount = 0;
		int currentCount = 0;

		//Get the number of games
		Read(dwCurrentOffset, (BYTE*)&m_NVRAMGameCount, sizeof(m_NVRAMGameCount));
		dwCurrentOffset+= sizeof(m_NVRAMGameCount);
		
		LogString(GAMES, "BuildYourself m_NVRAMGameCount=%u", m_NVRAMGameCount);

		int sizeOfGame(NVRAM_EGM_GAME_SIZE);

		//If there are any games:
		if (m_NVRAMGameCount)
		{
			// cycle through games
			while (currentCount < m_NVRAMGameCount && dwCurrentOffset < m_dwAllowedSize + m_dwOffset)
			{
				CGame* lpGame;

				//This is the EGM itself and should ALWAYS be there.
				//but we'll check anyway.
				if (IsActiveLocation(dwCurrentOffset))
				{
					lpGame = new CGame(m_type, true);
					lpGame->SetOffset(dwCurrentOffset);
					lpGame->BuildYourself();
					//This check is here because we saw a field issue which caused
					//subgame to replace the EGM location and the only way to recover is reset games info.
					//Checking for NVRAM_EGM_GAME_SIZE == sizeOfGame means that we're looking at EGM (game number 0).
					//After reading EGM, we change the sizeOfGame to NVRAM_SUBGAME_SIZE
					if (!lpGame->IsEGM() && NVRAM_EGM_GAME_SIZE == sizeOfGame)
					{
						LogString(GAMES, "BuildYourself remove game number %u at offset %u", lpGame->GetGameNumber(), dwCurrentOffset);
						lpGame->RemoveFromPermanentStore();
						delete lpGame;
						ResetGames();
						m_memoryReset = true;
						break;
					}
					else
					{
						m_gamesArray.push_back(lpGame);
						LogString(GAMES, "BuildYourself added game number %u at offset %u", lpGame->GetGameNumber(), dwCurrentOffset);
					}
					currentCount++;
				}
				else
				{
					LogString(GAMES, "BuildYourself inactive location at offset %u", dwCurrentOffset);
				}
				// move offset pointer based on EGM or subgame
				dwCurrentOffset += sizeOfGame;
				sizeOfGame = NVRAM_SUBGAME_SIZE;
			}
		}
	}
}

void CGames::BuildYourselfPreMGMD()
{
	if (m_type == MEMORY_NVRAM)
	{
		DWORD dwCurrentOffset = m_dwOffset + NVRAM_LOCATION_IS_ACTIVE;
		m_NVRAMGameCount = 0;

		//Get the number of games
		Read(dwCurrentOffset, (BYTE*)&m_NVRAMGameCount, sizeof(m_NVRAMGameCount));
		dwCurrentOffset += sizeof(m_NVRAMGameCount);

		//If there are any games:
		if (m_NVRAMGameCount)
		{
			CGame * lpGame;
			//This is the EGM itself and should ALWAYS be there.
			//but we'll check anyway.
			if (IsActiveLocation(dwCurrentOffset))
			{
				lpGame = new CGame(m_type, true);
				lpGame->SetOffset(dwCurrentOffset, true);
				lpGame->BuildYourself();
				m_gamesArray.push_back(lpGame);
			}
		}
	}
}

bool CGames::GameCountsMatch()
{
	DWORD dwCurrentOffset = m_dwOffset + NVRAM_LOCATION_IS_ACTIVE;
	DWORD dwCount = 0;
	DWORD dwActualCount =0;
	Read(dwCurrentOffset, (BYTE*)&dwCount, sizeof(dwCount));

	dwCurrentOffset+= sizeof(dwCurrentOffset);

	if (dwCount != 0)
	{
		//Special case for the egm game;
		if( IsActiveLocation(dwCurrentOffset))
		{
			dwActualCount++;
			dwCurrentOffset+= NVRAM_EGM_GAME_SIZE;
			while (((dwCurrentOffset + NVRAM_SUBGAME_SIZE) <= (m_dwAllowedSize + m_dwOffset)) && IsActiveLocation(dwCurrentOffset))
			{
				dwActualCount++;
				dwCurrentOffset+= NVRAM_SUBGAME_SIZE;
			}
		}
	}

	return ((dwCount > 0) && (dwCount == dwActualCount));
}

void CGames::DecrementEnabledNVRAMGameCount()
{
	if (m_NVRAMGameCount)
	{
		m_NVRAMGameCount--;
	}	
	UpdateGameCount();
}

void CGames::UpdateGameCount()
{
	if (m_type == MEMORY_NVRAM)
	{
		DWORD dwCurrentOffset = m_dwOffset + NVRAM_LOCATION_IS_ACTIVE;
		Write(dwCurrentOffset, (BYTE*)&m_NVRAMGameCount, sizeof(m_NVRAMGameCount));
	}
}

// Returns the amount in pennies for the provided credit value using the denom for the selected game.
__int64 CGames::ConvertCreditsForSelectedDenom( __int64 creditValue )
{
	__int64 amountInPennies = 0;

	CGame *selectedGame = GetCurrentGame();

	if ( selectedGame == NULL )
		selectedGame = GetEGM();

	if ( selectedGame != NULL )
	{
		amountInPennies = creditValue * selectedGame->GetDenomMultiplier();
	}

	return amountInPennies;
}

__int64 CGames::ConvertCredits( __int64 creditValue )
{
	__int64 amountInPennies = 0;

	CGame *game = GetCurrentGame();

	if ( game != NULL )
	{
		amountInPennies = creditValue * game->GetDenomMultiplier();
	}

	return amountInPennies;
}

void CGames::SetSASVersion(const string& sasVersion)
{
	m_SASVersion = sasVersion;
}

string CGames::GetSASVersion() const
{
	return m_SASVersion;
}

void CGames::SetGameId(const string& gameId)
{
	m_gameId = gameId;
}

string CGames::GetGameId() const
{
	return m_gameId;
}

void CGames::SetProgressiveGroup(byte progressiveGroup)
{
	m_progressiveGroup = progressiveGroup;
}

byte CGames::GetProgressiveGroup() const
{
	return m_progressiveGroup;
}

void CGames::CalcProgressiveGroup()
{
	bool done(m_progressiveGroup != CGame::NON_SAS_PROGRESSIVE_GROUP);
	for (int gameIndex(0); gameIndex < (int)m_gamesArray.size() && !done; ++gameIndex)
	{
		m_progressiveGroup = m_gamesArray[gameIndex]->GetProgressiveGroup();
		done = m_progressiveGroup != CGame::NON_SAS_PROGRESSIVE_GROUP;
	}
}

void CGames::SetAddress(byte address)
{
	m_address = address;
}

byte CGames::GetAddress() const
{
	return m_address;
}

void CGames::SetSerialNumber(const string& serialNumber)
{
	m_serialNumber = serialNumber;
}

string CGames::GetSerialNumber() const
{
	return m_serialNumber;
}

void CGames::SetAssetNumber(const string& assetNumber)
{
	m_assetNumber = assetNumber;
}

string CGames::GetAssetNumber() const
{
	return m_assetNumber;
}

void CGames::SetAccountingDenom(int accountingDenom)
{
	m_accountingDenom = accountingDenom;
}

int CGames::GetAccountingDenom() const
{
	return m_accountingDenom;
}

void CGames::SetMultiDenomEnabled(bool multi)
{
	m_multiDenomEnabled = multi;
}

bool CGames::GetMultiDenomEnabled() const
{
	return m_multiDenomEnabled;
}

void CGames::SetExtendedMetersEnabled(bool val)
{
	m_extendedMetersEnabled = val;
}

bool CGames::GetExtendedMetersEnabled() const
{
	return m_extendedMetersEnabled;
}

void CGames::SetAftEnabled(bool val)
{
	m_aftEnabled = val;
}

bool CGames::GetAftEnabled() const
{
	return m_aftEnabled;
}

void CGames::SetAftBonusEnabled(bool val)
{
	m_aftBonusEnabled = val;
}

bool CGames::GetAftBonusEnabled() const
{
	return m_aftBonusEnabled;
}

void CGames::SetAftInHouseEnabled(bool val)
{
	m_aftInHouseEnabled = val;
}

bool CGames::GetAftInHouseEnabled() const
{
	return m_aftInHouseEnabled;
}

void CGames::SetLegacyBonusEnabled(bool val)
{
	m_legacyBonusEnabled = val;
}

bool CGames::GetLegacyBonusEnabled() const
{
	return m_legacyBonusEnabled;
}

void CGames::SetEGMTransferMode(EGMTransferMode mode)
{
	m_egmTransferMode = mode;
}

EGMTransferMode CGames::GetEGMTransferMode() const
{
	return m_egmTransferMode;
}

void CGames::SetEftTransferToEnabled(bool val)
{
	m_eftTransferToEnabled = val;
}

bool CGames::GetEftTransferToEnabled() const
{
	return m_eftTransferToEnabled;
}

void CGames::SetEftTransferFromEnabled(bool val)
{
	m_eftTransferFromEnabled = val;
}

bool CGames::GetEftTransferFromEnabled() const
{
	return m_eftTransferFromEnabled;
}

void CGames::SetValidationExtensionsEnabled(bool val)
{
	m_validationExtensionsEnabled = val;
}

bool CGames::GetValidationExtensionsEnabled() const
{
	return m_validationExtensionsEnabled;
}

void CGames::SetLastTotalCoinOutMeterValue(__int64 val)
{
	m_lastTotalCoinOutMeterValue = val;
}

__int64 CGames::GetLastTotalCoinOutMeterValue() const
{
	return m_lastTotalCoinOutMeterValue;
}

CGames *CGames::GetEnabledGames() const
{
	m_criticalSection.lock();
	CGames *games = new CGames(MEMORY_NONE);

	// Add enabled games
	for (int i = 0; i < GetGameCount(); i++)
	{
		const CGame *game = GetGameByIndex(i);
		if (game != NULL && game->IsEnabled())
		{
			CGame *gameCopy = new CGame(*game);
			games->AddGame(gameCopy);
		}
	}

	// Copy other fields
	games->ShallowCopy(*this);

	m_criticalSection.unlock();

	return games;
}

void CGames::ShallowCopy(const CGames &games)
{
	m_SASVersion = games.GetSASVersion();
	m_gameId = games.GetGameId();
	m_address = games.GetAddress();
	m_serialNumber = games.GetSerialNumber();
	m_assetNumber = games.GetAssetNumber();
	m_accountingDenom = games.GetAccountingDenom();
	m_multiDenomEnabled = games.GetMultiDenomEnabled();
	m_extendedMetersEnabled = games.GetExtendedMetersEnabled();
	m_aftEnabled = games.GetAftEnabled();
	m_aftBonusEnabled = games.GetAftBonusEnabled();
	m_aftInHouseEnabled = games.GetAftInHouseEnabled();
	m_legacyBonusEnabled = games.GetLegacyBonusEnabled();
	m_egmTransferMode = games.GetEGMTransferMode();
	m_eftTransferToEnabled = games.GetEftTransferToEnabled();
	m_eftTransferFromEnabled = games.GetEftTransferFromEnabled();
	m_validationExtensionsEnabled = games.GetValidationExtensionsEnabled();
	m_lastTotalCoinOutMeterValue = games.GetLastTotalCoinOutMeterValue();
	m_progressiveGroup = games.GetProgressiveGroup();
	m_currentDenomMultiplier = games.GetCurrentDenom();
	m_lastGamelistTimestamp = games.m_lastGamelistTimestamp;
	m_completeGamelistFetched = games.m_completeGamelistFetched;
	m_implementedGames = games.m_implementedGames;
	m_numberOfEnabledGames = games.m_numberOfEnabledGames;
	m_lastEgmMetersSendTick = games.m_lastEgmMetersSendTick;
	m_egmMetersSendFrequency = games.m_egmMetersSendFrequency;
	m_lastSubgameMetersSendTick = games.m_lastSubgameMetersSendTick;
	m_subgameMetersSendFrequency = games.m_subgameMetersSendFrequency;
	m_forceEgmMetersUpdate = games.m_forceEgmMetersUpdate;;
}

void CGames::UpdateSubGameData(CGames *games)
{
	if (games != nullptr)
	{
		ShallowCopy(*games);
		for_each(m_gamesArray.begin(), m_gamesArray.end(),[&](CGame *game)
		{
			if (game != nullptr)
			{
				CGame *match = games->GetGame(game->GetGameNumber());
				if (match != nullptr)
				{
					// save existing denom
					int denom = game->GetDenomMultiplier();
					game->SetNonProgressiveGameData(*match);

					// reset saved denom
					game->SetDenom(denom, game->GetDenomType());
				}
			}
		});

		// Update lastCoinOutMeterValue
		SetLastTotalCoinOutMeterValue(games->GetLastTotalCoinOutMeterValue());
	}
}

int CGames::GetCurrentDenom() const
{
	return m_currentDenomMultiplier;
}

std::vector<CGame *>& CGames::GetGames()
{
    return m_gamesArray;
}

void CGames::UpdateLastMetersSentTime(bool egmMeters)
{
	if (egmMeters)
	{
		m_lastEgmMetersSendTick = m_operatingSystem.GetTickCount32();
		m_forceEgmMetersUpdate = false;
	}
	else
	{
		m_lastSubgameMetersSendTick = m_operatingSystem.GetTickCount32();
	}
}

void CGames::RefreshMetersSendFrequency(bool isNfdPoller)
{
	DWORD egmMetersSendTriggerOffset = 0;
	DWORD subgameMetersSendTriggerOffset = 0;

	// Calculate the frequency and time variation to use in sending 
	// EGM and subgame meter updates from a sentinel unit. 
	if (isNfdPoller)
	{
		m_egmMetersSendFrequency = 5 * MILLISECONDS_PER_MINUTE;     // 5 mins
		m_subgameMetersSendFrequency = 0.5 * MILLISECONDS_PER_DAY;  // twice-a-day
		egmMetersSendTriggerOffset = m_operatingSystem.GetRandRange(0, MILLISECONDS_PER_MINUTE);
		subgameMetersSendTriggerOffset = m_operatingSystem.GetRandRange(0, 10 * MILLISECONDS_PER_MINUTE);
	}
	else
	{
		m_egmMetersSendFrequency = m_subgameMetersSendFrequency = 60 * MILLISECONDS_PER_MINUTE; // each hour
		egmMetersSendTriggerOffset = subgameMetersSendTriggerOffset =
			m_operatingSystem.GetRandRange(0, 10 * MILLISECONDS_PER_MINUTE);
	}

	m_lastEgmMetersSendTick = m_operatingSystem.GetTickCount32() + egmMetersSendTriggerOffset;
	m_lastSubgameMetersSendTick = m_operatingSystem.GetTickCount32() + subgameMetersSendTriggerOffset;

	LogString(GAMES, "[%s]: IsNFDPoller: %d egmMetersSendTriggerOffset: %u "
		"subgameMetersSendTriggerOffset: %u",	__FUNCTION__, isNfdPoller, 
		egmMetersSendTriggerOffset, subgameMetersSendTriggerOffset);
}

bool CGames::IsTimeToSendMeters(bool egmMeters)
{
	bool timeToSendMeters = false;
	if (egmMeters && m_egmMetersSendFrequency)
	{
		timeToSendMeters = m_forceEgmMetersUpdate ||
			((signed)(m_operatingSystem.GetTickCount32() - m_lastEgmMetersSendTick) >= 
			(signed) m_egmMetersSendFrequency);
	}
	else if (!egmMeters && m_subgameMetersSendFrequency)
	{
		timeToSendMeters = ((signed)(m_operatingSystem.GetTickCount32() - m_lastSubgameMetersSendTick) >=
			(signed) m_subgameMetersSendFrequency);
	}

	return timeToSendMeters;
}

void CGames::ForceEgmMetersUpdate()
{
	LogString(GAMES, "%s %d", __FUNCTION__, __LINE__);
	m_forceEgmMetersUpdate = true;
}

time_t CGames::GetLastGamelistTimestamp()
{
	return m_lastGamelistTimestamp;
}

bool CGames::IsCompleteGamelistFetched()
{
	return m_completeGamelistFetched;
}

void CGames::ResetIsCompleteGamelistFetched()
{	
	LogString(GAMES, "%s %d", __FUNCTION__, __LINE__);
	m_completeGamelistFetched = false;
	{
		//This means GPE_OPTIONS_CHANGED occured
		//so we need to clear all of previously fetched subgames
		lock_guard<recursive_mutex> lock(m_criticalSection);
		for (const auto& item : m_gamesArray)
		{
			item->SetInfoFetchedFlag(false);
		}
	}
}

WORD CGames::UpdateLastGamelistTimestamp(time_t lastGamelistTimestamp)
{
	WORD gameCountsFetched(GetGameCountsFetched());

	LogString(GAMES, "UpdateLastGamelistTimestamp m_completeGamelistFetched: %d, m_implementedGames=%u, gameCountsFetched = %d", 
		m_completeGamelistFetched, m_implementedGames, gameCountsFetched);

	if ((m_implementedGames == gameCountsFetched) && !m_completeGamelistFetched)
	{
		LogString(GAMES, "All games information have been fetched.");
		m_lastGamelistTimestamp = lastGamelistTimestamp;
		m_completeGamelistFetched = true;
	}

	return gameCountsFetched;
}

WORD CGames::GetGameCountsFetched()
{
	WORD gameCountsFetched(0);

	{
		lock_guard<recursive_mutex> lock(m_criticalSection);
		for (const auto& item : m_gamesArray)
		{
			if (item->IsInfoFetched())
			{
				++gameCountsFetched;
			}
		}
	}

	return gameCountsFetched;
}

void CGames::SetNumberOfImplementedGames(WORD value)
{
	if (value > 0)
	{
		m_implementedGames = value;
		LogString(GAMES, "SetNumberOfImplementedGames to %u.", m_implementedGames);		
	}	
}

WORD CGames::GetNumberOfImplementedGames() const
{
	return m_implementedGames;
}

void CGames::SetNumberOfEnabledGames(DWORD value)
{
	if (value > 0)
	{
		value -= 1;//don't include the main EGM
	}
	m_numberOfEnabledGames = value;
}

DWORD CGames::GetNumberOfEnabledGames() const
{
	return m_numberOfEnabledGames;
}

bool CGames::HaveAllTheEnabledGamesBeenFetched() const
{
	bool allEnabledGamesHaveBeenFetched(true);
	{
		lock_guard<recursive_mutex> lock(m_criticalSection);
		for (const auto& item : m_gamesArray)
		{
			if (item->IsEnabled() && !item->IsInfoFetched())
			{
				LogString(GAMES, "GameNumber=%u has not been fetched.", item->GetGameNumber());
				allEnabledGamesHaveBeenFetched = false;
				break;
			}
		}
	}
	return allEnabledGamesHaveBeenFetched;
}

bool CGames::SyncTheEnabledGames(vector<DWORD>* vEnabledGameNumbers)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	bool enabledGamesMatchesNVRAM(true);

	auto gameIter = m_gamesArray.begin();
	while (gameIter != m_gamesArray.end())
	{
		if ((*gameIter)->IsEnabled())
		{
			DWORD gameNumber((*gameIter)->GetGameNumber());
			if ((find_if(vEnabledGameNumbers->begin(), vEnabledGameNumbers->end(), [&gameNumber](const DWORD value) {return value == gameNumber; }) != vEnabledGameNumbers->end()))
			{
				LogString(GAMES, "GameNumber=%u is still enabled.", gameNumber);
				++gameIter;
			}
			else
			{
				LogString(GAMES, "m_NVRAMGameCount=%u GameNumber=%u went from enabled to disabled.", m_NVRAMGameCount, gameNumber);
				enabledGamesMatchesNVRAM = false;
				CGame* tempGame(*gameIter);
				gameIter = m_gamesArray.erase(gameIter);
				tempGame->RemoveFromPermanentStore();
				DecrementEnabledNVRAMGameCount();

			}
		}
		else
		{
			++gameIter;
		}
	}

	return enabledGamesMatchesNVRAM;
}

string CGames::GetBonusPaymentMethod() const
{
	string bonusPaymentMethod;

	if (m_aftBonusEnabled)
	{
		bonusPaymentMethod = "SAS_AFT_BONUS";
	}
	else if (m_legacyBonusEnabled)
	{
		bonusPaymentMethod = "SAS_LEGACY_BONUS";
	}
	else
	{
		bonusPaymentMethod = "None";
	}

	return bonusPaymentMethod;
}

bool CGames::IsDenomValid(WORD denom)
{
	bool valid(false);

	//Skip 0 denom code when checking for a valid denom
	for (int index = 1; index < NUM_DENOM_CODE_MAPPINGS; index++)
	{
		if (denom == denomCodeMap[index].multiplier)
		{
			valid = true;
			break;
		}
	}

	return valid;
}