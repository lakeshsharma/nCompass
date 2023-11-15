#pragma once
#include "ConfigField.h"

class CConfigSenOption2Field :
	public CConfigField
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CConfigSenOption2Field(void);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CConfigSenOption2Field(void);

	/// <summary>
	/// Sets the Sentinel Options.
	/// </summary>
	/// <param name="senOption">IN - address of source buffer. If buffer is NULL then a length of zero is used.</param>
	/// <param name="length">IN - size of source buffer. If length is negative then a length of zero is used.</param>
	void SetSenOptions(const byte * senOption, int length);

	/// <summary>
	/// Returns the Sentinel Options.
	/// </summary>
	/// <returns>The Sentinel Options.</returns>
	DWORD GetSenOptions() const;

	/// <summary>
	/// Returns true if PBT Pin is not required, false otherwise.
	/// </summary>
	/// <returns>true if PBT Pin is not required, false otherwise.</returns>
	bool PBTPinNotRequired() const;
	
	/// <summary>
	/// Returns true if coupon promo out is enabled, false otherwise.
	/// </summary>
	/// <returns>true if coupon promo out is enabled, false otherwise.</returns>
	bool CouponPromoOutEnabled() const;
	
	/// <summary>
	/// Returns true if voucher out is enabled, false otherwise.
	/// </summary>
	/// <returns>true if voucher out is enabled, false otherwise.</returns>
	bool VoucherOutEnabled() const;
	
	/// <summary>
	/// Returns true if display Good Luck and session points is enabled, false otherwise.
	/// </summary>
	/// <returns>true if display Good Luck and session points is enabled, false otherwise.</returns>
	bool DisplayGoodLuckAndSessionPts() const;
	
	/// <summary>
	/// Returns true if display total and session points number is enabled, false otherwise.
	/// </summary>
	/// <returns>true if display total and session points number is enabled, false otherwise.</returns>
	bool DisplayTotalAndSessionPtsNum() const;
	
	/// <summary>
	/// Returns true if display points total as cash session is enabled, false otherwise.
	/// </summary>
	/// <returns>true if display points total as cash session is enabled, false otherwise.</returns>
	bool DisplayPtsTotalAsCashSession() const;

	/// <summary>
	/// Returns true if support for RCCL cards is enabled, false otherwise.
	/// </summary>
	/// <returns>true if support for RCCL cards is enabled, false otherwise.</returns>
	bool RcclCardsEnabled() const;

	/// <summary>
	/// Returns true if mini virtual player sessions are enabled, false otherwise.
	/// </summary>
	/// <returns>true if mini virtual player sessions are enabled, false otherwise.</returns>
	bool MiniVirtualSessionEnabled() const;

	/// <summary>
	/// Returns true if accrue points for NCEP play is enabled, false otherwise.
	/// </summary>
	/// <returns>true if accrue points for NCEP play is enabled, false otherwise.</returns>
	bool AccruePointsForNCEP() const;

	/// <summary>
	/// Returns true if accrue comp for NCEP play is enabled, false otherwise.
	/// </summary>
	/// <returns>true if accrue comp for NCEP play is enabled, false otherwise.</returns>
	bool AccrueCompForNCEP() const;

	/// <summary>
	/// Returns true if ONE-LINK progressive protocol communications and features are enabled, false otherwise.
	/// </summary>
	/// <returns>true if ONE-LINK progressive protocol communications and features are enabled, false otherwise.</returns>
	bool OneLinkProgressivesEnabled() const;

	/// <summary>
	/// Returns true if Mobile Connect (poller) communications are enabled, false otherwise.
	/// </summary>
	/// <returns>true if Mobile Connect (poller) communications are protocol communications enabled, false otherwise.</returns>
	bool MobileConnectCommunicationEnabled() const;

	/// <summary>
	/// Returns true if remote debugging is enabled, false otherwise.
	/// </summary>
	/// <returns>true if remote debugging is enabled, false otherwise.</returns>
	bool RemoteDebuggingEnabled() const;

	/// <summary>
	/// Returns true if partial transfer of cash (WAT) from game is enabled, false otherwise.
	/// </summary>
	/// <returns>true if partial transfer of cash (WAT) from game is enabled, false otherwise.</returns>
	bool PartialWATOutEnabled() const;

	/// <summary>
	/// Returns true if cashout to host is enabled at sentinel level, false otherwise.
	/// </summary>
	/// <returns>true if cashout to host is enabled at sentinel level, false otherwise.</returns>
	bool CashoutToHostEnabled() const;

	/// <summary>
	/// Returns true if unattended jackpot processing is enabled, false otherwise.
	/// </summary>
	/// <returns>True if unattended jackpot processing is enabled, false otherwise.</returns>
	bool UJPEnabled() const;

	/// <summary> 
	/// Returns true if PBT lock should be attempted prior to PBT 
	/// funds authorization. 
	/// <returns> true if PBT lock should be attempted prior to PBT 
	/// funds authorization.</returns> 
	bool ProbePBTLockEnabled() const;

	/// <summary>
	/// Returns true if MGMD(MulitGame MultiDenom) is enabled, false otherwise.
	/// </summary>
	/// <returns>True if MGMD is enabled, false otherwise.</returns>
	bool MGMDEnabled() const;

	/// <summary>
	/// Returns whether to display external credit verification screen even if there is no fee for transfer.
	/// </summary>
	/// <returns>True if to display external credit verification screen even if there is no fee for transfer, false otherwise.</returns>
	bool ExternalCreditTransferVerificationEnabled() const;

private:
	DWORD m_SenOption;

	bool m_PBTPinNotRequired;				//When Enabled a player pin code is not required (European)
	bool m_couponPromoOutEnabled;			//Sentinel will attempt to enable/disable EGM option bit for Prom out. Will send to OASIS ticket info regardless
	bool m_voucherOutEnabled;				//Sentinel will attempt to enable/disable EGM option bit for "Use printer as cashout device". Will send to OASIS ticket info regardless
	bool m_displayGoodLuckAndSessionPts;	//Good Luck Session as points display
	bool m_displayTotalAndSessionPtsNum;	//Total (points as number) / Session(points as number)
	bool m_displayPtsTotalAsCashSession;	//Will display the players point total as Cash for the session display
	bool m_miniVirtualSessionEnabled; // Mini virtual player session after player card out.
	bool m_accruePointsForNCEP;				//Points can be earned from NCEP Play
	bool m_accrueCompForNCEP;				//Comp can be earned from NCEP play
	bool m_oneLinkProgressivesEnabled; // ONE LINK progressive protocol communications and features.
	bool m_rcclCardsEnabled;				//Use RCCL style cards or OASIS cards
	bool m_ujpEnabled;						//Unattended Jackpot enabled
	bool m_probePBTLock;					//Attempt PBT lock before authorizing PBT amount
	bool m_mgmdEnabled;					// multigame multidenom enabled
	bool m_mobileConnectCommEnabled;	// floor communication with FS enabled
	bool m_remoteDebuggingEnabled;		// remote debugging enabled
	bool m_partialWATOutEnabled;        // partial transfer of cash from game enabled
	bool m_cashoutToHostEnabled;        // cashout to host enabled
	bool m_externalCreditTransferVerification; //Display external credit verification screen even if there is no fee for transfer
};

