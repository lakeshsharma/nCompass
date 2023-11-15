#pragma once
#include "ConfigField.h"

class CConfigSenOption1Field :
	public CConfigField
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CConfigSenOption1Field(void);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CConfigSenOption1Field(void);

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
	bool FillAmount() const;

	/// <summary>
	/// Returns true to monitor Spintek-reported hopper level for the EGM, false otherwise.
	/// </summary>
	/// <returns>true to monitor Spintek-reported hopper level for the EGM, false otherwise.</returns>
	bool UseHopper() const;
	
	/// <summary>
	/// Returns true if numeric point totals are to be displayed to the player, false otherwise.
	/// </summary>
	/// <returns>true if numeric point totals are to be displayed to the player, false otherwise.</returns>
	bool DisplayNumericPointTotal() const;
	
	/// <summary>
	/// Returns true if the player's name is to be displayed to the player, false otherwise.
	/// </summary>
	/// <returns>true if the player's name is to be displayed to the player, false otherwise.</returns>
	bool DisplayPlayerName() const;

	/// <summary>
	/// Returns true if the session points and the session point countdown are to be displayed to the player, false otherwise.
	/// </summary>
	/// <returns>true if the session points and the session point countdown are to be displayed to the player, false otherwise.</returns>
	bool DisplayCountdownAndSession() const;

	/// <summary>
	/// Returns true if total points and session points are to be displayed as cash value to the player, false otherwise.
	/// </summary>
	/// <returns>true if total points and session points are to be displayed as cash value to the player, false otherwise.</returns>
	bool DisplayTotalPtsSessionPtsAsCash() const;

	/// <summary>
	/// Returns true if PersonalBanker cash transfers are enabled for this Sentinel, false otherwise.
	/// </summary>
	/// <returns>true if PersonalBanker cash transfers are enabled for this Sentinel, false otherwise.</returns>
	/// <remarks>
	/// Additional requirements must be meet before cash transfers can be performed.
	/// </remarks>
	bool PBTCashEnable() const;

	/// <summary>
	/// Returns true if PersonalBanker player point transfers are enabled for this Sentinel, false otherwise.
	/// </summary>
	/// <returns>true if PersonalBanker player point transfers are enabled for this Sentinel, false otherwise.</returns>
	bool PBTPointsEnable() const;
	
	/// <summary>
	/// Returns true if PersonalBanker promotional credit transfers are enabled for this Sentinel, false otherwise.
	/// </summary>
	/// <returns>true if PersonalBanker promotional credit transfers are enabled for this Sentinel, false otherwise.</returns>
	bool PBTPromoEnable() const;
	
	/// <summary>
	/// Returns true if comp values are to be displayed to the player, false otherwise.
	/// </summary>
	/// <returns>true if comp values are to be displayed to the player, false otherwise.</returns>
	bool DisplayComp() const;
	
	/// <summary>
	/// Returns true if PersonalBanker points are transfered as promotional credits, false otherwise.
	/// </summary>
	/// <returns>true if PersonalBanker points are transfered as promotional credits, false otherwise.</returns>
	bool PBTPointsAsPromo() const;
	
	/// <summary>
	/// Returns true if Coins Won (credits won) player tracking accounting is enabled, false otherwise.
	/// </summary>
	/// <returns>true if Coins Won (credits won) player tracking accounting is enabled, false otherwise.</returns>
	bool CoinsWonAccounting() const;
	
	/// <summary>
	/// Returns true if Coins In (credits wagered) player tracking accounting is enabled, false otherwise.
	/// </summary>
	/// <returns>true if Coins In (credits wagered) player tracking accounting is enabled, false otherwise.</returns>
	bool CoinsInAccounting() const;
	
	/// <summary>
	/// Returns true if congratulations messages for session points earned are to be displayed to the player, false otherwise.
	/// </summary>
	/// <returns>true if congratulations messages for session points earned are to be displayed to the player, false otherwise.</returns>
	bool DisplayCongrats() const;
	
	/// <summary>
	/// Returns true if Theoretical (credits wagered * par) player tracking accounting is enabled, false otherwise.
	/// </summary>
	/// <returns>true if Theoretical (credits wagered * par) player tracking accounting is enabled, false otherwise.</returns>
	bool TheoreticalAccounting() const;
	
	/// <summary>
	/// Returns true if Coins Out (machine paid credits won) player tracking accounting is enabled, false otherwise.
	/// </summary>
	/// <returns>true if Coins Out (machine paid credits won) player tracking accounting is enabled, false otherwise.</returns>
	bool CoinsOutAccounting() const;
	
	/// <summary>
	/// Returns true if PersonalBanker sessions are to be initiated on player card in, false otherwise.
	/// </summary>
	/// <returns>true if PersonalBanker sessions are to be initiated on player card in, false otherwise.</returns>
	bool CardInPBT() const;
	
	/// <summary>
	/// Returns true if Employee PINs are required for normal access to employee menus.
	/// </summary>
	/// <returns>true if Employee PINs are required for normal access to employee menus.</returns>
	bool EmployeePinRequired() const;
	
	/// <summary>
	/// Returns true if session points awards are to be displayed for partial countdowns, false if they are to be displayed only after the countdown reaches zero.
	/// </summary>
	/// <returns>true if session points awards are to be displayed for partial countdowns, false if they are to be displayed only after the countdown reaches zero.</returns>
	bool PartialBonusAward() const;
	
	/// <summary>
	/// Returns true if total points are to be displayed as cash value to the player, false otherwise.
	/// </summary>
	/// <returns>true if total points are to be displayed as cash value to the player, false otherwise.</returns>
	bool DisplayPtsTotalAsCash() const;
	
	/// <summary>
	/// Returns true if good luck and session messages are to be displayed to the player, false otherwise.
	/// </summary>
	/// <returns>true if good luck and session messages are to be displayed to the player, false otherwise.</returns>
	bool DisplayGoodLuckAndSession() const;

	/// <summary>
	/// Returns true if ticket redemption for restricted promotional credits is enabled, false otherwise.
	/// </summary>
	/// <returns>true if ticket redemption for restricted promotional credits is enabled, false otherwise.</returns>
	bool CouponPromoInEnabled() const;
	
	/// <summary>
	/// Returns true if ticket redemption for non-restricted promotional credits is enabled, false otherwise.
	/// </summary>
	/// <returns>true if ticket redemption for non-restricted promotional credits is enabled, false otherwise.</returns>
	bool VoucherPromoInEnabled() const;
	
	/// <summary>
	/// Returns true if virtual sessiosn for player tracking after player card out are enabled, false otherwise.
	/// </summary>
	/// <returns>true if virtual sessiosn for player tracking after player card out are enabled, false otherwise.</returns>
	bool VirtualSessionEnabled() const;
	
	/// <summary>
	/// Returns true if ticket redemption is enabled, false otherwise.
	/// </summary>
	/// <returns>true if ticket redemption is enabled, false otherwise.</returns>
	bool TicketInEnabled() const;
	
	/// <summary>
	/// Returns true if the new style of PersonalBanker menus are to be used, false otherwise.
	/// </summary>
	/// <returns>true if the new style of PersonalBanker menus are to be used, false otherwise.</returns>
	bool UseNewPBMenus() const;
	
	/// <summary>
	/// Returns true if PersonalBanker transfers from the EGM are enabled, false otherwise.
	/// </summary>
	/// <returns>true if PersonalBanker transfers from the EGM are enabled, false otherwise.</returns>
	bool PBFromGameEnabled() const;
	
	/// <summary>
	/// Returns true if a player's total points are to be displayed to the player, false otherwise.
	/// </summary>
	/// <returns>true if a player's total points are to be displayed to the player, false otherwise.</returns>
	bool DisplayPlayerPtsTotal() const;
	
private:

	DWORD m_SenOption;
	
	bool m_fillAmount;				//If set, the Sentinel will force the user to enter a fill amount, otherwise the system will use the fill amount setup in the database for that machine
	bool m_useHopper;				//Used with Spintek Weighted Hopper systems where the Sentinel configures and monitors the hopper level
	bool m_useTicketDispenser;		//Not used. Carry over from DOS
	bool m_displayNumericPointTotal;		//Point Total will display on Card-In as numeric
	bool m_displayPlayerName;
	bool m_displayCountdownAndSession;		//Countdown xxx / Session display.
	bool m_displayTotalPtsSessionPtsAsCash;	// Total (points as Cash) / Session (points as Cash)
	bool m_personalJackpot;			//Not used. Carry over from DOS.
	bool m_PBTCashEnable;			//When set, PersonalBanker Cash functionality is enabled at the Sentinel
	bool m_PBTPointsEnable;			//When set PersonalBanker Points as Cash functionality will be enabled at the Sentinel
	bool m_PBTPromoEnable;			//When set, PersonalBanker Promo downloads are enabled at the Sentinel.
	bool m_displayComp;				//When set, the players comp total will be displayed on Card-In
	bool m_PBTPointsAsPromo;		//When set, Points will be downloaded as Promo Credits instead of Cash at the Sentinel
	bool m_coinsWonAccounting;		//Not Used. OASIS system has never implemented Coins Won Accounting.
	bool m_coinsInAccounting;		//When set, points will be earned based on the amount of Coin In played.
	bool m_displayCongrats;			//When set, a congratulations message will be displayed when a point is earned.
	bool m_theoreticalAccounting;	//When set, points will be earned based on the theoretical win.
	bool m_coinsOutAccounting;		//When set, points will be earned based on the amount of Coin out.
	bool m_cardInPBT;				//When set on Card-In immediately go to PBT menus
	bool m_employeePinRequired;		//When set all employee cards will require a PIN number to access menu options.
	bool m_gameZeroProgressiveIn;	//Not used. 
	bool m_partialBonusAward;		//
	bool m_displayPtsTotalAsCash;	//Point Total will display on Card-In as Cash
	bool m_displayGoodLuckAndSession;//Good Luck / Session (numeric)
	bool m_couponPromoInEnabled;	//When set, the Sentinel will redeem Promo Coupons 
	bool m_voucherPromoInEnabled;	//When set Voucher Promos will be accepted and redeemed by the Sentinel
	bool m_virtualSessionEnabled;	//When set, Virtual Sessions are enabled. The Sentinel will keep the Card inserted until the end of the current game.
	bool m_ticketInEnabled;			//When set, tickets can be inserted and redeemed.
	bool m_useNewPBMenus;			//Not used in PRIME. Used previously in 11.5.2NV and older.
	bool m_PBFromGameEnabled;		//When set, PersonalBanker uploads (From Game) will be enabled.
	bool m_displayPlayerPtsTotal;	//Will display the players point total as numeric for the session display 
	bool m_Lockout;					//Not used.

};
