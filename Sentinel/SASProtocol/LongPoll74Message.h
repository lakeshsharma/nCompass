#pragma once
#include "SASPollMessage.h"

#define XFER_TO_GAME 1
#define XFER_FROM_GAME 2
#define XFER_TO_PRINTER 4
#define COMMAND_BONUS_TO_GAME 8
#define RESPONSE_BONUS_TO_GAME 16
#define WIN_AMOUNT_PENDING_CASHOUT_TO_HOST 8

#define CASHOUT_TO_HOST_HOST_CONTROLLED 1
#define	CASHOUT_TO_HOST_ENABLED 2
#define HOST_CASHOUT_MODE 4

#define PRINTER_AVAILABLE_RECEIPTS 1
#define TRANSFER_TO_HOST_PARTIAL_AMOUNT 2
#define CUSTOM_TICKET_DATA_SUPPORTED 4
#define AFT_REGISTERED 8
#define INHOUSE_TRANSFERS_ENABLED 16
#define BONUS_TRANSFERS_ENABLED 32
#define DEBIT_TRANSFERS_ENABLED 64
#define ANY_AFT_ENABLED 128

#define LOCK_CODE_REQUEST_LOCK 0
#define LOCK_CODE_GAME_LOCKED 0
#define LOCK_CODE_GAME_LOCK_PENDING 0x40
#define LOCK_CODE_CANCEL_PENDING 0x80
#define LOCK_CODE_QUERY_STATUS 0xff
#define LOCK_CODE_GAME_NOT_LOCKED 0xff


class CLongPoll74Message :
	public CSASPollMessage
{
public:
	CLongPoll74Message(byte gameAddress);
public:
	~CLongPoll74Message(void);
public:
	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

	void SetXferToGame(bool bEnabled);
	void SetXferFromGame(bool bEnabled);
	void SetXferToPrinter(bool bEnabled);
	void SetBonusToGame(bool bEnabled);
	
	void SetLockCode(byte lockCode)
	{
		m_lockCode = lockCode;
	}

	byte GetLockCode()
	{
		return m_lockCode;
	}

	void SetLockTimeout(WORD timeout)
	{
		m_lockTimeout = timeout;
	}

	// response variables
	DWORD m_assetNumber;
	byte m_gameLockStatus;
	byte m_availableTransfers;
	byte m_hostCashoutStatus;
	byte m_AFTStatus;
	byte m_maxBufferIndex;
	DWORD m_currentCashableAmount;
	DWORD m_currentRestrictedAmount;
	DWORD m_currentNonrestrictedAmount;
	DWORD m_egmTransferLimit;
	WORD m_restrictedExpiration;
	WORD m_poolID;

	// Response -- available transfers
	bool ResponseXferToGame();
	bool ResponseXferFromGame();
	bool ResponseXferToPrinter();
	bool ResponseWinAmountPendingCashoutToHost();
	bool ResponseBonusAwardToGamingMachine();

	// Response -- Host cashout status
	bool ResponseCashoutToHostControlledByHost();
	bool ResponseCashoutToHostEnabled();
	bool ResponseCashoutToHostCurrentlyHard();

	// Response -- AFT Status
	bool ResponseAFTStatusPrinterAvailable();
	bool ResponseAFTStatusTransferToHostPartialAmounts();
	bool ResponseAFTStatusCustomTicketDataSupported();
	bool ResponseAFTStatusAFTRegistered();
	bool ResponseAFTStatusInHouseTransfersEnabled();
	bool ResponseAFTStatusBonusTransfersEnabled();
	bool ResponseAFTStatusDebitTransfersEnabled();
	bool ResponseAFTStatusAnyAFTEnabled();
	
	
private:
	byte m_lockCode;
	byte m_transferCondition;
	WORD m_lockTimeout;
};
