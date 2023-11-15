#include "PlayerChangeEvent.h"
#include "MobileConnectProtocolKeys.h"

CPlayerChangeEvent::CPlayerChangeEvent(CPlayer * player, int sessionId, string prevPlayerType, string playerType):
 m_sMessageType(ToPlayerDeviceService::pEventPlayerChange),
 m_uPlayer(player),
 m_uNewPlayerSessionId(sessionId),
 m_sPrevPlayerType(prevPlayerType),
 m_sNewPlayerType(playerType)
{
}

const std::string &CPlayerChangeEvent::GetMessageType() const {
    return m_sMessageType;
}

DWORD CPlayerChangeEvent::GetPlayerId() const {
    return ((m_uPlayer) ? (m_uPlayer->GetPlayerInfo()->GetPlayerId()) : (0));
}

std::string CPlayerChangeEvent::GetCardId() const {
    return ((m_uPlayer) ? (m_uPlayer->GetCard()->MakeCStringCardNumber(m_uPlayer->GetCard())) : string());
}

int CPlayerChangeEvent::GetNewPlayerSessionId() const {
    return m_uNewPlayerSessionId;
}

const std::string &CPlayerChangeEvent::GetPrevPlayerType() const {
    return m_sPrevPlayerType;
}

const std::string &CPlayerChangeEvent::GetNewPlayerType() const {
    return m_sNewPlayerType;
}

CPlayerChangeEvent::~CPlayerChangeEvent()
{
	delete m_uPlayer; 
}