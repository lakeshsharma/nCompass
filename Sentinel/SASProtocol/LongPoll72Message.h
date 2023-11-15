#pragma once
#include "SASPollMessage.h"
#include "../Ticket.h"


#define TRANSFER_REQUEST_FULL_ONLY 0
#define TRANSFER_REQUEST_PARTIAL_ALLOWED 1
#define TRANSFER_REQUEST_CANCEL_REQUEST 0x80
#define TRANSFER_REQUEST_STATUS 0xff

#define TRANSFER_FLAG_HOST_CASHOUT_ENABLE_CONTROL 0
#define TRANSFER_FLAG_HOST_CASHOUT_ENABLE 1
#define TRANSFER_FLAG_HOST_CASHOUT_MODE 2
#define TRANSFER_FLAG_CASHOUT_FROM_EGM_REQUEST 3
#define TRANSFER_FLAG_USE_CUSTOM_TICKET_DATA 5
#define TRANSFER_FLAG_ACCEPT_TRANSFER_ONLY_IF_LOCKED 6
#define TRANSFER_FLAG_TRANSACTION_RECEIPT_REQUEST 7

#define TRANSFER_STATUS_PENDING 0x40
#define TRANSFER_STATUS_FULL_SUCCESSFUL 0
#define TRANSFER_STATUS_PARTIAL_SUCCESSFUL 1
#define TRANSFER_STATUS_NO_INFO 0xff;


class CLongPoll72Message :
	public CSASPollMessage
{
public:
	CLongPoll72Message(byte gameAddress);


public:
	~CLongPoll72Message(void);
public:
	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

	void SetStatusQuery()
	{
		m_statusQuery = true;
	}

	void ClearStatusQuery()
	{
		m_statusQuery = false;
	}

	void SetTransferCode(byte transferCode);
	byte GetTransferCode();
	void SetTransferType(byte transferType);
	byte GetTransferType();
	void SetCashableAmount(DWORD cashableAmount);
	DWORD GetCashableAmount();
	void SetRestrictedAmount(DWORD restrictedAmount);
	DWORD GetRestrictedAmount();
	void SetNonrestrictedAmount(DWORD nonrestrictedAmount);
	DWORD GetNonrestrictedAmount();

	void SetTransferFlag(byte bit, bool val);
	bool GetTransferFlag(byte bit);
	void SetAssetNumber(DWORD assetNumber);
	DWORD GetAssetNumber();
	//void SetRegistrationKey(byte *registrationKey);
	void SetTransactionID(byte *transactionID, byte length);

	void SetExpiration(WORD days);
	WORD GetExpiration();
	void SetPoolID(WORD poolID);
	WORD GetPoolID();
	void SetReceiptData(byte *receiptData, byte length);

	// response fields
	byte m_transferStatus;
	byte m_receiptStatus;
	DWORD m_cashableAmount;
	DWORD m_restrictedAmount;
	DWORD m_nonrestrictedAmount;
	DWORD m_cumulativeCashableAmountMeter;
	DWORD m_cumulativeRestrictedAmountMeter;
	DWORD m_cumulativeNonrestrictedAmountMeter;
	DWORD m_assetNumber;
	time_t m_transactionDate;
	byte m_transactionIDLength;
	byte m_transactionID[20];
	WORD m_expiration; 
	WORD m_expirationDays;
	WORD m_poolID;
	byte m_transferFlags;

private:
	bool m_statusQuery;
	byte m_transferCode;
	byte m_transferType;
	//byte m_registrationKey[20]; not using registration
	byte m_receiptDataLength;
	byte m_receiptData[22];
};
