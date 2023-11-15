#pragma once
#include "../Config/EGMConfig.h"
#include "metersdef.h"
#include "Ticket.h"
#include "PollerProtocol/PollerMessage.h"

class CSASProtocolState
{
public:
	CSASProtocolState();
	~CSASProtocolState();

	/// <summary>
	/// Sets the egm configuration.
	/// </summary>
	/// <param name="config">The configuration.</param>
	void SetEGMConfig(CEGMConfig config);

	/// <summary>
	/// Gets the egm configuration.
	/// </summary>
	/// <returns></returns>
	CEGMConfig GetEGMConfig();

	/// <summary>
	/// Sets the ignore ticket printed.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetIgnoreTicketPrinted(bool value);

	/// <summary>
	/// Gets the ignore ticket printed.
	/// </summary>
	/// <returns></returns>
	bool GetIgnoreTicketPrinted();

	/// <summary>
	/// Sets the type of the snapshot.
	/// </summary>
	/// <param name="value">The value.</param>
	void SetSnapshotType(SnapshotType value);

	/// <summary>
	/// Gets the type of the snapshot.
	/// </summary>
	/// <returns></returns>
	SnapshotType GetSnapshotType();

	/// <summary>
	/// Gets the slotmast identifier.
	/// </summary>
	/// <returns></returns>
	long GetSlotmastID();

	/// <summary>
	/// Sets the slotmast identifier.
	/// </summary>
	/// <param name="slotmastID">The slotmast identifier.</param>
	void SetSlotmastID(long slotmastID);

	/// <summary>
	/// Gets the casino number.
	/// </summary>
	/// <returns>The casino number.</returns>
	byte GetCasinoNumber();

	/// <summary>
	/// Sets the casino number.
	/// </summary>
	/// <param name="slotmastID">The casino number.</param>
	void SetCasinoNumber(byte casinoNumber);

	/// <summary>
	/// Sets EnableEGM setting.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	/// <param name="startup">true if value is from NVRAM, false otherwise.</param>
	void SetEnableEGM(bool startup, bool value);

	/// <summary>
	/// Gets the EnableEGM setting.
	/// </summary>
	/// <returns></returns>
	bool GetEnableEGM();

	/// <summary>
	/// Sets EnableHostCashout setting.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetEnableCashoutToHost(bool value);

	/// <summary>
	/// Gets the EnableHostCashout setting.
	/// </summary>
	/// <returns></returns>
	bool GetEnableCashoutToHost();

	/// <summary>
	/// Sets whether cashout to host feature is enabled on EGM.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetCashoutToHostEnabledOnEGM(bool value);

	/// <summary>
	/// Gets whether cashout to host feature is enabled on EGM.
	/// </summary>
	/// <returns></returns>
	bool GetCashoutToHostEnabledOnEGM();

	/// <summary>
	/// Gets number of tries done to enable/disable cashout to host feature on EGM.
	/// </summary>
	/// <returns></returns>
	int GetCashoutToHostConfigTries();

	/// <summary>
	/// Sets number of tries done to enable/disable cashout to host feature on EGM.
	/// </summary>
	/// <param name="value">number of tries.</param>
	void SetCashoutToHostConfigTries(int tries);

	/// <summary>
	/// Sets the allowable enable disable.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetAllowableEnableDisable(bool value);

	/// <summary>
	/// Gets the allowable enable disable.
	/// </summary>
	/// <returns></returns>
	bool GetAllowableEnableDisable();

	/// <summary>
	/// Sets the allowable game delay poll.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetAllowableGameDelayPoll(bool value);

	/// <summary>
	/// Gets the allowable game delay poll.
	/// </summary>
	/// <returns></returns>
	bool GetAllowableGameDelayPoll();

	/// <summary>
	/// Sets the allow stepdown reset for WMS.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetAllowStepdownResetForWMS(bool value);

	/// <summary>
	/// Gets the allow stepdown reset for WMS.
	/// </summary>
	/// <returns></returns>
	bool GetAllowStepdownResetForWMS();

	/// <summary>
	/// Sets is enhanced ticketing flag.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetIsEnhancedTicketing(bool value);

	/// <summary>
	/// Gets is enhanced ticketing flag.
	/// </summary>
	/// <returns>Is enhanced ticketing</returns>
	bool GetIsEnhancedTicketing();

	/// <summary>
	/// Sets ticketing enabled flag.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetTicketingEnabled(bool value);

	/// <summary>
	/// Gets ticketing enabled flag.
	/// </summary>
	/// <returns>Is ticketing enabled</returns>
	bool GetTicketingEnabled();

	/// <summary>
	/// Sets ticketing in enabled flag.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetTicketingInEnabled(bool value);

	/// <summary>
	/// Gets ticketing in enabled flag.
	/// </summary>
	/// <returns>Is ticketing in enabled</returns>
	bool GetTicketingInEnabled();

	/// <summary>
	/// Sets ticketing out enabled flag.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetTicketingOutEnabled(bool value);

	/// <summary>
	/// Gets ticketing out enabled flag.
	/// </summary>
	/// <returns>Is ticketing out enabled</returns>
	bool GetTicketingOutEnabled();

	/// <summary>
	/// Sets the last printed ticket.
	/// </summary>
	/// <param name="ticket">The ticket.</param>
	void SetLastPrintedTicket(CTicket *ticket);

	/// <summary>
	/// Gets the last printed ticket.
	/// </summary>
	/// <returns>Last printed ticket</returns>
	CTicket *GetLastPrintedTicket();

	/// <summary>
	/// Sets the validation extensions.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetValidationExtensions(bool value);

	/// <summary>
	/// Gets the validation extensions.
	/// </summary>
	/// <returns>Validation extensions</returns>
	bool GetValidationExtensions();

	/// <summary>
	/// Sets the support multi denom extensions.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetSupportMultiDenomExtensions(bool value);

	/// <summary>
	/// Gets the support multi denom extensions.
	/// </summary>
	/// <returns>Multidenom extensions support</returns>
	bool GetSupportMultiDenomExtensions();

	/// <summary>
	/// Sets AFT lock request pending.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetLockPending(bool value);

	/// <summary>
	/// Gets the AFT lock request pending.
	/// </summary>
	/// <returns>AFT Lock request pending</returns>
	bool GetLockPending();

	/// <summary>
	/// Sets the last lock request.
	/// </summary>
	/// <param name="ticks">The ticks.</param>
	void SetLastLockRequest(DWORD ticks);

	/// <summary>
	/// Gets the last lock request.
	/// </summary>
	/// <returns>Last lock request ticks</returns>
	DWORD GetLastLockRequest();

	/// <summary>
	/// Sets probing pbt lock indicator
	/// </summary>
	/// <param name="lock">bpt lock</param>
	void SetProbingPBTLock(bool lock);

	/// <summary>
	/// Gets probing pbt lock indicator
	/// </summary>
	/// <returns>Probing PBT Lock indicator</returns>
	bool GetProbingPBTLock();

	/// <summary>
	/// Checks if transactionNeeds to be sent for lock/unlock EGM
	/// </summary>
	/// <param name="enableGame">true if game needs to be enabled, false otherwise.</param>
	/// <param name="ackedSuccessfully">true if poll was successful, false otherwise.</param>
	/// <param name="transactionType">OUT - transaaction code that needs to be sent to FloorLogix.</param>
	/// <returns>true if transaction needs to be sent, false otherwise.</returns>
	bool CheckIfTransactionNeedsToBeSentForLockUnlockEgm(bool enableGame, bool ackedSuccessfully, TransactionType& transactionType);

private:
	CEGMConfig m_config;
	bool m_ignoreTicketPrinted; //  set to true while a printed ticket is first being processed.
	SnapshotType m_snType;
	long m_slotmastID;
	byte m_casinoNumber;
	bool m_bEnableEGM;
	bool m_bEnableHostCashout;
	bool m_bCashoutToHostEnabledOnEGM;
	int  m_cashoutToHostConfigTries;
	bool m_bAllowEnableDisable;
	bool m_bAllowGameDelayPoll;
	bool m_bAllowStepdownResetForWMS;
	bool m_bIsEnhancedTicketing;
	bool m_ticketingEnabled;
	bool m_ticketingOutEnabled;
	bool m_ticketingInEnabled;
	CTicket *m_lastPrintedTicket;
	bool m_bValidationExtensions;
	bool m_bSupportMultiDemonExtensions;
	bool m_probingAftLock;

	// Need to track when/if lock asked for but not granted
	bool m_bLockPending;
	DWORD m_dwLastLockRequest;

	TransactionType m_lastLockUnlockTransactionType;
	bool m_updateFloorgixOfEgmState;
};


