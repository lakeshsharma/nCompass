#include "stdafx.h"
#include "Employee.h"

const int GLOBAL_GLOBAL_CARD_LEN = 5;
const byte GLOBAL_GLOBAL_CARD_NUM[GLOBAL_GLOBAL_CARD_LEN] = { 0x69, 0x09, 0xff, 0x00, 0x01 };

const TCHAR * GLOBAL_GLOBAL_PASSWORD = "31459";

const DWORD EMPLOYEE_PIN_VALIDATION_TIMEOUT	= 10000;	// Pin validation timeout in milliseconds.

CEmployee::CEmployee(CCard *card,
	const CConfig &config,
	bool goodbyeEnabled,
	bool idleTimeout,
	DWORD idleTimeoutDuration,
	DWORD activityTimestamp) :
	m_isGoodbyeEnabled(goodbyeEnabled),
	m_idleTimeout(idleTimeout),
	m_idleTimeoutDuration(idleTimeoutDuration),
	m_activityTimestamp(activityTimestamp)
{
	m_card = card;
	m_bIsVirtual = false;

	m_bEmployeePinRequired = config.EmployeePinRequired();
	m_globalGlobalPassword = config.GetGlobalCardPassword();
	m_bWaitingForPin = false;

	if ( m_card != NULL && m_card->Compare( (byte *)GLOBAL_GLOBAL_CARD_NUM, GLOBAL_GLOBAL_CARD_LEN) )
	{
		m_bIsGlobalGlobal = true;
	}
	else
	{
		m_bIsGlobalGlobal = false;
	}
}

CEmployee::CEmployee(const CEmployee &empRHS)
{
	m_bWaitingForPin = false;
	m_bEmployeePinRequired = false;

	*this = empRHS;
}

CEmployee::~CEmployee(void)
{
	if (m_card != NULL)
	{
		delete m_card;
	}
}

CEmployee & CEmployee::operator=( const CEmployee &empRHS )
{
	const CCard *card = empRHS.GetCard();
	if ( card != NULL )
		m_card = new CCard( *card );
	else
		m_card = NULL;

	m_bIsVirtual = empRHS.IsVirtual();
	m_isGoodbyeEnabled = empRHS.m_isGoodbyeEnabled;
	m_idleTimeout = empRHS.m_idleTimeout;
	m_idleTimeoutDuration = empRHS.m_idleTimeoutDuration;
	m_activityTimestamp = empRHS.m_activityTimestamp;

	if ( m_card != NULL && m_card->Compare( (byte *)GLOBAL_GLOBAL_CARD_NUM, GLOBAL_GLOBAL_CARD_LEN) )
	{
		m_bIsGlobalGlobal = true;
	}
	else
	{
		m_bIsGlobalGlobal = false;
	}

	return *this;
}

CardType CEmployee::GetEmployeeCardType( void )
{
	CardType retVal = CARD_UNKNOWN;

	if ( m_card != NULL )
	{
		retVal = m_card->GetCardType();
	}

	return retVal;
}


void CEmployee::CardRemoved( const time_t &removalTime, bool bMakeVirtual )
{
	m_card->SetCardOutTime( removalTime );

	if ( bMakeVirtual )
	{
		MakeVirtual( removalTime );
	}
}

bool CEmployee::MakeVirtual( const time_t &cardOutTime )
{
	bool retVal = true;

	if ( !m_bIsVirtual )
	{
		m_card->SetCardOutTime( cardOutTime );
		m_bIsVirtual = true;
	}
	else
	{
		retVal = false;
	}

	return retVal;
}

void CEmployee::MakeActive( CCard *newCard )
{
	if ( m_card != NULL )
	{
		delete m_card;
		m_card = newCard;
	}

	if ( m_card != NULL && m_card->Compare( (byte *)GLOBAL_GLOBAL_CARD_NUM, GLOBAL_GLOBAL_CARD_LEN) )
	{
		m_bIsGlobalGlobal = true;
	}
	else
	{
		m_bIsGlobalGlobal = false;
	}
}

bool CEmployee::IsGoodbyeEnabled() const
{
	return m_isGoodbyeEnabled;
}

void CEmployee::UpdateActivityTimestamp(DWORD activityTimestamp)
{
	m_activityTimestamp = activityTimestamp;
}

bool CEmployee::HasIdleTimeoutOccurred(DWORD tickCount) const
{
	bool timeout(false);

	if (m_idleTimeout)
	{
		DWORD elapsedTickCount = tickCount - m_activityTimestamp;
		timeout = elapsedTickCount >= m_idleTimeoutDuration;
	}

	return timeout;
}

bool CEmployee::IsVirtual(void) const
{
	return m_bIsVirtual;
}

const CCard *CEmployee::GetCard(void) const
{
	return m_card;
}

bool CEmployee::ComparePin( const std::string &pin )
{
	bool bPinMatches = false;

	if ( m_pin == pin )
	{
		bPinMatches = true;
	}

	else if ( m_bIsGlobalGlobal && (m_globalGlobalPassword == pin || GLOBAL_GLOBAL_PASSWORD == pin) )
	{
		bPinMatches = true;
	}

	return bPinMatches;
}

void CEmployee::SetWaitingForPin( bool bWaitingForPin )
{
	m_bWaitingForPin = bWaitingForPin;

	if ( bWaitingForPin )
	{
		m_pinRequestTickCount = CUtilities::GetTickCount();
	}
}

// 2011-04-25  Caller is responsible for handling timeout when m_bIsGlobalGlobal is true. 
bool CEmployee::IsDoneWaitingForPin( void )
{
	bool bDone = false;

	if ( m_bWaitingForPin )
	{
		DWORD currentTick = CUtilities::GetTickCount();
		if ( currentTick >= m_pinRequestTickCount + EMPLOYEE_PIN_VALIDATION_TIMEOUT || currentTick < m_pinRequestTickCount )
		{
			bDone = true;
		}
	}

	return bDone;
}
