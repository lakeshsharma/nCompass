#include "stdafx.h"
#include "SASProtocol.h"

// Sentinel includes.
#include "../InternalMessage.h"
#include "resource.h"
#include "SASPollMessage.h"
#include "SASSerialWrapper.h"
#include "../Ticket.h"
#include "LongPoll4DMessage.h"
#include "LongPoll7BMessage.h"
#include "LongPoll7DMessage.h"
#include "LongPoll70Message.h"
#include "LongPoll71Message.h"
#include "Diagnostics.h"
#include "DirectoryManager.h"

void CSASProtocol::ProcessSetTicketingInfo(CInternalMessage *msg)
{
	CTicketingInfo *ticketInfo = msg->GetTicketingInfoData();
	m_sasState.SetCasinoNumber(ticketInfo->GetCasinoNumber());
	m_address1 = ticketInfo->GetCasinoAddress1();
	m_address2 = ticketInfo->GetCasinoAddress2();
	m_casinoName = ticketInfo->GetCasinoName();
	m_couponPromoEnabled = ticketInfo->IsCouponPromoEnabled();
	m_bVoucherPromoInEnabled = ticketInfo->IsVoucherPromoInEnabled();
	m_sasState.SetTicketingEnabled(ticketInfo->IsTicketingEnabled());
	m_sas->SetTicketExpirationDuration(ticketInfo->GetExpirationInDays());
	m_slotNumber = (WORD)ticketInfo->GetSlotNumber();
	m_sasState.SetTicketingOutEnabled(ticketInfo->IsTicketingOutEnabled());
	m_sasState.SetTicketingInEnabled(ticketInfo->IsTicketingInEnabled());

	// initialize ticketing
	SetTicketValidationParms();

	GetTicketValidationData( false );
}

void CSASProtocol::ProcessTicketStored()
{
	//m_ignoreTicketPrinted = false;
	// determine what the ticket is
	// Tracker #19946 -- resolution is to clear ticket after poller processing, not here.
	//CTicket *ticket = msg->GetTicketData();
	//if (ticket->IsTicketIn())
	//{
	//	//switch(ticket->ActionCode())
	//	//{
	//	//	// case :
	//	//}

	//}
	//else
	//{
	//	// printed ticket is stored in nvram
	//	ClearTicketOut(ticket);
	//}
}

void CSASProtocol::ClearTicketOut()
{
	// Tracker #19946
	if (m_sasState.GetLastPrintedTicket())
	{
		CLongPoll4DMessage *clearTicket = new CLongPoll4DMessage(m_gameAddress);
		clearTicket->SetPeekOption(false);
		clearTicket->Fill();
		m_sasPollQueue->Add(clearTicket,QUEUE_PRIORITY_HIGHEST);
		clearTicket->WaitForResponse();
		if (clearTicket->GetReturnValue() == SASPOLL_SUCCESS)
		{
			CTicket *newTicket = clearTicket->GetResponseTicket();
			CTicket *lastPrintedTicket = m_sasState.GetLastPrintedTicket();

			// Make sure a new ticket didn't sneak in...(don't know how that would happen)
			if (lastPrintedTicket != nullptr && *newTicket != *lastPrintedTicket)
			{
				// New ticket, send INT_MSG_ADD_TICKET, how did it happen?
				newTicket->SetSequenceNumber(m_ticketSequenceNumber++);
				if (m_ticketSequenceNumber == 0)
					m_ticketSequenceNumber = 1;

				CInternalMessage *msg = new CInternalMessage(INT_MSG_SEND_TICKET,newTicket);
				SendMessageToInternals( msg, QUEUE_PRIORITY_HIGHEST ); 
			
				// cause reset of validation sequence number
				GetTicketValidationData( true );
			}
			else
				delete newTicket;

			if(lastPrintedTicket != nullptr)
			{
				m_sasState.SetLastPrintedTicket(nullptr);
			}

		}
		// delete clearTicketPoll
		delete clearTicket;
	}
}

void CSASProtocol::SetTicketValidationParms( void )
{

	if (m_sasState.GetTicketingOutEnabled())
	{
		// Set some ticket config stuff with long poll 7b
		// Tracker #21546 -- Modifying ticketing settings on EGM.
		CLongPoll7BMessage lp7b(m_gameAddress);
		lp7b.SetPrinterCashout(m_sasState.GetTicketingOutEnabled());
		lp7b.SetPrintRestrictedTickets(false);
		lp7b.SetCashableExpiration(m_sas->GetTicketExpirationDuration());
		// Tracker #22671 -- Set ticket redemption on egm
		lp7b.SetTicketRedemption(m_sasState.GetTicketingInEnabled());
		lp7b.Fill();
		m_sasPollQueue->Add(&lp7b,QUEUE_PRIORITY_HIGHEST);
		lp7b.WaitForResponse();
		bool egmSupportsLP7B(lp7b.GetReturnValue() == SASPOLL_SUCCESS);
		SendMessageToInternals(m_sas->SetEgmSupportsLP7B(egmSupportsLP7B), QUEUE_PRIORITY_NORMAL);

		// Set ticket fields with long poll 7D
		CLongPoll7DMessage lp7d(m_gameAddress);
		if (m_sas->GetTicketExpirationDuration() == NO_EXPIRATION_7D)
			lp7d.SetExpiration(NO_EXPIRATION_7B);
		else if (m_sas->GetTicketExpirationDuration() > MAX_EXPIRATION_7B)
			lp7d.SetExpiration(MAX_EXPIRATION_7B);
		else
			lp7d.SetExpiration((byte)m_sas->GetTicketExpirationDuration());
		lp7d.SetHostID(m_slotNumber);
		
		const int ADDRESS_LENGTH = 41;
		byte location[ADDRESS_LENGTH], address1[ADDRESS_LENGTH], address2[ADDRESS_LENGTH];
		CUtilities::ConvertCharsToChars((LPCTSTR)m_casinoName.c_str(), (char *)location, ADDRESS_LENGTH);
		CUtilities::ConvertCharsToChars((LPCTSTR)m_address1.c_str(),  (char *)address1, ADDRESS_LENGTH);
		CUtilities::ConvertCharsToChars((LPCTSTR)m_address2.c_str(),  (char *)address2, ADDRESS_LENGTH);

		lp7d.SetAddressData(location,m_casinoName.length(),address1, m_address1.length(), address2, m_address2.length());
		lp7d.Fill();
		m_sasPollQueue->Add(&lp7d,QUEUE_PRIORITY_HIGHEST);
		lp7d.WaitForResponse();
	}
}

void CSASProtocol::ProcessTicketInsertedGP()
{
	LogString(TICKETING_LOG, "ProcessTicketInsertedGP sending INT_MSG_TICKET_INSERTED");
	// send ticket_inserted message to internals
	CInternalMessage *msg = new CInternalMessage(INT_MSG_TICKET_INSERTED);
	SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);

	// temporary for debugging
	//ProcessTicketIn();
}

void CSASProtocol::ProcessTicketIn()
{
	// Get ticket validation information using long poll 0x70
	CLongPoll70Message lp70(m_gameAddress);
	lp70.Fill();
	m_sasPollQueue->Add(&lp70, QUEUE_PRIORITY_HIGHEST);
	lp70.WaitForResponse();

	if (lp70.GetReturnValue() == SASPOLL_SUCCESS)
	{
		LogString(TICKETING_LOG, "ProcessTicketIn-INT_MSG_SEND_TICKET");
		// send created ticket to internals
		CTicket *ticket = lp70.GetResponseTicket();
		ticket->SetSequenceNumber(m_ticketSequenceNumber++);
		if (m_ticketSequenceNumber == 0)
			m_ticketSequenceNumber = 1;
		CInternalMessage *msg = new CInternalMessage(INT_MSG_SEND_TICKET, ticket);
		SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);
	}
	else
	{
		LogString(TICKETING_LOG, "ProcessTicketIn LP70 failed.");
	}
}

void CSASProtocol::ProcessTicketResponse(CInternalMessage *msg)
{
	// Send accept/reject to EGM via long poll 71
	CTicket *ticket = msg->GetTicketData();
	msg->SetData((CTicket *)NULL);

	LogString(TICKETING_LOG, "ProcessTicketResponse type=%u m_couponPromoEnabled=%u m_bVoucherPromoInEnabled=%u", ticket->Type(), m_couponPromoEnabled, m_bVoucherPromoInEnabled);

	switch(ticket->Type())
	{
	case VoucherTicket:
		break;
	case CouponPromoGameGenerated:
	case CouponPromo:
		if (!m_couponPromoEnabled)
			ticket->SetResultCode(RejectVoucher);
		break;
	case VoucherPromo:
		if (!m_bVoucherPromoInEnabled)
			ticket->SetResultCode(RejectVoucher);
		break;
	default:
		break;
	}

	// If authorized to vend, ensure amount is not over maximum allowed
	if (ticket->ResultCode() == AuthorizedToVendVoucher && !ticket->IsAmountLessThanMax())
	{
		LogString(TICKETING_LOG, "ProcessTicketResponse sending tltVoucherOvrLmtBef to internals. Change to RejectVoucher");

		// log and send tilt
		SendTiltToInternals(tltVoucherOvrLmtBef);

		CDiagnostics::ReportPBTTicketDiagnosticToFile(CDirectoryManager::PBTTicketDiagnosticsFilePath(), CUtilities::LoadResourceString(IDS_TKT_REDEEM_AMOUNT_OVER_LIMIT_BEFORE), ticket->ToStringForLog(), tltVoucherOvrLmtBef);

		// Change to reject
		ticket->SetResultCode(RejectVoucher);
	}
	CLongPoll71Message lp71(m_gameAddress);
	lp71.SetSendTicket(ticket);
	lp71.SetTicketExpirationDuration(m_sas->GetTicketExpirationDuration());
	lp71.ClearStatusQuery();
	lp71.Fill();
	m_sasPollQueue->Add(&lp71,QUEUE_PRIORITY_HIGHEST);
	lp71.WaitForResponse();
	
	CTicket *responseTicket = lp71.GetResponseTicket();

	// Keep ticketType
	TicketType ticketType = ticket->Type();

	delete ticket;

	// Send status to internals, if it isn't null
	// if null, don't send anything, let internals expire ticket and ask for status
	if (responseTicket)
	{
		LogString(TICKETING_LOG, "ProcessTicketResponse seding INT_MSG_TICKET_STATUS to internals ticketType=%u", ticketType);
		responseTicket->SetType(ticketType, true);
		CInternalMessage *resMsg = new CInternalMessage(INT_MSG_TICKET_STATUS, responseTicket);
		resMsg->SetData((WORD)lp71.m_ticketStatus);
		SendMessageToInternals(resMsg, QUEUE_PRIORITY_HIGHEST);
		// Tracker #20938 - get ticket meters here
		GetTicketMeters(false);
	}
	else
	{
		LogString(TICKETING_LOG, "ProcessTicketResponse responseTicket is null.");
	}
}

void CSASProtocol::ProcessFinalTicketStatus()
{
	CLongPoll71Message lp71(m_gameAddress);
	lp71.SetStatusQuery();
	lp71.SetTicketExpirationDuration(m_sas->GetTicketExpirationDuration());
	lp71.Fill();
	m_sasPollQueue->Add(&lp71,QUEUE_PRIORITY_HIGHEST);
	lp71.WaitForResponse();

	CTicket *ticket = lp71.GetResponseTicket();
	if (ticket)
	{
		// i guess if we get ACK for TX107(2), we will get ProcessTicketResponse() called which
		// will set response (reject/accept) in LP71 and send to SAS. 
		// maybe >> if we dont get ACK from Poller, we will get GPE_TICKET_XFER_COMPLETE (expire) and here
		// we will query ticket status from LP71 and send that to PC. 
		// I guess if we get VoucherRejectedByGame here, then that may be our case. But may not be sometimes.
		// we instead want to send a new tilt here...

		// either we can have logic here, add in internals, or decide whether to send these TXes or convert to tilts
		// at poller resend layer. if we get ack of tx107(2) and have tx107(53) also in resend list,
		// we can unblock sending tx107
		LogString(TICKETING_LOG, "ProcessFinalTicketStatus sending INT_MSG_TICKET_STATUS status=%u", lp71.m_ticketStatus);
		GetTicketMeters(false);
		CInternalMessage *msg = new CInternalMessage(INT_MSG_TICKET_STATUS, ticket);
		msg->SetData((WORD)lp71.m_ticketStatus);
		SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);
	}
	else
	{
		LogString(TICKETING_LOG, "ProcessFinalTicketStatus ticket is null.");
	}
}
