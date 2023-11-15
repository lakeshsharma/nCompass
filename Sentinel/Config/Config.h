#pragma once

#define NUM_SEN_OPTIONS 42 // Must be equal to _countof(TXT_SEN_OPTIONS).
#define NUM_GLOBAL_OPTIONS   20

#include "NVRAMConfig.h"
#include "EEPROMConfig.h"
#include "../Hardware/MemoryStorage.h"
#include "../IPAddress.h"
#include "../Utilities.h"
#include "../Ticket.h"
#include "../GlobalDefs.h"
#include "DataSubscriptionProvider.h"

const std::string NO_EXTENSIBLE_SETTINGS_TOPIC = "NoExtensibleSettings";

union u_ExtensibleFlags
{
	// first representation (member of union) 
	struct tExtensibleFlags
	{
		DWORD isPoller_12_11_6000_or_Higher	 :  1; //Bit0
		DWORD isNFDPoller				     :  1; //Bit1
		DWORD displayBonusCounterInAttractor :  1; //Bit2
		DWORD ticketInResend				 :  1; //Bit3
		DWORD restartUIAfterxDays			 :	8; //Bit4-11
		DWORD sasReadTimeOutMultiplier		 :  5; //Bit12-16
		DWORD useSDSCardFormat				 :  1; //Bit17
		DWORD enableLocationServices		 :  1; //Bit18
		DWORD spare							 : 13; //Bit19-31
	} m_data;
     
	// second representation (member of union) 
	DWORD m_wordValue;

	u_ExtensibleFlags()
	{
		Reset();
	}

	void Reset()
	{
		m_wordValue = 0;
		m_data.restartUIAfterxDays = DEFAULT_RESTART_UI_AFTER_X_DAYS;
		m_data.sasReadTimeOutMultiplier = DEFAULT_SAS_READ_TIMEOUT_MULTIPLIER;
	}

	bool IsPoller_12_11_6000_or_Higher() const
	{
		return m_data.isPoller_12_11_6000_or_Higher;
	}

	bool IsNFDPoller() const
	{
		return m_data.isNFDPoller;
	}

	bool TicketInResend() const
	{
		return m_data.ticketInResend;
	}

	DWORD RestartUIAfterxDays() const
	{
		return m_data.restartUIAfterxDays;
	}

	DWORD GetSASReadTimeOutMultiplier() const
	{
		return m_data.sasReadTimeOutMultiplier;
	}

	bool UseSDSCardFormat() const
	{
		return m_data.useSDSCardFormat;
	}

	bool EnableLocationServices() const
	{
		return m_data.enableLocationServices;
	}
};

class CConfigExtFile; // Declare to avoid circular includes.

typedef enum
{
	ACCOUNTING_COINS_IN = 0,
	ACCOUNTING_COINS_OUT,
	ACCOUNTING_THEORETICAL,
	ACCOUNTING_COINS_WON,
	ACCOUNTING_CASHABLE_PLAYED,
	ACCOUNTING_CASHABLE_THEORETICAL,
} AccountingType;


class CConfig : public CDataSubscriptionProvider
{
public:
	CConfig(bool isLcd, const std::string& firmwareVersion);
	virtual ~CConfig(void);

	virtual void BuildYourself();

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

private:
	void SetOffsets();
	void InitializeMACAddress();

public:

	/**************************
	**    NVRAM Items	     **
	**************************/
	

	//TBD Logs

	DWORD ConfigNVRAMSize;

	//-----------------------------------------
	// Config from SCF
	//-----------------------------------------
		WORD SetDenomination(WORD denomination);

		/// <summary>
		/// Returns the denomination.
		/// </summary>
		/// <returns>The denomination.</returns>
		WORD GetDenomination() const;

		WORD SetSlotMastRevision(WORD slotMastRevision);
		WORD GetSlotMastRevision();

		/// <summary>
		/// Sets the casino number.
		/// </summary>
		/// <param name="casinoNumber">IN - The new value for casino number.</param>
		/// <returns>The previous value for casino number.</returns>
		byte SetCasinoNumber(byte casinoNumber);

		/// <summary>
		/// Gets the casino number.
		/// </summary>
		/// <returns>The casino number.</returns>
		byte GetCasinoNumber() const;

		/// <summary>
		/// Sets the CommID.
		/// </summary>
		/// <param name="commID">IN - The CommID.</param>
		/// <returns>The previous CommID.</returns>
		BYTE SetCommID(BYTE commID);

		/// <summary>
		/// Gets the CommID.
		/// </summary>
		/// <returns>The CommID.</returns>
		BYTE GetCommID() const;
		
		//TBD The SenOptions Go In Here

		/// <summary>
		/// Sets the slot number.
		/// </summary>
		/// <param name="slotNumber">IN - The new value for the slot number.</param>
		/// <returns> The previous value for the slot number.</returns>
		int SetSlotNumber(int slotNumber);

		/// <summary>
		/// Gets the slot number.
		/// </summary>
		/// <returns> The slot number.</returns>
		int GetSlotNumber() const;

		/// <summary>
		/// Sets the player card prefixes.
		/// </summary>
		/// <param name="playerCardsPrefix">IN - The player card prefixes.</param>
		void SetPlayerCardPrefixes(const std::vector<BYTE> &playerCardsPrefix);

		/// <summary>
		/// Gets the player card prefixes.
		/// </summary>
		/// <returns>A vector of valid card player prefixes.</returns>
		const std::vector<BYTE> GetPlayerCardPrefixes() const;

		/// <summary>
		/// Determines whether "prefix" is a valid player card prefix.
		/// </summary>
		/// <param name="prefix">IN - The prefix to check.</param>
		/// <param name="isUniverseCard">IN - true if the card is a Universe card, false otherwise.</param>
		/// <returns>true if "prefix" is a valid player card prefix, false otherwise.</returns>
		bool IsPlayerCardPrefix(byte prefix, bool isUniverseCard);

		/// <summary>
		/// Gets the RCCL card player prefix.
		/// </summary>
		/// <returns>The RCCL card player prefix.</returns>
		BYTE GetRcclCardPlayerPrefix() const;

		/// <summary>
		/// Sets the global card prefix.
		/// </summary>
		/// <param name="globalCardPrefix">IN - The global card prefix.</param>
		/// <returns>The previous value for the global card prefix.</returns>
		BYTE SetGlobalCardPrefix(BYTE globalCardPrefix);

		/// <summary>
		/// Gets the global card prefix.
		/// </summary>
		/// <returns>The value for the global card prefix.</returns>
		BYTE GetGlobalCardPrefix() const;

		/// <summary>
		/// Sets the mechanic card prefix.
		/// </summary>
		/// <param name="mechanicCardPrefix">IN - The mechanic card prefix.</param>
		/// <returns>The previous value for the mechanic card prefix.</returns>
		BYTE SetMechanicCardPrefix(BYTE mechanicCardPrefix);

		/// <summary>
		/// Gets the mechanic card prefix.
		/// </summary>
		/// <returns>The value for the mechanic card prefix.</returns>
		BYTE GetMechanicCardPrefix() const;

		/// <summary>
		/// Sets the floor card prefix.
		/// </summary>
		/// <param name="floorCardPrefix">IN - The floor card prefix.</param>
		/// <returns>The previous value for the floor card prefix.</returns>
		BYTE SetFloorCardPrefix(BYTE floorCardPrefix);

		/// <summary>
		/// Gets the floor card prefix.
		/// </summary>
		/// <returns>The value for the floor card prefix.</returns>
		BYTE GetFloorCardPrefix() const;

		/// <summary>
		/// Sets the temporary card prefix.
		/// </summary>
		/// <param name="tempCardPrefix">IN - The temporary card prefix.</param>
		/// <returns>The previous value for the temporary card prefix.</returns>
		BYTE SetTempCardPrefix(BYTE tempCardPrefix);

		/// <summary>
		/// Gets the temporary card prefix.
		/// </summary>
		/// <returns>The value for the temporary card prefix.</returns>
		BYTE GetTempCardPrefix() const;

		/// <summary>
		/// Sets the casino name.
		/// </summary>
		/// <param name="casinoName">IN - The new value for casino name.</param>
		void SetCasinoName(LPCTSTR casinoName);

		/// <summary>
		/// Gets the casino name.
		/// </summary>
		/// <returns>A pointer to the casino name.</returns>
		const std::string &GetCasinoName() const;

		/// <summary>
		/// Sets line 1 of the casino address.
		/// </summary>
		/// <param name="casinoAddress1">IN - The new value for line 1 of the casino address.</param>
		void SetCasinoAddress1(LPCTSTR casinoAddress1);

		/// <summary>
		/// Gets line 1 of the casino address.
		/// </summary>
		/// <returns>A pointer to line 1 of the casino address.</returns>
		const std::string &GetCasinoAddress1() const;

		/// <summary>
		/// Sets line 1 of the casino address.
		/// </summary>
		/// <param name="casinoAddress2">IN - The new value for line 1 of the casino address.</param>
		void SetCasinoAddress2(LPCTSTR casinoAddress2);

		/// <summary>
		/// Gets line 2 of the casino address.
		/// </summary>
		/// <returns>A pointer to line 2 of the casino address.</returns>
		const std::string &GetCasinoAddress2() const;

		int SetPointsPerDollar(int pointsPerDollar);

		/// <summary>
		/// Returns the points per dollar.
		/// </summary>
		/// <returns>The points per dollar.</returns>
		int GetPointsPerDollar() const;

		bool SetLossLimitActive(bool lossLimitActive);

		/// <summary>
		/// Returns true if loss limit is active, false otherwise.
		/// </summary>
		/// <returns></returns>
		bool GetLossLimitActive() const;

		int SetLossLimitSessionNumber(int lossLimitSessionNumber);
		int GetLossLimitSessionNumber();

		WORD SetLossLimitSessionAmount(WORD lossLimitSessionAmount);
		WORD GetLossLimitSessionAmount();

		WORD SetLossLimitBillValidatorOffAmount(WORD lossLimitBillValidatorOffAmount);
		WORD GetLossLimitBillValidatorOffAmount();

		/// <summary>
		/// Sets the type of the ticket expiration.
		/// </summary>
		/// <param name="expType">IN - The new value for the type of the expiration.</param>
		/// <returns>The previous value for the type of the ticket expiration.</returns>
		TicketExpirationType SetTicketExpirationType( TicketExpirationType expType);

		/// <summary>
		/// Gets the type of the ticket expiration.
		/// </summary>
		/// <returns>The type of the ticket expiration.</returns>
		TicketExpirationType GetTicketExpirationType() const;

		/// <summary>
		/// Sets the duration of the ticket expiration.
		/// </summary>
		/// <param name="expirationDuration">IN - The new value for the duration of the expiration.</param>
		/// <returns>The previous value for the duration of the ticket expiration.</returns>
		int SetTicketExpirationDuration( int expirationDuration );

		/// <summary>
		/// Gets the duration of the ticket expiration.
		/// </summary>
		/// <returns>The duration of the ticket expiration.</returns>
		int GetTicketExpirationDuration() const;

		/// <summary>
		/// Sets whether ticketing is enabled.
		/// </summary>
		/// <param name="ticketingEnabled">IN - The new value for ticketing enabled -- true for enabled, false otherwise.</param>
		/// <returns>The previous value for ticketing enabled -- true for enabled, false otherwise.</returns>
		bool SetTicketingEnabled( bool ticketingEnabled );

		/// <summary>
		/// Returns true if ticketing is enabled, false otherwise.
		/// </summary>
		/// <returns>true if ticketing is enabled, false otherwise.</returns>
		bool TicketingEnabled() const;

		bool SetSystemGeneratedTicketValidation ( bool systemGenerated );
		bool SystemGeneratedTicketValidation();

		/// <summary>
		/// Sets whether ticket buffering support is enabled.
		/// </summary>
		/// <param name="tickBuffEnabled">IN - The new value for ticket buffering support enabled -- true for enabled, false otherwise.</param>
		/// <returns>The previous value for ticket buffering support enabled -- true for enabled, false otherwise.</returns>
		bool SetTicketBufferingEnabled( bool tickBuffEnabled );

		/// <summary>
		/// Returns true if ticket buffering support is enabled, false otherwise.
		/// </summary>
		/// <returns>true if ticket buffering support is enabled, false otherwise.</returns>
		bool TicketBufferingEnabled() const;

		WORD SetPlayerTrackingMultiplier( WORD playerTrackingMult );
		int GetPlayerTrackingMultiplier();

		WORD SetPlayerTrackingDivisor( WORD playerTrackingDiv );
		int GetPlayerTrackingDivisor();

		DWORD SetVirtualSessionNoPlayTimeout(DWORD virtualSessionNoPlayTimeout);
		DWORD GetVirtualSessionNoPlayTimeout();

		DWORD SetVirtualSessionMaxDuration(DWORD virtualSessionMaxDuration);
		DWORD GetVirtualSessionMaxDuration();

		DWORD SetCardTimeout(DWORD cardTimeout);
		DWORD GetCardTimeout();

		bool SetFloorLock(bool floorLock);
		bool FloorLock();

		bool SetAllowFloorLockout(bool allowFloorLockout);
		bool AllowFloorLockout();

		short SetTimezoneOffset( short timezoneOffset );
		short GetTimezoneOffset( void );

		bool SetUseDST( bool useDST );
		bool UseDST( void );

		bool SetAccruePointsForNCEP(bool accruePointsForNCEP);
		bool GetAccruePointsForNCEP();
		bool GetGlobalAccruePointsForNCEP();

		bool SetAccrueCompForNCEP(bool accrueCompForNCEP);
		bool GetAccrueCompForNCEP();
		bool GetGlobalAccrueCompForNCEP();

		bool SetSenOpAccruePointsForNCEP(bool accruePointsForNCEP);
		bool SetSenOpAccrueCompForNCEP(bool accrueCompForNCEP);

		/// <summary>
		/// Returns true if there is an EGM game in progress, false otherwise.
		/// </summary>
		/// <returns>true if there is an EGM game in progress, false otherwise.</returns>
		/// <remarks>
		/// Note: Returns false after startup regardless of the last known EGM state.
		/// </remarks>
		bool GetGameInProgress() const;

		/// <summary>
		/// Sets whether there is an EGM game in progress.
		/// </summary>
		/// <param name="gameInProgress">IN - true if there is an EGM game in progress, false otherwise.</param>
		/// <returns>The previous value for whether there is an EGM game in progress.</returns>
		/// <remarks>
		/// Note: Set false at startup regardless of the last known EGM state.
		/// </remarks>
		bool SetGameInProgress(bool gameInProgress);

		bool GetServiceWindowOpen();
		bool SetServiceWindowOpen(bool serviceWindowOpen);

	//---------------------------------
	// END of Config From SCF
	//---------------------------------

	void SetBonusTable(BYTE *data);
	int GetBonusTable();
	
	bool SetEGMLockoutState(bool EGMLockoutState);
	bool GetEGMLockoutState();

	bool SetMemoryLost(bool memoryLost);
	bool GetMemoryLost();
	
	int SetLastCoinsPlayed(int lastCoinsPlayed);
	int GetLastCoinsPlayed() const;
	

	void SetLastTicketInfo(BYTE *ticket);//TBD not sure if this should be handled by ticketing
	BYTE *GetLastTicketInfo();
	
	int SetLastPBTransactionID(int lastPBTransactionID);
	int GetLastPBTransactionID();

	bool SetPBResponsePending(bool PBResponsePending);
	bool GetPBResponsePending();

	bool SetPBTransferInProgress(bool PBTransferInProgress);
	bool GetPBTransferInProgress();

	int SetCurrentPBTransactionID(int currentPBTransactionID);
	int GetCurrentPBTransactionID();

	int SetPBTransferSequenceNumber(int PBTransferSequenceNumber);
	int GetPBTransferSequenceNumber();

	int SetPBCreditSource(int PBCreditSource);
	int GetPBCreditSource();

	DWORD SetPBTransferAmount(DWORD PBTransferAmount);
	DWORD GetPBTransferAmount();

	void SetPBLog();
	LPCTSTR GetPBLog();

	void SetPBValidLog();
	LPCTSTR GetPBValidLog();

	/***************
	** TBD boundaries should be an object
	***************/
	//bool SetBonusEventBoundaries(int boundaries);
	//int GetBonusEventBoundaries();

	bool SetPBReleaseNeeded(bool PBReleaseNeeded);
	bool GetPBReleaseNeeded();

	bool SetTicketVendResponseInProgress(bool inProgress);//TBD should be handeled by ticketing
	bool GetTicketVendResponseInProgress();

	int SetEmployeeAuthorizationOCR(int employeeAuthorizationOCR);
	int GetEmployeeAuthorizationOCR();

	void SetTicketVendResponse();//TBD Ticket object needed
	void GetTicketVendResponse();//TBD: Ticket Object

	void SetAttractorMessage(LPCTSTR attractorMessage);
	LPCTSTR GetAttractorMessage();

	void SetGoodCardReadMessage(LPCTSTR goodCardReadMessage);

	/// <summary>
	/// Returns a pointer to the good card read message.
	/// </summary>
	/// <returns>A pointer to the good card read message.</returns>
	LPCTSTR GetGoodCardReadMessage() const;

	void SetBadCarReadMessage(LPCTSTR badCarReadMessage);
	LPCTSTR GetBadCarReadMessage();

	void SetBirthdayMessage(LPCTSTR birthdayMessage);

	/// <summary>
	/// Returns a pointer to the birthday message.
	/// </summary>
	/// <returns>A pointer to the birthday message.</returns>
	LPCTSTR GetBirthdayMessage() const;

	void SetAnniversaryMessage(LPCTSTR anniversaryMessage);

	/// <summary>
	/// Returns a pointer to the anniversary message.
	/// </summary>
	/// <returns>A pointer to the anniversary message.</returns>
	LPCTSTR GetAnniversaryMessage() const;

	void SetCongratulationsMessage(LPCTSTR congratulationsMessage);

	/// <summary>
	/// Returns a pointer to the congratulations message.
	/// </summary>
	/// <returns>A pointer to the congratulations message.</returns>
	LPCTSTR GetCongratulationsMessage() const;

	void SetWelcomeMessage(LPCTSTR welcomeMessage);

	/// <summary>
	/// Returns a pointer to the welcome message.
	/// </summary>
	/// <returns>A pointer to the welcome message.</returns>
	LPCTSTR GetWelcomeMessage() const;

	/// <summary>
	/// Sets whether to only diplay session points and what text to display.
	/// </summary>
	/// <param name="message">IN - text to display. NULL or empty string means display as normal.</param>
	void SetSessionPointsMessage(LPCTSTR message);

	/// <summary>
	/// Returns whether to display only session points and what text to display.
	/// </summary>
	/// <returns>Session Points text string. NULL or empty string means display as normal.</returns>
	LPCTSTR GetSessionPointsMessage() const;

	/// <summary>
	/// Sets text to display if a Balance Retrieval.
	/// </summary>
	/// <param name="message">IN - text to display.</param>
	void SetBalanceRetrievalMessage(LPCTSTR message);

	/// <summary>
	/// Returns text to display if a Balance Retrieval.
	/// </summary>
	/// <returns>Balance Retrieval text string.</returns>
	LPCTSTR GetBalanceRetrievalMessage() const;

	/// <summary>
	/// Sets the daily points display suffix message.
	/// </summary>
	/// <param name="dailyPointsDisplaySuffixMessage">The daily points display suffix message.</param>
	void SetDailyPointsDisplaySuffixMessage(LPCTSTR dailyPointsDisplaySuffixMessage);

	/// <summary>
	/// Gets the daily points display suffix message.
	/// </summary>
	/// <returns></returns>
	LPCTSTR GetDailyPointsDisplaySuffixMessage() const;

	// UI_REWRITE_CLEANUP, seems we fill below from 401 but these are not used. Eventually we use from LcdShell.xml,
	// like BalanceTierPointText.
	/// <summary>
	/// Sets the tier points label.
	/// </summary>
	/// <param name="tierPointsLabel">The tier points label.</param>
	void SetTierPointsLabel(LPCTSTR tierPointsLabel);

	/// <summary>
	/// Gets the tier points label.
	/// </summary>
	/// <returns>tier points label</returns>
	LPCTSTR GetTierPointsLabel() const;

	/// <summary>
	/// Sets label to display over Points/Cash.
	/// </summary>
	/// <param name="pointsOrCashLabel">IN - label to display.</param>
	void SetPointsOrCashLabel(LPCTSTR pointsOrCashLabel);

	/// <summary>
	/// Returns label to display over Points/Cash.
	/// </summary>
	/// <returns>Label string.</returns>
	LPCTSTR GetPointsOrCashLabel() const;

	/// <summary>
	/// Sets whether to show Points or Account Balance.
	/// </summary>
	/// <param name="showPointsVsAccountBalance">IN - 'true' if to show Points. Otherwise, show Account Balance.</param>
	/// <returns>Original value.</returns>
	bool SetShowPointsVsAccountBalance(bool showPointsVsAccountBalance);

	/// <summary>
	/// Returns whether to show Points or Account Balance.
	/// </summary>
	/// <returns>'true' if to show Points. Otherwise, show Account Balance.</returns>
	bool GetShowPointsVsAccountBalance() const;

	// Touch Screen Protocol settings.
	byte SetTouchScreenProtocolType(byte touchScreenProtocolType);
	byte GetTouchScreenProtocolType();

	WORD SetTouchScreenProtocolVendorId(WORD touchScreenProtocolVendorId);
	WORD GetTouchScreenProtocolVendorId();

	DWORD SetTouchScreenProtocolBaud(DWORD touchScreenProtocolBaud);
	DWORD GetTouchScreenProtocolBaud();

	void SetTouchScreenProtocolName(LPCTSTR touchScreenProtocolName);
	const std::string &GetTouchScreenProtocolName();

	byte IncrementMixerRebootCount();
	byte ResetMixerRebootCount();
	byte SetMixerRebootCount( byte rebootCount );
	byte GetMixerRebootCount();

	bool SetVideoInputModeToAutoDetect( bool autoDetect );
	bool IsVideoInputModeAutoDetect() const;

	bool SetVideoOutputModeToConfig( bool autoConfig, bool nativeConfig );
	bool IsVideoOutputModeAutoConfig() const;
	bool IsVideoOutputModeNativeConfig() const;

	DesktopOrientationType SetOrientationType( DesktopOrientationType rotationType );
	DesktopOrientationType GetOrientationType() const;

public:

	/******************************
	***   EEPROM Items          ***
	******************************/

	int SetSlotMastID(int slotMastID);
	int GetSlotMastID();

	const std::string &GetNetworkAdapterName() const;

	/// <summary>
	/// Gets a pointer to the primary MAC address as a string.
	/// </summary>
	/// <returns>A pointer to the primary MAC address as a string.</returns>
	const std::string &GetMACAddressAsString() const;

	/// <summary>
	/// Gets a pointer to the secondary MAC address as a string.
	/// </summary>
	/// <returns>A pointer to the secondary MAC address as a string.</returns>
	const std::string &GetMACAddress2AsString() const;

	/// <summary>
	/// Gets a pointer to the primary MAC address as a BYTE array.
	/// </summary>
	/// <returns>A pointer to the parimary MAC address as a BYTE array.</returns>
	const BYTE * GetMacAddressAsBytes() const;

	/// <summary>
	/// Gets a pointer to the secondary MAC address as a BYTE array.
	/// </summary>
	/// <returns>A pointer to the secondary MAC address as a BYTE array.</returns>
	const BYTE * GetMacAddress2AsBytes() const;

	byte SetMachineType(byte machineType);
	byte GetMachineType();

	byte SetHardpinType( byte hardPinType, int hardPinNumber );
	byte GetHardpinType( int hardPinNumber );

	bool SetHardpinActiveState( bool bActiveStateHigh, int hardPinNumber );
	bool GetHardpinActiveState( int hardPinNumber );

	byte SetSerialProtocol(byte serialProtocol);
	byte GetSerialProtocol();

	//int SetTouchScreenInfo(int touchScreenInfo);
	//int GetTouchScreenInfo();

	byte SetScreenBrightness(byte screenBrightness);
	byte GetScreenBrightness();

	void SetSpeakerVolume(byte leftSpeakerVolume, byte rightSpeakerVolume);
	byte GetLeftSpeakerVolume();
	byte GetRightSpeakerVolume();

	/// <summary>
	/// Sets the global card password.
	/// </summary>
	/// <param name="globalCardPassword">IN - The global card password.</param>
	/// <returns>The previous value for the global card password.</returns>
	WORD SetGlobalCardPassword(WORD globalCardPassword);

	/// <summary>
	/// Gets the global card password.
	/// </summary>
	/// <returns>The global card password.  Returns an empty string if there is no global card password.</returns>
	std::string GetGlobalCardPassword() const;

//	bool SetPlayerPointMultiplier(int playerPointMultiplier);
//	int GetPlayerPointMultiplier();

	/// <summary>
	/// Writes the host address (poller IP) into EEPROM memory.
	/// </summary>
	/// <param name="hostAddressOrName">IN - host address/name of poller.</param>
	/// <returns>True - if passed hostAddressOrName is different from existing hostAddress. Else returns false.</returns>
	bool SetHostAddressOrName(const std::string& hostAddressOrName);
	char * GetHostAddressOrName();
	
	int SetHostPort( int port );
	int GetHostPort( void );

	bool SetDHCPConfig(bool DHCPAuto);
	bool GetDHCPConfig();

	void SetMyIP(const std::string& myIP);
	void SetMyIP(CIPAddress const &ipAddress);
	void GetMyIP(CIPAddress &ipAddress);

	void SetDefaultGateway(const std::string& defaultGateway);
	void SetDefaultGateway(CIPAddress const &ipAddress);
	void GetDefaultGateway(CIPAddress &ipAddress);

	void SetSubnetMask(const std::string& subnetMask);
	void SetSubnetMask(CIPAddress const &ipAddress);
	void GetSubnetMask(CIPAddress &ipAddress);

	void SetPreferredDNSServer(const std::string& preferredDnsServer);
	void SetPreferredDNSServer(CIPAddress const &ipAddress);
	void GetPreferredDNSServer(CIPAddress &ipAddress);

	void SetAlternateDNSServer(const std::string& alternateDnsServer);
	void SetAlternateDNSServer(CIPAddress const &ipAddress);
	void GetAlternateDNSServer(CIPAddress &ipAddress);

	void SetDomain(const std::string& domain);
	std::string GetDomain() const;

	void SetEGMProtocolState( EGMProtocolState protocolState );
	EGMProtocolState GetEGMProtocolState( void );
	
	void SetSentinelOnlineState( SentinelOnlineState state, bool bSetInPersistMemory = true );
	SentinelOnlineState GetSentinelOnlineState( void );

	WORD SetGameDenom(WORD gameDenom);

	/// <summary>
	/// Returns the game denomination.
	/// </summary>
	/// <returns>The game denomination.</returns>
	WORD GetGameDenom() const;

	void SetBatteryStatus( BatteryStatus batteryStatus );
	BatteryStatus GetBatteryStatus( void );
	std::string GetBatteryStatusText() const;

	void SetLastBatteryCheckTime( time_t const &checkTime );
	time_t GetLastBatteryCheckTime( void );

	/// <summary>
	/// Sets whether to use the EGM hopper level.
	/// </summary>
	/// <param name="VirtualSessionEnabled">IN - true to use the EGM hopper level, false otherwise.</param>
	/// <returns>The previous value for whether to use the EGM hopper level.</returns>
	bool SetUseHopperLevel(bool useHopperLevel);
	
	/// <summary>
	/// Returns true to use the EGM hopper level, false otherwise.
	/// </summary>
	/// <returns>true if to use the EGM hopper level, false otherwise.</returns>
	bool UseHopperLevel() const;

	/// <summary>
	/// Sets whether (normal) virtual sessions are enabled.
	/// </summary>
	/// <param name="VirtualSessionEnabled">IN - true if (normal) virtual sessions are enabled, false otherwise.</param>
	/// <returns>The previous value for whether (normal) virtual sessions are enabled.</returns>
	bool SetVirtualSessionEnabled(bool VirtualSessionEnabled);

	/// <summary>
	/// Returns true if (normal) virtual sessions are enabled, false otherwise.
	/// </summary>
	/// <returns>true if (normal) virtual sessions are enabled, false otherwise.</returns>
	bool VirtualSessionEnabled() const;

	bool SetDisplayPlayerPtsTotal(bool displayPlayerPtsTotal);

	/// <summary>
	/// Returns true if displaying the player points total is enabled, false otherwise.
	/// </summary>
	/// <returns>true if displaying the player points total is enabled, false otherwise.</returns>
	bool DisplayPlayerPtsTotal() const;

	/// <summary>
	/// Sets whether displaying the player name is enabled.
	/// </summary>
	/// <param name="VirtualSessionEnabled">IN - true if displaying the player name is enabled, false otherwise.</param>
	/// <returns>The previous value for whether displaying the player name is enabled.</returns>
	bool SetDisplayPlayerName(bool displayPlayerName);

	/// <summary>
	/// Returns true if displaying the player name is enabled, false otherwise.
	/// </summary>
	/// <returns>true if displaying the player name is enabled, false otherwise.</returns>
	bool DisplayPlayerName() const;

	bool SetDisplayCountdownAndSession(bool displayCountdownAndSession);

	/// <summary>
	/// Returns true if displaying countdown and session is enabled, false otherwise.
	/// </summary>
	/// <returns>true if displaying countdown and session is enabled, false otherwise.</returns>
	bool DisplayCountdownAndSession() const;

	bool SetDisplayGoodLuckAndSession(bool displayGoodLuckAndSession);

	/// <summary>
	/// Returns true if displaying good luck and session is enabled, false otherwise.
	/// </summary>
	/// <returns>true if displaying good luck and session is enabled, false otherwise.</returns>
	bool DisplayGoodLuckAndSession() const;

	bool SetDisplayGoodLuckAndSessionPts(bool displayGoodLuckAndSessionPts);

	/// <summary>
	/// Returns true if displaying good luck and session points is enabled, false otherwise.
	/// </summary>
	/// <returns>true if displaying good luck and session points is enabled, false otherwise.</returns>
	bool DisplayGoodLuckAndSessionPts() const;

	bool SetDisplayNumericPointTotal(bool displayNumericPointTotal);

	/// <summary>
	/// Returns true if displaying numeric point total is enabled, false otherwise.
	/// </summary>
	/// <returns>true if displaying numeric point total is enabled, false otherwise.</returns>
	bool DisplayNumericPointTotal() const;

	bool SetDisplayComps(bool displayComps);

	/// <summary>
	/// Returns true if displaying comps is enabled, false otherwise.
	/// </summary>
	/// <returns>true if displaying comps is enabled, false otherwise.</returns>
	bool DisplayComps() const;

	//bool SetDisplayCash(bool displayCash);
	//bool DisplayCash();

	bool SetDisplayCongrats(bool displayCongrats);

	/// <summary>
	/// Returns true if displaying congratulations is enabled, false otherwise.
	/// </summary>
	/// <returns>true if displaying congratulations is enabled, false otherwise.</returns>
	bool DisplayCongrats() const;

	bool SetDisplayPtsTotalAsCash(bool displayPtsTotalAsCash);

	/// <summary>
	/// Returns true if displaying points total as cash is enabled, false otherwise.
	/// </summary>
	/// <returns>true if displaying points total as cash is enabled, false otherwise.</returns>
	bool DisplayPtsTotalAsCash() const;

	bool SetDisplayPtsTotalAsCashSession(bool displayPtsTotalAsCashSession);

	/// <summary>
	/// Returns true if displaying points total as cash session is enabled, false otherwise.
	/// </summary>
	/// <returns>true if displaying points total as cash session is enabled, false otherwise.</returns>
	bool DisplayPtsTotalAsCashSession() const;

	bool SetDisplayTotalAndSessionPtsNum(bool displayTotalAndSessionPtsNum);

	/// <summary>
	/// Returns true if displaying total and session points as numeric is enabled, false otherwise.
	/// </summary>
	/// <returns>true if displaying total and session points as numeric is enabled, false otherwise.</returns>
	bool DisplayTotalAndSessionPtsNum() const;

	bool SetDisplayTotalPtsSessionPtsAsCash(bool displayTotalPtsSessionPtsAsCash);

	/// <summary>
	/// Returns true if displaying total points and session points as cash is enabled, false otherwise.
	/// </summary>
	/// <returns>true if displaying total points and session points as cash is enabled, false otherwise.</returns>
	bool DisplayTotalPtsSessionPtsAsCash() const;

//	bool SetDisplaySessionOnly(bool displaySessionOnly);
	//bool DisplaySessionOnly();

//	bool SetDisplayPersistantTotal(bool displayPersistantTotal);
//	bool DisplayPersistantTotal();

	bool SetPBTCashEnabled_local(bool pbtCashEnabled_local);

	/// <summary>
	/// Returns true if PersonalBanker cash transfers are enabled, false otherwise.
	/// </summary>
	/// <returns>true if PersonalBanker cash transfers are enabled, false otherwise.</returns>
	bool PBTCashEnabled() const;

	bool PBTCashEnabledGlobal();

	bool SetPBTPointsAsPromo(bool pbtPointsAsPromo);

	/// <summary>
	/// Returns true if PersonalBanker points are used as promotional funds, false otherwise.
	/// </summary>
	/// <returns>true if PersonalBanker points are used as promotional funds, false otherwise.</returns>
	bool PBTPointsAsPromo() const;

	bool SetPBTPointsEnabled_local(bool pbtPointsEnabled_local);

	/// <summary>
	/// Returns true if PersonalBanker point transfers are enabled, false otherwise.
	/// </summary>
	/// <returns>true if PersonalBanker point transfers are enabled, false otherwise.</returns>
	bool PBTPointsEnabled() const;

	bool PBTPointsEnabledGlobal() const;

	bool SetPBTPromoEnabled_local(bool pbtPromoEnabled_local);

	/// <summary>
	/// Returns true if PersonalBanker promotional transfers are enabled, false otherwise.
	/// </summary>
	/// <returns>true if PersonalBanker promotional transfers are enabled, false otherwise.</returns>
	bool PBTPromoEnabled() const;

	bool PBTPromoEnabledGlobal();

	//bool SetPersonalBankerPointsAsPromo(bool personalBankerPointsAsPromo);
//	bool PersonalBankerPointsAsPromo();

	bool SetCoinsWonAccounting(bool coinsWonAccounting);
	bool CoinsWonAccounting();

	bool SetCoinsInAccounting(bool coinsInAccounting);
	bool CoinsInAccounting();

	bool SetCoinsOutAccounting(bool coinsOutAccounting);
	bool CoinsOutAccounting();

	bool SetTheoreticalAccounting(bool theoreticalAccounting);
	bool TheoreticalAccounting();

	AccountingType GetAccountingType( void );

	bool SetCardInPBT(bool cardInPBT);

	/// <summary>
	/// Returns true if card in PBT is enabled, false otherwise.
	/// </summary>
	/// <returns>true if card in PBT is enabled, false otherwise.</returns>
	bool CardInPBT() const;

	/// <summary>
	/// Sets whether employee PINs are required.
	/// </summary>
	/// <param name="employeePinRequired">IN - true if employee PINs are required, false otherwise.</param>
	/// <returns>The previous value for whether employee PINs are required.</returns>
	bool SetEmployeePinRequired(bool employeePinRequired);

	/// <summary>
	/// Gets whether employee PINs are required.
	/// </summary>
	/// <returns> true if employee PINs are required, false otherwise.</returns>
	bool EmployeePinRequired() const;

	bool SetFillAmount(bool fillAmount);
	bool FillAmount();

	bool SetPartialBonusAward(bool PartialBonusAward);

	/// <summary>
	/// Returns true if partial bonus awards are enabled, false otherwise.
	/// </summary>
	/// <returns>true if partial bonus awards are enabled, false otherwise.</returns>
	bool PartialBonusAward() const;

	bool SetPBFromGameEnabled(bool m_PBFromGameEnabled);

	/// <summary>
	/// Returns true if PersonalBanker transfers from the EGM are enabled, false otherwise.
	/// </summary>
	/// <returns>true if PersonalBanker transfers from the EGM are enabled, false otherwise.</returns>
	bool PBFromGameEnabled() const;

	/// <summary>
	/// Sets whether coupon promo in support is enabled.
	/// </summary>
	/// <param name="couponPromoInEnabled">IN - The new value for coupon promo in support enabled -- true for enabled, false otherwise.</param>
	/// <returns>The previous value for coupon promo in support enabled -- true for enabled, false otherwise.</returns>
	bool SetCouponPromoInEnabled(bool couponPromoInEnabled);

	/// <summary>
	/// Returns true if coupon promo in support is enabled, false otherwise.
	/// </summary>
	/// <returns>true if coupon promo in support is enabled, false otherwise.</returns>
	bool CouponPromoInEnabled() const;

	bool SetCouponPromoOutEnabled(bool couponPromoInEnabled);
	bool CouponPromoOutEnabled();

	/// <summary>
	/// Sets whether ticket promo in support is enabled.
	/// </summary>
	/// <param name="voucherPromoInEnabled">IN - The new value for ticket promo in support enabled -- true for enabled, false otherwise.</param>
	/// <returns>The previous value for ticket promo in support enabled -- true for enabled, false otherwise.</returns>
	bool SetVoucherPromoInEnabled(bool voucherPromoInEnabled);

	/// <summary>
	/// Returns true if ticket promo in support is enabled, false otherwise.
	/// </summary>
	/// <returns>true if ticket promo in support is enabled, false otherwise.</returns>
	bool VoucherPromoInEnabled() const;

	/// <summary>
	/// Returns true if PersonalBanker CEP transfers from the EGM are enabled, false otherwise.
	/// </summary>
	/// <returns>true if PersonalBanker CEP transfers from the EGM are enabled, false otherwise.</returns>
	bool CEPOutEnabled() const;

	/// <summary>
	/// Returns true if PersonalBanker NCEP transfers from the EGM are enabled, false otherwise.
	/// </summary>
	/// <returns>true if PersonalBanker NCEP transfers from the EGM are enabled, false otherwise.</returns>
	bool NCEPOutEnabled() const;

	bool SetPBTPinNotRequired(bool PBTPinNotRequired);
	bool PBTPinNotRequired();

	/// <summary>
	/// Sets whether ticket out support is enabled.
	/// </summary>
	/// <param name="voucherOutEnabled">IN - The new value for ticket out support enabled -- true for enabled, false otherwise.</param>
	/// <returns>The previous value for ticket out support enabled -- true for enabled, false otherwise.</returns>
	bool SetVoucherOutEnabled(bool voucherOutEnabled);

	/// <summary>
	/// Returns true if ticket out support is enabled, false otherwise.
	/// </summary>
	/// <returns>true if ticket out support is enabled, false otherwise.</returns>
	bool VoucherOutEnabled() const;
	
	/// <summary>
	/// Sets the RCCL cards enabled status.
	/// </summary>
	/// <param name="rcclCardsEnabled">IN - true to enable RCCL cards, false to disable.</param>
	/// <returns>The previous value for the the RCCL cards enabled status.</returns>
	bool SetRcclCardsEnabled(bool rcclCardsEnabled);

	/// <summary>
	/// Gets the RCCL cards enabled status.
	/// </summary>
	/// <returns>true if RCCL cards are enabled, false if disabled.</returns>
	bool RcclCardsEnabled() const;

	/// <summary>
	/// Sets the NCL cards enabled status.
	/// </summary>
	/// <param name="rcclCardsEnabled">IN - true to enable NCL cards, false to disable.</param>
	/// <returns>The previous value for the the NCL cards enabled status.</returns>
	bool SetNclCardsEnabled(bool nclCardsEnabled);

	/// <summary>
	/// Gets the NCL cards enabled status.
	/// </summary>
	/// <returns>true if NCL cards are enabled, false if disabled.</returns>
	bool NclCardsEnabled() const;

	/// <summary>
	/// Sets whether ticket in support is enabled.
	/// </summary>
	/// <param name="ticketingEnabled">IN - The new value for ticket in support enabled -- true for enabled, false otherwise.</param>
	/// <returns>The previous value for ticket in support enabled -- true for enabled, false otherwise.</returns>
	bool SetTicketInEnabled(bool TicketInEnabled);

	/// <summary>
	/// Returns true if ticket in support is enabled, false otherwise.
	/// </summary>
	/// <returns>true if ticket in support is enabled, false otherwise.</returns>
	bool TicketInEnabled() const;

	bool SetLOState(bool loState);
	bool LOState();


	//-----------------------------------
	bool SetPBTPromoEnabled_Global( bool pbtPromoEnabled_global);

	bool SetPBTPointsEnabled_Global(bool pbtPointsEnabled_global);

	bool SetPBTCashEnabled_Global(bool pbtCashEnabled_global);

	bool SetPartialWATOutEnabled_Global(bool partialWATOutEnabled_global);

	bool SetCashTransferTracking( bool cashTransferTracking);
	bool GetCashTransferTracking();

	int SetProgressiveDenomination(int progressiveDenomination);
	int GetProgressiveDenomination();

	int SetPendingSlotmast(int slotmast);
	int GetPendingSlotmast();
	
	__int64 SetLastGameStart(__int64 lastGameStart);
	__int64 GetLastGameStart();

	__int64 SetCoinInAtLastGameStart(__int64 coinInAtLastGameStart);
	__int64 GetCoinInAtLastGameStart();

	/// <summary>
	/// Sets the coins won at last game end.
	/// </summary>
	/// <param name="coinsWonAtLastGameEnd">IN - The coins won at last game end.</param>
	/// <returns>The previous value.</returns>
	__int64 SetCoinsWonAtLastGameEnd(__int64 coinsWonAtLastGameEnd);

	/// <summary>
	/// Gets the coins won at last game end.
	/// </summary>
	/// <returns>The coins won at last game end.</returns>
	__int64 GetCoinsWonAtLastGameEnd() const;

	void SetLastCardRead(const byte* cardID);
	void ClearLastCardRead(void);
	byte * GetLastCardRead();

	DWORD SetSenOption1Raw( DWORD senOption1Raw );
	DWORD GetSenOption1Raw( void );

	DWORD SetSenOption2Raw( DWORD senOption2Raw );
	DWORD GetSenOption2Raw( void );

	/// <summary>
	/// Returns true if automatic PersonalBanker Auto Upload Credits from the EGM are enabled, false otherwise.
	/// </summary>
	/// <returns>true if automatic PersonalBanker Auto Upload Credits from the EGM are enabled, false otherwise.</returns>
	bool AutoUploadCredits() const;

	bool NcepAutoUpload() const;
	bool CepAutoUpload() const;
	bool WatAutoUpload() const;

	bool SetJackpotA8Reset ( bool resetToCredit );
	bool JackpotA8Reset();

	/// <summary>
	/// Sets whether Universe cards are enabled.
	/// </summary>
	/// <param name="universeEnabled">IN - true to enable Universe cards, false to disable.</param>
	/// <returns>The previous value for whether Universe cards were enabled</returns>
	bool SetUniverseEnabled(bool universeEnabled);

	/// <summary>
	/// Gets whether Universe cards are enabled.
	/// </summary>
	/// <returns>true if Universe cards are enabled, false if disabled.</returns>
	bool GetUniverseEnabled(void) const;

	/// <summary>
	/// Sets whether mixed mode is enabled.
	/// </summary>
	/// <param name="mixedModeEnabled">IN - The new value for mixed mode enabled -- true to enable mixed mode, false to disable.</param>
	/// <returns>The previous value for whether mixed mode was enabled.</returns>
	bool SetMixedModeEnabled(bool mixedModeEnabled);

	/// <summary>
	/// Returns true if mixed mode is enabled, false if disabled.
	/// </summary>
	/// <returns>true if mixed mode is enabled, false if disabled.</returns>
	bool GetMixedModeEnabled(void) const;

	/// <summary>
	/// Sets the Universe property codes.
	/// </summary>
	/// <param name="universePropCodes">IN - The Universe property codes.</param>
	void SetUniversePropCodes(const std::vector<WORD> &universePropCodes);

	/// <summary>
	/// Gets the Universe property codes.
	/// </summary>
    /// <returns>A vector to the Universe property codes 
    /// stored.</returns> 
	/// <remarks>
	/// The propCodeCount is the count last set by SetUniversePrefixes() or SetUniversePropCodes().
	/// </remarks>
	const std::vector<WORD> GetUniversePropCodes() const;

	/// <summary>
	/// Sets the Universe player card prefixes.
	/// </summary>
	/// <param name="playerCardsPrefix">IN - The Universe player card prefixes.</param>
	void SetUniversePrefixes(const std::vector<BYTE> &universePrefixes);

	/// <summary>
	/// Gets the Universe player card prefixes.
	/// </summary>
    /// <returns>A vector to the valid Universe player card 
    /// prefixes.</returns> 
	/// <remarks>
	/// The prefixCount is the count last set by SetUniversePrefixes() or SetUniversePropCodes().
	/// </remarks>
	const std::vector<BYTE> GetUniverseCardPrefixes() const;

	CIPAddress GetActualIP();	

	DWORD SetFeatureFlagsRaw( DWORD featureFlags1Raw );
	DWORD GetFeatureFlagsRaw( void );

	bool SetFeatureFlags (bool atomicRedemptions, bool jackpotExternalBonus, bool externalCreditToGame, bool multiGameMultiDenom);
	void GetFeatureFlags(bool& atomicRecemptions, bool& jackpotExternalBonus, bool &externalCreditToGame, bool& multiGameMultiDenom);

	bool SetAtomicRedemptions(bool atomicRedemptions);
	bool AtomicRedemptions();

	bool SetJackpotExternalBonus(bool jackpotExternalBonus);
	bool JackpotExternalBonus();

	bool SetMultiGameMultiDenom(bool multiGameMultiDenom);
	bool MultiGameMultiDenom();

	/// <summary>
	/// Set the configured card reader track number.
	/// </summary>
	/// <param name="cardReaderTrackNumber">IN - The card reader track number.</param>
	/// <returns>The previous value of the card reader track.</returns>
	/// <remarks>
	/// If cardReaderTrackNumber is not valid then set the card reader track to the default value.
	/// </remarks>
	BYTE SetCardReaderTrackNumber(BYTE cardReaderTrackNumber);

	/// <summary>
	/// Gets the configured card reader track number.
	/// </summary>
	/// <returns>The configured card reader track number.</returns>
	BYTE CardReaderTrackNumber(void) const;

	/// <summary>
	/// Gets the unattended jackpot enabled.
	/// </summary>
	/// <returns>Unattended jackpot enabled.</returns>
	bool GetUJPEnabled() const;

	/// <summary>
	/// Sets unattended jackpot enabled.
	/// </summary>
	/// <param name="ujpEnabled">if set to <c>true</c> [ujp enabled].</param>
	/// <returns>Previous unattended jackpot enabled value.</returns>
	bool SetUJPEnabled(bool ujpEnabled);

        /// <summary> 
        /// Sets sentinel option to probe for PBT lock success 
        /// </summary> 
        void SetProbePBTLock(bool probeLock);

        /// <summary> 
        /// Gets sentinel option to probe for PBT lock success 
        /// <returns>Probe for PBT lock sentinel option</returns> 
        /// </summary> 
        bool GetProbePBTLock() const;

	/// <summary>
	/// Gets the MGMD enabled.
	/// </summary>
	/// <returns>Unattended jackpot enabled.</returns>
	bool GetMGMDEnabled() const;

	/// <summary>
	/// Sets MGMD enabled.
	/// </summary>
	/// <param name="mgmdEnabled">if set to <c>true</c> [mgmd enabled].</param>
	/// <returns>Previous MGMD enabled value.</returns>
	bool SetMGMDEnabled(bool mgmdEnabled);

	/// <summary>
	/// Sets whether mini-virtual player sessions are enabled.
	/// </summary>
	/// <param name="miniVirtualSessionEnabled">true for mini-virtual player sessions enabled, false otherwise.</param>
	/// <returns>The previous value for whether mini-virtual player sessions are enabled.</returns>
	/// <remarks>
	/// If VirtualSessionEnabled() is true then (normal) virtual sessions have precedence over mini-virtual sessions.
	/// </remarks>
	bool SetMiniVirtualSessionEnabled(bool miniVirtualSessionEnabled);

	/// <summary>
	/// Gets whether mini-virtual player sessions are enabled.
	/// </summary>
	/// <returns>true if mini-virtual player sessions are enabled, false otherwise.</returns>
	/// <remarks>
	/// If VirtualSessionEnabled() is true then (normal) virtual sessions have precedence over mini-virtual sessions.
	/// </remarks>
	bool MiniVirtualSessionEnabled() const;

	/// <summary>
	/// Sets whether one link progressives are enabled.
	/// </summary>
	/// <param name="oneLinkProgressivesEnabled">true for one link progressives enabled, false otherwise.</param>
	/// <returns>The previous value for whether one link progressives are enabled.</returns>
	bool SetOneLinkProgressivesEnabled(bool oneLinkProgressivesEnabled);

	/// <summary>
	/// Gets whether one link progressives are enabled.
	/// </summary>
	/// <returns>true if one link progressives are enabled, false otherwise.</returns>
	bool OneLinkProgressivesEnabled() const;

	/// <summary>
	/// Sets whether Mobile Connect (poller) communication is enabled.
	/// </summary>
	/// <param name="mobileConnectCommunicationEnabled">true for floor communication is, false otherwise.</param>
	/// <returns>The previous value for whether mobileConnectCommunicationEnabled was enabled.</returns>
	bool SetMobileConnectCommunicationEnabled(bool mobileConnectCommunicationEnabled);
	
	/// <summary>
	/// Gets whether Mobile Connect (poller) communication is enabled.
	/// </summary>
	/// <returns>true if Mobile Connect (poller) communication is enabled, false otherwise.</returns>
	bool MobileConnectCommunicationEnabled() const;

	/// <summary>
	/// Sets whether remote debugging is enabled.
	/// </summary>
	/// <param name="value">The desired value for remote debugging</param>
	/// <returns>The previous value for whether remote debugging was enabled.</returns>
	bool SetRemoteDebuggingEnabled(bool value);

	/// <summary>
	/// Gets whether remote debugging is enabled.
	/// </summary>
	/// <returns>true if remote debugging is enabled, false otherwise.</returns>
	bool RemoteDebuggingEnabled() const;

	/// <summary>
	/// Sets whether partial transfer of cash (WAT) from game is enabled
	/// </summary>
	/// <param name="partialWatOutEnabled">IN - The new value for partial WAT-out support enabled -- true for enabled, false otherwise.</param>
	/// <returns>The previous value for partial WAT-out support enabled -- true for enabled, false otherwise.</returns>
	bool SetPartialWATOutEnabled(bool partialWatOutEnabled);

	/// <summary>
	/// Returns true if partial transfer of cash (WAT) from game is enabled
	/// </summary>
	/// <returns>true if partial transfer of cash (WAT) from game is enabled, false otherwise.</returns>
	bool PartialWATOutEnabled() const;

	/// <summary>
	/// Returns true if global setting for partial transfer of cash (WAT) from game is enabled
	/// </summary>
	/// <returns>true if global setting for partial transfer of cash (WAT) from game is enabled, false otherwise.</returns>
	bool PartialWATOutEnabled_Global() const;

	/// <summary>
	/// Sets whether EGM cashout to host global option is enabled
	/// </summary>
	/// <param name="partialWatOutEnabled">IN - The new value for cashout to host global option -- true for enabled, false otherwise.</param>
	/// <returns>The previous value for cashout to host global option -- true for enabled, false otherwise.</returns>
	bool SetCashoutToHostEnabled_Global(bool cashoutToHostEnabled_global);


	/// <summary>
	/// Sets whether EGM cashout to host is enabled
	/// </summary>
	/// <param name="partialWatOutEnabled">IN - The new value for cashout to host support enabled -- true for enabled, false otherwise.</param>
	/// <returns>The previous value for cashout to host support enabled -- true for enabled, false otherwise.</returns>
	bool SetCashoutToHostEnabled(bool cashoutToHostEnabled);

	/// <summary>
	/// Returns true if EGM cashout to host is enabled
	/// </summary>
	/// <returns>true if EGM cashout to host is enabled, false otherwise.</returns>
	bool CashoutToHostEnabled() const;

	/// <summary>
	/// Returns true if global setting for EGM cashout to host is enabled
	/// </summary>
	/// <returns>true if global setting for EGM cashout to host is enabled, false otherwise.</returns>
	bool CashoutToHostEnabled_Global() const;

	/// <summary>
	/// Sets whether player profile button is enabled.
	/// </summary>
	/// <param name="displayProfileButton">if set to <c>true</c> [display profile button].</param>
	/// <returns>The previous value for whether display player profile button flag is enabled.</returns>
	bool SetDisplayPlayerProfileButton(bool displayProfileButton);

	/// <summary>
	/// Sets whether global MGMD is enabled.
	/// </summary>
	/// <param name="globalMGMD">if set to <c>true</c> [global MGMD].</param>
	/// <returns>The previous value for whether global MGMD flag is enabled.</returns>
	bool SetGlobalMGMD(bool globalMGMD);

	/// <summary>
	/// Determines whether the player profile button is enabled or not.
	/// </summary>
	/// <returns>true to display player profile button, false otherwise.</returns>
	bool DisplayPlayerProfileButton() const;

	/// <summary>
	/// Determines whether the global MGMD is enabled or not.
	/// </summary>
	/// <returns>true if global MGMD is enabled, false otherwise.</returns>
	bool GetGlobalMGMDEnabled() const;

	/// <summary>
	/// Sets whether the game supports start exceptions.
	/// </summary>
	/// <param name="gameStart">T/F supports game start 
	/// exceptions</param> 
	void SetGameStartExceptions(bool gameStart);

	/// <summary>
	/// Gets game start exception support
	/// </summary>
	/// <returns>true if game supports start 
	/// exceptions</returns> 
	bool GetGameStartExceptions();

	/// <summary>
	/// Sets whether the game supports end exceptions.
	/// </summary>
	/// <param name="gameEnd">T/F supports game end 
	/// exceptions</param> 
	void SetGameEndExceptions(bool gameEnd);

	/// <summary>
	/// Gets game end exception support
	/// </summary>
	/// <returns>true if game supports end 
	/// exceptions</returns> 
	bool GetGameEndExceptions();		
	
	/// <summary>
	/// Gets the bonus counter notification priority
	/// </summary>
	/// <returns>bonus counter notification priority</returns> 
	WORD GetBonusCounterNotificationPriority() const;

	/// <summary>
	/// Gets the bonus counter nRich Level priority
	/// </summary>
	/// <returns>bonus counter nRich Level priority</returns>
	WORD GetBonusCounternRichLevelPriority() const;

	/// <summary>
	/// Gets the bonus counter Splashdownpriority
	/// </summary>
	/// <returns>bonus counter Splashdown priority</returns>
	WORD GetBonusCounterSplashdownPriority() const;

	/// <summary>
	/// Gets the bonus counter Progressive Odometer priority
	/// </summary>
	/// <returns>bonus counter Progressive Odometer priority</returns>
	WORD GetBonusCounterProgressiveOdometerPriority() const;

	/// <summary>
	/// Gets the bonus counter PTE priority
	/// </summary>
	/// <returns>bonus counter PTE priority</returns>
	WORD GetBonusCounterPtePriority() const;

	/// <summary>
	/// Gets the bonus counter Scatter Countdown Timer priority
	/// </summary>
	/// <returns>bonus counter Scatter Countdown Timer priority</returns>
	WORD GetBonusCounterScatterCountdownTimerPriority() const;

	/// <summary>
	/// Gets the bonus counter prize odometer priority
	/// </summary>
	/// <returns>bonus counter prize odometer priority</returns>
	WORD GetBonusCounterPrizePriority() const;

	/// <summary>
	/// Gets the bonus counter playxgety odometer priority
	/// </summary>
	/// <returns>bonus counter playxgety odometer priority</returns>
	WORD GetBonusCounterPlayXGetYPriority() const;

	/// <summary>
	/// Sets external credit to  game option is supported by sentinel.
	/// </summary>
	/// <param name="bCreditTogame">T/F (The FL sentinel options external credit to game enabled)
	///</param> 
	bool SetExternalCreditToGame(bool bCreditTogame);

	/// <summary>
	/// Gets external credit to  game option is supported by sentinel.
	/// </summary>
	/// <returns>true external credit to  game option is supported by sentinel</returns> 
	bool GetExternalCreditToGameEnabled( void ) const;

	/// <summary>
	/// Gets external credit verification enabled flag.
	/// </summary>
	bool ExternalCreditVerificationEnabled() const;

	/// <summary>
	/// Sets the Global Excessive Voucher Out Enabled Flag
	/// </summary>
	/// <param name="enabled">enabled flag</param>
	/// <returns>The previous value for Global Excessive Voucher Out Enabled Flag.</returns>
	bool SetGlobalExcessiveVoucherOutEnabledFlag(bool enabled);

	/// <summary>
	/// Gets the Global Excessive Voucher Out Enabled Flag
	/// </summary>
	/// <returns>true if Global Excessive Voucher Out is enabled, false otherwise.</returns> 
	bool GetGlobalExcessiveVoucherOutEnabledFlag() const;

	/// <summary>
	/// Sets the Excessive Voucher Out Threshold
	/// </summary>
	/// <param name="threshold">threshold value</param>
	/// <returns>The previous Excessive Voucher Out Threshold.</returns>
	DWORD SetExcessiveVoucherOutThreshold(DWORD threshold);

	/// <summary>
	/// Gets the Excessive Voucher Out Threshold
	/// </summary>
	/// <returns>Excessive Voucher Out Threshold.</returns> 
	DWORD GetExcessiveVoucherOutThreshold();

	/// <summary>
	/// Sets the Total Bill In Threshold
	/// </summary>
	/// <param name="threshold">threshold value</param>
	/// <returns>The previous Total Bill In Threshold.</returns>
	DWORD SetTotalBillInThreshold(DWORD threshold);

	/// <summary>
	/// Gets the Total Bill In Threshold
	/// </summary>
	/// <returns>Total Bill In Threshold.</returns> 
	DWORD GetTotalBillInThreshold();

	/// <summary>
	/// Sets the Coin In Threshold
	/// </summary>
	/// <param name="threshold">threshold value</param>
	/// <returns>The previous Coin In Threshold.</returns>
	DWORD SetCoinInThreshold(DWORD threshold);

	/// <summary>
	/// Gets the Coin In Threshold
	/// </summary>
	/// <returns>Total Coin In Threshold.</returns> 
	DWORD GetCoinInThreshold();

	/// <summary>
	/// Sets the Coin Out Threshold
	/// </summary>
	/// <param name="threshold">threshold value</param>
	/// <returns>The previous Coin Out Threshold.</returns>
	DWORD SetCoinOutThreshold(DWORD threshold);

	/// <summary>
	/// Gets the Coin Out Threshold
	/// </summary>
	/// <returns>Total Coin Out Threshold.</returns> 
	DWORD GetCoinOutThreshold();

	/// <summary>
	/// Sets whether auto upload credits global is enabled
	/// </summary>
	/// <param name="autoUploadCreditsGlobal">IN - The new value for auto upload credits global -- true for enabled, false otherwise.</param>
	/// <returns>The previous value for auto upload credits global enabled -- true for enabled, false otherwise.</returns>
	bool SetAutoUploadCreditsGlobal(bool autoUploadCreditsGlobal);

	/// <summary>
	/// Returns true if auto upload credits global is enabled
	/// </summary>
	/// <returns>true if auto upload credits global is enabled, false otherwise.</returns>
	bool AutoUploadCreditsGlobal() const;

	/// <summary>
	/// Sets Poller_12_11_6000_or_Higher flag.
	/// </summary>
	void SetPoller_12_11_6000_or_Higher(bool value);

	/// <summary>
	/// Gets the Poller_12_11_6000_or_Higher flag
	/// </summary>
	/// <returns>true if Poller is version 12.11.6000 or higher, false otherwise.</returns>
	bool IsPoller_12_11_6000_or_Higher() const;

	/// <summary>
	/// Sets whether poller is NFD compatible
	/// </summary>
	void SetNFDPoller(bool value);

	/// <summary>
	/// Gets whether poller is NFD compatible
	/// </summary>
	/// <returns>true if Poller is NFD compatible (packet version 14 or higher), false otherwise.</returns>
	bool IsNFDPoller() const;

	/// <summary>
	/// Sets ticket in resend flag.
	/// </summary>
	/// /// <returns>The previous value for ticket in resend flag.</returns>
	bool SetTicketInResend(bool value);

	/// <summary>
	/// Sets enable location services flag.
	/// </summary>
	/// <returns>The previous value for enable location services flag.</returns>
	bool SetEnableLocationServices(bool value);

	// <summary>
	/// Gets enable location services flag.
	/// </summary>
	/// <returns>value of enableLocationServices flag from extensible settings</returns>
	bool GetEnableLocationServices() const;

	/// <summary>
	/// Sets the restart ui after x days value.
	/// </summary>
	/// <returns>The previous value for restart ui after x days.</returns>
	DWORD SetRestartUIAfterxDays(DWORD value);

	/// <summary>
	/// Gets the restart ui after x days value.
	/// </summary>
	/// <returns>restart ui after x days value.</returns>
	DWORD GetRestartUIAfterxDays() const;

	/// <summary>
	/// Sets SAS read timeout multiplier value.
	/// </summary>
	/// <returns>The previous value for SAS read timeout multiplier.</returns>
	DWORD SetSASReadTimeOutMultiplier(DWORD value);

	/// <summary>
	/// Gets the SAS read timeout multiplier value.
	/// </summary>
	/// <returns>SAS read timeout multiplier value.</returns>
	DWORD GetSASReadTimeOutMultiplier();

	/// <summary>
	/// Sets the UJP Tax Withholding global flag.
	/// </summary>
	/// <returns>the old UJP Tax Withholding global flag</returns>
	bool SetUJPTaxWithholding_Global(bool enabled);

	/// <summary>
	/// Returns true if UJP Tax Withholding is enabled, false otherwise.
	/// </summary>
	/// <returns>true if UJP Tax Withholding is enabled, false otherwise.</returns>
	bool UJPTaxWithholding() const;

	/// <summary>
	/// Sets federal tax withholding percentage.
	/// </summary>
	/// <returns>the old federal tax withholding percentage</returns>
	float SetFederalTaxWithholdingPercent(float percent);

	/// <summary>
	/// Gets federal tax withholding percentage.
	/// </summary>
	/// <returns>federal tax withholding percentage</returns>
	float GetFederalTaxWithholdingPercent() const;

	/// <summary>
	/// Sets state tax withholding percentage.
	/// </summary>
	/// <returns>the old state tax withholding percentage</returns>
	float SetStateTaxWithholdingPercent(float percent);

	/// <summary>
	/// Gets state tax withholding percentage.
	/// </summary>
	/// <returns>state tax withholding percentage</returns>
	float GetStateTaxWithholdingPercent() const;

	/// <summary>
	/// Sets the number of subgames mismatch check variable
	/// </summary>
	/// <param name="value">IN - Number of subgames mismatch check.</param>
	/// <returns>the old Number of subgames mismatch check.</returns>
	DWORD SetNumberOfSubgamesMismatchCheck(DWORD value);

	/// <summary>
	/// Returns the number of subgames mismatch check.
	/// </summary>
	/// <returns>the number of subgames mismatch check.</returns>	
	DWORD GetNumberOfSubgamesMismatchCheck() const;

	/// <summary>
	/// Sets the auto card out Seconds to countdown credits
	/// </summary>
	/// <param name="numberOfseconds">number Of seconds</param>
	/// <returns>the previous auto card out seconds to countdown credits</returns>
	DWORD SetAutoCardOutSecToCountdownCredits(DWORD numberOfseconds);

	/// <summary>
	/// Sets the auto card out Seconds to countdown without credits
	/// </summary>
	/// <param name="numberOfseconds">number Of seconds</param>
	/// <returns>the previous auto card out seconds to countdown without credits</returns>
	DWORD SetAutoCardOutSecToCountdownNoCrdts(DWORD numberOfseconds);

	/// <summary>
	/// Sets the auto card out countdown seconds
	/// </summary>
	/// <param name="numberOfseconds">number Of seconds</param>
	/// <returns>the previous auto card out countdown seconds</returns>
	DWORD SetAutoCardOutCountdownSeconds(DWORD numberOfseconds);

	/// <summary>
	/// Sets the auto card out countdown seconds
	/// </summary>
	/// <param name="numberOfseconds">number Of seconds</param>
	/// <returns>the previous auto card out countdown seconds</returns>
	DWORD SetAutoCardOutSecToAlert(DWORD numberOfseconds);

	/// <summary>
	/// Gets the auto cardOut seconds to countdown credits
	/// </summary>
	/// <returns>auto cardOut seconds to countdown credits</returns> 
	DWORD GetAutoCardOutSecToCountdownCredits() const;

	/// <summary>
	/// Gets the auto cardOut seconds to countdown without credits
	/// </summary>
	/// <returns>auto cardOut seconds to countdown without credits</returns> 
	DWORD GetAutoCardOutSecToCountdownNoCrdts() const;

	/// <summary>
	/// Gets the auto cardOut countdown seconds
	/// </summary>
	/// <returns>auto cardOut countdown seconds</returns> 
	DWORD GetAutoCardOutCountdownSeconds() const;

	/// <summary>
	/// Gets the auto cardout seconds before alerting the user with a ding sound.
	/// </summary>
	/// <returns>auto cardout seconds before alerting the user with a ding sound.</returns> 
	DWORD GetAutoCardOutSecToAlert() const;

	/// <summary>
	/// Sets the Extensible System Settings flag
	/// </summary>
	/// <param name="extensibleSystemSettings">the new extensible system settings string</param>
	void SetExtensibleSystemSettings(const string& extensibleSystemSettings);

	/// <summary>
	/// Returns the extensible system settings string
	/// </summary>
	/// <returns>the extensible system settings string.</returns>	
	std::string GetExtensibleSystemSettingsStr() const;

	/// <summary>
	/// Gets the extended buckets countdown display interval in seconds.
	/// </summary>
	/// <returns>the extended buckets countdown display interval in seconds</returns> 
	DWORD GetCountdownDisplayIntervalInSeconds() const;

	/// <summary>
	/// Sets the extended buckets countdown display interval in seconds.
	/// </summary>
	/// <param name="numberOfseconds">number Of seconds</param>
	/// <returns>the previous extended buckets countdown display interval in seconds</returns>
	DWORD SetCountdownDisplayIntervalInSeconds(const DWORD numberOfseconds);

	/// <summary>
	/// Gets whether points countdowns are controlled by universe or not.
	/// </summary>
	/// <returns>Points countdowns are controlled by universe or not</returns> 
	bool GetPointsCountdownControlByUniverse() const;

	/// <summary>
	/// Sets points countdowns are controlled by universe or not.
	/// </summary>
	/// <param name="flag">Points countdowns are controlled by universe or not</param>
	/// <returns>the previous value of whether points countdowns are controlled by universe or not.</returns>
	bool SetPointsCountdownControlByUniverse(const bool flag);

	/// <summary>
	/// Sets the system time.
	/// </summary>
	/// <param name="systemTime">System time</param>
	void SetSystemTime(const SYSTEMTIME& systemTime);

	/// <summary>
	/// Sets the Reboot ASAP threshold in MB
	/// </summary>
	/// <param name="value">Reboot ASAP threshold in MB</param>
	/// <returns>the previous value of Reboot ASAP threshold in MB.</returns>
	DWORD SetRebootAsapThresholdInMb(DWORD value);

	/// <summary>
	/// Returns Reboot ASAP threshold in MB.
	/// </summary>
	/// <returns>Reboot ASAP threshold in MB</returns>	
	DWORD GetRebootAsapThresholdInMb() const;
	
	/// <summary>
	/// Sets the Reboot Warning threshold in MB
	/// </summary>
	/// <param name="value">Reboot Warning threshold in MB</param>
	/// <returns>the previous value of Reboot Warning threshold in MB.</returns>
	DWORD SetRebootWarningThresholdMb(DWORD value);

	/// <summary>
	/// Returns Reboot Warning threshold in MB.
	/// </summary>
	/// <returns>Reboot Warning threshold in MB</returns>	
	DWORD GetRebootWarningThresholdMb() const;

	/// <summary>
	/// Sets the Restart UI ASAP threshold in MB
	/// </summary>
	/// <param name="value">Restart UI ASAP threshold in MB</param>
	/// <returns>the previous value of Restart UI ASAP threshold in MB.</returns>
	DWORD SetRestartUIAsapThresholdMb(DWORD value);

	/// <summary>
	/// Returns UI ASAP threshold in MB.
	/// </summary>
	/// <returns>UI ASAP threshold in MB</returns>	
	DWORD GetRestartUIAsapThresholdMb() const;

	/// <summary>
	/// Sets the Restart UI after the drop threshold in MB
	/// </summary>
	/// <param name="value">Restart UI after the drop threshold in MB</param>
	/// <returns>the previous value of Restart UI after the drop threshold in MB.</returns>
	DWORD SetRestartUIAfterDropThresholdMb(DWORD value);

	/// <summary>
	/// Returns UI after the drop threshold in MB.
	/// </summary>
	/// <returns>UI after the drop threshold in MB</returns>	
	DWORD GetRestartUIAfterDropThresholdMb() const;

	/// <summary>
	/// Sets the Restart UI delay hours
	/// </summary>
	/// <param name="value">Restart UI delay hours</param>
	/// <returns>the previous value of Restart UI delay hours.</returns>
	DWORD SetRestartUIDelayHours(DWORD value);

	/// <summary>
	/// Returns Restart UI delay hours.
	/// </summary>
	/// <returns>Restart UI delay hours</returns>
	DWORD GetRestartUIDelayHours() const;

	/// <summary>
	/// Sets whether to use SDS card format or not
	/// </summary>
	/// <param name="value">Use SDS card format</param>
	/// <returns>the previous value of use SDS card format.</returns>
	bool SetUseSDSCardFormat(bool value);

	/// <summary>
	/// Returns whether to use SDS card format or not.
	/// </summary>
	/// <returns>Use SDS card format</returns>
	bool GetUseSDSCardFormat() const;

	/// <summary>
	/// Gets extensible settings
	/// </summary>
	/// <returns>.</returns>
	DWORD GetExtensibleSettings() const;

	/// <summary>
	/// Resets the extensible settings.
	/// </summary>
	void ResetExtensibleSettings();

	/// <summary>
	/// Returns the nConnect URL.
	/// </summary>
	/// <returns>the nConnect URL.</returns>	
	std::string GetnConnectUrl() const;

	/// <summary>
	/// Sets the nConnect URL.
	/// </summary>
	/// <param name="value">nConnect URL</param>  
	void SetnConnectUrl(const std::string& value);

	/// <summary>
	/// Returns the nConnect token.
	/// </summary>
	/// <returns>the nConnect token.</returns>	
	std::string GetnConnectToken() const;

	/// <summary>
	/// Sets the nConnect token.
	/// </summary>
	/// <param name="value">nConnect token</param>  
	void SetnConnectToken(const std::string& value);

	/// <summary>
	/// Returns the avatar URL.
	/// </summary>
	/// <returns>the avatar URL.</returns>	
	std::string GetAvatarUrl() const;

	/// <summary>
	/// Sets the avatar URL.
	/// </summary>
	/// <param name="value">avatar URL</param>  
	void SetAvatarUrl(const std::string& value);

	/// <summary>
	/// Returns the avatar token.
	/// </summary>
	/// <returns>the avatar token.</returns>	
	std::string GetAvatarToken() const;

	/// <summary>
	/// Sets the avatar token.
	/// </summary>
	/// <param name="value">avatar token</param>  
	void SetAvatarToken(const std::string& value);

	/// <summary>
	/// Returns the request service URL.
	/// </summary>
	/// <returns>the request service URL.</returns>	
	std::string GetRequestServiceUrl() const;

	/// <summary>
	/// Sets the request service URL.
	/// </summary>
	/// <param name="value">request service URL</param>  
	void SetRequestServiceUrl(const std::string& value);

	/// <summary>
	/// Returns the request service token.
	/// </summary>
	/// <returns>the request service token.</returns>	
	std::string GetRequestServiceToken() const;

	/// <summary>
	/// Sets the request service token.
	/// </summary>
	/// <param name="value">request service token</param>  
	void SetRequestServiceToken(const std::string& value);

	/// <summary>
	/// Returns the drinks on tap URL.
	/// </summary>
	/// <returns>the drinks on tap URL.</returns>	
	std::string GetDrinksOnTapUrl() const;

	/// <summary>
	/// Sets the drinks on tap URL.
	/// </summary>
	/// <param name="value">the drinks on tap URL</param>  
	void SetDrinksOnTapUrl(const std::string& value);

	/// <summary>
	/// Returns the drinks on tap token.
	/// </summary>
	/// <returns>the drinks on tap token.</returns>	
	std::string GetDrinksOnTapToken() const;

	/// <summary>
	/// Sets the drinks on tap token.
	/// </summary>
	/// <param name="value">the drinks on tap token</param>  
	void SetDrinksOnTapToken(const std::string& value);

	/// <summary>s
	/// Returns the social distance URL.
	/// </summary>
	/// <returns>the social distance URL.</returns>	
	std::string GetSocialDistanceUrl() const;

	/// <summary>
	/// Sets the social distance token.
	/// </summary>
	/// <param name="value">social distance URL</param>  
	void SetSocialDistanceUrl(const std::string& value);

	/// <summary>
	/// Returns the social distance token.
	/// </summary>
	/// <returns>the social distance token.</returns>	
	std::string GetSocialDistanceToken() const;

	/// <summary>
	/// Sets the social distance token.
	/// </summary>
	/// <param name="value">social distance token</param>  
	void SetSocialDistanceToken(const std::string& value);

	/// <summary>
	/// Returns Social Distance countdown delay.
	/// </summary>
	/// <returns>Social Distance countdown delay</returns>	
	DWORD GetSocialDistanceCountdownDelay() const;

	/// <summary>
	/// Sets Social Distance countdown delay
	/// </summary>
	/// <param name="value">Social Distance countdown delay</param>  
	void SetSocialDistanceCountdownDelay(const DWORD value);

	/// <summary>
	/// Returns Social Distance ignore dirty.
	/// </summary>
	/// <returns>Social Distance ignore dirty</returns>	
	bool GetSocialDistanceIgnoreDirty() const;

	/// <summary>
	/// Sets Social Distance ignore dirty
	/// </summary>
	/// <param name="value">Social Distance ignore dirty</param>  
	void SetSocialDistanceIgnoreDirty(const bool value);

	/// <summary>
	/// Returns Social Distance manual clean enabled.
	/// </summary>
	/// <returns>Social Distance manual clean enabled</returns>	
	bool GetSocialDistanceManualCleanEnabled() const;

	/// <summary>
	/// Sets Social Distance manual clean enabled
	/// </summary>
	/// <param name="value">Social Distance manual clean enabled</param>  
	void SetSocialDistanceManualCleanEnabled(const bool value);

	static DWORD m_sasReadTimeOutMultiplier;

 private:
	static const BYTE DEFAULT_RCCL_PLAYER_CARD_PREFIX = 0;
	static const int RCCL_PLAYER_CARD_PREFIXES_INDEX = 0;

	CEEPROMConfig m_eepromConfig;
	CNVRAMConfig m_nvramConfig;

	void CheckForNVRAMUpgrade( void );
	bool ConfigIntegrityCheck( void );
	void ResetConfigMemory( void );
	void SetRestartValues();
    WORD GetDesktopOrientationAsWord();
    byte GetDesktopOrientationAsByte();
	
	std::string m_strMACAddress;
	std::string m_strMACAddress2;
	std::string m_strNetworkAdaptername;
	BYTE m_macAddress[SIZE_OF_MAC_ADDRESS];
	BYTE m_macAddress2[SIZE_OF_MAC_ADDRESS];

	CIPAddress m_actualIPAddress; //no need to store this in permanent store

	/**************************
	**    NVRAM Items	     **
	**************************/
	
//  These Next few item should either have their own classes, or
//  be a part of another class (e.g. Logs)
//	BYTE *m_transactionBuffers;
//	WCHAR m_logs[SIZE_OF_LOGS];
//	BYTE *m_handpayHistoryLog;
//	int m_bonusTable;
//	BYTE *m_lastTicketInfo;
	
	bool m_EGMLockoutState;
	//enum m_Unused2
	//{
		//TBD
	//};
	byte m_lastCardRead[CARD_ID_LEN];
	bool m_memoryLost;
	int m_lastCoinsPlayed;

	int m_Unused2;
	int m_Unused3;
	WORD m_unused4;

	bool m_cashoutToHostEnabled_global;
	bool m_cashoutToHostEnabled;
	//PBT Stuff
	int m_lastPBTransactionID;
	bool m_PBResponsePending;
	bool m_pbtPromoEnabled_global;
	bool m_pbtPointsEnabled_global;
	bool m_pbtCashEnabled_global;
	bool m_partialWATOutEnabled_global;
	bool m_autoUploadCreditsGlobal;

	int m_PBTransactionID;
	int m_PBTransferSequenceNumber;
	int m_PBCreditSource;
	bool m_PBTransferInProgress;
	int m_currentPBTransactionID;
	DWORD m_PBTransferAmount;
	bool m_PBReleaseNeeded;

	int m_bonusEventBoundaries;  //TBD not sure if this should be an int
	
	bool m_ticketVendResponseInProgress;
	int m_employeeAuthorizationOCR;

	//Display Stuff

	TCHAR m_attractorMessage[SIZE_OF_MESSAGES];
	TCHAR m_goodCardReadMessage[SIZE_OF_MESSAGES];
	TCHAR m_badCarReadMessage[SIZE_OF_MESSAGES];
	TCHAR m_birthdayMessage[SIZE_OF_MESSAGES];
	TCHAR m_anniversaryMessage[SIZE_OF_MESSAGES];
	TCHAR m_congratulationsMessage[SIZE_OF_MESSAGES];
	TCHAR m_welcomeMessage[SIZE_OF_MESSAGES];
	TCHAR m_sessionPointsMessage[SIZE_OF_MESSAGES];
	TCHAR m_balanceRetrievalMessage[SIZE_OF_MESSAGES];
	TCHAR m_dailyPointsDisplaySuffixMessage[SIZE_OF_MESSAGES];

	TCHAR m_pointsOrCashLabel[SIZE_OF_LABEL];	
	TCHAR m_tierPointsLabel[SIZE_OF_LABEL];

	bool m_showPointsVsAccountBalance;

	std::string m_casinoAddress1;
	std::string m_casinoAddress2;
	
	bool m_lossLimitMessage;

	bool m_cashTransferTracking;
	int	 m_progressiveDenomination;

	int m_pendingSlotmast; //SlotmastID change request is pending

	__int64 m_lastGameStart;			//the game start meter from when it last changed
	__int64 m_coinInAtLastGameStart;	//the coin in meter value at the last Game start change.
	__int64 m_coinsWonAtLastGameEnd; // The sum of the coins won meter values at the last game end.
	bool m_gameInProgress;
	bool m_serviceWindowOpen;
	
	WORD m_denomination;

	std::string m_touchScreenProtocolName;
	std::string m_socialDistanceUrl;
	std::string m_socialDistanceToken;
	bool m_socialDistanceIgnoreDirty;
	bool m_socialDistanceManualCleanEnabled;
	DWORD m_socialDistanceCountdownDelay;

	//------------------------------
	// Config From SCF - NVRAM
	//------------------------------
		//Denomination is in EEPROM
		//CasinoNumber is in EEPROM
		//CommID is in EEPROM
			//-----------------
			//SenOptions
			//-----------------
				bool	m_fillAmount;	
				bool    m_useHopperLevel;
				bool    m_virtualSessionEnabled;
				bool	m_displayPlayerPtsTotal;
				bool	m_displayPlayerName;
				bool	m_displayCountdownAndSession;
				bool	m_displayGoodLuckAndSession;
				bool	m_displayGoodLuckAndSessionPts;
				bool    m_displayNumericPointTotal;
				bool	m_displayComps;
				bool	m_displayCongrats;
				bool    m_displayPtsTotalAsCash;
				bool	m_displayPtsTotalAsCashSession;
				bool	m_displayTotalAndSessionPtsNum;
				bool	m_displayTotalPtsSessionPtsAsCash;
				bool	m_pbtCashEnabled_local;
				bool    m_pbtPointsAsPromo;
				bool	m_pbtPointsEnabled_local;
				bool	m_pbtPromoEnabled_local;
				bool	m_coinsWonAccounting;
				bool	m_coinsInAccounting;
				bool	m_coinsOutAccounting;	
				bool	m_theoreticalAccounting;	
				bool	m_cardInPBT;	
				bool	m_employeePinRequired;
				bool	m_PartialBonusAward;
				bool    m_PBFromGameEnabled;
				bool	m_couponPromoInEnabled;	
				bool	m_couponPromoOutEnabled;
				bool	m_voucherPromoInEnabled;
				bool    m_CEPOutEnabled_notUsed;
				bool	m_NCEPOutEnabled_notUsed;
				bool	m_PBTPinNotRequired;
				bool	m_voucherOutEnabled;
				bool	m_ticketInEnabled;
				bool	m_autoUploadCredits_notUsed;
				bool m_miniVirtualSessionEnabled;
				bool	m_senOpAccruePointsForNCEP;
				bool	m_senOpAccrueCompForNCEP;
				bool m_oneLinkProgressivesEnabled;
				bool    m_rcclCardsEnabled;
				bool m_nclCardsEnabled;
				bool m_ujpEnabled;
				bool m_mgmdEnabled;
				bool m_mobileConnectCommEnabled;
				bool m_remoteDebuggingEnabled;
				bool m_partialWATOutEnabled;
			//---------------
			//End SenOptions
			//---------------
		//Slotnumber is in EEPROM
		std::vector<BYTE> m_playerCardPrefixes;
		BYTE m_globalCardPrefix;
		BYTE m_mechanicCardPrefix;
		BYTE m_floorCardPrefix;
		BYTE m_tempCardPrefix;

		std::string m_strCasinoName;
		int m_pointsPerDollar;
		
		bool m_bLossLimitActive;
		DWORD m_lossLimitSessionNumber;
		WORD m_lossLimitSessionAmount;
		WORD m_lossLimitBillValidatorOffAmount;
		
		TicketExpirationType m_ticketExpType;
		int m_ticketExpirationDuration;
		bool m_ticketingEnabled;
		bool m_systemGeneratedTicketValidation;
		bool m_ticketBufferingEnabled;

		WORD m_playerTrackingMultiplier;
		WORD m_playerTrackingDivisor;

		DWORD m_virtualSessionNoPlayTimeout;
		DWORD m_virtualSessionMaxDuration;

		DWORD m_cardTimeout;
		bool m_floorLock;
		bool m_allowFloorLockout;
		short m_timezoneOffset;
		bool m_useDST;
		WORD m_slotMastRevision;

		DWORD m_senOption1Raw;
		DWORD m_senOption2Raw;

		//Auto Cardout Config Values
		DWORD m_autoCardOutSecToCountdownCredits;
		DWORD m_autoCardOutSecToCountdownNoCrdts;
		DWORD m_autoCardOutCountdownSeconds;
		DWORD m_autoCardOutSecToAlert;

		std::string m_nConnectUrl;
		std::string m_nConnectToken;
		std::string m_avatarUrl;
		std::string m_avatarToken;
		std::string m_requestServiceUrl;
		std::string m_requestServiceToken;
		std::string m_drinksOnTapUrl;
		std::string m_drinksOnTapToken;

		bool m_jackpotA8Reset;
		bool m_accruePointsForNCEP;
		bool m_accrueCompForNCEP;

	    //Penn Universal Card project stuff		
		bool m_universeEnabled;
		bool m_mixedModeEnabled;
		std::vector<BYTE> m_universePlayerCardPrefixes;
		std::vector<WORD> m_universePropCodes;

		//12.x specific items
		DWORD m_featureFlagsRaw;
		bool m_atomicRedemptions;

		bool m_jackpotExternalBonus;

		BYTE m_cardReaderTrackNumber;

        bool m_ExternalCreditToGameEnabled;
		bool m_multiGameMultiDenom;

	//Memory Threshold Values
	DWORD m_rebootAsapThresholdInMb;
	DWORD m_rebootWarningThresholdMb;
	DWORD m_restartUIAsapThresholdMb;
	DWORD m_restartUIAfterDropThresholdMb;
	DWORD m_restartUIDelayHours;

	//------------------------
	// End Config Form SCF
	//------------------------


	/******************************
	***   EEPROM Items          ***
	******************************/

	int m_slotMastID;
	BYTE m_casinoNumber;
	BYTE m_commID;
	byte m_machineType;
	int m_slotNumber;
	byte m_hardPinTypes[NUM_HARD_PINS];
	byte m_hardPinActiveState;		// Each bit specifies if the active state for the pin is high or low.
	byte m_serialProtocol;
	byte m_screenBrightness;
	byte m_leftSpeakerVolume;
	byte m_rightSpeakerVolume;
	WORD m_globalCardPassword;
	bool m_DHCPAuto;
	char m_hostAddressOrName[SIZE_OF_NAME_IP + 1];
	int m_hostPort;
	byte m_myIP[IP_ADDRESS_SIZE];
	byte m_defaultGateway[IP_ADDRESS_SIZE];
	byte m_subnetMask[IP_ADDRESS_SIZE];
	byte m_preferredDNSServer[IP_ADDRESS_SIZE];
	byte m_alternateDNSServer[IP_ADDRESS_SIZE];
	string m_domain;
	EGMProtocolState m_egmProtocolState;
	SentinelOnlineState m_sentinelOnlineState; 
	WORD m_gameDenom;
	BatteryStatus m_batteryStatus;
	time_t m_lastBatteryCheckTime;
	byte m_touchScreenProtocolType;
	WORD m_touchScreenProtocolVendorId;
	DWORD m_touchScreenProtocolBaud;
	byte m_mixerRebootCount;
	bool m_autoDetectInputVideoMode;
	byte m_outputVideoModeConfig;
	DesktopOrientationType m_desktopOrientationType;
	
//--------------------------------------------
//  NVRAM offsets
//---------------------------------------------
	DWORD	m_EGMLockoutStateNVRAMOffset;
	DWORD	m_memoryLostNVRAMOffset;
	DWORD	m_lastCoinsPlayedNVRAMOffset;
	DWORD	m_promptForFillAmountNVRAMOffset;
	DWORD	m_Unused2NVRAMOffset;
	DWORD	m_Unused3NVRAMOffset;
	DWORD	m_lastPBTransactionIDNVRAMOffset;
	DWORD	m_PBResponsePendingNVRAMOffset;
	DWORD	m_pbtPromoEnabled_globalNVRAMOffset;
	DWORD	m_pbtPointsEnabled_globalNVRAMOffset;
	DWORD	m_pbtCashEnabled_globalNVRAMOffset;
	DWORD	m_PBTransactionIDNVRAMOffset;
	DWORD	m_PBTransferSequenceNumberNVRAMOffset;
	DWORD	m_PBCreditSourceNVRAMOffset;
	DWORD	m_PBTransferInProgressNVRAMOffset;
	DWORD	m_currentPBTransactionIDNVRAMOffset;
	DWORD	m_PBTransferAmountNVRAMOffset;
	DWORD	m_PBReleaseNeededNVRAMOffset;
	DWORD	m_bonusEventBoundariesNVRAMOffset;
	DWORD	m_ticketVendResponseInProgressNVRAMOffset;
	DWORD	m_employeeAuthorizationOCRNVRAMOffset;
	DWORD	m_attractorMessageNVRAMOffset;
	DWORD	m_goodCardReadMessageNVRAMOffset;
	DWORD	m_badCarReadMessageNVRAMOffset;
	DWORD	m_birthdayMessageNVRAMOffset;
	DWORD	m_anniversaryMessageNVRAMOffset;
	DWORD	m_congratulationsMessageNVRAMOffset;
	DWORD	m_welcomeMessageNVRAMOffset;
	DWORD	m_sessionPointsMessageNVRAMOffset;
	DWORD	m_balanceRetrievalMessageNVRAMOffset;
	DWORD	m_pointsOrCashLabelNVRAMOffset;
	DWORD	m_showPointsVsAccountBalanceNVRAMOffset;
	DWORD	m_casinoNameNVRAMOffset;
	DWORD	m_numberOfCardPrefixesNVRAMOffset;
	DWORD	m_playerCardPrefixesNVRAMOffset;
	DWORD	m_globalCardPrefixNVRAMOffset;
	DWORD	m_mechanicCardPrefixNVRAMOffset;
	DWORD	m_floorCardPrefixNVRAMOffset;
	DWORD	m_tempCardPrefixNVRAMOffset;
	DWORD	m_bLossLimitActiveNVRAMOffset;
	DWORD	m_lossLimitSessionNumberNVRAMOffset;
	DWORD	m_lossLimitSessionAmountNVRAMOffset;
	DWORD	m_lossLimitBillValidatorOffAmountNVRAMOffset;
	DWORD	m_lossLimitMessageNVRAMOffset;
	DWORD	m_cashTransferTrackingNVRAMOffset;
	DWORD	m_progressiveDenominationNVRAMOffset;
	DWORD	m_floorLockNVRAMOffset;
	DWORD	m_ticketExpTypeNVRAMOffset;
	DWORD	m_ticketExpirationDurationNVRAMOffset;
	DWORD	m_ticketingEnabledNVRAMOffset;
	DWORD	m_systemGeneratedTicketValidationNVRAMOffset;
	DWORD	m_ticketBufferingEnabledNVRAMOffset;
	DWORD	m_playerTrackingMultiplierNVRAMOffset;
	DWORD	m_playerTrackingDivisorNVRAMOffset;
	DWORD	m_virtualSessionMaxDurationNVRAMOffset;
	DWORD	m_virtualSessionNoPlayTimeoutNVRAMOffset;
	DWORD	m_cardTimeoutNVRAMOffset;
	DWORD	m_employeeAuthorizationPasswordNVRAMOffset;
	DWORD	m_fillAmountNVRAMOffset;	
	DWORD	m_useHopperLevelNVRAMOffset;	
	DWORD   m_virtualSessionEnabledNVRAMOffset;
	DWORD	m_displayPlayerPtsTotalNVRAMOffset;
	DWORD	m_displayPlayerNameNVRAMOffset;
	DWORD	m_displayCountdownAndSessionNVRAMOffset;
	DWORD   m_displayGoodLuckAndSessionNVRAMOffset;
	DWORD   m_displayGoodLuckAndSessionPtsNVRAMOffset;
	DWORD   m_displayNumericPointTotalNVRAMOffset;
	DWORD	m_displayCompsNVRAMOffset;
	DWORD	m_displayCongratsNVRAMOffset;
	DWORD	m_displayPtsTotalAsCashNVRAMOffset;
	DWORD	m_displayPtsTotalAsCashSessionNVRAMOffset;
	DWORD	m_displayTotalAndSessionPtsNumNVRAMOffset;
	DWORD   m_displayTotalPtsSessionPtsAsCashNVRAMOffset;
	DWORD	m_pbtCashEnabled_localNVRAMOffset;
	DWORD   m_pbtPointsAsPromoNVRAMOffset;
	DWORD	m_pbtPointsEnabled_localNVRAMOffset;
	DWORD	m_pbtPromoEnabled_localNVRAMOffset;
	DWORD	m_coinsWonAccountingNVRAMOffset;		
	DWORD	m_coinsInAccountingNVRAMOffset;
	DWORD	m_coinsOutAccountingNVRAMOffset;
	DWORD	m_theoreticalAccountingNVRAMOffset;
	DWORD	m_cardInPBTNVRAMOffset;
	DWORD	m_employeePinRequiredNVRAMOffset;
	DWORD	m_PartialBonusAwardNVRAMOffset;
	DWORD   m_PBFromGameEnabledNVRAMOffset;
	DWORD	m_couponPromoInEnabledNVRAMOffset;
	DWORD   m_couponPromoOutEnabledNVRAMOffset;
	DWORD   m_voucherPromoInEnabledNVRAMOffset;
	DWORD   m_CEPOutEnabledNVRAMOffset;
	DWORD	m_NCEPOutEnabledNVRAMOffset;
	DWORD	m_PBTPinNotRequiredNVRAMOffset;
	DWORD   m_voucherOutEnabledNVRAMOffset;
	DWORD	m_rcclCardsEnabledNVRAMOffset;
	DWORD	m_ticketInEnabledNVRAMOffset;
	DWORD	m_loStateNVRAMOffset;
	DWORD	m_pendingSlotmastNVRAMOffset;
	DWORD   m_allowFloorLockoutNVRAMOffset;
	DWORD	m_timezoneOffsetNVRAMOffset;
	DWORD	m_useDSTNVRAMOffset;
	DWORD   m_slotMastRevisionNVRAMOffset;
	DWORD   m_lastGameStartNVRAMOffset;
	DWORD   m_coinInAtLastGameStartNVRAMOffset;
	DWORD   m_lastCardReadNVRAMOffset;
	DWORD   m_casinoAddress1NVRAMOffset;
	DWORD	m_casinoAddress2NVRAMOffset;
	DWORD	m_senOption1RawNVRAMOffset;
	DWORD	m_senOption2RawNVRAMOffset;
	DWORD   m_denominationNVRAMOffset;
	DWORD   m_pointsPerDollarNVRAMOffset;
	DWORD	m_autoUploadCreditsNVRAMOffset;
	DWORD   m_nudgePortNVRAMOffset;
	DWORD	m_nudgeIPNVRAMOffset;
	DWORD	m_cacheAnnouncementPortNVRAMOffset;
	DWORD	m_cacheAnnouncementIPNVRAMOffset;
	DWORD	m_jackpotA8ResetNVRAMOffset;
	DWORD   m_accruePointsForNCEPNVRAMOffset;
	DWORD   m_accrueCompForNCEPNVRAMOffset;
	DWORD   m_senOpAccruePointsForNCEPNVRAMOffset;
	DWORD   m_senOpAccrueCompForNCEPNVRAMOffset;
	DWORD   m_gameInProgressNVRAMOffset;
	DWORD   m_serviceWindowOpenNVRAMOffset;
	DWORD   m_touchScreenProtocolNameNVRAMOffset;
    DWORD   m_universeEnabledNVRAMOffset;
	DWORD   m_mixedModeEnabledNVRAMOffset;
	DWORD   m_numOfUniversePropCodesNVRAMOffset;
	DWORD   m_universePropCodesNVRAMOffset;
	DWORD   m_universePlayerPrefixesNVRAMOffset;
	DWORD	m_featureFlagsRawNVRAMOffset;
	DWORD   m_atomicRedemptionsNVRAMOffset;
	DWORD	m_jackpotExternalBonusNVRAMOffset;
	DWORD	m_externalCreditToGameNVRAMOffset;
	DWORD	m_multiGameMultiDenomNVRAMOffset;
	DWORD m_nclCardsEnabledNVRAMOffset;
	DWORD m_cardReaderTrackNumberNVRAMOffset;
	DWORD m_negligibleCreditsThresholdNVRAMOffset;
	DWORD m_employeeOverPlayerIdleCardOutNVRAMOffset;
	DWORD m_employeeIdleCardOutSecondsNVRAMOffset;
	DWORD m_playerRecardEmployeeCardOutNVRAMOffset;
	DWORD m_dailyPointsDisplaySuffixMessageNVRAMOffset;
	DWORD m_tierPointsLabelNVRAMOffset;	

	DWORD m_notificationShowPromotionNVRAMOffset;
	DWORD m_notificationPlayerTrackingNVRAMOffset;

	DWORD m_customButton1enableNVRAMOffset;
	DWORD m_customMenuButton1CountNVRAMOffset;

	DWORD m_customButton2enableNVRAMOffset;
	DWORD m_customMenuButton2CountNVRAMOffset;

	DWORD m_sessionActiveNegligibleCreditsTimeoutSecondsNVRAMOffset;
	DWORD m_Unused4NVRAMOffset;
	DWORD m_sessionActiveNonNegligibleCreditsTimeoutSecondsNVRAMOffset;

	DWORD m_coinsWonAtLastGameEndNVRAMOffset;

	DWORD m_resortMenuButtonCountNVRAMOffset;
	DWORD m_ujpEnabledNVRAMOffset;
	DWORD m_mgmdEnabledNVRAMOffset;
	DWORD m_configExt3CheckByteNVRAMOffset;
	DWORD m_miniVirtualSessionEnabledNVRAMOffset;
	DWORD m_oneLinkProgressivesEnabledNVRAMOffset;
	DWORD m_mobileConnectCommunicationEnabledNVRAMOffset;
	DWORD m_remoteDebuggingEnabledNVRAMOffset;
	DWORD m_partialWATOutEnabledNVRAMOffset;
	DWORD m_partialWATOutEnabledGlobalNVRAMOffset;
	DWORD m_autoUploadCreditsGlobalNVRAMOffset;
	DWORD m_cashoutToHostEnabledNVRAMOffset;
	DWORD m_cashoutToHostEnabledGlobalNVRAMOffset;
	DWORD m_extensibleFlagsNVRAMOffset;
	DWORD m_taxWithholdingEnabledOffsetNVRAMOffset;
	DWORD m_federalTaxWithholdingPercentOffsetNVRAMOffset;
	DWORD m_stateTaxWithholdingPercentOffsetNVRAMOffset;

	DWORD m_numberOfSubgamesMismatchCheckOffset;
	DWORD m_systemTimeSprinkleOffset;
	DWORD m_systemTimeOffset;

	DWORD m_configExt4CheckByteNVRAMOffset;
	DWORD m_rebootUiRestartOverrideOffset;

	DWORD m_configExt5CheckByteNVRAMOffset;
	DWORD m_displayPlayerProfileButtonOffset;
	DWORD m_globalMGMDOffset;
	DWORD m_gameStartExceptionsOffset;
	DWORD m_gameEndExceptionsOffset;
	DWORD m_gameListChunkSizeOffset;
	DWORD m_externalCreditTransferCheckByteOffset;

	DWORD m_configExt6CheckByteNVRAMOffset;
	DWORD m_bonusCounterNotificationPriorityOffset;
	DWORD m_bonusCounternRichLevelPriorityOffset;
	DWORD m_bonusCounterSplashdownPriorityOffset;
	DWORD m_bonusCounterProgressiveOdometerPriorityOffset;
	DWORD m_bonusCounterPtePriorityOffset;
	DWORD m_bonusCounterScatterCountdownTimerPriorityOffset;
	DWORD m_bonusCounterPrizePriorityOffset;

	DWORD m_configExt7CheckByteNVRAMOffset;
	DWORD m_globalExcessiveVoucherOutEnabledFlagOffset;
	DWORD m_excessiveVoucherOutThresholdOffset;
	DWORD m_totalBillInThresholdOffset;
	DWORD m_coinInThresholdOffset;
	DWORD m_coinOutThresholdOffset;
	DWORD m_bonusCounterPlayXGetYPriorityOffset;

	DWORD m_rebootAsapThresholdInMbOffset;
	DWORD m_rebootWarningThresholdMbOffset;
	DWORD m_restartUIAsapThresholdMbOffset;
	DWORD m_restartUIAfterDropThresholdMbOffset;
	DWORD m_restartUIDelayHoursOffset;


	// Use a check byte for CConfig extensions as the NVRAM version is incorrect on 
	// Braemac units with carrier board FPGA versions prior to R3.6.
	DWORD m_configExt2CheckByteNVRAMOffset;
	static const byte m_configExt2CheckByteValue = 0xc3; // 2015-07-16 Braemac units typically ship with 0xAA or 0x55 in this location.
	static const byte m_configExt3CheckByteValue = 0xc9; // 2015-07-16 Braemac units typically ship with 0xAA or 0x55 in this location.
	static const byte m_configExt4CheckByteValue = 0xcf; // 2015-12-30 Braemac units typically ship with 0xAA or 0x55 in this location.
	static const byte m_configExt5CheckByteValue = 0xd3; // 2016-05-25 Braemac units typically ship with 0xAA or 0x55 in this location.
	static const byte m_externalCreditTransferCheckByteValue = 0xd4; 
	static const byte m_configExt6CheckByteValue = 0xd9; // 2016-05-25 Braemac units typically ship with 0xAA or 0x55 in this location.
	static const byte m_configExt7CheckByteValue = 0xdf; // 2016-05-25 Braemac units typically ship with 0xAA or 0x55 in this location.

	bool ValidateCardReaderTrackNumber(BYTE &cardReaderTrack);

	void GetValue(DWORD nvramOffset, bool &value, bool defaultValue, int &invalidCount);
	void GetValue (DWORD nvramOffset, WORD &value, WORD defaultValue, WORD minimumValue, WORD maximumValue, int &invalidCount);

// #pragma region configuration extension members
private:
	void GetConfigExt2();

	void LoadConfigExtFromFileValues(const CConfigExtFile &configExtFile);

	void SetConfigExt1Defaults();
	void SetConfigExt2Defaults();
	void SetConfigExt3Defaults();
	void SetConfigExt4Defaults();
	void SetConfigExt5Defaults();
	void SetConfigExt6Defaults();
	void SetConfigExt7Defaults();

	void SetBonusCounterNotificationPriority(WORD priority);
	void SetBonusCounternRichLevelPriority(WORD priority);
	void SetBonusCounterSplashdownPriority(WORD priority);
	void SetBonusCounterProgressiveOdometerPriority(WORD priority);
	void SetBonusCounterPtePriority(WORD priority);
	void SetBonusCounterScatterCountdownTimerPriority(WORD priority);
	void SetBonusCounterPrizePriority(WORD priority);
	void SetBonusCounterPlayXGetYPriority(WORD priority);
	
	void SetExternalCreditTransferDefault();

public:
	/// <summary>
	/// Loads and deletes a configuration extensions file.
	/// </summary>
	void LoadConfigExtFile();

	/// <summary>
	/// Sets the negligible credits threshold for treating the credit meter as having zero credits.
	/// </summary>
	/// <param name="negligibleCreditsThreshold">IN - the threshold value.</param>
	/// <returns>The previously configured value.</returns>
	WORD SetNegligibleCreditsThreshold(WORD negligibleCreditsThreshold);

	/// <summary>
	/// Gets the negligible credits threshold for treating the credit meter as having zero credits.
	/// </summary>
	/// <returns>The negligible credits threshold for treating the credit meter as having zero credits.</returns>
	WORD GetNegligibleCreditsThreshold() const;

	/// <summary>
	/// Sets whether employee cards time out when inserted over a (virtual) player session.
	/// </summary>
	/// <param name="employeeOverPlayerIdleCardOut">IN - true to enable the time out, false to disable.</param>
	/// <returns>The previously configured value.</returns>
	bool SetEmployeeOverPlayerIdleCardOut(bool employeeOverPlayerIdleCardOut);

	/// <summary>
	/// Returns true if employee cards time out when inserted over a (virtual) player session, false otherwise.
	/// </summary>
	/// <returns>true if the time out is enabled, false otherwise.</returns>
	bool GetEmployeeOverPlayerIdleCardOut() const;

	/// <summary>
	/// Sets the time period for employee cards to time out when inserted over a (virtual) player session.
	/// </summary>
	/// <param name="employeeIdleCardOutSeconds">IN - The time out period.</param>
	/// <returns>The previously configured value.</returns>
	WORD SetEmployeeIdleCardOutSeconds(WORD employeeIdleCardOutSeconds);

	/// <summary>
	/// Gets the time period for employee cards to time out when inserted over a (virtual) player session.
	/// </summary>
	/// <returns>The time period for employee cards to time out when inserted over a (virtual) player session.</returns>
	WORD GetEmployeeIdleCardOutSeconds() const;

	/// <summary>
	/// Sets whether to recard a player after an employee card out.
	/// </summary>
	/// <param name="playerRecardEmployeeCardOut">IN - true to recard a player after an employee card out, false otherwise.</param>
	/// <returns>The previously configured value.</returns>
	bool SetPlayerRecardEmployeeCardOut(bool playerRecardEmployeeCardOut);

	/// <summary>
	/// Gets whether to recard a player after an employee card out.
	/// </summary>
	/// <returns>true to recard a player after an employee card out, false otherwise.</returns>
	bool GetPlayerRecardEmployeeCardOut() const;

	/// <summary>
	/// Sets time for showing promotion notification
	/// </summary>
	/// <returns>previous value</returns>
	WORD SetNotificationForShowPromotionTime(WORD showPromotionTime);

	/// <summary>
	/// Gets time for when promotions shows
	/// </summary>
	/// <returns>current value</returns>
	WORD GetNotificationForShowPromotionTime() const;

	/// <summary>
	/// Sets time for showing player tracking
	/// </summary>
	/// <returns>previous value</returns>
	WORD SetNotificationForPlayerTrackingTime(WORD playerTrackingTime);

	/// <summary>
	/// Gets time for when player tracking shows
	/// </summary>
	/// <returns>current value</returns>
	WORD GetNotificationForPlayerTrackingTime() const;

	/// <summary>
	/// Sets bool for enabling CustomButton1
	/// </summary>
	/// <returns>previous value</returns>
	bool SetCustomButton1Enabled(bool enable);

	/// <summary>
	/// Gets whether Custom button 1 is enabled
	/// </summary>
	/// <returns>current value</returns>
	bool GetCustomButton1Enabled() const;

	/// <summary>
	/// Sets bool for enabling CustomButton2
	/// </summary>
	/// <returns>previous value</returns>
	bool SetCustomButton2Enabled(bool enable);

	/// <summary>
	/// Gets whether Custom button 2 is enabled
	/// </summary>
	/// <returns>current value</returns>
	bool GetCustomButton2Enabled() const;

	/// <summary>
	/// Sets number of resort menu buttons
	/// </summary>
	/// <param name="count">the count</param>
	/// <returns>previous value</returns>
	WORD SetResortMenuButtonCount(WORD count);

	/// <summary>
	/// Gets count of Resort Menu buttons
	/// </summary>
	/// <returns>current value</returns>
	WORD GetResortMenuButtonCount() const;

	/// <summary>
	/// Sets number of custom menu 1 buttons
	/// </summary>
	/// <param name="count">the count</param>
	/// <returns>previous value</returns>
	WORD SetCustomMenuButton1Count(WORD count);

	/// <summary>
	/// Gets count of Custom Menu 1 buttons
	/// </summary>
	/// <returns>current value</returns>
	WORD GetCustomMenuButton1Count() const;

	/// <summary>
	/// Sets number of custom menu 2 buttons
	/// </summary>
	/// <param name="count">the count</param>
	/// <returns>previous value</returns>
	WORD SetCustomMenuButton2Count(WORD count);

	/// <summary>
	/// Gets count of Custom Menu 2 buttons
	/// </summary>
	/// <returns>current value</returns>
	WORD GetCustomMenuButton2Count() const;

	/// <summary>
	/// Sets the timeout seconds for an uncarded session active with negligible credits.
	/// </summary>
	/// <param name="sessionActiveNegligibleCreditsTimeoutSeconds">IN - The timeout seconds for an uncarded session active with negligible credits.</param>
	/// <returns>The previously configured value.</returns>
	WORD SetSessionActiveNegligibleCreditsTimeoutSeconds(WORD sessionActiveNegligibleCreditsTimeoutSeconds);

	/// <summary>
	/// Sets the timeout seconds for an uncarded session active with negligible credits.
	/// </summary>
	/// <returns>The timeout seconds for an uncarded session active with negligible credits.</returns>
	WORD GetSessionActiveNegligibleCreditsTimeoutSeconds() const;

	/// <summary>
	/// Sets the timeout seconds for an uncarded session active with non-negligible credits.
	/// </summary>
	/// <param name="sessionActiveNonNegligibleCreditsTimeoutSeconds">IN - The timeout seconds for an uncarded session active with non-negligible credits.</param>
	/// <returns>The previously configured value.</returns>
	WORD SetSessionActiveNonNegligibleCreditsTimeoutSeconds(WORD sessionActiveNonNegligibleCreditsTimeoutSeconds);

	/// <summary>
	/// Gets the timeout seconds for an uncarded session active with non-negligible credits.
	/// </summary>
	/// <returns>The timeout seconds for an uncarded session active with non-negligible credits.</returns>
	WORD GetSessionActiveNonNegligibleCreditsTimeoutSeconds() const;

	/// <summary>
	/// Sets the reboot UI restart override.
	/// </summary>
	/// <param name="rebootUiRestartOverride">if set to <c>true</c> [reboot UI restart override].</param>
	/// <returns>The previous value for whether reboot UI restart override are enabled</returns>
	bool SetRebootUiRestartOverride(bool rebootUiRestartOverride);

	/// <summary>
	/// Returns the reboot UI restart override enabled flag.
	/// </summary>
	/// <returns>true if RebootUiRestartOverride is enabled, false otherwise.</returns>
	bool GetRebootUiRestartOverrideEnabled() const;

	/// <summary>
	/// Sets the Game List Chunk size.
	/// </summary>
	/// <param name="gameListChunkSize">IN - Game List chunk size.</param>
	/// <returns>The previously configured value.</returns>
	WORD SetGameListChunkSize(WORD gameListChunkSize);

	/// <summary>
	/// Gets the Game List Chunk size.
	/// </summary>
	/// <returns>The Game List chunk size.</returns>
	WORD GetGameListChunkSize() const;

	/// <summary>
	/// Sets the SAS version.
	/// </summary>
	/// <param name="version">SAS version.</param>
	void SetSASVersion(const std::string& version);

	/// <summary>
	/// Sets the mixer firmware versions.
	/// </summary>
	/// <param name="mixerFirmwareVersion">The mixer firmware version.</param>
	/// <param name="m_touchFirmwareVersion">The m_touch firmware version.</param>
	void SetMixerFirmwareVersions(const std::string& mixerFirmwareVersion, const std::string& m_touchFirmwareVersion);

	void SetMixerFirmwareVersion(const std::string& mixerFirmwareVersion);

	/// <summary>
	/// Sets the target mixer firmware versions.
	/// </summary>
	/// <param name="mixerFirmwareVersion">The mixer firmware version.</param>
	/// <param name="carrierFirmwareVersion">The carrier firmware 
	/// version.</param> 
	void SetTargetFirmwareVersions(const std::string& mixerFirmwareVersion, const std::string& carrierFirmwareVersion);

	/// <summary>
	/// Sets the checksum value.
	/// </summary>
	/// <param name="value">The new checksum value.</param>
	void SetImageChecksum(DWORD value);

	/// <summary>
	/// Gets the checksum.
	/// </summary>
	/// <returns>checksum value</returns>
	DWORD GetImageChecksum() const;

	/// <summary>
	/// Sets the received bad denom flag.
	/// </summary>
	/// <param name="value">The received bad denom flag.</param>
	void SetReceivedBadDenomFlag(DWORD value);

	/// <summary>
	/// Gets the received bad denom flag.
	/// </summary>
	/// <returns>true if last denom received is bad, false otherwise.</returns>
	bool GetReceivedBadDenomFlag() const;

	/// <summary>
	/// Gets the firmware version.
	/// </summary>
	/// <returns>firmware version</returns>
	std::string GetFirmwareVersion() const;

private:
	WORD m_negligibleCreditsThreshold;
	bool m_employeeOverPlayerIdleCardOut;
	WORD m_employeeIdleCardOutSeconds;
	bool m_playerRecardEmployeeCardOut;

	WORD m_notificationShowPromotion;
	WORD m_notificationPlayerTracking;

	bool m_customButton1enable;
	WORD m_customMenuButton1Count;

	bool m_customButton2enable;
	WORD m_customMenuButton2Count;

	WORD m_sessionActiveNegligibleCreditsTimeoutSeconds;
	WORD m_sessionActiveNonNegligibleCreditsTimeoutSeconds;
	WORD m_gameListChunkSize;

	WORD m_resortMenuButtonCount;

	bool m_rebootUiRestartOverride;
	bool m_displayPlayerProfileButton;
	bool m_globalMGMD;
	bool m_gameStartExceptions;
	bool m_gameEndExceptions;
	bool m_isLcd;

	WORD m_bonusCounterNotificationPriority;
	WORD m_bonusCounternRichLevelPriority;
	WORD m_bonusCounterSplashdownPriority;
	WORD m_bonusCounterProgressiveOdometerPriority;
	WORD m_bonusCounterPtePriority;
	WORD m_bonusCounterScatterCountdownTimerPriority;
	WORD m_bonusCounterPrizePriority;
	WORD m_bonusCounterPlayXGetYPriority;

	bool m_globalExcessiveVoucherOutEnabledFlag;
	DWORD m_excessiveVoucherOutThreshold;
	DWORD m_totalBillInThreshold;
	DWORD m_coinInThreshold;
	DWORD m_coinOutThreshold;

	u_ExtensibleFlags m_extensibleFlags;

	//UJP Tax Withholding variables
	bool m_taxWithholdingEnabled;
	float m_federalTaxWithholdingPercent;
	float m_stateTaxWithholdingPercent;

	DWORD m_numberOfSubgamesMismatchCheck;
	DWORD m_countdownDisplayIntervalInSec;
	bool m_pointsCountdownControlByUniverse;

	std::string m_sasVersion;
	DWORD m_imageChecksum;
	std::string m_firmwareVersion;
	std::string m_mixerFirmwareVersion;
	std::string m_targetFirmwareVersion;
	std::string m_targetMixerFirmwareVersion;
	std::string m_touchFirmwareVersion;

	std::string m_extensibleSystemSettings;

    bool m_probePBTLock;

	bool m_senOptions[NUM_SEN_OPTIONS];
	static const std::string TXT_SEN_OPTIONS[NUM_SEN_OPTIONS];

	bool m_globalOptions[NUM_GLOBAL_OPTIONS];
	static const std::string TXT_GLOBAL_OPTIONS[NUM_GLOBAL_OPTIONS];

	bool m_receivedBadDenom;

	FRIEND_TEST(ConfigUnitTests, NVRAMOffsetsUnitTest);
};
