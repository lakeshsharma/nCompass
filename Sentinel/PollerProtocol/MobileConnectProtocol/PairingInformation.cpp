
#include "Logging/LogString.h"
#include "PollerProtocol/MobileConnectProtocol/PairingEvent.h"
#include "PairingInformation.h"

static const DWORD BUFSIZE = 32;
const std::string BEACON_DISCONNECTED("Disconnected");
const std::string BEACON_IDLE_SET("IdleIdSet");
const std::string BEACON_IDLE_NOT_SET("NotAbleToSetIdleId");

const std::string CARD_INSERT("Insert");
const std::string CARD_REMOVE("Remove");
const std::string EXISTING_EMPLOYEE_SESSION("ExistingEmployeeSession");
const std::string EXISTING_PLAYER_SESSION("ExisingPlayerSession");
const std::string PLAYER_NOT_CONFIRMED("PlayerNotConfirmed");
const std::string ALREADY_INSERTED("AlreadyInserted");
const std::string UNKNOWN_CARD("UnknownCard");
const std::string SHORT_CODE("ShortCode");
const std::string PAIRING_ID_SET("PairingIDSet");
const std::string SESSION_IN_PROGRESS("SessionInProgress");
const std::string GAME_NOT_ENABLED("GameNotEnabled");
const std::string BEACON_FAILURE("BeaconFailure");
const std::string ERROR_STATUS("Error");
const std::string PAIRING_TYPE_MISMATCH("PairingTypeMismatch");
const std::string BEACON("Beacon");

CPairingInformation& CPairingInformation::Instance() {
	static CPairingInformation instance;	 
	return instance;
}

CPairingInformation::CPairingInformation()
	: m_bPairingEnabled(CPairingInformation::DEFAULT_PAIRING_ENABLED),
	m_uPairingTimeOutMs(CPairingInformation::DEFAULT_PAIRING_TIMEOUT_MS),
	m_waitForPairing(false),
	m_waitForRequestCardChange(false),
	m_fallbackEnabled(false),
	m_mobileCardedSession(false),
	m_measuredPower(CPairingInformation::DEFAULT_MEASURED_POWER)
{}


void CPairingInformation::UpdateFromPairingSettingsEvent(CPairingSettingsEvent* pairingSettings)
{
	LogString(POLLER_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
	if(nullptr != pairingSettings)
	{
		m_sPairingType = pairingSettings->GetPairingType();
		m_bPairingEnabled = pairingSettings->IsPairingEnabled();
		m_uPairingTimeOutMs = pairingSettings->GetPairingTimeOutMs();
		m_sIdlePairingId = pairingSettings->GetIdlePairingId();
		m_sIdlePairingChecksum = pairingSettings->GetIdlePairingChecksum();
		m_measuredPower = pairingSettings->GetMeasuredPower();
	}
}

const std::string &CPairingInformation::GetPairingType() const
{
    return m_sPairingType;
}

void CPairingInformation::SetPairingType(const std::string &pairingType)
{
    m_sPairingType = pairingType;
}

bool CPairingInformation::IsPairingEnabled() const
{
    return m_bPairingEnabled;
}

void CPairingInformation::SetPairingEnabled(bool pairingEnabled)
{
    m_bPairingEnabled = pairingEnabled;
}

uint32_t CPairingInformation::GetPairingTimeOutMs() const
{
    return m_uPairingTimeOutMs;
}

void CPairingInformation::SetPairingTimeOutMs(uint32_t pairingTimeOutMs)
{
    m_uPairingTimeOutMs = pairingTimeOutMs;
}

const std::string &CPairingInformation::GetIdlePairingId() const
{
    return m_sIdlePairingId;
}

void CPairingInformation::SetIdlePairingId(const std::string &idlePairingId)
{
    m_sIdlePairingId = idlePairingId;
}

const std::string &CPairingInformation::GetIdlePairingChecksum() const
{
    return m_sIdlePairingChecksum;
}

void CPairingInformation::SetIdlePairingChecksum(const std::string &idlePairingChecksum)
{
    m_sIdlePairingChecksum = idlePairingChecksum;
}

bool CPairingInformation::IsWaitingForPairing() const
{
	return m_waitForPairing;
}

void CPairingInformation::SetWaitingForPairing(bool val)
{
	m_waitForPairing = val;
}

void CPairingInformation::SetPairingRequestTime(time_t time)
{
	m_pairingRequestTime = time;
}

time_t CPairingInformation::GetPairingRequestTime()
{
	return m_pairingRequestTime;
}

bool CPairingInformation::IsWaitingForRequestCardChange() const
{
	return m_waitForRequestCardChange;
}

void CPairingInformation::SetWaitingForRequestCardChange(bool val)
{
	m_waitForRequestCardChange = val;
}

void CPairingInformation::SetRequestCardChangeTime(time_t time)
{
	m_requestCardChangeTime = time;
}

time_t CPairingInformation::GetRequestCardChangeTime()
{
	return m_requestCardChangeTime;
}

uint8_t CPairingInformation::GetMeasuredPower() const
{
	return m_measuredPower;
}

bool CPairingInformation::CheckMobilePairingTimeOut()
{
	time_t currentTime = CUtilities::GetCurrentTime();
	bool ret = false;
	
	if (IsWaitingForPairing() && 
			((currentTime - GetPairingRequestTime()) > GetPairingTimeOutMs()/MILLISECONDS_PER_SECOND))
	{
		ret = true;
	}
	return ret;
}

bool CPairingInformation::CheckMobileRequestCardChangeTimeOut()
{
	time_t currentTime = CUtilities::GetCurrentTime();
	bool ret = false;
	
	if (IsWaitingForRequestCardChange() && 
			((currentTime - GetRequestCardChangeTime()) > GetPairingTimeOutMs()/MILLISECONDS_PER_SECOND))
	{
		ret = true;
	}
	return ret;
}

void CPairingInformation::SetCorrelationId(MessageTypeForCorrelation type, std::string correlation)
{
	m_correlationId[type] = correlation;
}

std::string CPairingInformation::GetCorrelationId(MessageTypeForCorrelation type)
{
	return m_correlationId[type];
}

bool CPairingInformation::IsFallbackEnabled()
{
	return m_fallbackEnabled;
}

void CPairingInformation::SetFallbackEnabled(bool val)
{
	m_fallbackEnabled = val;
}

bool CPairingInformation::IsMobileCardedSession() const
{
	return m_mobileCardedSession;
}

void CPairingInformation::SetMobileCardedSession(bool val)
{
	m_mobileCardedSession = val;
}

void CPairingInformation::HandlePairingEvent(const CPairingEvent* pairingEvent, CNotificationBox& notificationBox, 
	CBeaconManager* beaconManager, bool& success, std::string& status)
{
	if ((!GetPairingType().compare(SHORT_CODE) && !pairingEvent->GetPairingType().compare(SHORT_CODE)) || (!pairingEvent->GetPairingType().compare(SHORT_CODE) && 
		IsFallbackEnabled()))
	{
		if (pairingEvent->GetIsSuccess() == true)
		{
			notificationBox.SetMobileState(MobileState::PleaseEnterCode, pairingEvent->GetPairingId());
			SetWaitingForRequestCardChange(true);
			SetRequestCardChangeTime(CUtilities::GetCurrentTime());
			// handle fallback to passcode
			SetFallbackEnabled(false);
		}
		else
		{
			notificationBox.SetMobileState(MobileState::ErrorOccured, pairingEvent->GetStatus());
			success = false;
			status = pairingEvent->GetStatus();
		}
	}
	else if (!GetPairingType().compare(BEACON) && !pairingEvent->GetPairingType().compare(BEACON))
	{
		if (pairingEvent->GetIsSuccess() == true)
		{
            if ((success = beaconManager->SetEddystoneFrameData(pairingEvent->GetPairingId(), pairingEvent->GetPairingChecksum(), GetMeasuredPower())))
			{
				status = PAIRING_ID_SET;
				SetRequestCardChangeTime(CUtilities::GetCurrentTime());
			}
			else
			{
				status = BEACON_FAILURE;
				// handle fallback to passcode
				SetWaitingForPairing(true);
				SetFallbackEnabled(true);
			}

			SetWaitingForRequestCardChange(success);

		}
		else
		{
			notificationBox.SetMobileState(MobileState::ErrorOccured, pairingEvent->GetStatus());
			success = false;
			status = pairingEvent->GetStatus();
		}
	}
	else
	{
		cout << "HandlePairingEvent Error" << endl; 

		status = PAIRING_TYPE_MISMATCH;
		success = false;
		notificationBox.SetMobileState(MobileState::ErrorOccured, status);
	}
}

