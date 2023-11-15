#pragma once
#include "Hardware/MemoryStorage.h"
#include "TicketingInfo.h"
#include "Config/Config.h"
#include "Ticket.h"
#include "UI/Logs.h"

const int MAX_BUFFERED_TICKETS = 30;

class CTickets : public CDataSubscriptionProvider,
			     public CMemoryStorage
{
public:
	/// <summary>
	/// Creates tickets object.
	/// </summary>
	/// <param name="config">IN - Config object to use for configuring ticket information.</param>
	CTickets( const CConfig &config );

	/// <summary>
	/// Destructor responsible for deleting stored tickets.
	/// </summary>
	virtual ~CTickets(void);

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	/// <summary>
	/// Configure ticket information from config object.
	/// </summary>
	/// <param name="config">IN - Config object to use for configuring ticket information.</param>
	void ConfigSettings( const CConfig &config );

	/// <summary>
	/// Get CTicketingInfo object
	/// </summary>
	/// <returns>A pointer with read/write access to the SCTicketingInfo object</returns>
	CTicketingInfo * GetTicketingInfo( void );

	/// <summary>
	/// Conditionally stores a ticket object.
	/// </summary>
	/// <param name="ticket">IN - CTicket object to store</param>
	/// <returns>True if stored ticket was a duplicate</returns>
	bool StoreTicket( const CTicket &ticket );
	
	/// <summary>
	/// Updates stored ticket object
	/// </summary>
	/// <param name="ticket">IN - CTicket object to use for update</param>
	/// <returns>True if ticket was updated</returns>
	bool UpdateTicket( const CTicket &ticket );

	/// <summary>
	/// Logs final ticket status and removes stored ticket in case of a ticket in.
	/// </summary>
	/// <param name="ticket">IN - CTicket object to finalize</param>
	/// <returns></returns>
	void FinalizeTicket( const CTicket &ticket );

	/// <summary>
	/// Removes stored ticket in.
	/// </summary>
	/// <remarks>
	/// Exported as a public function to support existing unit tests.
	/// </remarks>
	void RemoveTicketIn( void );

	/// <summary>
	/// Removes stored ticket out.
	/// </summary>
	/// <remarks>
	/// Exported as a public function to support existing unit tests.
	/// </remarks>
	void RemoveTicketOut( void );

	/// <summary>
	/// Sets the ticket out pending count.
	/// </summary>
	/// <param name="pendingCount">IN - The ticket out pending count.</param>
	void SetNumberOfTicketsPending( int pendingCount );

	/// <summary>
	/// Gets number of ticket outs pending.
	/// </summary>
	/// <returns>Number of printed tickets pending.</returns>
	int GetNumberOfTicketsPending( void );

	/// <summary>
	/// Determines if maximum ticket pending count has been reached.
	/// </summary>
	/// <returns>True if max count has been reached.</returns>
	bool HasMaxTicketOutPending( void ) const;

	/// <summary>
	/// Gets the maximum ticket pending count.
	/// </summary>
	/// <returns>Returns maximum ticket out pending allowed.</returns>
	int GetMaxTicketOutPending( void ) const;

	/// <summary>
	/// Gets a pointer with read/write access to the ticket logs.
	/// </summary>
	/// <returns>Returns a pointer with read/write access to the ticket logs. Never returns NULL.</returns>
	CLogs *GetTicketLogs( void );

	/// <summary>
	/// Resets the ticket in expire time.
	/// </summary>
	void ResetTicketInExpire( void );

	/// <summary>
	/// Determines if ticket in has expired.
	/// </summary>
	/// <returns>True if ticket is expired, false otherwise.</returns>
	bool HasTicketInExpired( void ) const;

	/// <summary>
	/// Determines if ticket in exists.
	/// </summary>
	/// <returns>True if ticket in exists, false otherwise.</returns>
	bool DoesTicketInExist( void ) const;

	/// <summary>
	/// Gets the ticket in sequence number.
	/// </summary>
	/// <returns>Returns the sequence number.</returns>
	WORD GetTicketInSequenceNumber( void ) const;

	/// <summary>
	/// Sets the final status received flag.
	/// </summary>
	void SetFinalStatusReceived( void );

	/// <summary>
	/// Gets the final status received.
	/// </summary>
	/// <returns>Final status received.</returns>
	bool WasFinalStatusReceived( void ) const;

	/// <summary>
	/// Determines if the specified ticket is a duplicate of stored ticket.
	/// </summary>
	/// <param name="ticket">IN - The specified ticket</param>
	/// <returns>True if the specified ticket is a duplicate, false otherwise.</returns>
	bool IsDuplicate( const CTicket &ticket );

	/// <summary>
	/// Compares the ticket amount with the existing ticket amount.
	/// </summary>
	/// <param name="ticket">The new ticket.</param>
	/// <returns>True if the ticket amounts match or no existing ticket.</returns>
	bool CompareTicketInAmount(CTicket *ticket);

	/// <summary>
	/// Get a pointer to the validation number.
	/// </summary>
	/// <param name="validationLength">OUT - The length of the validation number.</param>
	/// <returns>A pointer to the validation number.</returns>
	byte *TicketInValidationNumber(byte &validationLength) const;

	__int64 TicketInAmount() const;

	/// <summary>
	/// Sets the voucher buffering enabled.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetVoucherBufferingEnabled(bool value);

	CTicket* GetTicketIn() const { return m_ticketIn; }

private:
	/// <summary>
	/// Sets the instance from memory.
	/// </summary>
	virtual void BuildYourself();

	/// <summary>
	/// Stores tickets data to NVRAM
	/// </summary>
	void StoreTicketsDataToNVRAM( void );

	/// <summary>
	/// Stores ticket in data to NVRAM
	/// </summary>
	void StoreTicketInToNVRAM( void );

	/// <summary>
	/// Stores ticket out data to NVRAM
	/// </summary>
	void StoreTicketOutToNVRAM( void );

private:
	CTicketingInfo *m_ticketingInfo;

	CTicket *m_ticketOut;
	CTicket *m_ticketIn;

	int m_maxTicketsOutPending;
	int m_currentTicketsOutPending;

	time_t m_ticketInExpireTime;

	bool m_finalStatusReceived;
	bool m_voucherBufferingEnabled;
	CLogs m_ticketLogs;
};
