#pragma once
#include "Log.h"
#include "../GlobalDefs.h"
#include "ScreenDefs.h"
#include "../PersonalBankerTransfer.h"
#include "../Card.h"

class CPersonalBankerLog : public CLog
{
public:
// #pragma region constructors
	/// <summary>
	/// PB transfer constructor for a CPersonalBankerLog instance.
	/// </summary>
	CPersonalBankerLog( CPersonalBankerTransfer *pbTransfer, PersonalBankerTransferType transferType, __int64 amount, const time_t dateTime, bool isPartial = false );

	/// <summary>
	/// NVRAM constructor for a CPersonalBankerLog instance.
	/// </summary>
	CPersonalBankerLog( byte *nvramBuffer );

	/// <summary>
	/// Copy constructor for a CPersonalBankerLog instance.
	/// </summary>
	CPersonalBankerLog( CPersonalBankerLog &pRHS );
// #pragma endregion constructors

// #pragma region destructor
	/// <summary>
	/// Destructor for a CPersonalBankerLog instance.
	/// </summary>
	~CPersonalBankerLog(void);
// #pragma endregion destructor

// #pragma region operators
	/// <summary>
	/// Assignment operator for a CPersonalBankerLog instance.
	/// </summary>
	CPersonalBankerLog &operator=( CPersonalBankerLog &pRHS );
// #pragma endregion operators

	int GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength );

	__int64 GetAmount( void )
	{
		return m_amount;
	}

	bool IsSuccessful( void )
	{
		return m_bSuccessful;
	}

	bool IsPartial(void)
	{
		return m_bPartial;
	}

	bool IsCashoutToHost(void)
	{
		return m_bCashoutToHost;
	}

	int GetSource( void )
	{
		return m_source;
	}

	std::string GetSourceString() const
	{
		std::string source;

		switch (m_source)
		{
		case CashToGame:
			source = TXT_PLAYER_CASH_TO_GAME;
			break;
		case PointsToGame:
			source = TXT_PLAYER_POINTS_TO_GAME;
			break;
		case PromoDollarsToGame:
			source = TXT_PLAYER_PROMO_TO_GAME;
			break;
		case CashFromGame:
			source = m_bCashoutToHost ? TXT_PLAYER_CASHOUT_CASH_FROM_GAME : 
				(m_bPartial ? TXT_PLAYER_PARTIAL_CASH_FROM_GAME : TXT_PLAYER_CASH_FROM_GAME);
			break; 
		case PointsFromGame:
			source = m_bCashoutToHost ? TXT_PLAYER_CASHOUT_POINTS_FROM_GAME :
				(m_bPartial ? TXT_PLAYER_PARTIAL_POINTS_FROM_GAME : TXT_PLAYER_POINTS_FROM_GAME);
			break;
		case PromoDollarsFromGame:
			source = m_bCashoutToHost ? TXT_PLAYER_CASHOUT_PROMOS_FROM_GAME : TXT_PLAYER_PROMO_FROM_GAME;
			break;
		case ExternalCredit:
			source = TXT_CREDIT_TRANSFER;
			break;
		default:
			break;
		}

		return source;
	}

	byte *GetCardID( int &length )
	{
		length = m_cardIDLen;
		return m_cardID;
	}

	char *GetTransID( int &length )
	{
		length = m_transIDLength;
		return m_transID;
	}
	
	int GetSequenceNumber( void )
	{
		return m_seqNum;
	}

	bool IsPending( void )
	{
		return m_bIsPending;
	}

	void SetIsPending( bool pending )
	{
		m_bIsPending = pending;
	}

	PollerCancellationType GetCancellationType( void )
	{
		return m_cancelCode;
	}
	

private:
	__int64 m_amount;
	bool m_bSuccessful;
	int m_source;
	int m_cardIDLen;
	byte *m_cardID;
	int m_transIDLength;
	char *m_transID;
	int m_seqNum;
	bool m_bIsPending;
	bool m_bPartial;
	bool m_bCashoutToHost;
	PollerCancellationType m_cancelCode;

};
