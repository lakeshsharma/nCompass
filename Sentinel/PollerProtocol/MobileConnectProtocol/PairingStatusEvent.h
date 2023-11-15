#ifndef _PAIRING_STATUS_EVENT_H
#define _PAIRING_STATUS_EVENT_H

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"

class CPairingStatusEvent : public CInternalMsgContent<INT_MSG_CONTENT_PAIRING_STATUS_EVENT>
{
public:
    CPairingStatusEvent(bool isSuccess, std::string status);

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


#endif //_PAIRING_STATUS_EVENT_H
