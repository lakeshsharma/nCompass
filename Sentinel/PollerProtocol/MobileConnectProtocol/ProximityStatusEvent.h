#ifndef _PROXIMITY_STATUS_EVENT_H
#define _PROXIMITY_STATUS_EVENT_H

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"

extern const std::string PROXIMITY_ID_SET;
extern const std::string PROXIMITY_ID_SET_INACTIVE;
extern const std::string BEACON_ERROR;

class CProximityStatusEvent : public CInternalMsgContent<INT_MSG_CONTENT_PROXIMITY_STATUS_EVENT>
{
public:
    CProximityStatusEvent(bool isSuccess, std::string status);

public:
	/// <summary>
	/// Get Message Type.
	/// </summary>
	/// <returns>string</returns>
    const std::string &GetMsgType() const;
	
	/// <summary>
	/// Is Success.
	/// </summary>
	/// <returns>bool</returns>
    bool IsSuccess() const;
	
	/// <summary>
	/// Get Status.
	/// </summary>
	/// <returns>string</returns>
    const std::string &GetStatus() const;

private:
    std::string m_sMsgType;
    bool m_bSuccess;
    std::string m_sStatus;

};

#endif //_PROXIMITY_STATUS_EVENT_H
