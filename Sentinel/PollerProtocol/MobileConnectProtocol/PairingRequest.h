#ifndef _PAIRING_REQUEST_H
#define _PAIRING_REQUEST_H

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"
#include "MobileConnectProtocolKeys.h"

class CPairingRequest  :
        public CInternalMsgContent<INT_MSG_CONTENT_PAIRING_REQUEST>
{
public:
    CPairingRequest(std::string msgType = ToPlayerDeviceService::pRequestPairing);
	
	/// <summary>
	/// Get Message Type.
	/// </summary>
	/// <returns>string</returns>
    const std::string &GetMsgType() const;
	
	/// <summary>
	/// Set Message Type.
	/// </summary>
	/// <param name="msgType">IN - data.</param>
    void SetMsgType(const std::string &msgType);

private:
    std::string m_msgType;
};


#endif //SENTINEL_CPAIRINGREQUEST_H
