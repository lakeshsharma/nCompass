#pragma once

#include "Hardware/NonVolatileMemoryMaps.h"
#include "InternalMsgContent.h"
static const DWORD MAX_PLAYER_GB_CARD_NUMBER_LENGTH = 20;

class CPlayerGlobalBalance :
	public CInternalMsgContent<INT_MSG_CONTENT_GLOBALBALANCE>
{
public:
	CPlayerGlobalBalance();
	CPlayerGlobalBalance( CPlayerGlobalBalance &piRHS );
	CPlayerGlobalBalance( byte *nvramStream );

public:
	~CPlayerGlobalBalance();

	CPlayerGlobalBalance &operator=( CPlayerGlobalBalance &piRHS ); 

	int GetNvramBuffer( byte *aNvramBuffer, int aNvramBufferLength );

	void SetCardNumber( byte *aCardNumber, int aCardNumberLen );
	byte *GetCardNumber( int &aCardNumberLen );

    void PutTotalRedeemablePoints( LONGLONG aTotalRedeemablePoints )
    { 
        m_totalRedeemablePoints = aTotalRedeemablePoints;
    }

    void PutTotalRedeemableComps( LONGLONG aTotalRedeemableComps )
    { 
        m_totalRedeemableComps = aTotalRedeemableComps;
    }

    void PutTotalPoints( LONGLONG aTotalPoints )
    { 
        m_totalPoints = aTotalPoints;
    }

    void PutTotalComps( LONGLONG aTotalComps )
    { 
        m_totalComps = aTotalComps;
    }

    LONGLONG GetTotalRedeemablePoints(){ return m_totalRedeemablePoints; }
    LONGLONG GetTotalRedeemableComps (){ return m_totalRedeemableComps ; }

	/// <summary>
	/// Returns the total points.
	/// </summary>
	/// <returns>The total points.</returns>
    LONGLONG GetTotalPoints() const;

    LONGLONG GetTotalComps (){ return m_totalComps ; }

private:
	byte *m_cardNumber;
	int m_cardNumberLen;

   	LONGLONG m_totalRedeemablePoints;
	LONGLONG m_totalRedeemableComps;
	LONGLONG m_totalPoints;
	LONGLONG m_totalComps;

};
