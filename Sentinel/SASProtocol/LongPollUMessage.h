#pragma once
#include "SASPollMessage.h"

#define XU_POLLSIZE 6
#define XU_RESPONSESIZE 12

class CLongPollUMessage :
	public CSASPollMessage
{
public:
	CLongPollUMessage(byte gameAddress);
public:
	~CLongPollUMessage(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

	void SetTransactionNumber(byte transactionNumber)
	{
		m_transNumber = transactionNumber;
	}

	byte GetTransactionNumber()
	{
		return m_transNumber;
	}

	void SetPollValue(byte longPoll)
	{
		m_longPoll = longPoll;
	}

	void SetAck(byte ack)
	{
		m_ack = ack;
	}

	byte GetAck()
	{
		return m_ack;
	}

	void SetAmount(DWORD amount)
	{
		m_transferAmount = amount;
	}

	DWORD GetAmount()
	{
		return m_transferAmount;
	}

	byte GetStatus()
	{
		return m_status;
	}



private:
	byte m_longPoll;
	byte m_ack;
	byte m_transNumber;
	DWORD m_transferAmount;
	byte m_status;



};

