#include "stdafx.h"

#include "Sentinel.h"
#include "Tickets.h"
#include "UI/TicketLog.h"

using namespace std;

const int TICKET_IN_EXPIRE_SECONDS = 30;

CTickets::CTickets( const CConfig &config ) : 
CDataSubscriptionProvider(TICKET_LOGS),
m_ticketLogs(MEMORY_NVRAM, LOG_TICKET, TICKET_LOGS)
{
	m_type = MEMORY_NVRAM;
	m_dwAllowedSize = NVRAM_TICKETS_SIZE;
	m_dwOffset = NVRAM_TICKETS_OFFSET;
	m_bOffsetIsSet = true;

	m_ticketingInfo = new CTicketingInfo( config );

	ConfigSettings( config );

	m_ticketIn = NULL;
	m_ticketOut = NULL;

	m_finalStatusReceived = false;
	m_voucherBufferingEnabled = false;
	m_currentTicketsOutPending = 0;
	ResetTicketInExpire();

	BuildYourself();
}

CTickets::~CTickets(void)
{
	RemoveTicketIn();
	RemoveTicketOut();

	if ( m_ticketingInfo != NULL )
	{
		delete m_ticketingInfo;
	}
}

string CTickets::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId)
{
    UNREFERENCED_PARAMETER(&topic); 
    UNREFERENCED_PARAMETER(&whereId); 

	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key("VoucherBufferingEnabled");	writer.Bool(m_voucherBufferingEnabled);
	writer.Key("TotalVouchers");			writer.Int(m_currentTicketsOutPending);
	writer.Key(TICKET_LOGS.c_str());
	{
		writer.StartArray();
		{
			int numLogs(m_ticketLogs.GetNumItems());
			string timeStamp, amount, status, validationNumber, sequenceNumber;
			CTicketLog* log((CTicketLog *)m_ticketLogs.GetFirst());
			for (int index = 0; index < numLogs; index++)
			{
				if (log != nullptr)
				{
					writer.StartObject();
					{
						timeStamp = FormatString("(%d) %s", index + 1, log->GetDateTimeAsString().c_str());
						amount = FormatString("$%s.%02d", CUtilities::GetFormattedNumberString((DWORD)(log->GetAmount() / 
							(__int64)ONE_DOLLAR_IN_CENTS)).c_str(), (int)(log->GetAmount() % (__int64)ONE_DOLLAR_IN_CENTS));
						status = log->GetTicketIn() == true ? log->GetTicketInActionText() : "Printed";
						validationNumber = log->GetMaskedValidationNum();
						sequenceNumber = FormatString("%d", log->GetSequenceNum());

						writer.Key("TimeStamp");		writer.String(timeStamp);
						writer.Key("Amount");			writer.String(amount);
						writer.Key("Status");			writer.String(status);
						writer.Key("ValidationNumber");	writer.String(validationNumber);
						writer.Key("SequenceNumber");	writer.String(sequenceNumber);
					}
					writer.EndObject();
					log = (CTicketLog *)m_ticketLogs.GetNext();
				}
				else
				{
					break;
				}
			}
		}
		writer.EndArray();
	}
	writer.EndObject();

	return writeStream.m_str;
}

void CTickets::BuildYourself( void )
{
	bool bTicketsExists = CMemoryStorage::IntegrityCheck() && IsActiveLocation( NVRAM_TICKETS_CHECKBYTE_OFFSET, NVRAM_TICKETS_CHECKBYTE_VALUE );

	if ( bTicketsExists )
	{
		byte nvramBuffer[NVRAM_MAX_READ];

		if ( !Read( NVRAM_TICKETS_DATA_OFFSET, nvramBuffer, NVRAM_TICKETS_DATA_SIZE ) )
			LogString(ERROR_LOG, "Could not read tickets data from NVRAM" );

		byte *bufferPos = nvramBuffer;
		memcpy( &m_currentTicketsOutPending, bufferPos, sizeof( m_currentTicketsOutPending ) );
		bufferPos += sizeof( m_currentTicketsOutPending );
		memcpy( &m_finalStatusReceived, bufferPos, sizeof( m_finalStatusReceived ) );
		bufferPos += sizeof( m_finalStatusReceived );

		if ( IsActiveLocation( NVRAM_TICKET_IN_OFFSET, NVRAM_TICKETS_CHECKBYTE_VALUE ) )
		{
			if ( !Read( NVRAM_TICKET_IN_OFFSET + NVRAM_TICKETS_CHECKBYTE_SIZE, nvramBuffer, NVRAM_TICKET_SIZE ) )
				LogString(ERROR_LOG, "Could not read ticket-in data from NVRAM" );
			m_ticketIn = new CTicket(nvramBuffer);
		}
		else
		{
			m_ticketIn = NULL;
		}

		if ( IsActiveLocation( NVRAM_TICKET_OUT_OFFSET, NVRAM_TICKETS_CHECKBYTE_VALUE ) )
		{
			if ( !Read( NVRAM_TICKET_OUT_OFFSET + NVRAM_TICKETS_CHECKBYTE_SIZE, nvramBuffer, NVRAM_TICKET_SIZE ) )
				LogString(ERROR_LOG, "Could not read ticket-out data from NVRAM" );
			m_ticketOut = new CTicket(nvramBuffer);
		}
		else
		{
			m_ticketOut = NULL;
		}
	}
	else
	{
		SetActiveLocation( NVRAM_TICKETS_CHECKBYTE_OFFSET, NVRAM_TICKETS_CHECKBYTE_VALUE );
		StoreTicketsDataToNVRAM();
		StoreTicketInToNVRAM();
		StoreTicketOutToNVRAM();
	}
}

void CTickets::ConfigSettings( const CConfig &config )
{
	m_ticketingInfo->ConfigSettings( config );

	if ( m_ticketingInfo->IsTicketingOutEnabled() )
	{
		if ( m_ticketingInfo->IsTicketBufferingAllowed() )
		{
			m_maxTicketsOutPending = MAX_BUFFERED_TICKETS;
		}
		else
		{
			m_maxTicketsOutPending = 1;
		}
	} 
	else
	{
		m_maxTicketsOutPending = 0;
	}

	StoreTicketsDataToNVRAM();
}

CTicketingInfo * CTickets::GetTicketingInfo( void )
{
	return m_ticketingInfo;
}

// Return true if ticket was a duplicate
bool CTickets::StoreTicket( const CTicket &ticket )
{
	bool bTicketIsDuplicate = IsDuplicate( ticket );

	if (!bTicketIsDuplicate)
	{
		// Store the ticket in
		if ( ticket.IsTicketIn() )
		{
			// If the ticket-in is already there see if it is a duplicate.
			// If not delete the existing one
			if ( m_ticketIn != NULL )
			{
				// We should not be here.
				RemoveTicketIn();
			}
			
			// Store this ticket if we dont have a duplicate
			if ( m_ticketIn == NULL )
			{
				m_ticketIn = new CTicket(ticket);
				m_finalStatusReceived = false;
			}

			StoreTicketInToNVRAM();
		}

		// Store the ticket out
		else
		{
			// If the ticket-out is already there see if it is a duplicate.
			// If not delete the existing one
			if ( m_ticketOut != NULL )
			{
				delete m_ticketOut;
				m_ticketOut = NULL;
			}

			// Store this ticket if we dont have a duplicate
			if ( m_ticketOut == NULL )
			{
				m_ticketOut = new CTicket(ticket);
				m_finalStatusReceived = false;
			}

			StoreTicketOutToNVRAM();
		}
	}

	return bTicketIsDuplicate;
}

bool CTickets::IsDuplicate( const CTicket &ticket )
{
	bool bTicketIsDuplicate = false;

	if ( ticket.IsTicketIn() )
	{
		// If the ticket-in is already there see if it is a duplicate.
		if ( m_ticketIn != NULL )
		{
			if ( ticket == *m_ticketIn )
			{
				bTicketIsDuplicate = true;
			}
		}
			
	}
	else
	{
		// If the ticket-out is already there see if it is a duplicate.
		if ( m_ticketOut != NULL )
		{
			if ( ticket == *m_ticketOut )
			{
				bTicketIsDuplicate = true;
			}
		}
	}

	return bTicketIsDuplicate;
}

bool CTickets::UpdateTicket( const CTicket &ticket )
{
	bool bTicketUpdated = false;

	if ( ticket.IsTicketIn() && m_ticketIn != NULL && *m_ticketIn == ticket )
	{
		// A timing issue sometimes results in a second ticket-in response coming from poller.  This unwanted ticket has resultcode = 0 and ticket id = 0
		// ignore this ticket.
		// Tracker #022930 
		if (ticket.ResultCode() != Acknowledged || ticket.ID() != 0)
		{
			int previousTicketNumber = m_ticketIn->Number();
			delete m_ticketIn;
			m_ticketIn = new CTicket( ticket );
			bTicketUpdated = true;
	
			// Don't write over a good ticket number.
			if ( m_ticketIn->Number() <= 0 )
			{
				m_ticketIn->SetTicketNumber( previousTicketNumber );
			}
	
			StoreTicketInToNVRAM();
		}
	}

	else if ( !ticket.IsTicketIn() && m_ticketOut != NULL && *m_ticketOut == ticket )
	{
		delete m_ticketOut;
		m_ticketOut = new CTicket( ticket );
		bTicketUpdated = true;

		StoreTicketOutToNVRAM();
	}

	return bTicketUpdated;
}

void CTickets::FinalizeTicket( const CTicket &ticket )
{
	if ( ticket.IsTicketIn() )
	{
		//this is done because the ticket returned from the game does not have the correct sequence number but does have the 
		//correct final action code
		if (m_ticketIn)
		{
			m_ticketIn->SetActionCode( ticket.ActionCode() );
			m_ticketLogs.Add(new CTicketLog(*m_ticketIn));
			RemoveTicketIn();
		}
	}
	else
	{
		m_ticketLogs.Add(new CTicketLog(ticket));
	}

	m_finalStatusReceived = false;
}

void CTickets::ResetTicketInExpire( void )
{
	m_ticketInExpireTime = CUtilities::GetCurrentTime() + TICKET_IN_EXPIRE_SECONDS;
}

bool CTickets::HasTicketInExpired( void ) const
{
	bool bExpired = false;

	if ( m_ticketIn != NULL && CUtilities::GetCurrentTime() >= m_ticketInExpireTime )
	{
		bExpired = true;
	}

	return bExpired;
}

void CTickets::RemoveTicketIn( void )
{
	if ( m_ticketIn != NULL )
	{
		delete m_ticketIn;
		m_ticketIn = NULL;
		StoreTicketInToNVRAM();
	}
}

void CTickets::RemoveTicketOut( void )
{
	if ( m_ticketOut != NULL )
	{
		delete m_ticketOut;
		m_ticketOut = NULL;
		StoreTicketOutToNVRAM();
	}
}

void CTickets::SetNumberOfTicketsPending( int pendingCount )
{
	m_currentTicketsOutPending = pendingCount;
}

int CTickets::GetNumberOfTicketsPending( void )
{
	return m_currentTicketsOutPending;
}

bool CTickets::HasMaxTicketOutPending( void ) const
{
	return m_currentTicketsOutPending >= m_maxTicketsOutPending;  
}

/// <summary>
/// Gets the maximum ticket pending count.
/// </summary>
/// <returns>Returns maximum ticket out pending allowed.</returns>
int CTickets::GetMaxTicketOutPending( void ) const
{
	return m_maxTicketsOutPending;
}

CLogs *CTickets::GetTicketLogs( void )
{
	return &m_ticketLogs;
}

bool CTickets::DoesTicketInExist( void ) const
{
	return m_ticketIn != NULL;
}

WORD CTickets::GetTicketInSequenceNumber( void ) const
{
	WORD seqNum = 0;
	if ( m_ticketIn != NULL )
	{
		seqNum = m_ticketIn->SequenceNumber();
	}
	return seqNum;
}

void CTickets::SetFinalStatusReceived( void )
{
	m_finalStatusReceived = true;
}

bool CTickets::WasFinalStatusReceived( void ) const
{
	return m_finalStatusReceived;
}

void CTickets::StoreTicketsDataToNVRAM( void )
{
	byte nvramBuffer[NVRAM_TICKETS_DATA_SIZE];

	byte *bufferPos = nvramBuffer;

	memcpy( bufferPos, &m_currentTicketsOutPending, sizeof( m_currentTicketsOutPending ) );
	bufferPos += sizeof( m_currentTicketsOutPending );

	memcpy( bufferPos, &m_finalStatusReceived, sizeof( m_finalStatusReceived ) );
	bufferPos += sizeof( m_finalStatusReceived );

	int bytesInBuffer = bufferPos - nvramBuffer;
	if ( bytesInBuffer > 0 )
	{
		if ( !Write( NVRAM_TICKETS_DATA_OFFSET, nvramBuffer, bytesInBuffer ) )
			LogString(ERROR_LOG, "Could not write tickets data to NVRAM" );
	}
	else
	{
		LogString(ERROR_LOG, "Could not get tickets data to write to NVRAM" );
	}
}

void CTickets::StoreTicketInToNVRAM( void )
{
	byte ticketCheckByteValue = m_ticketIn != NULL ? NVRAM_TICKETS_CHECKBYTE_VALUE : ~NVRAM_TICKETS_CHECKBYTE_VALUE;

	if ( !Write( NVRAM_TICKET_IN_OFFSET, &ticketCheckByteValue, sizeof (ticketCheckByteValue) ) )
		LogString(ERROR_LOG, "Could not write ticket check byte to NVRAM" );

	if ( m_ticketIn != NULL )
	{
		byte nvramBuffer[NVRAM_TICKET_SIZE];
		int bytesInBuffer = m_ticketIn->GetNvramBuffer( nvramBuffer, NVRAM_TICKET_SIZE );

		if ( bytesInBuffer > 0 )
		{
			if ( !Write( NVRAM_TICKET_IN_OFFSET + NVRAM_TICKETS_CHECKBYTE_SIZE, nvramBuffer, bytesInBuffer ) )
				LogString(ERROR_LOG, "Could not write ticket-in data to NVRAM" );
		}
		else
		{
			LogString(ERROR_LOG, "Could not get player card data to write to NVRAM" );
		}
	}
}

void CTickets::StoreTicketOutToNVRAM( void )
{
	byte ticketCheckByteValue = m_ticketOut != NULL ? NVRAM_TICKETS_CHECKBYTE_VALUE : ~NVRAM_TICKETS_CHECKBYTE_VALUE;

	if ( !Write( NVRAM_TICKET_OUT_OFFSET, &ticketCheckByteValue, sizeof (ticketCheckByteValue) ) )
		LogString(ERROR_LOG, "Could not write ticket check byte to NVRAM" );

	if ( m_ticketOut != NULL )
	{
		byte nvramBuffer[NVRAM_TICKET_SIZE];
		int bytesInBuffer = m_ticketOut->GetNvramBuffer( nvramBuffer, NVRAM_TICKET_SIZE );

		if ( bytesInBuffer > 0 )
		{
			if ( !Write( NVRAM_TICKET_OUT_OFFSET + NVRAM_TICKETS_CHECKBYTE_SIZE, nvramBuffer, bytesInBuffer ) )
				LogString(ERROR_LOG, "Could not write ticket-out data to NVRAM" );
		}
		else
		{
			LogString(ERROR_LOG, "Could not get player card data to write to NVRAM" );
		}
	}
}

bool CTickets::CompareTicketInAmount(CTicket *ticket)
{
	bool retVal = true;

	if (m_ticketIn)
	{
		if (m_ticketIn->Amount() != ticket->Amount())
		{
			retVal = false;
		}
	}

	return retVal;
}


byte *CTickets::TicketInValidationNumber(byte &validationLength) const
{
	byte *result = nullptr;

	if (m_ticketIn != nullptr)
	{
		result = const_cast<byte *>(m_ticketIn->ValidationNumber(validationLength));
	}
	return result;
}

__int64 CTickets::TicketInAmount() const
{
	__int64 amount = 0;
	if (m_ticketIn != nullptr)
	{
		amount = m_ticketIn->Amount();
	}

	return amount;
}

void CTickets::SetVoucherBufferingEnabled(bool value)
{
	m_voucherBufferingEnabled = value;
}