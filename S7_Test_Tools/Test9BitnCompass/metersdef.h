/*Base Machine Meters*/

#pragma once

const long INVALID_METER_NUMBER = -1;

enum oasisMeters {
	
	mNone	 = 0,	// No meter
	mCD	= 1,	//{Coin Drop}
	mSDr	= 2,	//{Slot Door}
	mDDr	= 3,	//{Drop Door}
	mLDr	= 4,	//{Logic Door}
	mCDr	= 5,	//{Cash Door}
	mFDr	= 6,	//{Aux Fill Door}
	mPBi_NA  = 7,   //{Personal Banker - Cashless In}
	mPtR	= 8,	//{Personal Banker - Point Redemption to Credit Meter}
	mPBo_NA  = 9,   //{Personal Banker - Cashless Out}
	mPBp_NA  = 10,  //{Personal Banker - Cashless Promo In}
	mPBn	 =11,	//{Personal Banker - NonCashable In}
	mD1		= 12,	//{Ones}
	mD2		= 13,	//{Twos}
	mD5		= 14,	//{Fives}
	mD10	= 15,	//{Tens}
	mD20	= 16,	//{Twenties}
	mD50	= 17,	//{Fifties}
	mD100	= 18,	//{Hundreds}
	mCoup	= 19,	//{Coupons}
	mTO	= 20,	//{Ticket Out}
	mHop	= 21,	//{Hopper Level}
	mCoupV =  22,   //{Coupon Value}
	mTVal  =  23,   //{Ticket Value}??
	mDr    =  24,   //{Door}
	mCrd   =  25,   //{Credit Meter}
	mTCi   =  26,   //{True Coin In}
	mTCo   =  27,   //{True Coin out}
	mBD     = 28,   //{Bill Drop Credits}
	mTHP    = 29,   //{Total Hand Pay}
	mCTktCN = 30,   //{coupon ticket cash number}
	mCTktCV  =31,   //{coupon ticket cash value}
	mCTktPN  =32,   //{Coupon ticket Promo number}
	mCTktPV = 33,   //{Coupon ticket Promo Value}
	mHPCshRcptCnt   = 34,
	mHPCshRcptVal   = 35,
	mHPWinRcptCnt   = 36,
	mHPWinRcptVal   = 37,
	mActualCD       = 38,  //{Actual coin drop}
	mCPromoOutVal   = 39,
	mCPromoOutCnt   = 40,
	
	mUsr1   = 41,   //{User-defined meter #1 -- for use with generic pins.}
	mUsr2   = 42,   //{User-defined meter #2 -- for use with generic pins.}
	mUsr3   = 43,   //{User-defined meter #3 -- for use with generic pins.}
	mUsr4   = 44,   //{User-defined meter #4 -- for use with generic pins.}
	mSelG   = 45,   //{Current selected game number -- for multi-game slots.}
	mNumG   = 46,
	mBDr    = 47,   //{Belly Door}
	mDivr   = 48,
	mRBP    = 49,
	mSDSClk  =50,
	mSDSDat  =51,
	mITGTP   =52,
	
	/*Game Meters - Individual Games  GameMeter + (18 * GameNum)*/
	gCI		= 53,	//{Coin In}
	gCO		= 54,	//{Coin Out}
	gGS		= 55,	//{Game Start}
	gGW		= 56,	//{Games Won}
	gMC		= 57,	//{Max Coin Bet}
	gCC		= 58,	//{Cancel Credit}
	gBW		= 59,	//{Bonus Won.  Not used.}
	gJP		= 60,	//{Jackpot Hand Pay}
	gPI		= 61, 	//{Progressive Coin In}
	gEJP	= 62,	//{Electronic Jack Pot.  Obsolete.}
	gCW		= 63,	//{Credit Won.  Not used.}
	gTJP		= 64,	//{Total Jackpot.  Obsolete.}
	
	
	mHCC        = 66, //{Hand-paid Canceled Credits.}
	//{coind drop meter --- see mActualCD meter #38}
	//{mCDC         67} {Coin Drop (Credits) -- NOT Total Drop.}
	mActualSlotDr = 67,
	mD200       = 68, //{Two Hundreds}
	mD500       = 69, //{Five Hundreds}
	mD1000      = 70, //{One Thousands}
	mPbAC2G     = 71, //{Personal Banker AFT Cashable To Game}
	mPbAR2G     = 72, //{Personal Banker AFT Restricted To Game}
	mPbAN2G     = 73, //{Personal Banker AFT NonResticted To Game}
	mPbAC2H     = 74, //{Personal Banker AFT Cashable To Host}
	mPbAR2H     = 75, //{Personal Banker AFT Restricted To Host}
	mPbAN2H     = 76, //{Personal Banker AFT NonResticted To Host}
	mPbAD2G     = 77, //{AFT Debit Card to Game}
	mBonusMach  = 78, //{External Bonusing machine payout}
	mBonusAtt   = 79, //{External Bonusing Attendant payout}
	mPrgPayAtt  = 80, //{Attendant Pay Progressive payout}
	mPrgPayMac  = 81, //{Machine Pay Progressive payout}
	mTValueCI   = 82, //{Physical Coin In - VALUE}
	mTValueCO   = 83, //{Physical Coin Out - Value}
	mRstrPlayd  = 84, //{Sum of all restricted xfers that were played(not cashed out etc) at the game}
	mURstrPlayd = 85, //{Sum of all UNrestricted xfers that were played(not cashed out etc) at the game}
	mGameWTPP   = 86, //{Weighted Theoretical Payback Percentage}
	mTotalDrop	= 87, //{Total Drop should include bill drop and ticket drop}
	mVchrTktDrop = 88,	//{Total voucher and Ticket drop meter}
	mChassisDoor = 89,	//The Chassis door access security switch
	// New meters for 'No Points For Promo' functionality
	mNcepCredits = 90, //{Current NCEP (non-cashable restricted promotional) credits on credit meter.}
	mNonNcepPlayed = 91, //{Sum of credits that were played that were NOT in meter 84.}
	mMeterLast  //
};

enum subgameMeters
{
	SUBGAME_METER_GAMES_PLAYED = 0,
	SUBGAME_METER_COIN_IN = 1,
	SUBGAME_METER_COIN_OUT = 2,
	SUBGAME_METER_ATT_PAID_JACKPOT = 3,
	SUBGAME_METER_GAMES_WON = 4,
	SUBGAME_METER_ATT_PAID_PROG = 5,
	SUBGAME_METER_ATT_PAID_EXT_BONUS = 6,
	SUBGAME_METER_MACH_PAID_PROG = 7,
	SUBGAME_METER_MACH_PAID_EXT_BONUS = 8,
	SUBGAME_METER_WTPP = 9,
	SUBGAME_METER_LAST
};

const long INVALID_METER_VALUE = -1;

typedef enum 
{
	SnapshotNone			= 0,
    MeterCardIn             = 1,
    MaintenancePlayBegin    = 2,
    MaintenancePlayEnd      = 3,
    EGMRAMClear             = 4,
    InitialMeters           = 5,
    MpDropDoor              = 6,
    MpCashDoor              = 7,
    MpCurrencyCard          = 8,
    FinalMeters             = 9,
    MachineMoveBegin        = 10,
    MachineMoveEnd          = 11,
} SnapshotType;
