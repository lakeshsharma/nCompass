#include "stdafx.h"
#include "SASProtocolState.h"
#include "Ticket.h"


CSASProtocolState::CSASProtocolState()
	: m_config(MEMORY_NONE),
	m_ignoreTicketPrinted(true),
	m_snType(SnapshotNone),
	m_slotmastID(0),
	m_casinoNumber(0),
	m_bEnableEGM(true),
	m_bEnableHostCashout(false),
	m_bCashoutToHostEnabledOnEGM(false),
	m_cashoutToHostConfigTries(0),
	m_bAllowEnableDisable(true),
	m_bAllowGameDelayPoll(true),
	m_bAllowStepdownResetForWMS(false),
	m_bIsEnhancedTicketing(false),
	m_ticketingEnabled(false),
	m_ticketingOutEnabled(false),
	m_ticketingInEnabled(false),
	m_lastPrintedTicket(nullptr),
	m_bValidationExtensions(false),
	m_bSupportMultiDemonExtensions(false),
	m_probingAftLock(false),
	m_bLockPending(false),
	m_dwLastLockRequest(0),
	m_lastLockUnlockTransactionType(EGM_UNLOCK_ACK),
	m_updateFloorgixOfEgmState(false)
{
}


CSASProtocolState::~CSASProtocolState()
{
	if (m_lastPrintedTicket != nullptr)
	{
		delete m_lastPrintedTicket;
	}
}

void CSASProtocolState::SetEGMConfig(CEGMConfig config)
{
	m_config = config;
}

CEGMConfig CSASProtocolState::GetEGMConfig()
{
	return m_config;
}

void CSASProtocolState::SetIgnoreTicketPrinted(bool value)
{
	m_ignoreTicketPrinted = value;
}

bool CSASProtocolState::GetIgnoreTicketPrinted()
{
	return m_ignoreTicketPrinted;
}

void CSASProtocolState::SetSnapshotType(SnapshotType value)
{
	m_snType = value;
}

SnapshotType CSASProtocolState::GetSnapshotType()
{
	return m_snType;
}

long CSASProtocolState::GetSlotmastID()
{
	return m_slotmastID;
}

void CSASProtocolState::SetSlotmastID(long slotmastID)
{
	m_slotmastID = slotmastID;
}

byte CSASProtocolState::GetCasinoNumber()
{
	return m_casinoNumber;
}

void CSASProtocolState::SetCasinoNumber(byte casinoNumber)
{
	m_casinoNumber = casinoNumber;
}

void CSASProtocolState::SetEnableEGM(bool startup, bool value)
{
	m_updateFloorgixOfEgmState = !startup && m_bEnableEGM != value;
	m_bEnableEGM = value;
}

bool CSASProtocolState::GetEnableEGM()
{
	return m_bEnableEGM;
}

void CSASProtocolState::SetEnableCashoutToHost(bool value)
{
	m_bEnableHostCashout = value;
}

bool CSASProtocolState::GetEnableCashoutToHost()
{
	return m_bEnableHostCashout;
}

void CSASProtocolState::SetCashoutToHostEnabledOnEGM(bool value)
{
	m_bCashoutToHostEnabledOnEGM = value;
}

bool CSASProtocolState::GetCashoutToHostEnabledOnEGM()
{
	return m_bCashoutToHostEnabledOnEGM;
}

void CSASProtocolState::SetCashoutToHostConfigTries(int tries)
{
	m_cashoutToHostConfigTries = tries;
}

int CSASProtocolState::GetCashoutToHostConfigTries()
{
	return m_cashoutToHostConfigTries;
}

void CSASProtocolState::SetAllowableEnableDisable(bool value)
{
	m_bAllowEnableDisable = value;
}

bool CSASProtocolState::GetAllowableEnableDisable()
{
	return m_bAllowEnableDisable;
}

void CSASProtocolState::SetAllowableGameDelayPoll(bool value)
{
	m_bAllowGameDelayPoll = value;
}

bool CSASProtocolState::GetAllowableGameDelayPoll()
{
	return m_bAllowGameDelayPoll;
}

void CSASProtocolState::SetAllowStepdownResetForWMS(bool value)
{
	m_bAllowStepdownResetForWMS = value;
}

bool CSASProtocolState::GetAllowStepdownResetForWMS()
{
	return m_bAllowStepdownResetForWMS;
}

void CSASProtocolState::SetIsEnhancedTicketing(bool value)
{
	m_bIsEnhancedTicketing = value;
}

bool CSASProtocolState::GetIsEnhancedTicketing()
{
	return m_bIsEnhancedTicketing;
}

void CSASProtocolState::SetTicketingEnabled(bool value)
{
	m_ticketingEnabled = value;
}

bool CSASProtocolState::GetTicketingEnabled()
{
	return m_ticketingEnabled;
}

void CSASProtocolState::SetTicketingInEnabled(bool value)
{
	m_ticketingInEnabled = value;
}

bool CSASProtocolState::GetTicketingInEnabled()
{
	return m_ticketingInEnabled;
}

void CSASProtocolState::SetTicketingOutEnabled(bool value)
{
	m_ticketingOutEnabled = value;
}

bool CSASProtocolState::GetTicketingOutEnabled()
{
	return m_ticketingOutEnabled;
}

void CSASProtocolState::SetLastPrintedTicket(CTicket *ticket)
{
	if (m_lastPrintedTicket != nullptr)
	{
		delete m_lastPrintedTicket;
	}
	m_lastPrintedTicket = ticket;
}

CTicket *CSASProtocolState::GetLastPrintedTicket()
{
	return m_lastPrintedTicket;
}

void CSASProtocolState::SetValidationExtensions(bool value)
{
	m_bValidationExtensions = value;
}

bool CSASProtocolState::GetValidationExtensions()
{
	return m_bValidationExtensions;
}

void  CSASProtocolState::SetSupportMultiDenomExtensions(bool value)
{
	m_bSupportMultiDemonExtensions = value;
}

bool CSASProtocolState::GetSupportMultiDenomExtensions()
{
	return m_bSupportMultiDemonExtensions;
}

void CSASProtocolState::SetLockPending(bool value)
{
	m_bLockPending = value;
}

bool CSASProtocolState::GetLockPending()
{
	return m_bLockPending;
}

void CSASProtocolState::SetLastLockRequest(DWORD ticks)
{
	m_dwLastLockRequest = ticks;
}

DWORD CSASProtocolState::GetLastLockRequest()
{
	return m_dwLastLockRequest;
}

void CSASProtocolState::SetProbingPBTLock(bool lock)
{
	m_probingAftLock = lock;
}

bool CSASProtocolState::GetProbingPBTLock()
{
	return m_probingAftLock;
}

bool CSASProtocolState::CheckIfTransactionNeedsToBeSentForLockUnlockEgm(bool enableGame, bool ackedSuccessfully, TransactionType& transactionType)
{
	bool updateFloorgixOfEgmState(false);

	if (enableGame)
	{
		transactionType = ackedSuccessfully ? EGM_UNLOCK_ACK : EGM_UNLOCK_NACK;
	}
	else
	{
		transactionType = ackedSuccessfully ? EGM_LOCK_ACK : EGM_LOCK_NACK;
	}


	TransactionType oldlastLockUnlockTransactionType(m_lastLockUnlockTransactionType);
	m_lastLockUnlockTransactionType = transactionType;

	if (m_updateFloorgixOfEgmState)
	{
		updateFloorgixOfEgmState = (oldlastLockUnlockTransactionType != m_lastLockUnlockTransactionType);
		if (EGM_LOCK_ACK == transactionType || EGM_UNLOCK_ACK == transactionType)
		{
			m_updateFloorgixOfEgmState = false;
		}
	}

	return updateFloorgixOfEgmState;
}