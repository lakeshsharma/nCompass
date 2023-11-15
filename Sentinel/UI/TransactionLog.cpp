#include "stdafx.h"
#include "TransactionLog.h"
#include "../Utilities.h"

CTransactionLog::CTransactionLog( TransactionType msgType, __int64 amount, time_t time )
{
	m_transactionType = msgType;

	m_amount = amount;

	m_dateTime = time;
}

CTransactionLog::~CTransactionLog(void)
{
}

CTransactionLog::CTransactionLog( CTransactionLog &tRHS )
{
	*this = tRHS;
}

CTransactionLog & CTransactionLog::operator=( CTransactionLog &tRHS )
{	
	m_dateTime = tRHS.GetDateTime();
	m_amount = tRHS.GetAmount();
	m_transactionType = tRHS.GetTransactionType();			

	return *this;

}

CTransactionLog::CTransactionLog( byte *nvramBuffer)
{
	byte *bufferPos = nvramBuffer;

	m_dateTime = CUtilities::MakeTimeFromNvramBuffer( bufferPos, NVRAM_TRANSACTION_LOG_DATA_SIZE );
	bufferPos += NVRAM_TIME_SIZE;

	memcpy( &m_amount, bufferPos, sizeof( m_amount ) );
	bufferPos += sizeof( m_amount );

	memcpy( &m_transactionType, bufferPos, sizeof( m_transactionType ) );
}

int CTransactionLog::GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength )
{
	byte *bufferPos = nvramBuffer;

	//Load nvramBuffer with relevant data
	if ( nvramBufferLength >= (int)NVRAM_TRANSACTION_LOG_DATA_SIZE )
	{
		CUtilities::GetNvramBufferFromTime( m_dateTime, bufferPos, NVRAM_TIME_SIZE );
		bufferPos += NVRAM_TIME_SIZE;

		memcpy( bufferPos, &m_amount, sizeof( m_amount ) );
		bufferPos += sizeof( m_amount );

		memcpy( bufferPos, &m_transactionType, sizeof( m_transactionType ) );
		bufferPos += sizeof( m_transactionType );
	}
	
	return (bufferPos - nvramBuffer);
}
