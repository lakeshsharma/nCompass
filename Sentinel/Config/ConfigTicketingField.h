#pragma once
#include "ConfigField.h"
#include "../Ticket.h"

class CConfigTicketingField :
	public CConfigField
{
public:
	static const int OLDER_FLOORLOGIX_BUFFER_LENGTH = 5;

	CConfigTicketingField(void);
	~CConfigTicketingField(void);

	void SetTicketingOptions(const byte * buffer, int length);
	TicketExpirationType GetTicketExpirationType() const;
	WORD GetTicketExpirationDuration() const;
	bool TicketingEnabled() const;
	bool SystemGeneratedTicketValidationNumbers() const;
	bool TicketBufferingEnabled() const;

private:

	TicketExpirationType m_ticketExpirationType;
	WORD m_ticketExpirationDuration;
	bool m_ticketingEnabled;
	bool m_systemGeneratedTicketValidationNumbers;
	bool m_ticketBufferingEnabled;
};
