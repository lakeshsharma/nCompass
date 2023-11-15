#ifndef SENTINEL_REQUESTCARDCHANGE_H
#define SENTINEL_REQUESTCARDCHANGE_H

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"

class CCardChangeRequest: public CInternalMsgContent<INT_MSG_CONTENT_CARD_CHANGE_REQUEST>
{
public:
    CCardChangeRequest(const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Get correlation Id.
	/// </summary>
	/// <returns>string</returns>
	std::string GetCorrelationId() const;
	
	/// <summary>
	/// Get device type.
	/// </summary>
	/// <returns>string</returns>
    std::string GetDeviceType() const;
	
	/// <summary>
	/// Get device id.
	/// </summary>
	/// <returns>string</returns>
    std::string GetDeviceId() const;
	
	/// <summary>
	/// Get card number.
	/// </summary>
	/// <returns>string</returns>
    std::string GetCardNumber() const;
	
	/// <summary>
	/// Get change type field from message.
	/// </summary>
	/// <returns>string</returns>
    std::string GetChangeType() const;

private:
	std::string m_correlationId;

    std::string m_sDeviceType;
    std::string m_sDeviceId;
    std::string m_sCardNumber;
    std::string m_sChangeType;
};


#endif //SENTINEL_REQUESTCARDCHANGE_H
