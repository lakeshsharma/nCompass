
#include "Logging/LogString.h"
#include "MobilePBTSessionInfo.h"
#include "../../PersonalBankerTransfer.h"

const std::string FROM_GAME("FromGame");
const std::string TO_GAME("ToGame");
const std::string SESSION_TIMEOUT("SessionTimeOut");
const std::string INVALID_PIN_LENGTH("InvalidPinLength");
const std::string AUTHORIZATION_DENIED("AuthorizationDenied");
const std::string SAS_ERROR("SASError");
const std::string SUCCESS("Success");
const std::string ERROR("Error");
const std::string CASH("Cash");
const std::string CASHABLE_PROMO("CashablePromo");
const std::string NON_CASHABLE_PROMO("NonCashablePromo");
const std::string ALL("All");
const std::string DENOM_MISMATCH("DenomMismatch");
const std::string INVALID_TRANSFER_TYPE("InvalidTransferType");
const std::string PBT_ALREADY_INPROGRESS("PbtAlreadyInProgress");
const std::string NO_MOBILE_SESSION("NoMobileSession");
const std::string INVALID_AMOUNT("InvalidAmount");
const std::string LOCK_NOT_AVAILABLE("LockNotAvaialble");
const std::string TRANSFER_AMOUNT_OVER_LIMIT("TransferAmountOverLimit");
const std::string EMPLOYEE_SESSION_INPROGRESS("EmployeeSessionInProgress");
const std::string PBT_DISABLED("PBTDisabled");

CMobilePBTSessionInfo& CMobilePBTSessionInfo::Instance() {
  static CMobilePBTSessionInfo instance;	 
  return instance;
}

CMobilePBTSessionInfo::CMobilePBTSessionInfo()
	: m_mobileTransferType(Undeclared),
	  m_mobileAmountInPennies(0),
	  m_mobileFundsTransferBalanceRequest(false),
	  m_mobileFundTransferRequest(false),
	  m_mobileDenomMismatch(false),
	  m_mobileSASError(false),
	  m_mobilePBSession(false),
	  m_cashable(0),
	  m_cashPromo(0),
	  m_nonCashablePromo(0),
	  m_isWallet_2_0_CompatiblePDS(true)
{}

PersonalBankerTransferType CMobilePBTSessionInfo::GetMobileTransferType() const
{
	return m_mobileTransferType;
}
void CMobilePBTSessionInfo::SetMobileTransferType(PersonalBankerTransferType type)
{
	m_mobileTransferType = type;
}

DWORD CMobilePBTSessionInfo::GetMobileAmountInPennies() const
{
	return m_mobileAmountInPennies;
}
void CMobilePBTSessionInfo::SetMobileAmountInPennies(DWORD amount)
{
	m_mobileAmountInPennies = amount;
}

bool CMobilePBTSessionInfo::IsMobileFundsTransferBalanceRequest() const
{
	return m_mobileFundsTransferBalanceRequest;
}
void CMobilePBTSessionInfo::SetMobileFundsTransferBalanceRequest(bool val)
{
	m_mobileFundsTransferBalanceRequest = val;
}

bool CMobilePBTSessionInfo::IsMobileFundTransferRequest() const
{
	return m_mobileFundTransferRequest;
}
void CMobilePBTSessionInfo::SetMobileFundTransferRequest(bool val)
{
	m_mobileFundTransferRequest = val;
}

bool CMobilePBTSessionInfo::IsMobilePBSession() const
{
	return m_mobilePBSession;
}

void CMobilePBTSessionInfo::SetMobilePBSession(bool val)
{
	m_mobilePBSession = val;
}

bool CMobilePBTSessionInfo::IsMobileDenomMismatch() const
{
	return m_mobileDenomMismatch;
}

void CMobilePBTSessionInfo::SetMobileDenomMismatch(bool val)
{
	m_mobileDenomMismatch = val;
}

bool CMobilePBTSessionInfo::IsMobileSASError() const
{
	return m_mobileSASError;
}
void CMobilePBTSessionInfo::SetMobileSASError(bool val)
{
	m_mobileSASError = val;
}

std::string CMobilePBTSessionInfo::GetMobilePin() const
{
	return m_mobilePin;
}
void CMobilePBTSessionInfo::SetMobilePin(std::string str)
{
	m_mobilePin = str;
}

std::string CMobilePBTSessionInfo::GetMobileFundType() const
{
	return m_mobileFundType;
}
void CMobilePBTSessionInfo::SetMobileFundType(std::string str)
{
	m_mobileFundType = str;
}
	
std::string CMobilePBTSessionInfo::GetMobileTransferDirection() const
{
	return m_mobileTransferDirection;
}
void CMobilePBTSessionInfo::SetMobileTransferDirection(std::string str)
{
	m_mobileTransferDirection = str;
}

void CMobilePBTSessionInfo::SetError(std::string err)
{
	m_errorMessage = err;
}

std::string CMobilePBTSessionInfo::GetError()
{
	return m_errorMessage;
}

void CMobilePBTSessionInfo::ClearError()
{
	m_errorMessage.clear();
}

void CMobilePBTSessionInfo::SetBalances(DWORD cashable, DWORD cashPromo, DWORD nonCashablePromo)
{
	m_cashable = cashable;
	m_cashPromo = cashPromo;
	m_nonCashablePromo = nonCashablePromo;
}

void CMobilePBTSessionInfo::ClearBalances()
{
	m_cashable = m_cashPromo = m_nonCashablePromo = 0;
}

DWORD CMobilePBTSessionInfo::GetCashableBalance()const
{
	return m_cashable;
}

DWORD CMobilePBTSessionInfo::GetCashPromoBalance()const
{
	return m_cashPromo;
}

DWORD CMobilePBTSessionInfo::GetNonCashablePromoBalance()const
{
	return m_nonCashablePromo;
}

void CMobilePBTSessionInfo::SetWallet_2_0_CompatiblePDS(bool flag)
{
	m_isWallet_2_0_CompatiblePDS = flag;
}

bool CMobilePBTSessionInfo::IsWallet_2_0_CompatiblePDS() const
{
	return m_isWallet_2_0_CompatiblePDS;
}

PersonalBankerTransferType CMobilePBTSessionInfo::ConvertToPBTransferType(std::string fundType, std::string transferDirection)
{
	LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);
	PersonalBankerTransferType ret = Undeclared;
	if (!transferDirection.compare(TO_GAME))
	{
		if(!fundType.compare(CASH))
			ret = CashToGame;
		else if (!fundType.compare(CASHABLE_PROMO))
			ret = PointsToGame;
		else if (!fundType.compare(NON_CASHABLE_PROMO))
			ret = PromoDollarsToGame;
	}
	else if (!transferDirection.compare(FROM_GAME))
	{
		if(!fundType.compare(CASH))
			ret = CashFromGame;
		else if (!fundType.compare(CASHABLE_PROMO))
			ret = PointsFromGame;
		else if (!fundType.compare(NON_CASHABLE_PROMO))
			ret = PromoDollarsFromGame;
		else if (!fundType.compare(ALL))
			ret = AllFromGame;
	}

    return ret;
}


