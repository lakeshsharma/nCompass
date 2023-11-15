#pragma once
#include "SASPollMessage.h"
#include "../Ticket.h"


class CLongPoll71Message :
	public CSASPollMessage
{
public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CLongPoll71Message"/> class.
	/// </summary>
	/// <param name="gameAddress">IN - The game address.</param>
	/// <remarks>
	/// m_bValidationExtensions and GetValidationExtensionsSupported() default to true.
	/// </remarks>
	CLongPoll71Message(byte gameAddress);

	CTicket *m_sendTicket;
	CTicket *m_responseTicket;

public:
	~CLongPoll71Message(void);
public:
	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);

	/// <summary>
	/// Sets the send buffer according to the previously specified instance values.
	/// </summary>
	void Fill();

	CTicket *GetResponseTicket( void )
	{
		return m_responseTicket;
	}

	void SetSendTicket(CTicket *ticket)
	{
		m_sendTicket = ticket;
	}

	/// <summary>
	/// Sets the ticket expiration duration in days.
	/// </summary>
	/// <param name="ticketExpirationDuration">IN - The ticket expiration duration in days.</param>
	void SetTicketExpirationDuration(WORD ticketExpirationDuration);

	bool GetValidationExtensionsSupported()
	{
		return m_bValidationExtensions;
	}

	void SetValidationExtensionsSupported(bool isSupported)
	{
		m_bValidationExtensions = isSupported;
	}

	void SetStatusQuery()
	{
		m_statusQuery = true;
	}

	void ClearStatusQuery()
	{
		m_statusQuery = false;
	}

	byte m_ticketStatus;

	static void FormatTicketExpire(time_t validationDate, time_t currentDate, DWORD expireDurationDays, byte *expireBCD, int expireBCDLen);

private:
	bool m_statusQuery;
	bool m_bValidationExtensions;	
	WORD m_ticketExpirationDuration;
	
	static const DWORD DaysInYear;

};
