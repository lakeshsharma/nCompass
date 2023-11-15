#include "stdafx.h"
#include "TicketingInfo.h"

// Sentinel includes.
#include "SASProtocol/SASPollMessage.h"

CTicketingInfo::CTicketingInfo( const CConfig &config )
{
	ConfigSettings( config );
}

CTicketingInfo::~CTicketingInfo(void)
{
}

CTicketingInfo::CTicketingInfo(CTicketingInfo &tiRHS)
{
	*this = tiRHS;
}

CTicketingInfo & CTicketingInfo::operator=( CTicketingInfo &tiRHS )
{
	m_expType = tiRHS.GetExpirationType();
	m_ticketExpirationDuration = tiRHS.GetExpirationDuration();

	m_bTicketingIsEnabled = tiRHS.IsTicketingEnabled();
	m_bTicketingInIsEnabled = tiRHS.IsTicketingInEnabled();
	m_bTicketingOutIsEnabled = tiRHS.IsTicketingOutEnabled();

	m_bAllowTicketBuffering = tiRHS.IsTicketBufferingAllowed();
	m_bVoucherPromoInEnabled = tiRHS.IsVoucherPromoInEnabled();
	m_bCouponPromoInEnabled = tiRHS.IsCouponPromoEnabled();

	m_ticketCasinoNumber = tiRHS.GetCasinoNumber();

	m_ticketCasinoName = tiRHS.GetCasinoName();

	m_ticketCasinoAddress1 = tiRHS.GetCasinoAddress1();
	m_ticketCasinoAddress2 = tiRHS.GetCasinoAddress2();

	m_slotNumber = tiRHS.GetSlotNumber();

	return *this;
}

void CTicketingInfo::ConfigSettings( const CConfig &config )
{
	m_expType = config.GetTicketExpirationType();
	m_ticketExpirationDuration = config.GetTicketExpirationDuration();
	m_bTicketingIsEnabled = config.TicketingEnabled();
	m_bTicketingInIsEnabled = config.TicketInEnabled();
	m_bTicketingOutIsEnabled = config.VoucherOutEnabled();
	m_bAllowTicketBuffering = config.TicketBufferingEnabled();
	m_bVoucherPromoInEnabled = config.VoucherPromoInEnabled();
	m_bCouponPromoInEnabled = config.CouponPromoInEnabled();
	m_ticketCasinoNumber = config.GetCasinoNumber();
	m_ticketCasinoName = config.GetCasinoName();
	m_ticketCasinoAddress1 = config.GetCasinoAddress1();
	m_ticketCasinoAddress2 = config.GetCasinoAddress2();
	m_slotNumber = config.GetSlotNumber();
}

// Ruturns 0 idf ticket does not expire or expires more than 256 days from now.
WORD CTicketingInfo::GetExpirationInDays( void )
{

	WORD daysToExpire = MAX_DAYS_TO_EXPIRE_TICKETS;

	switch( m_expType )
	{
	case EXPIRES_SECONDS:
	case EXPIRES_MINUTES:
	case EXPIRES_HOURS:
		daysToExpire = 1;
		break;
	case EXPIRES_DAYS:
		if ( m_ticketExpirationDuration <= MAX_DAYS_TO_EXPIRE_TICKETS )
		{
			daysToExpire = m_ticketExpirationDuration;
		}
		break;
	case EXPIRES_YEARS:
		daysToExpire = 0;
		break;
	default:
		break;
	}

	return daysToExpire;
}
