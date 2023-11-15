#pragma once

#include "Ticket.h"
#include "Config/Config.h"
#include "InternalMsgContent.h"

class CTicketingInfo :
	public CInternalMsgContent<INT_MSG_CONTENT_TICKETINGINFO>
{
public:
	/// <summary>
	/// Standard CConfig constructor. Initializes a new instance of the <see cref="CTicketingInfo"/> class.
	/// </summary>
	/// <param name="config">The config.</param>
	CTicketingInfo( const CConfig &config );

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CTicketingInfo"/> class.
	/// </summary>
	/// <param name="infoRHS">The instance to copy from.</param>
	CTicketingInfo(CTicketingInfo &infoRHS);

	virtual ~CTicketingInfo(void);

	CTicketingInfo &operator=( CTicketingInfo &infoRHS );  

	TicketExpirationType GetExpirationType( void )
	{
		return m_expType;
	}

	void SetExpirationType( TicketExpirationType expType )
	{
		m_expType = expType;
	}

	int GetExpirationDuration( void )
	{
		return m_ticketExpirationDuration;
	}

	void SetExpirationDuration( int expDuration )
	{
		m_ticketExpirationDuration = expDuration;
	}

	void ConfigSettings( const CConfig &config );

	/// <summary>
	/// Gets the ticket expiration duration in days.
	/// </summary>
	/// <returns>The ticket expiration duration in days.</returns>
	WORD GetExpirationInDays( void );

	void SetTicketingEnabled( bool bTicketingEnabled )
	{
		m_bTicketingIsEnabled = bTicketingEnabled;
	}
	bool IsTicketingEnabled( void )
	{
		return m_bTicketingIsEnabled;
	}
	bool IsTicketingInEnabled( void )
	{
		return (m_bTicketingInIsEnabled && m_bTicketingIsEnabled);
	}
	bool IsTicketingOutEnabled( void )
	{
		return (m_bTicketingOutIsEnabled && m_bTicketingIsEnabled);
	}

	bool IsTicketBufferingAllowed( void )
	{
		return m_bAllowTicketBuffering;
	}

	bool IsVoucherPromoInEnabled( void )
	{
		return m_bVoucherPromoInEnabled;
	}

	bool IsCouponPromoEnabled( void )
	{
		return m_bCouponPromoInEnabled;
	}

	void SetCasinoNameAddress( LPCTSTR name, LPCTSTR address1, LPCTSTR address2 )
	{
		m_ticketCasinoName = name;
		m_ticketCasinoAddress1 = address1;
		m_ticketCasinoAddress2 = address2;
	}

	byte GetCasinoNumber( void )
	{
		return m_ticketCasinoNumber;
	}

	const std::string &GetCasinoName( void )
	{
		return m_ticketCasinoName;
	}
	const std::string &GetCasinoAddress1(void)
	{
		return m_ticketCasinoAddress1;
	}
	const std::string &GetCasinoAddress2(void)
	{
		return m_ticketCasinoAddress2;
	}

	long GetSlotNumber( void )
	{
		return m_slotNumber;
	}


private:
	TicketExpirationType m_expType;
	int m_ticketExpirationDuration;

	bool m_bTicketingIsEnabled;
	bool m_bTicketingInIsEnabled;
	bool m_bTicketingOutIsEnabled;

	bool m_bAllowTicketBuffering;

	bool m_bVoucherPromoInEnabled;

	bool m_bCouponPromoInEnabled;

	byte m_ticketCasinoNumber;
	std::string m_ticketCasinoName;
	std::string m_ticketCasinoAddress1;
	std::string m_ticketCasinoAddress2;

	long m_slotNumber;
};
