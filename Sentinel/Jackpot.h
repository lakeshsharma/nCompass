#pragma once

#include "Hardware/MemoryStorage.h"
#include "InternalMsgContent.h"
#include "EnumMap.h"
#include "DataSubscriptionProvider.h"
#include "Config/Config.h"
#include "Hardware/MemoryStorage.h"
#include "ProcessDump/OperatingSystem.h"

class CWebDiagnosticsCommonInfoBase;
namespace SentinelNativeUnitTests { class WebDiagnosticsJackpotInfoTests; }


namespace UJPScreen
{
	enum Type
	{
		PinEntry,
		PinSuccessWithTax,//Please wait while we adjust the machine credits to withhold taxes required by law.\nThank you!
		PinSuccessWithoutTax,//Please wait while we pay your jackpot winnings to your slot machine.\nThank you!
		PinError,//Invalid PIN
		Complete,
		CompleteWithError,
		CanceledHandpayReset,
		ErrorUnknown,//Unknown Error
		ErrorCreditReset,//Jackpot Pending to be Paid to the Game by Attendant
		ErrorInsufficientInfo,//Insufficient Credentials – Check ID Expiration and/or Address
		ErrorEGMNotCapable,//Slot Configuration – Check EGM Remote Keyback Settings and/or Limits
		ErrorJackpotAboveLimit,//Unattended Taxable Jackpot Limit Exceeded
		ErrorAccountLocked,//Account Locked
		ErrorSessionExpired,//Session Timed Out
		ErrorPlayerInVirtualSession,//Jackpot Pending to be Paid by Attendant
		CardedTeaser,
		UncardedTeaser
	};
	extern EnumMap<Type> emap;
}

typedef enum
{
	JT_NONE = 0,
	JT_HANDPAY = 128,
	JT_CANCEL_CREDIT = 129,
	JT_TOPAWARD = 130,
	JT_EXTERNALBONUS = 131,
} JackpotType;

typedef enum
{
	JTRANS_COMPOSITE,				// Handpay jackpot
	JTRANS_PROGRESSIVE_HOPPER_PAID,	// Progressive jackpot hopper paid
	JTRANS_PROGRESSIVE_SCANNER_ONLY,// Progressive jackpot scanner only
	JTRANS_NONE,					// Added to allow for blank JackpotTransactionTypes
} JackpotTransactionType;

namespace UnattendedJackpotType
{
	enum Type
	{
		UJPTYPE_PROCESS,
		UJPTYPE_CARDED_TEASER,
		UJPTYPE_UNCARDED_TEASER,
		UJPTYPE_CANCEL_INSUFFICIENT_INFO,
		UJPTYPE_CANCEL_EGM_NOT_CAPABLE,
		UJPTYPE_CANCEL_ABOVE_LIMIT,
		UJPTYPE_CANCEL_ACCOUNT_LOCKED,
		UJPTYPE_CANCEL_SESSION_EXPIRED,
		UJPTYPE_CANCEL_PLAYER_IN_VIRTUAL_SESSION,
		UJPTYPE_NONE
	};
	extern EnumMap<Type> emap;
}

namespace UnattendedJackpotPINResponseType
{
	enum Type
	{
		UJPPIN_AUTHORIZED,
		UJPPIN_NOTAUTHORIZED,
		UJPPIN_ACCOUNTLOCKED,
	};
	extern EnumMap<Type> emap;
}

namespace UnattendedJackpotState
{
	enum Type
	{
		UJPNone,
		UJPPinEntry,
		UJPPinSent,
		UJPAuthorized,
		UJPCreditReset,
		UJPCreditAndRemoteReset,
		UJPCanceled,
		UJPFailed,
		UJPCanceledHandpayReset,
		UJPCanceledReboot,
		UJPCanceledVirtualPlayer,
		UJPCanceledTimedOut,
		UJPCanceledTaskbarButtonPress,
		UJPCanceledCloseMediaWindow,
		UJPCanceledAccountLocked,
		UJPCanceledPlayerIdMismatch,
		UJPCanceledInsufficientInfo,
		UJPCanceledEgmNotCapable,
		UJPCanceledAboveLimit,
		UJPCanceledCallHost,
		UJPPinError,
		UJPCardedTeaser,
		UJPUncardedTeaser,
	};
	extern EnumMap<Type> emap;
}

namespace UnattendedJackpotStatus
{
	enum Type
	{
		UJPStatusCanceled,
		UJPStatusFailed,
		UJPStatusSuccessKeyToCredit,
		UJPStatusSuccessKeyToCreditRemoteReset,
		UJPStatusCanceledHandpayReset,
		UJPStatusCanceledReboot,
		UJPStatusCanceledVirtualPlayer,
		UJPStatusCanceledTimedOut,
		UJPStatusCanceledTaskbarButtonPress,
		UJPStatusCanceledCloseMediaWindow,
		UJPStatusCanceledAccountLocked,
		UJPStatusCanceledPlayerIdMismatch,
		UJPStatusCanceledInsufficientInfo,
		UJPStatusCanceledEgmNotCapable,
		UJPStatusCanceledAboveLimit,
		UJPStatusCanceledCallHost,
	};
	extern EnumMap<Type> emap;
}

class CJackpot : public CDataSubscriptionProvider,
	public CMemoryStorage,
	public CInternalMsgContent<INT_MSG_CONTENT_JACKPOT>
{
public:
	CJackpot(MEMORY_TYPE memoryType = MEMORY_NONE, bool bBuildFromNVRAM = false, IOperatingSystem& operatingSystem = COperatingSystem::m_operatingSystem);
	CJackpot(CJackpot& jackpotRHS);
	~CJackpot();

	void BuildYourself();

	CJackpot& operator=(CJackpot& jackpotRHS);

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	/// <summary>
	/// Sets all the jackpot data
	/// </summary>
	/// <param name="keyToCreditAvailable">true if the key to credit is available.</param>
	void SetJackpotData(const byte* cardID, int cardLength, bool keyToCreditAvailable, DWORD coinsPlayed);

	void SetJackpotAmount(uint64_t amount)
	{
		m_jackpotAmount = amount;
		StoreToNVRAM(__FUNCTION__);
	}

	uint64_t GetJackpotAmount() const;

	void SetJackpotType(JackpotType type)
	{
		m_jpType = type;
		StoreToNVRAM(__FUNCTION__);
	}

	JackpotType GetJackpotType() const
	{
		return m_jpType;
	}

	void SetJackpotTransactionType(JackpotTransactionType type)
	{
		m_transType = type;
		StoreToNVRAM(__FUNCTION__);
	}

	JackpotTransactionType GetJackpotTransactionType() const
	{
		return m_transType;
	}

	byte GetProgressiveLevel() const
	{
		return m_progressiveLevel;
	}

	void SetProgressiveLevel(byte progressiveLevel)
	{
		m_progressiveLevel = progressiveLevel;
		StoreToNVRAM(__FUNCTION__);
	}

	DWORD GetCoinsPlayed() const
	{
		return m_coinsPlayed;
	}

	void SetCoinsPlayed(DWORD coinsPlayed)
	{
		m_coinsPlayed = coinsPlayed;
		StoreToNVRAM(__FUNCTION__);
	}

	/// <summary>
	/// Sets if key to credit available.
	/// </summary>
	/// <param name="keyToCreditAvailable">true if the key to credit is available.</param>
	void SetKeyToCreditAvailable(bool keyToCreditAvailable);

	/// <summary>
	/// Is key-to-credit available for this jackpot
	/// </summary>
	/// <returns>true if key to credit has been determined to be available</returns>
	bool IsKeyToCreditAvailable() const;

	static bool DoesJackpotExistInNVRAM();

	typedef enum
	{
		ResetSuccess,
		ResetUnavailable,
		NotInHandpay,
		ResetFailure,
	} ResetResult;

	/// <summary>
	/// Sets the Unattended state of this jackpot.
	/// </summary>
	/// <param name="state">The unattended jackpot state.</param>
	void SetUnattendedState(UnattendedJackpotState::Type state);

	/// <summary>
	/// Gets the current Unattended state of this Jackpot.
	/// </summary>
	/// <returns>The current unattended jackpot state</returns>
	UnattendedJackpotState::Type GetUnattendedState() const;

	/// <summary>
	/// Gets the current Unattended state string of this Jackpot.
	/// </summary>
	/// <returns>current Unattended state string of this Jackpot.</returns>
	const char* GetUnattendedStateStr() const;

	/// <summary>
	/// Gets the jackpot status.
	/// </summary>
	/// <returns>The jackpot status.</returns>
	UnattendedJackpotStatus::Type GetJackpotStatus() const;

	/// <summary>
	/// Gets the jackpot status string of this Jackpot.
	/// </summary>
	/// <returns>jackpot status string of this Jackpot.</returns>
	const char* GetJackpotStatusStr() const;

	/// <summary>
	/// Sets the unattended state timestamp.
	/// </summary>
	/// <param name="tickCount">The tick count.</param>
	void SetUnattendedStateTimestamp(DWORD tickCount);

	/// <summary>
	/// Gets the unattended state timestamp.
	/// </summary>
	/// <returns>Unattended state timestamp</returns>
	DWORD GetUnattendedStateTimestamp() const;

	/// <summary>
	/// Sets the jackpot transaction as sent.
	/// </summary>
	void SetTransactionSent();

	/// <summary>
	/// Determines whether the jackpot was transaction sent.
	/// </summary>
	/// <returns>true if jackpot transaction was sent</returns>
	bool IsTransactionSent() const;

	/// <summary>
	/// Sets the hand pay reset.
	/// </summary>
	/// <param name="handpayReset">if set to <c>true</c> [handpay reset].</param>
	void SetHandPayReset(bool handpayReset);

	/// <summary>
	/// Gets the hand pay reset.
	/// </summary>
	/// <returns>Was handpay reset</returns>
	bool GetHandPayReset() const;

	/// <summary>
	/// Sets the jackpot identifier.
	/// </summary>
	/// <param name="jackpotID">The jackpot identifier.</param>
	void SetJackpotID(int jackpotID);

	/// <summary>
	/// Gets the jackpot identifier.
	/// </summary>
	/// <returns>Jackpot ID</returns>
	int GetJackpotID() const;

	/// <summary>
	/// Sets the card identifier.
	/// </summary>
	/// <param name="">The cardID.</param>
	/// <param name="">cardID length</param>
	void SetCardID(const byte* cardID, int dataLen);

	/// <summary>
	/// Gets the card identifier.
	/// </summary>
	/// <returns>Pointer to 5 byte Card ID</returns>
	const byte* GetCardID() const;

	/// <summary>
	/// Sets the unattended state after reset attempt.
	/// </summary>
	/// <param name="keyToCreditResult">The key to credit result.</param>
	/// <param name="remoteResetResult">The remote reset result.</param>
	void SetUnattendedResetResult(ResetResult keyToCreditResult, ResetResult remoteResetResult);

	/// <summary>
	/// Determines whether if jackpot is in a state where it is considered complete.
	/// </summary>
	/// <returns>true if jackpot object can be considered complete</returns>
	bool IsJackpotComplete() const;

	/// <summary>
	/// Determines whether a unattended session is in progress for this jackpot.
	/// </summary>
	/// <returns>true if currently processing an unattended jackpot</returns>
	bool IsUnattendedJackpotInProgress() const;

	/// <summary>
	/// Has unattended jackpot expired and is in progress.
	/// </summary>
	/// <param name="expireTicks">Time to allow jackpot to be processed</param>
	/// <returns>True if jackpot is canceled in this call</returns>
	bool IsExpiredUnattendedJackpot(DWORD expireTicks) const;

	/// <summary>
	/// Sets tax withholding configuration
	/// </summary>
	/// <param name="taxWithholdingEnabled">true if tax withholding enabled flag is true, false otherwise.</param>
	/// <param name="federalPercentage">federal Percentage</param>
	/// <param name="statePercentage">staet Percentage</param>
	void SetTaxWithholdingConfig(bool taxWithholdingEnabled, float federalPercentage, float statePercentage);

	/// <summary>
	/// Gets the tax withholding enabled flag.
	/// </summary>
	/// <returns>true if tax withholding enabled flag is true, false otherwise.</returns>
	bool IsTaxWithholdingEnabled() const;

	/// <summary>
	/// Gets the federal tax withholding percent.
	/// </summary>
	/// <returns>the federal tax withholding percent</returns>
	float GetFederalTaxWithholdingPercent() const;

	/// <summary>
	/// Gets the tax withholding percent.
	/// </summary>
	/// <returns>state tax withholding percent</returns>
	float GetStateTaxWithholdingPercent() const;

	/// <summary>
	/// Sets tax withholding values
	/// </summary>
	/// <param name="federal">federal withholding value in pennies.</param>
	/// <param name="state">state withholding value in pennies.</param>
	/// <param name="federalTaxOverridden">Federal Tax Overridden flag</param>
	/// <param name="stateTaxOverridden">State Tax Overridden flag</param>
	void SetTaxWithholding(DWORD federal, DWORD state, bool federalTaxOverridden, bool stateTaxOverridden);

	/// <summary>
	/// Checks if tax withholding failed.
	/// </summary>
	/// <returns>true if tax withholding failed, false otherwise</returns>
	bool TaxWithholdingFailed() const;

	/// <summary>
	/// Gets the Federal Tax Overriden flag.
	/// </summary>
	/// <returns>true if Federal Tax is overriden, false otherwise.</returns>
	bool IsFederalTaxOverriden() const;

	/// <summary>
	/// Gets the State Tax Overriden flag.
	/// </summary>
	/// <returns>true if State Tax is overriden, false otherwise.</returns>
	bool IsStateTaxOverriden() const;

	/// <summary>
	/// Gets the Jackpot Amount after taxes.
	/// </summary>
	/// <returns>Jackpot Amount after taxes</returns>
	uint64_t GetJackpotAmountAfterTaxes() const;

	/// <summary>
	/// Gets the Federal Tax Witholding Amount in pennies.
	/// </summary>
	/// <returns>Federal Tax Witholding Amount in pennies</returns>
	DWORD GetFederalTaxWithholding() const;

	/// <summary>
	/// Gets the State Tax Witholding Amount in pennies.
	/// </summary>
	/// <returns>State Tax Witholding Amount in pennies</returns>
	DWORD GetStateTaxWithholding() const;

	/// <summary>
	/// Gets the total Tax Witholding Amount in pennies.
	/// </summary>
	/// <returns>Total Tax Witholding Amount in pennies</returns>
	DWORD GetTotalWithholdingInPennies() const;

	/// <summary>
	/// Sets the player PIN
	/// </summary>
	/// <param name="pin">player PIN</param>
	void SetPlayerPin(const std::string& pin);

	/// <summary>
	/// Gets the player PIN
	/// </summary>
	/// <returns>Player PIN</returns>
	std::string GetPlayerPin() const;

	///	<summary>
	/// Updates JackpotInfo web diagnostics.
	///	</summary>
	///	<param name="commonInfo">CWebDiagnosticsCommonInfoBase common info.</param>
	void UpdateWebDiagnostics(const CWebDiagnosticsCommonInfoBase& commonInfo, const CConfig& config) const;

	/// <summary>
	/// Get a string with the CJackpot member variables formatted suitable for logging.
	/// </summary>
	/// <param name="jackpot">IN - The jackpot instance. MAY be nullptr.</param>
	/// <param name="description">IN - Reason for logging.</param>
	/// <remarks>
	/// 2015-12-14 The value for m_handpayReset is not included in the string, as it is currently unused.
	/// </remarks>
	static std::string GetLogInfoString(const CJackpot* jackpot, LPCTSTR description);

	/// <summary>
	/// Log the member variables for a CJackpot instance.
	/// </summary>
	/// <param name="jackpot">IN - The jackpot instance. MAY be nullptr.</param>
	/// <param name="description">IN - Reason for logging.</param>
	static void LogInfo(const CJackpot* jackpot, LPCTSTR description);

	std::string GetStartupScreen() const;

private:
	IOperatingSystem& m_operatingSystem;
	std::queue<UJPScreen::Type> m_ujpScreensToDisplay;
	UJPScreen::Type m_ujpScreen;
	int m_jackpotID;
	JackpotType m_jpType;
	JackpotTransactionType m_transType;
	uint64_t m_jackpotAmount;
	byte m_progressiveLevel;
	DWORD m_coinsPlayed;
	bool m_keyToCreditAvailable;
	UnattendedJackpotState::Type m_unattendedState;
	DWORD m_unattendedStateTimestamp;
	bool m_jackpotTransactionSent;
	byte m_cardID[CARD_ID_LEN];
	bool m_handpayReset;
	//UJP Tax Withholding variables
	std::string m_pin;
	bool m_taxWithholdingEnabled;
	float m_federalTaxWithholdingPercent;
	float m_stateTaxWithholdingPercent;

	DWORD m_federalTaxWithholding;
	DWORD m_stateTaxWithholding;
	bool m_federalTaxOverridden;
	bool m_stateTaxOverridden;

	DWORD m_displayStartTickCount;

	void SetJackpotExistanceToNVRAM(bool bJackpotExists);
	void StoreToNVRAM(const char* functionName) const;
	void SetScreenToDisplay();

	static EnumSparseMap<JackpotType> m_typeEmap;
	static EnumMap<JackpotTransactionType> m_transactionTypeEmap;

	void WriteWebDiagnostics(const CWebDiagnosticsCommonInfoBase& commonInfoBase, std::ostream& os, const CConfig& config) const;
	friend SentinelNativeUnitTests::WebDiagnosticsJackpotInfoTests;
	friend CMemoryStorage;
};

