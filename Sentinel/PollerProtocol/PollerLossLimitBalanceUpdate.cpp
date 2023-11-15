#include "stdafx.h"
#include "PollerLossLimitBalanceUpdate.h"

CPollerLossLimitBalanceUpdate::CPollerLossLimitBalanceUpdate(byte * message)
{
	byte offset = 0;

	memcpy(&m_buffer.card_id, message + offset, sizeof(m_buffer.card_id));
	offset+= sizeof(m_buffer.card_id);

	memcpy(&m_buffer.withdrawalAmount, message + offset, sizeof(m_buffer.withdrawalAmount));
	offset+= sizeof(m_buffer.withdrawalAmount);

	memcpy(&m_buffer.newBalance, message + offset, sizeof(m_buffer.newBalance));
}

CPollerLossLimitBalanceUpdate::~CPollerLossLimitBalanceUpdate(void)
{
}

byte* CPollerLossLimitBalanceUpdate::GetCardID()
{
	return m_buffer.card_id;
}

WORD CPollerLossLimitBalanceUpdate::GetWithdrawalAmount()
{
	return m_buffer.withdrawalAmount;
}

WORD CPollerLossLimitBalanceUpdate::GetNewBalance()
{
	return m_buffer.newBalance;
}

