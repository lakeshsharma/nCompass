#ifndef SENTINEL_CCARDCHANGEEVENT_H
#define SENTINEL_CCARDCHANGEEVENT_H

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"

class CCardChangeEvent  :
        public CInternalMsgContent<INT_MSG_CONTENT_CARD_CHANGE_EVENT>
{
public:
    CCardChangeEvent(std::string status, bool isSuccess, std::string correlationId);
	
	/// <summary>
	/// Get message type.
	/// </summary>
	/// <returns>string</returns>
    const std::string& GetMessageType() const;
		
	/// <summary>
	/// Get status text.
	/// </summary>
	/// <returns>string</returns>
    const std::string& GetStatusText() const;
		
	/// <summary>
	/// Get IsSuccess field from message.
	/// </summary>
	/// <returns>bool</returns>
    bool GetIsSuccess() const;
		
	/// <summary>
	/// Get correlation Id.
	/// </summary>
	/// <returns>bool</returns>
	std::string GetCorrelationId() const;

private:
    std::string m_sMessageType;
    std::string m_sStatusText;
    bool m_bSuccess;
	std::string m_correlationId;
};


#endif //SENTINEL_CCARDCHANGEEVENT_H
