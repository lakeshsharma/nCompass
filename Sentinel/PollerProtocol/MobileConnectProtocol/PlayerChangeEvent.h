#ifndef SENTINEL_PLAYERCHANGEEVENT_H
#define SENTINEL_PLAYERCHANGEEVENT_H

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"
#include "Player.h"

class CPlayerChangeEvent  :
        public CInternalMsgContent<INT_MSG_CONTENT_PLAYER_CHANGE_EVENT> {
public:
    CPlayerChangeEvent(
					   CPlayer * player,
                       int newPlayerSessionId,
                       std::string prevPlayerType,
                       std::string newPlayerType);
	
	~CPlayerChangeEvent();

public:
	/// <summary>
	/// Get Message Type.
	/// </summary>
	/// <returns>string</returns>
    const std::string &GetMessageType() const;
	
	/// <summary>
	/// Get PlayerId.
	/// </summary>
	/// <returns>DWORD</returns>
    DWORD GetPlayerId() const;
	
	/// <summary>
	/// Get CardId.
	/// </summary>
	/// <returns>string</returns>
    std::string GetCardId() const;
	
	/// <summary>
	/// Get New Player SessionId.
	/// </summary>
	/// <returns>int</returns>
    int GetNewPlayerSessionId() const;
	
	/// <summary>
	/// Get Prev PlayerType.
	/// </summary>
	/// <returns>string</returns>
    const std::string &GetPrevPlayerType() const;
	
	/// <summary>
	/// Get New Player Type.
	/// </summary>
	/// <returns>string</returns>
    const std::string &GetNewPlayerType() const;

private:
    std::string m_sMessageType;
    CPlayer * 	m_uPlayer;
    int m_uNewPlayerSessionId;
    std::string m_sPrevPlayerType;
    std::string m_sNewPlayerType;
};


#endif //SENTINEL_PLAYERCHANGEEVENT_H
