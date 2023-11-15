#include "stdafx.h"

#include "InternalsFramework.h"
#include "Diagnostics.h"
#include "resource.h"

using namespace std;

// Initialize tickets at startup
void CInternalsFramework::InitializeTickets( void )
{
	lock_guard<recursive_mutex> lock(m_ticketsCriticalSection);

	m_tickets = new CTickets( m_config );

	SendTicketingInfoToGCF();

	m_dataSubscriptionManager.RegisterProvider(m_tickets);
}

// Uninitialize tickets at shutdown
void CInternalsFramework::UninitializeTickets( void )
{
	lock_guard<recursive_mutex> lock(m_ticketsCriticalSection);

	if ( m_tickets != NULL )
	{
		delete m_tickets;
		m_tickets = NULL;
	}
}

void CInternalsFramework::SetTicketsConfig( void )
{
	lock_guard<recursive_mutex> lock(m_ticketsCriticalSection);

	if ( m_tickets != NULL )
	{
		m_tickets->ConfigSettings( m_config );

	}
}

void CInternalsFramework::SendTicketingInfoToGCF( void )
{
	lock_guard<recursive_mutex> lock(m_ticketsCriticalSection);

	if ( m_tickets != NULL )
	{
		CTicketingInfo *ticketingInfo = new CTicketingInfo( *m_tickets->GetTicketingInfo() );

		CInternalMessage *msg = new CInternalMessage( INT_MSG_SET_TICKETING_INFO, ticketingInfo );
		SendMessageToGCF( msg, QUEUE_PRIORITY_HIGH );	
	}
}

void CInternalsFramework::ProcessTicketInserted( void )
{
	lock_guard<recursive_mutex> lock(m_ticketsCriticalSection);

	LogString(TICKETING_LOG, "ProcessTicketInserted");

	// If the ticket-in exists we need to see if we can get a final status on it
	if ( m_tickets->DoesTicketInExist() )
	{ 
		LogString(TICKETING_LOG, "ProcessTicketInserted-RemoveTicketIn ticketNumber=%d amount=%lld sequenceNumber=%u ID=%lld", 
			m_tickets->GetTicketIn()->Number(),
			m_tickets->GetTicketIn()->Amount(),
			m_tickets->GetTicketIn()->SequenceNumber(),
			m_tickets->GetTicketIn()->ID());

		GenerateTilt(tltFinalTicketInStatusNotReceived);
		m_tickets->RemoveTicketIn();
	}
	
	LogString(TICKETING_LOG, "ProcessTicketInserted-Sending INT_MSG_PROCESS_TICKET_IN");
	// Set the ticket-in to expire 30 seconds from now.
	m_tickets->ResetTicketInExpire();

	CInternalMessage *msg = new CInternalMessage( INT_MSG_PROCESS_TICKET_IN );
	SendMessageToGCF( msg, QUEUE_PRIORITY_HIGH );	
}

void CInternalsFramework::ProcessTicketFromGame( CTicket &ticket )
{
	lock_guard<recursive_mutex> lock(m_ticketsCriticalSection);

	// Internals knows the machine number, so set it in the ticket.
	ticket.SetMachineNumber( m_config.GetSlotNumber() );

	// Store the ticket in the tickets object
	// Only call store ticket if it is a duplicate ticket or ticket in. Printed tickets get stored later.
	bool bDuplicate = m_tickets->IsDuplicate( ticket );

	LogString(TICKETING_LOG, "ProcessTicketFromGame IsTicketIn=%u bDuplicate=%u.", ticket.IsTicketIn(), bDuplicate);

	if (ticket.IsTicketIn() || bDuplicate)
	{
		LogString(TICKETING_LOG, "ProcessTicketFromGame StoreTicket");
		m_tickets->StoreTicket( ticket );
	}

	// Send the ticket transaction message to the SCF if it is not a duplicate.
	if ( !bDuplicate )
	{
		CTicket *transTicket = new CTicket( ticket );
		CInternalMessage *transactionMsg = PrepareTransactionMessage( INT_MSG_TRANSACTION_TICKET, IsPlayerVirtual() );
		transactionMsg->SetData( transTicket );
		
		//Add play session id
		DWORD psid = m_playerSession.GetSessionId();
        LogString(POLLER_LOG, "%s: PollerProtocol: Play Session ID Sent from internal: %u", __FUNCTION__, psid);
		transactionMsg->SetData(psid);

		SendTransactionMessageToSCF( transactionMsg, QUEUE_PRIORITY_HIGH );

		LogString(TICKETING_LOG, "ProcessTicketFromGame sending INT_MSG_TRANSACTION_TICKET to the Poller psid=%u", psid);
	}

	// If the ticket was duplicate, tell the GCF that it was already stored
	else
	{
		LogString(TICKETING_LOG, "ProcessTicketFromGame ticket was duplicate - sending INT_MSG_ALLOW_TICKET_PRINTED to GCF");

		SendTicketStoredToGCF( ticket );

		// If we have not reached our maximum ticket pending, tell GCF that it is cool to print tickets.
		if ( !m_tickets->HasMaxTicketOutPending() )
		{
			CInternalMessage *msg = new CInternalMessage( INT_MSG_ALLOW_TICKET_PRINTED );
			SendMessageToGCF( msg, QUEUE_PRIORITY_HIGH );	
		}
	}
}

void CInternalsFramework::ProcessTicketStored( CTicket &ticket )
{
	// Indicates ticket transaction stored
	lock_guard<recursive_mutex> lock(m_ticketsCriticalSection);
	{		
		if ( ticket.IsTicketIn() )
		{
			if ( m_tickets->WasFinalStatusReceived() )
			{
				LogString(TICKETING_LOG, "ProcessTicketStored-FinalStatusReceived-FinalizeTicket");
				m_tickets->FinalizeTicket( ticket );
			}
		}
		else 
		{
			LogString(TICKETING_LOG, "ProcessTicketStored-StoreTicket-SendTicketStoredToGCF");
			// Printed tickets don't get stored in NVRAM until after transaction is stored. Store it now
			m_tickets->StoreTicket(ticket);
			SendTicketStoredToGCF( ticket );
		}
	}
}

void CInternalsFramework::ProcessTicketResponse( CTicket &ticket, DWORD scfTicketID )
{
	lock_guard<recursive_mutex> lock(m_ticketsCriticalSection);

	WORD wIsTicketIn = ticket.IsTicketIn();
	// Internals knows the machine number, so set it in the ticket.
	ticket.SetMachineNumber( m_config.GetSlotNumber() );

	// Update ticket in the tickets object.
	if ( ticket.IsTicketIn() )
	{
		bool bUpdated = m_tickets->UpdateTicket( ticket );

		// TBD. Need to do more checking here.

		// Send the ticket response to the GCF if we were able to update this ticket.
		if ( bUpdated )
		{
			CTicket *newTicket = new CTicket(ticket);
			CInternalMessage *msg = new CInternalMessage( INT_MSG_TICKET_RESPONSE, newTicket );
			SendMessageToGCF( msg, QUEUE_PRIORITY_HIGH );
		}
	}
	else
	{
		m_tickets->FinalizeTicket( ticket );
	}

	// Tell the SCF that we received this ticket response so it can be removed from list.
	CInternalMessage *msg = new CInternalMessage( INT_MSG_TICKET_RESPONSE_RECEIVED, scfTicketID );
	msg->SetData(wIsTicketIn);
	SendMessageToSCF( msg, QUEUE_PRIORITY_HIGH );	
}

void CInternalsFramework::ProcessTicketStatus( CTicket *ticket, TicketInStatus status )
{
	lock_guard<recursive_mutex> lock(m_ticketsCriticalSection);

	// Internals knows the machine number, so set it in the ticket.
	ticket->SetMachineNumber( m_config.GetSlotNumber() );

	// The GCF does not have the sequence number at this point. So set it from our stored ticket.
	if ( m_tickets->DoesTicketInExist() )
	{
		ticket->SetSequenceNumber( m_tickets->GetTicketInSequenceNumber() );
	}

	LogString(TICKETING_LOG, "ProcessTicketStatus IsTicketIn=%u status=%u", ticket->IsTicketIn(), status);

	switch ( status )
	{
	case TICKET_IN_STATUS_STACKED:

		LogString(TICKETING_LOG, "ProcessTicketStatus TICKET_IN_STATUS_STACKED");

		m_tickets->SetFinalStatusReceived();

		// check ticket amount
		// Make sure ticket amount is less than max value
		if (ticket->IsTicketIn())
		{

			if (!ticket->IsAmountLessThanMax())
			{
				LogString(TICKETING_LOG, "ProcessTicketStatus sending tltVoucherOvrLmtAftr");

				TiltCode tilt = tltVoucherOvrLmtAftr;
				CDiagnostics::ReportPBTTicketDiagnosticToFile(CDirectoryManager::PBTTicketDiagnosticsFilePath(), CUtilities::LoadResourceString(IDS_TKT_REDEEM_AMOUNT_OVER_LIMIT_AFTER), ticket->ToStringForLog(), tilt);

				// log and send tilt
				GenerateTilt(tilt);
			}
			else if (!m_tickets->CompareTicketInAmount(ticket))
			{
				LogString(TICKETING_LOG, "ProcessTicketStatus sending tltVoucherMismatch");

				TiltCode tilt = tltVoucherMismatch;
				CDiagnostics::ReportPBTTicketDiagnosticToFile(CDirectoryManager::PBTTicketDiagnosticsFilePath(), CUtilities::LoadResourceString(IDS_TKT_REDEEM_AMOUNT_MISMATCH_AFTER), ticket->ToStringForLog(), tilt);

				// log and send tilt
				GenerateTilt(tilt);

			}
		}
		SendTicketInFinalToSCF(ticket);
		break;
	case TICKET_IN_STATUS_PENDING:
		LogString(TICKETING_LOG, "ProcessTicketStatus TICKET_IN_STATUS_PENDING");
		// This will timeout again.
		delete ticket;				/// The ticket needs to be consumed.
		break;
	case TICKET_IN_STATUS_ERROR:
		
		LogString(TICKETING_LOG, "ProcessTicketStatus TICKET_IN_STATUS_ERROR");

		m_tickets->SetFinalStatusReceived();

		// Check to see if new ticket has a validation number. If not, update with existing ticketIn.
		if (ticket->ValidationLength() != VALIDATION_NUMBER_LENGTH || ticket->Amount() != m_tickets->TicketInAmount())
		{
			LogString(TICKETING_LOG, "ProcessTicketStatus doesn't have a validation number, update with existing ticketIn");
			byte validationLength = 0;
			byte *validationNumber = m_tickets->TicketInValidationNumber(validationLength);
			ticket->SetValidationNumber(validationNumber, validationLength);

			__int64 amount = m_tickets->TicketInAmount();
			ticket->SetAmount(amount);
		}

		SendTicketInFinalToSCF( ticket );
		break;
	default:
		break;
	}
}

void CInternalsFramework::SetTicketOutPendingCount( WORD pendingCount )
{
	bool bCountSet = false;

	// If we get this message before the tickets object is instantiated, 
	//  wait for the tickets object to be instntiated.
	while ( !bCountSet )
	{
		unique_lock<recursive_mutex> cs(m_ticketsCriticalSection);

		if ( m_tickets != NULL )
		{
			bCountSet = true;

			m_tickets->SetNumberOfTicketsPending( pendingCount );

			// If we have not reached our maximum ticket pending, tell GCF that it is cool to print tickets.
			// Also don't turn on ticket printing if we are on-line (tracker 21622)
			if ( !m_tickets->HasMaxTicketOutPending() && m_EGMConfig.GetSentinelOnlineState() == sentinelOnline )
			{
				CInternalMessage *msg = new CInternalMessage( INT_MSG_ALLOW_TICKET_PRINTED );
				SendMessageToGCF( msg, QUEUE_PRIORITY_HIGH );	
			}
		}

		cs.unlock();

		if ( !bCountSet )
		{
			CUtilities::Sleep( 100 );
		}
	}
}

void CInternalsFramework::TicketTimedEvents( void )
{
	lock_guard<recursive_mutex> lock(m_ticketsCriticalSection);

	if ( m_tickets != NULL && m_tickets->HasTicketInExpired() )
	{
		LogString(TICKETING_LOG, "TicketTimedEvents-RequestTicketStatusFromGame");
		RequestTicketStatusFromGame();
	}
}

void CInternalsFramework::RequestTicketStatusFromGame( void )
{
	LogString(TICKETING_LOG, "RequestTicketStatusFromGame");
	m_tickets->ResetTicketInExpire();

	CInternalMessage *msg = new CInternalMessage( INT_MSG_GET_TICKET_STATUS );
	SendMessageToGCF( msg, QUEUE_PRIORITY_HIGH );	
}

void CInternalsFramework::SendTicketInFinalToSCF( CTicket *ticket )
{
	CInternalMessage *transactionMsg = PrepareTransactionMessage( INT_MSG_TRANSACTION_TICKET, IsPlayerVirtual() );
	transactionMsg->SetData( ticket );
	
	//Add play session id
	DWORD psid = m_playerSession.GetSessionId();
	LogString(POLLER_LOG, "%s: PollerProtocol: Play Session ID Sent from internal: %u", __FUNCTION__, psid);
	transactionMsg->SetData(psid);

	SendTransactionMessageToSCF( transactionMsg, QUEUE_PRIORITY_HIGH );
}

void CInternalsFramework::SendTicketStoredToGCF( CTicket &ticket )
{
	CTicket *newTicket = new CTicket(ticket);
	CInternalMessage *storedMsg = new CInternalMessage( INT_MSG_TICKET_STORED, newTicket );
	SendMessageToGCF( storedMsg, QUEUE_PRIORITY_HIGH );
}
