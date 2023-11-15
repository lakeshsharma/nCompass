#include "PlayerSessionChangeEvent.h"
#include "MobileConnectProtocolKeys.h"

CPlayerSessionChangeEvent::CPlayerSessionChangeEvent(CPlayer * player, 
	PlayerSessionChangeEventType evtType, int sessionId, DWORD creditInPennies,
	DWORD jackpotAmount, bool isPBTEnabled, WORD cashoutErrorFlags, std::string sasErrorText):
	m_sMessageType(ToPlayerDeviceService::pEventPlayerSessionChange),
	m_evtType(evtType),
	m_uPlayer(player),
	m_iPlayerSessionId(sessionId),
	m_creditInPennies(creditInPennies),
	m_jackpotAmount(jackpotAmount),
	m_isPbtEnabled(isPBTEnabled),
	m_cashoutErrorFlags(cashoutErrorFlags),
	m_sSASErrorText(sasErrorText)
{
}

const std::string &CPlayerSessionChangeEvent::GetMessageType() const
{
    return m_sMessageType;
}

DWORD CPlayerSessionChangeEvent::GetPlayerId() const 
{
    return m_uPlayer->GetPlayerInfo()->GetPlayerId();
}

std::string CPlayerSessionChangeEvent::GetCardId() const 
{
    return m_uPlayer->GetCard()->MakeCStringCardNumber(m_uPlayer->GetCard());
}

int CPlayerSessionChangeEvent::GetPlayerSessionId() const 
{
    return m_iPlayerSessionId;
}

DWORD CPlayerSessionChangeEvent::GetTotalPoints() const
{
	return m_uPlayer->GetTotalPoints();
}

DWORD CPlayerSessionChangeEvent::GetSessionPoints() const
{
	return m_uPlayer->GetSessionAndBonusPoints();
}

DWORD CPlayerSessionChangeEvent::GetSessionCountdown() const
{
	return m_uPlayer->GetCurrentCountdown();
}

DWORD CPlayerSessionChangeEvent::GetCreditInPennies() const
{
	return m_creditInPennies;
}

DWORD CPlayerSessionChangeEvent::GetPointsToNextTier() const
{
	return m_uPlayer->GetPlayerInfo()->GetPointsTillNextTier();
}

PlayerSessionChangeEventType CPlayerSessionChangeEvent::GetSessionChangeEventType() const
{
	return m_evtType;
}

DWORD CPlayerSessionChangeEvent::GetJackpotAmount() const
{
	return m_jackpotAmount;
}

bool CPlayerSessionChangeEvent::GetPbtEnabled() const
{
	return m_isPbtEnabled;
}

WORD CPlayerSessionChangeEvent::GetCashoutErrorFlags() const
{
	return m_cashoutErrorFlags;
}

std::string CPlayerSessionChangeEvent::GetSASErrorText() const
{
	return m_sSASErrorText;
}

CPlayerSessionChangeEvent::~CPlayerSessionChangeEvent()
{
	delete m_uPlayer; 
}
