#include "stdafx.h"
#include "PlayerGlobalBalance.h"
#include "Utilities.h"

CPlayerGlobalBalance::CPlayerGlobalBalance()
{
	m_cardNumber = NULL;
	m_cardNumberLen = 0;

    m_totalRedeemablePoints = 0;
	m_totalRedeemableComps = 0;
	m_totalPoints = 0;
	m_totalComps = 0;
}

CPlayerGlobalBalance::CPlayerGlobalBalance( CPlayerGlobalBalance &piRHS )
{
	m_cardNumber = NULL;
	m_cardNumberLen = 0;
	*this = piRHS;
}

CPlayerGlobalBalance::CPlayerGlobalBalance( byte *nvramBuffer)
{
	memcpy( &m_cardNumberLen, nvramBuffer, sizeof( m_cardNumberLen ) );
	nvramBuffer += sizeof( m_cardNumberLen );

	if( m_cardNumberLen > 0 ){
		m_cardNumber = new byte[ m_cardNumberLen ];
		memcpy( m_cardNumber, nvramBuffer, m_cardNumberLen );
	} else {
		m_cardNumber = NULL;
	}
	nvramBuffer += MAX_PLAYER_GB_CARD_NUMBER_LENGTH;

	memcpy( &m_totalRedeemablePoints, nvramBuffer, sizeof( m_totalRedeemablePoints ) );
	nvramBuffer += sizeof( m_totalRedeemablePoints );

	memcpy( &m_totalRedeemableComps, nvramBuffer, sizeof( m_totalRedeemableComps ) );
	nvramBuffer += sizeof( m_totalRedeemableComps );

	memcpy( &m_totalPoints, nvramBuffer, sizeof( m_totalPoints ) );
	nvramBuffer += sizeof( m_totalPoints );

	memcpy( &m_totalComps, nvramBuffer, sizeof( m_totalComps ) );
	nvramBuffer += sizeof( m_totalComps );
}

CPlayerGlobalBalance & CPlayerGlobalBalance::operator=( CPlayerGlobalBalance &piRHS )
{
	int cardNumLen;
	byte *cardNum = piRHS.GetCardNumber( cardNumLen );
	SetCardNumber( cardNum, cardNumLen );

    PutTotalRedeemablePoints( piRHS.GetTotalRedeemablePoints() );
    PutTotalRedeemableComps( piRHS.GetTotalRedeemableComps() );
    PutTotalPoints( piRHS.GetTotalPoints() );
    PutTotalComps( piRHS.GetTotalComps() );
    
	return *this;
}

int CPlayerGlobalBalance::GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength )
{
	byte *bufferPos = nvramBuffer;

	if( nvramBufferLength >= (int)NVRAM_PLAYER_GLOBAL_BALANCE_SIZE ){
		memcpy( bufferPos, &m_cardNumberLen, sizeof( m_cardNumberLen ) );
		bufferPos += sizeof( m_cardNumberLen );

		memcpy( bufferPos, m_cardNumber, m_cardNumberLen );
		bufferPos += MAX_PLAYER_GB_CARD_NUMBER_LENGTH;

		memcpy( bufferPos, &m_totalRedeemablePoints, sizeof( m_totalRedeemablePoints ) );
		bufferPos += sizeof( m_totalRedeemablePoints );

		memcpy( bufferPos, &m_totalRedeemableComps, sizeof( m_totalRedeemableComps ) );
		bufferPos += sizeof( m_totalRedeemableComps );

		memcpy( bufferPos, &m_totalPoints, sizeof( m_totalPoints ) );
		bufferPos += sizeof( m_totalPoints );

		memcpy( bufferPos, &m_totalComps, sizeof( m_totalComps ) );
		bufferPos += sizeof( m_totalComps );
    }

	return (bufferPos - nvramBuffer);
}

CPlayerGlobalBalance::~CPlayerGlobalBalance()
{
    if( m_cardNumber != NULL ){
		delete [] m_cardNumber;
    }
}

void CPlayerGlobalBalance::SetCardNumber( byte *cardNumber, int cardNumberLen )
{
    if( m_cardNumber != NULL ){
		delete [] m_cardNumber;
    }

	if( cardNumberLen > 0 && cardNumber != NULL ){
		m_cardNumber = new byte[cardNumberLen];
		memcpy( m_cardNumber, cardNumber, cardNumberLen );
		m_cardNumberLen = cardNumberLen;
	} else {
		m_cardNumber = NULL;
		m_cardNumberLen = 0;
	}

}

byte * CPlayerGlobalBalance::GetCardNumber( int &cardNumberLen )
{
	cardNumberLen = m_cardNumberLen;

	return m_cardNumber;
}

LONGLONG CPlayerGlobalBalance::GetTotalPoints() const
{
	return m_totalPoints;
}
