#pragma once

#include "../Ticket.h"
#include "Log.h"
#include "../GlobalDefs.h"

const int NUMBER_OF_DIGITS_TO_MASK = 5;

class CTicketLog: public CLog
{
public:
	/// <summary>
	/// Standard CTicket constructor. Initializes a new instance of the <see cref="CTicketLog"/> class.
	/// </summary>
	/// <param name="ticket">IN - The CTicket instance to construct from.</param>
	CTicketLog( const CTicket &ticket );

	/// <summary>
	/// NVRAM buffer constructor. Initializes a new instance of the <see cref="CTicketLog"/> class.
	/// </summary>
	/// <param name="nvramBuffer">The nvram buffer. MUST NOT be NULL.</param>
	CTicketLog( const byte *nvramBuffer);

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CTicketLog"/> class.
	/// </summary>
	/// <param name="tRHS">IN - The instance to copy from.</param>
	CTicketLog( const CTicketLog &tRHS );

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CTicketLog"/> class.
	/// </summary>
	~CTicketLog(void);

	/// <summary>
	/// Assignment operator.
	/// </summary>
	/// <param name="tRHS">IN - The instance to assign from.</param>
	/// <returns>A reference to the target of the assignment</returns>
	CTicketLog &operator=( const CTicketLog &tRHS );

	/// <summary>
	/// Gets the display text for the ticket in (redemption attempt) result.
	/// </summary>
	/// <returns>The display text for the ticket in (redemption attempt) result.</returns>
	std::string GetTicketInActionText(void) const;

	/// <summary>
	/// Sets a byte buffer with data that represents the CTicketLog instance.
	/// The byte buffer is suitable for storing in NVRAM.
	/// </summary>
	/// <param name="nvramBuffer">The byte buffer to be set.</param>
	/// <param name="nvramBufferLength">Length of the byte buffer.</param>
	/// <returns>The number of bytes of nvramBuffer that were set.</returns>
	int GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength ) const;

	/// <summary>
	/// Gets the amount associated with the ticket.
	/// </summary>
	/// <returns>The amount associated with the ticket.</returns>
	__int64 GetAmount( void ) const;

	/// <summary>
	/// Returns true if the instance is for a ticket inserted, false otherwise.
	/// </summary>
	/// <returns>true if the instance is for a ticket inserted, false otherwise.</returns>
	bool GetTicketIn( void ) const;

	/// <summary>
	/// Gets the validation num.
	/// </summary>
	/// <returns>The validation num.</returns>
	std::string GetValidationNum() const;

	/// <summary>
	/// Gets the masked validation num.
	/// </summary>
	/// <returns>The masked validation num.</returns>
	std::string GetMaskedValidationNum() const;

	/// <summary>
	/// Gets the sequence number.
	/// </summary>
	/// <returns>The sequence number.</returns>
	int GetSequenceNum( void ) const;	   

	/// <summary>
	/// Gets the ticket action code.
	/// </summary>
	/// <returns>The ticket action code.</returns>
	TicketActionCode GetTicketActionCode( void ) const;

private:
	__int64 m_amount;
	bool m_isTicketIn;
	TicketActionCode m_ticketAction;
	std::string m_validationNum;
	WORD m_sequenceNum;
};
