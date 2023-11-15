

#ifndef _PAIRINGINFORMATION_H
#define _PAIRINGINFORMATION_H


#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"
#include "PairingSettingsEvent.h"
#include "UI/NotificationBox.h"
#include "BeaconManager.h"

extern const std::string CARD_INSERT;
extern const std::string CARD_REMOVE;
extern const std::string EXISTING_EMPLOYEE_SESSION;
extern const std::string EXISTING_PLAYER_SESSION;
extern const std::string PLAYER_NOT_CONFIRMED;
extern const std::string ALREADY_INSERTED;
extern const std::string UNKNOWN_CARD;
extern const std::string SHORT_CODE;
extern const std::string PAIRING_ID_SET;
extern const std::string SESSION_IN_PROGRESS;
extern const std::string GAME_NOT_ENABLED;
extern const std::string BEACON_FAILURE;
extern const std::string ERROR_STATUS;
extern const std::string PAIRING_TYPE_MISMATCH;
extern const std::string BEACON;

typedef enum
{
	RequestCardChange,
	RequestFundsTransferBalance,
	RequestFundsTransfer,

}MessageTypeForCorrelation;

extern const std::string BEACON_DISCONNECTED;
extern const std::string BEACON_IDLE_SET;
extern const std::string BEACON_IDLE_NOT_SET;

// Forward declare class to declare friend for unit test access.
namespace SentinelUnitTests { class PairingInformation; }

class CPairingInformation
{
public:
	//friend class declarations for unit test access.
	friend class SentinelUnitTests::PairingInformation;

	static const DWORD DEFAULT_PAIRING_TIMEOUT_MS = 10000; // 10 sec
	static const bool DEFAULT_PAIRING_ENABLED = false;
	static const uint8_t DEFAULT_MEASURED_POWER = 0;
	/// <summary>
	/// Returns instance of CPairingInformation(singleton).
	/// </summary>
	/// <returns>CPairingInformation</returns>
	static CPairingInformation& Instance();
	
	/// <summary>
	/// Update members from PairingSettingsEvent.
	/// </summary>
	/// <param name="pairingSettings">IN - event data.</param>
	void UpdateFromPairingSettingsEvent(CPairingSettingsEvent* pairingSettings);
	
	/// <summary>
	/// Get Pairing Type.
	/// </summary>
	/// <returns>string</returns>
    const std::string &GetPairingType() const;
	
	/// <summary>
	/// Set Pairing Type.
	/// </summary>
	/// <param name="pairingType">IN - type.</param>
    void SetPairingType(const std::string &pairingType);
	
	/// <summary>
	/// Is Pairing Enabled.
	/// </summary>
	/// <returns>bool</returns>
    bool IsPairingEnabled() const;
	
	/// <summary>
	/// Set Pairing Enabled.
	/// </summary>
	/// <param name="pairingEnabled">IN - data.</param>
    void SetPairingEnabled(bool pairingEnabled);
	
	/// <summary>
	/// Get Pairing TimeOut in ms.
	/// </summary>
	/// <returns>uint32_t</returns>
    uint32_t GetPairingTimeOutMs() const;
	
	/// <summary>
	/// Set Pairing TimeOut in ms.
	/// </summary>
	/// <param name="pairingTimeOutMs">IN - data.</param>
    void SetPairingTimeOutMs(uint32_t pairingTimeOutMs);
	
	/// <summary>
	/// Get Idle PairingId.
	/// </summary>
	/// <returns>string</returns>
    const std::string& GetIdlePairingId() const;
	
	/// <summary>
	/// Set Idle PairingId.
	/// </summary>
	/// <param name="idlePairingId">IN - id.</param>
    void SetIdlePairingId(const std::string &idlePairingId);
	
	/// <summary>
	/// Get Idle PairingChecksum.
	/// </summary>
	/// <returns>PairingChecksum string</returns>
    const std::string& GetIdlePairingChecksum() const;
	
	/// <summary>
	/// Set Idle PairingChecksum.
	/// </summary>
	/// <param name="idlePairingChecksum">IN - id.</param>
    void SetIdlePairingChecksum(const std::string &idlePairingChecksum);
	
	/// <summary>
	/// Is Waiting For Pairing.
	/// </summary>
	/// <returns>bool</returns>
	bool IsWaitingForPairing() const;
	
	/// <summary>
	/// Set Pairing Request Time.
	/// </summary>
	/// <param name="time_t">IN - time.</param>
	void SetPairingRequestTime(time_t);
	
	/// <summary>
	/// Get Pairing Request Time.
	/// </summary>
	/// <returns>time_t</returns>
	time_t GetPairingRequestTime();
	
	/// <summary>
	/// Set Waiting For Pairing.
	/// </summary>
	/// <param name="val">IN - data.</param>
	void SetWaitingForPairing(bool val);
	
	/// <summary>
	/// Is Waiting For Request Card Change.
	/// </summary>
	/// <returns>bool</returns>
	bool IsWaitingForRequestCardChange() const;
	
	/// <summary>
	/// Set Request Card Change Time.
	/// </summary>
	/// <param name="time_t">IN - time.</param>
	void SetRequestCardChangeTime(time_t);
	
	/// <summary>
	/// Get Request Card Change Time.
	/// </summary>
	/// <returns>time_t</returns>
	time_t GetRequestCardChangeTime();
	
	/// <summary>
	/// Set Waiting For Request Card Change.
	/// </summary>
	/// <param name="val">IN - data.</param>
	void SetWaitingForRequestCardChange(bool val);
	
	/// <summary>
	/// Check Mobile Pairing TimeOut.
	/// </summary>
	/// <returns>bool</returns>
	bool CheckMobilePairingTimeOut();
	
	/// <summary>
	/// Check Mobile Request CardChange TimeOut.
	/// </summary>
	/// <returns>bool</returns>
	bool CheckMobileRequestCardChangeTimeOut();
	
	/// <summary>
	/// Set CorrelationId.
	/// </summary>
	/// <param name="type">IN - message type.</param>
	/// <param name="correlation">IN - id.</param>
	void SetCorrelationId(MessageTypeForCorrelation type, std::string correlation);
	
	/// <summary>
	/// Get CorrelationId.
	/// </summary>
	/// <param name="type">IN - message type.</param>
	/// <returns>string</returns>
	std::string GetCorrelationId(MessageTypeForCorrelation type);
	
	/// <summary>
	/// Is Fallback Enabled.
	/// </summary>
	/// <returns>bool</returns>
	bool IsFallbackEnabled();
	
	/// <summary>
	/// Set Fallback Enabled.
	/// </summary>
	/// <param name="bool">IN - data.</param>
	void SetFallbackEnabled(bool);
	
	/// <summary>
	/// Is Mobile CardedSession.
	/// </summary>
	/// <returns>bool</returns>
	bool IsMobileCardedSession() const;
	
	/// <summary>
	/// Set Mobile CardedSession.
	/// </summary>
	/// <param name="bool">IN - data.</param>
	void SetMobileCardedSession(bool);
	
	/// <summary>
	/// Get beacon measured power.
	/// </summary>
	/// <returns>uint8_t</returns>
	uint8_t GetMeasuredPower() const;

	/// <summary>
	/// Handle PairingEvent.
	/// </summary>
	/// <param name="pairingEvent">IN - data.</param>
	/// <param name="m_notificationBox">IN - UI.</param>
	/// <param name="m_beaconManager">IN - beacon.</param>
	/// <param name="success">OUT - data.</param>
	/// <param name="status">OUT - data.</param>
	void HandlePairingEvent(const CPairingEvent* pairingEvent, CNotificationBox& m_notificationBox, CBeaconManager* m_beaconManager, bool& success, std::string& status);

	// forbid copy, move and assignment
    CPairingInformation(const CPairingInformation&) = delete;
    CPairingInformation& operator =(const CPairingInformation&) = delete;
    CPairingInformation(CPairingInformation&&) = delete;
    CPairingInformation& operator =(CPairingInformation&&) = delete;

protected:
    CPairingInformation();

private:
	// pairing settings
    std::string m_sPairingType;
    bool m_bPairingEnabled;
    uint32_t m_uPairingTimeOutMs;
    std::string m_sIdlePairingId;
    std::string m_sIdlePairingChecksum;
	// pairing logic
	bool m_waitForPairing;
	time_t m_pairingRequestTime;
	bool m_waitForRequestCardChange;
	time_t m_requestCardChangeTime;
	// correlation id
	std::map<MessageTypeForCorrelation,std::string> m_correlationId;
	// handle fallback to passcode
	bool m_fallbackEnabled;
	// carded session
	bool m_mobileCardedSession;

	uint8_t m_measuredPower;
};


#endif //_PAIRINGINFORMATION_H
