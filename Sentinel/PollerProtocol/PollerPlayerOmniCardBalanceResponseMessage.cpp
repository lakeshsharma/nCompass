#include "stdafx.h"
#include "PollerPlayerOmniCardBalanceResponseMessage.h"

CPollerPlayerOmniCardBalanceResponseMessage::CPollerPlayerOmniCardBalanceResponseMessage( byte *response)
{
	m_lpBuffer = NULL;
	
	// parse response, fill in m_playerResponse;
	byte offset = 0;
	memcpy( m_playerResponse.card_id, response + offset, sizeof( m_playerResponse.card_id ) );
	offset += sizeof( m_playerResponse.card_id );

	memcpy( &m_playerResponse.totalRedeemablePoints, response + offset, sizeof( m_playerResponse.totalRedeemablePoints ) );
	offset += sizeof( m_playerResponse.totalRedeemablePoints );

	memcpy( &m_playerResponse.totalRedeemableComps, response + offset, sizeof( m_playerResponse.totalRedeemableComps ) );
	offset += sizeof( m_playerResponse.totalRedeemableComps );

	memcpy( &m_playerResponse.totalPoints, response + offset, sizeof( m_playerResponse.totalPoints ) );
	offset += sizeof( m_playerResponse.totalPoints );

	memcpy( &m_playerResponse.totalComps, response + offset, sizeof( m_playerResponse.totalComps ) );
}

CPollerPlayerOmniCardBalanceResponseMessage::~CPollerPlayerOmniCardBalanceResponseMessage()
{

}

PlayerOmniCardBalanceResponse CPollerPlayerOmniCardBalanceResponseMessage::GetPlayerResponse()
{
	return m_playerResponse;
}
