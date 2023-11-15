#pragma once
#include "Log.h"
#include "../GlobalDefs.h"
#include "../Jackpot.h"

class CAttendantPaidPayoutLog : public CLog
{
public:
	CAttendantPaidPayoutLog( __int64 amount, JackpotTransactionType type );
	CAttendantPaidPayoutLog( byte *nvramBuffer);
	CAttendantPaidPayoutLog( CAttendantPaidPayoutLog &aRHS );

public:
	virtual ~CAttendantPaidPayoutLog(void);

	CAttendantPaidPayoutLog &operator=( CAttendantPaidPayoutLog &aRHS );
	int GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength );

	void SetJackpotType( JackpotTransactionType type )
	{
		m_type = type;
	}
	JackpotTransactionType GetJackpotType( void )
	{
		return m_type;
	}

	void SetAmount( __int64 amount )
	{
		m_amount = amount;
	}
	__int64 GetAmount( void )
	{
		return m_amount;
	}

private:
	JackpotTransactionType m_type;
	__int64 m_amount;
};