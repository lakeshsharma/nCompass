#ifndef _FUNDS_TRANSFER_EVENT_H
#define _FUNDS_TRANSFER_EVENT_H

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"

class CFundsTransferEvent : public CInternalMsgContent<INT_MSG_CONTENT_FUNDS_TRANSFER_EVENT>
{
public:
    CFundsTransferEvent(std::string pin, std::string fundType, std::string transferDirection, bool success, std::string status,
		DWORD cashable, DWORD cashablePromo, DWORD nonCashablePromo, std::string correlationId);

public:
	/// <summary>
	/// Returns mesage type.
	/// </summary>
	/// <returns>string</returns>
    const std::string& GetMessageType() const;
	
	/// <summary>
	/// Returns Pin.
	/// </summary>
	/// <returns>string</returns>
	const std::string& GetPin() const;
	
	/// <summary>
	/// Returns Fund Type.
	/// </summary>
	/// <returns>string</returns>
	const std::string& GetFundType() const;
	
	/// <summary>
	/// Returns Transfer Direction.
	/// </summary>
	/// <returns>string</returns>
	const std::string& GetTransferDirection() const;
	
	/// <summary>
	/// Returns 'IsSuccess' value.
	/// </summary>
	/// <returns>string</returns>
	bool IsSuccess() const;
	
	/// <summary>
	/// Returns status.
	/// </summary>
	/// <returns>string</returns>
	const std::string& GetStatus() const;
	
	/// <summary>
	/// Returns Cashable.
	/// </summary>
	/// <returns>string</returns>
    DWORD GetCashable() const;
	
	/// <summary>
	/// Returns NonCashablePromo.
	/// </summary>
	/// <returns>string</returns>
	DWORD GetNonCashablePromo() const;
	
	/// <summary>
	/// Returns CashablePromo.
	/// </summary>
	/// <returns>string</returns>
	DWORD GetCashablePromo() const;
	
	/// <summary>
	/// Returns CorrelationId.
	/// </summary>
	/// <returns>string</returns>
	std::string GetCorrelationId() const;

private:
	std::string m_messageType;
	std::string m_pin;
	std::string m_fundType;
	std::string m_transferDirection; 
	bool m_success; 
	std::string m_status;
	DWORD m_cashable;
	DWORD m_cashablePromo;
	DWORD m_nonCashablePromo;

	std::string m_correlationId;
	
};


#endif //_FUNDS_TRANSFER_EVENT_H