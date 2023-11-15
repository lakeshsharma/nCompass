#pragma once

#include "Log.h"
#include "../PollerProtocol/PollerMessage.h"

class CTransactionLog : public CLog
{
public:
	CTransactionLog( TransactionType msgType, __int64 amount, time_t time );
	CTransactionLog( byte *nvramBuffer);
	CTransactionLog( CTransactionLog &tRHS );
public:
	~CTransactionLog(void);

	CTransactionLog &operator=( CTransactionLog &tRHS );

	int GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength );

	__int64 GetAmount( void )
	{
		return m_amount;
	}

	TransactionType GetTransactionType( void )
	{
		return m_transactionType;
	}

private:
	__int64 m_amount;
	TransactionType m_transactionType;
};
