

#ifndef _MOBILE_PBT_SESSION_INFO_H
#define _MOBILE_PBT_SESSION_INFO_H


#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"
#include "../../PersonalBankerTransfer.h"

extern const std::string FROM_GAME;
extern const std::string TO_GAME;
extern const std::string SESSION_TIMEOUT;
extern const std::string INVALID_PIN_LENGTH;
extern const std::string AUTHORIZATION_DENIED;
extern const std::string SAS_ERROR;
extern const std::string SUCCESS;
extern const std::string ERROR;
extern const std::string CASH;
extern const std::string CASHABLE_PROMO;
extern const std::string NON_CASHABLE_PROMO;
extern const std::string ALL;
extern const std::string DENOM_MISMATCH;
extern const std::string INVALID_TRANSFER_TYPE;
extern const std::string PBT_ALREADY_INPROGRESS;
extern const std::string NO_MOBILE_SESSION;
extern const std::string INVALID_AMOUNT;
extern const std::string LOCK_NOT_AVAILABLE;
extern const std::string TRANSFER_AMOUNT_OVER_LIMIT;
extern const std::string EMPLOYEE_SESSION_INPROGRESS;
extern const std::string PBT_DISABLED;

// Forward declare class to declare friend for unit test access.
namespace SentinelNativeUnitTests { class MobilePBTSessionInfo; }

class CMobilePBTSessionInfo
{
public:
	/// <summary>
	/// Returns CMobilePBTSessionInfo instance(singleton).
	/// </summary>
	/// <returns>CMobilePBTSessionInfo</returns>
	static CMobilePBTSessionInfo& Instance();
	
	/// <summary>
	/// Returns Mobile Transfer Type.
	/// </summary>
	/// <returns>PersonalBankerTransferType</returns>
	PersonalBankerTransferType GetMobileTransferType() const;
	
	/// <summary>
	/// Set Returns Mobile Transfer Type.
	/// </summary>
	/// <param name="PersonalBankerTransferType">IN - type.</param>
	void SetMobileTransferType(PersonalBankerTransferType);
	
	/// <summary>
	/// Get Mobile AmountInPennies.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetMobileAmountInPennies() const;
	
	/// <summary>
	/// Set Mobile AmountInPennies.
	/// </summary>
	/// <param name="amount">IN - amount.</param>
	void SetMobileAmountInPennies(DWORD amount);
	
	/// <summary>
	/// Is Mobile Funds Transfer Balance Request.
	/// </summary>
	/// <returns>bool</returns>
	bool IsMobileFundsTransferBalanceRequest() const;
	
	/// <summary>
	/// Set Mobile Funds Transfer Balance Request.
	/// </summary>
	/// <param name="bool">IN - data.</param>
	void SetMobileFundsTransferBalanceRequest(bool);
	
	/// <summary>
	/// Is Mobile Fund Transfer Request.
	/// </summary>
	/// <returns>bool</returns>
	bool IsMobileFundTransferRequest() const;
	
	/// <summary>
	/// Set Mobile Fund Transfer Request.
	/// </summary>
	/// <param name="bool">IN - data.</param>
	void SetMobileFundTransferRequest(bool);
	
	/// <summary>
	/// Is Mobile Denom Mismatch.
	/// </summary>
	/// <returns>bool</returns>
	bool IsMobileDenomMismatch() const;
	
	/// <summary>
	/// Set Mobile Denom Mismatch.
	/// </summary>
	/// <param name="bool">IN - data.</param>
	void SetMobileDenomMismatch(bool);

	/// <summary>
	/// Is Mobile SAS Error.
	/// </summary>
	/// <returns>bool</returns>
	bool IsMobileSASError() const;
	
	/// <summary>
	/// Set Mobile SAS Error.
	/// </summary>
	/// <param name="bool">IN - data.</param>
	void SetMobileSASError(bool val);
	
	/// <summary>
	/// Is Mobile PB Session.
	/// </summary>
	/// <returns>bool</returns>
	bool IsMobilePBSession() const;

	/// <summary>
	/// Set Mobile PB Session.
	/// </summary>
	/// <param name="bool">IN - data.</param>
	void SetMobilePBSession(bool val);
	
	/// <summary>
	/// Get Mobile Pin.
	/// </summary>
	/// <returns>string</returns>
	std::string GetMobilePin() const;
	
	/// <summary>
	/// Set Mobile Pin.
	/// </summary>
	/// <param name="string">IN - data.</param>
	void SetMobilePin(std::string);
	
	/// <summary>
	/// Get Mobile Fund Type.
	/// </summary>
	/// <returns>string</returns>
	std::string GetMobileFundType() const;
	
	/// <summary>
	/// Set Mobile Fund Type.
	/// </summary>
	/// <param name="string">IN - event data.</param>
	void SetMobileFundType(std::string);
	
	/// <summary>
	/// Get Mobile Transfer Direction.
	/// </summary>
	/// <returns>string</returns>
	std::string GetMobileTransferDirection() const;
	
	/// <summary>
	/// Set Mobile Transfer Direction.
	/// </summary>
	/// <param name="string">IN - data.</param>
	void SetMobileTransferDirection(std::string);
	
	/// <summary>
	/// Set Error.
	/// </summary>
	/// <param name="string">IN - data.</param>
	void SetError(std::string err);
	
	/// <summary>
	/// Get Error.
	/// </summary>
	/// <returns>string</returns>
	std::string GetError();
	
	/// <summary>
	/// Clear Error.
	/// </summary>
	void ClearError();
	
	/// <summary>
	/// Set Balances.
	/// </summary>
	/// <param name="DWORD">IN - data.</param>
	/// <param name="DWORD">IN - data.</param>
	/// <param name="DWORD">IN - data.</param>
	void SetBalances(DWORD cashable, DWORD cashPromo, DWORD nonCashablePromo);
		
	/// <summary>
	/// Clear Balances.
	/// </summary>
	void ClearBalances();
		
	/// <summary>
	/// Get Cashable Balance.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetCashableBalance()const;
	
	/// <summary>
	/// Get Cash Promo Balance.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetCashPromoBalance()const;
		
	/// <summary>
	/// Get NonCashable Promo Balance.
	/// </summary>
	/// <returns>DWORD</returns>
	DWORD GetNonCashablePromoBalance()const;

	/// <summary>
	/// Set wallet 2.0 compatible PDS flag.
	/// </summary>
	/// <param name="flag">IN - data.</param>
	void SetWallet_2_0_CompatiblePDS(bool flag);

	/// <summary>
	/// Returns whether PDS is wallet 2.0 compatible or not.
	/// </summary>
	/// <returns>bool</returns>
	bool IsWallet_2_0_CompatiblePDS() const;

	/// <summary>
	/// Convert To PBTransferType.
	/// </summary>
	/// <param name="fundType">IN - fund Type.</param>
	/// <param name="transferDirection">IN - transfer Direction.</param>
	/// <returns>PersonalBankerTransferType</returns>
	PersonalBankerTransferType ConvertToPBTransferType(std::string fundType, std::string transferDirection);

	// forbid copy, move and assignment
    CMobilePBTSessionInfo(const CMobilePBTSessionInfo&) = delete;
    CMobilePBTSessionInfo& operator =(const CMobilePBTSessionInfo&) = delete;
    CMobilePBTSessionInfo(CMobilePBTSessionInfo&&) = delete;
    CMobilePBTSessionInfo& operator =(CMobilePBTSessionInfo&&) = delete;
	
protected:
    CMobilePBTSessionInfo();

private:
	PersonalBankerTransferType m_mobileTransferType;
	DWORD m_mobileAmountInPennies;
	bool m_mobileFundsTransferBalanceRequest;
	bool m_mobileFundTransferRequest;
	bool m_mobileDenomMismatch;
	bool m_mobileSASError;
	bool m_mobilePBSession;
	std::string m_mobilePin;
	std::string m_mobileFundType;
	std::string m_mobileTransferDirection;
	std::string m_errorMessage;
	DWORD m_cashable;
	DWORD m_cashPromo;
	DWORD m_nonCashablePromo;
	bool m_isWallet_2_0_CompatiblePDS;

	//friend class declarations for unit test access.
	friend class SentinelNativeUnitTests::MobilePBTSessionInfo;
};


#endif //_MOBILE_PBT_SESSION_INFO_H
