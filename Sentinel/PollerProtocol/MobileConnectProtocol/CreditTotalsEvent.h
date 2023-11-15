#ifndef _CREDIT_TOTALS_EVENT_H
#define _CREDIT_TOTALS_EVENT_H

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"
#include "../../PersonalBankerInfo.h"

class CFundsTransferBalanceEvent : public CInternalMsgContent<INT_MSG_CONTENT_FUNDS_TRANSFER_BALANCE_EVENT>
{
public:
    CFundsTransferBalanceEvent(DWORD cashable, DWORD cashablePromo, DWORD nonCashablePromo, 
							CPersonalBankerInfo *pbInfo, std::string error, std::string correlationId,
							bool isCashFunctionalityUnavailable);
	~CFundsTransferBalanceEvent();

	/// <summary>
	/// Get Message type.
	/// </summary>
	/// <returns>string</returns>
    const std::string& GetMessageType() const;
	
	/// <summary>
	/// Get Cashable.
	/// </summary>
	/// <returns>DWORD</returns>
    DWORD GetCashable() const;
		
	/// <summary>
	/// Get NonCashable Promo.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetNonCashablePromo() const;
		
	/// <summary>
	/// Get Cashable Promor.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetCashablePromo() const;
		
	/// <summary>
	/// Get correlation id.
	/// </summary>
	/// <returns>string</returns>
	std::string GetCorrelationId() const;

	/// <summary>
	/// Is Success.
	/// </summary>
	/// <returns>bool</returns>
	bool IsSuccess() const;
	
	/// <summary>
	/// Get Status .
	/// </summary>
	/// <returns>string</returns>
	std::string GetStatus() const;
	
	/// <summary>
	/// Are Some Properties Offline.
	/// </summary>
	/// <returns>bool</returns>
	bool AreSomePropertiesOffline() const;
	
	/// <summary>
	/// Is Banned From Redemption.
	/// </summary>
	/// <returns>bool</returns>
	bool IsBannedFromRedemption() const;

	/// <summary>
	/// Is partial transfer of cash from game is enabled.
	/// </summary>
	/// <returns>bool</returns>
	bool IsPartialTransferCashFromGameEnabled() const;
	
	/// <summary>
	/// Get Cash Balance.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetCashBalance() const;
	
	/// <summary>
	/// Get Promo Balance.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetPromoBalance() const;
	
	/// <summary>
	/// Get Points Available.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetPointsAvailable() const;
	
	/// <summary>
	/// Get Points Per Dollar.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetPointsPerDollar() const;
	
	/// <summary>
	/// Get Minimum Points.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetMinimumPoints() const;
	
	/// <summary>
	/// Get Maximum Points.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetMaximumPoints() const;
	
	/// <summary>
	/// Get Minimum CashDown.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetMinimumCashDown() const;
	
	/// <summary>
	/// Get Maximum CashDown.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetMaximumCashDown() const;
	
	/// <summary>
	/// Get Minimum PromoDown.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetMinimumPromoDown() const;
	
	/// <summary>
	/// Get Maximum PromoDown.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetMaximumPromoDown() const;
	
	/// <summary>
	/// Checks if cash account is inactive.
	/// </summary>
	/// <returns>true if cash is inactive, false otherwise.</returns>
	bool IsCashInactive() const;

	/// <summary>
	/// Gets if cash functionality is unavailable.
	/// </summary>
	/// <returns>true if cash functionality is unavailable, false otherwise.</returns>
	bool IsCashFunctionalityUnavailable() const;

private:
	std::string m_sMsgType;
	DWORD m_cashable;
	DWORD m_cashablePromo;
	DWORD m_nonCashablePromo;
	const CPersonalBankerInfo *m_pbInfo;
	std::string m_error;

	std::string m_correlationId;
	bool m_isCashFunctionalityUnavailable;

	static const DWORD AmountZero = 0;
};


#endif //_CREDIT_TOTALS_EVENT_H