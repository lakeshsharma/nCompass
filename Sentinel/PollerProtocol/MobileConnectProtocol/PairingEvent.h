#ifndef _PAIRINGEVENT_H
#define _PAIRINGEVENT_H

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"

class CPairingEvent: public CInternalMsgContent<INT_MSG_CONTENT_PAIRING_EVENT> {
public:
    CPairingEvent(std::string msgType,
                  std::string deviceType,
                  std::string deviceId,
                  std::string pairingType,
                  std::string pairingId,
                  std::string pairingChecksum,
                  bool success,
                  std::string status);

    CPairingEvent(const rapidjson::Document &jsonDoc);
	
	/// <summary>
	/// Get Device Type.
	/// </summary>
	/// <returns>string</returns>
	const std::string& GetDeviceType() const;
	
	/// <summary>
	/// Get DeviceId.
	/// </summary>
	/// <returns>string</returns>
	const std::string& GeDeviceId() const;
	
	/// <summary>
	///Get Pairing Type.
	/// </summary>
	/// <returns>string</returns>
	const std::string& GetPairingType() const;
	
	/// <summary>
	/// Get PairingId.
	/// </summary>
	/// <returns>string</returns>
	const std::string& GetPairingId() const;
	
    /// <summary>
	/// Get Pairing checksum string.
	/// </summary>
	/// <returns>string</returns>
	const std::string& GetPairingChecksum() const;

	/// <summary>
	/// Get Status.
	/// </summary>
	/// <returns>string</returns>
	const std::string& GetStatus() const;
	
	/// <summary>
	/// Get IsSuccess value.
	/// </summary>
	/// <returns>string</returns>
	bool GetIsSuccess() const;
	

private:
    std::string m_sMsgType;
    std::string m_sDeviceType;
    std::string m_sDeviceId;
    std::string m_sPairingType;
    std::string m_sPairingId;
    std::string m_sPairingChecksum;
    bool m_bSuccess;
    std::string m_sStatus;
};

#endif //_PAIRINGEVENT_H
