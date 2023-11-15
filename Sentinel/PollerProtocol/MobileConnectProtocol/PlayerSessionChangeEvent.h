#pragma once

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"
#include "Player.h"

typedef enum
{
	SESSION_CHANGE_EVENT_GAMEPLAY,
	SESSION_CHANGE_EVENT_JACKPOT_LOCKUP,
	SESSION_CHANGE_EVENT_JACKPOT_RESET,
	SESSION_CHANGE_EVENT_CASHOUT_ERROR,
} PlayerSessionChangeEventType;

class CPlayerSessionChangeEvent  :
        public CInternalMsgContent<INT_MSG_CONTENT_PLAYER_SESSION_CHANGE_EVENT> {
public:
    CPlayerSessionChangeEvent(
					   CPlayer * player,
					   PlayerSessionChangeEventType evtType,
                       int newPlayerSessionId,
                       DWORD creditInPennies,
					   DWORD jackpotAmount = 0,
					   bool isPBTEnabled = true,
					   WORD cashoutErrorFlags = 0,
					   std::string sasErrorText = "");
	
	~CPlayerSessionChangeEvent();

public:
	/// <summary>
	/// Get Message Type.
	/// </summary>
	/// <returns>string</returns>
	const std::string &GetMessageType() const;
	
	/// <summary>
	/// Get PlayerId.
	/// </summary>
	/// <returns>DWORD</returns>
    DWORD GetPlayerId() const;
	
	/// <summary>
	/// Get CardId.
	/// </summary>
	/// <returns>string</returns>
    std::string GetCardId() const;
	
	/// <summary>
	/// Get Player SessionId.
	/// </summary>
	/// <returns>int</returns>
    int GetPlayerSessionId() const;
	
	/// <summary>
	/// Get Total Points.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetTotalPoints() const;
	
	/// <summary>
	/// Get Session Points.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetSessionPoints() const;
	
	/// <summary>
	/// Get Session Countdown.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetSessionCountdown() const;
	
	/// <summary>
	/// Get Points To Next Tier.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetPointsToNextTier() const;
	
	/// <summary>
	/// Get Credit In Pennies.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetCreditInPennies() const;

	/// <summary>
	/// Get Session change event type
	/// </summary>
	/// <returns>PlayerSessionChangeEventType</returns>
	PlayerSessionChangeEventType GetSessionChangeEventType() const;

	/// <summary>
	/// Get Jackpot Amount
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetJackpotAmount() const;

	/// <summary>
	/// Get PBT enabled (whether PBT to be enabled for mobile device)
	/// </summary>
	/// <returns>bool</returns>
	bool GetPbtEnabled() const;

	/// <summary>
	/// Get Cashout Error Flags
	/// </summary>
	/// <returns>WORD</returns>
	WORD GetCashoutErrorFlags() const;

	/// <summary>
	/// Get SAS Error text.
	/// </summary>
	/// <returns>string</returns>
	std::string GetSASErrorText() const;

private:
    std::string m_sMessageType;
	PlayerSessionChangeEventType m_evtType;
    CPlayer * 	m_uPlayer;
    int m_iPlayerSessionId;
	DWORD m_creditInPennies;
	DWORD m_jackpotAmount;
	bool m_isPbtEnabled;
	WORD m_cashoutErrorFlags;
	std::string m_sSASErrorText;
};


