#pragma once
#include "../Hardware/MemoryStorage.h"
#include "../GlobalDefs.h"
#include "EEPROMConfig.h"
#include "InternalMsgContent.h"
#include "DataSubscriptionProvider.h"

namespace SentinelNativeUnitTests
{
	class EGMConfigTests;
}

class CEGMConfig : public CDataSubscriptionProvider,
				   public CEEPROMConfig, 
				   public CInternalMsgContent<INT_MSG_CONTENT_EGMCONFIG>
{
public:
	CEGMConfig( MEMORY_TYPE memoryType );
	CEGMConfig( CEGMConfig &eRHS );
	CEGMConfig &operator=( CEGMConfig &eRHS );
	virtual ~CEGMConfig(void);

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	void SetEGMType( EGMType egmType );
	virtual void BuildYourself();

	EGMType GetEGMType( void ) const
	{
		return m_egmType;
	}

	void SetEGMProtocol( EGMProtocol egmProtocol )
	{
		m_egmProtocol = egmProtocol;
		StoreDataToEEPROM();
	}
	
	/// <summary>
	/// Gets the egm protocol.
	/// </summary>
	/// <returns>the egm protocol</returns>
	EGMProtocol GetEGMProtocol(void) const;

	/// <summary>
	/// Gets the EGM protocol type, accounting for the EGM protocol state.
	/// </summary>
	/// <returns>The EGM protocol if the EGM protocol state is protocolStateEnabled, otherwise NO_EGM_PROTOCOL.</returns>
	EGMProtocol GetEGMProtocolType() const;

	/// <summary>
	/// Gets whether the EGM protocol supports progressives.
	/// </summary>
	/// <returns>true if GetEGMProtocolType() is SAS5xx or SAS6xx, otherwise false.</returns>
	bool GetProgressivesSupportedByEGM() const;

	/// <summary>
	/// Sets the EGM polling speed code.
	/// </summary>
	/// <param name="pollingSpeed">IN - The EGM polling speed code.</param>
	void SetEGMPollingSpeed( EGMPollingSpeed pollingSpeed );

	/// <summary>
	/// Gets the EGM polling speed code.
	/// </summary>
	/// <returns>The EGM polling speed code.</returns>
	EGMPollingSpeed GetEGMPollingSpeed( void ) const;

	/// <summary>
	/// Sets the EGM protocol state.
	/// </summary>
	/// <param name="pollingSpeed">IN - The EGM protocol state.</param>
	void SetEGMProtocolState( EGMProtocolState protocolState );
		
	/// <summary>
	/// Gets the EGM protocol state.
	/// </summary>
	/// <returns>The EGM protocol state</returns>
	EGMProtocolState GetEGMProtocolState( void ) const;

	void SetSentinelOnlineState( SentinelOnlineState state, bool bSetInPersistMemory = true )
	{
		m_sentinelOnlineState = state;
		if (sentinelComingOnline == m_sentinelOnlineState) m_slotMastIdResponse.clear();
		if ( bSetInPersistMemory ) StoreDataToEEPROM();
	}

	SentinelOnlineState GetSentinelOnlineState( void )
	{
		return m_sentinelOnlineState;
	}

	/// <summary>
	/// Gets the online state string.
	/// </summary>
	/// <returns>The online state string</returns>
	std::string GetSentinelOnlineStateStr() const;

	bool CanPerformMachineOnline() const
	{
		return ( m_egmProtocol != NO_EGM_PROTOCOL && m_sentinelOnlineState == sentinelOffline );
	}

	bool CanPerformMachineOffline() const
	{
		return ( m_egmProtocolState == protocolStateEnabled && m_sentinelOnlineState == sentinelOnline );
	}

	bool CanBeginMachineMove() const
	{
		return ( m_egmProtocolState == protocolStateEnabled && m_sentinelOnlineState == sentinelOnline );
	}

	bool CanCompleteMachineMove() const
	{
		return ( m_egmProtocol != NO_EGM_PROTOCOL && m_sentinelOnlineState == sentinelOfflineMove );
	}

	void SetSlotmastIDResponse(SlotmastIDResponse response);

private:
	EGMType m_egmType;
	EGMProtocol m_egmProtocol;
	EGMPollingSpeed m_egmPollingSpeed;
	EGMProtocolState m_egmProtocolState;
	SentinelOnlineState m_sentinelOnlineState;
	std::string	m_slotMastIdResponse;

	void StoreDataToEEPROM( void );

private:
	friend SentinelNativeUnitTests::EGMConfigTests;
};

