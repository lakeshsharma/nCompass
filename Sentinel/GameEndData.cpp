#include "stdafx.h"
#include "GameEndData.h"


CGameEndData::CGameEndData(CGame *currentGame,
	long lastPlayedCashableAmount,
	long lastPlayedRestrictedAmount,
	long lastWinAmount, 
	PokerHand *pokerHand,
	int currentGameDenom, 
	double basePointsEarned,
	bool offline) :
	m_lastPlayedCashableAmount(lastPlayedCashableAmount),
	m_lastPlayedRestrictedAmount(lastPlayedRestrictedAmount),
	m_lastWinAmount(lastWinAmount),
	m_currentGameDenomMult(currentGameDenom),
	m_basePointsEarned(basePointsEarned),
	m_offline(offline)
{
	m_currentGame = currentGame;
	if (pokerHand != nullptr)
		m_pokerHand = *pokerHand;
}

CGameEndData::~CGameEndData()
{
	delete m_currentGame;
}

const CGame *CGameEndData::GetCurrentGame() const
{
	return m_currentGame;
}

long CGameEndData::GetLastPlayedCashableAmount() const
{
	return m_lastPlayedCashableAmount;
}

long CGameEndData::GetLastPlayedRestrictedAmount() const
{
	return m_lastPlayedRestrictedAmount;
}

long CGameEndData::GetLastWinAmount() const
{
	return m_lastWinAmount;
}

double CGameEndData::GetBasePointsEarned() const
{
	return m_basePointsEarned;
}

const PokerHand &CGameEndData::GetPokerHand() const
{
	return m_pokerHand;
}

int CGameEndData::GetCurrentDenom() const
{
	return m_currentGameDenomMult;
}

void CGameEndData::SetOffline(bool offline)
{
	m_offline = offline;
}

bool CGameEndData::GetOffline()
{
	return m_offline;
}
