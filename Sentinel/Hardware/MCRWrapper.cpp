#include "stdafx.h"
#include "MCRWrapper.h"

// Sentinel includes.
#include "Config/Config.h"
#include "Hardware/WatchDog.h"
#include "IOCTLOffsets.h"
#include "Logging/LogString.h"

using namespace std;

const int CARD_BUFFER_LEN = 256;
const DWORD CARD_READ_TIMEOUT = 1000; // For both data and idle timeouts.
const DWORD CARD_WRITE_TIMEOUT = 500;
const DWORD BEZEL_UPDATE_TIMEOUT = 500;
const DWORD GPIO_BASE_ADDRESS = 0x40e00000;

const int MCR_RECONNECT_ON_ERROR_COUNT = 1000;
const int TRACK2 = 2;

const int DATA_REPORT_EVENT = HID_CARDRDR_DATA_REPORT_OFFSET_EVENT;
const int DATA_REPORT_VALUE = HID_CARDRDR_DATA_REPORT_OFFSET_VALUE;
const int DATA_REPORT_DATA_TYPE_OFFSET = HID_CARDRDR_DATA_REPORT_OFFSET_VALUE + 1;
const int DATA_REPORT_INSERTED = HID_CARDRDR_DATA_REPORT_VALUE_INSERTED;
const int DATA_REPORT_REMOVED = HID_CARDRDR_DATA_REPORT_VALUE_REMOVED;
const int DATA_REPORT_CARD_PRESENT = HID_CARDRDR_DATA_REPORT_VALUE_PARTIALlY_INSERTED;
const int CARD_DATA_BUFFER_SIZE = HID_CARDRDR_DATA_REPORT_SIZE; // MUST be a least m_cardReaderLayer.m_context.GetFeatureReportByteLength() bytes.
const int CARD_DATA_INDEX_LOC = 1;
const int CARD_DATA_SIZE_LOC = 3;
const int CARD_DATA_TYPE_LOC = 4;
const int CARD_DATA_LOC = 5;
const int CARD_DATA_FULL_LENGTH = 59;
const int RFIDDATA = 5;

#define MCR_IOCTL_READ_TRACK_1 CTL_CODE(0x8001, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define MCR_IOCTL_READ_TRACK_2 CTL_CODE(0x8001, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define MCR_IOCTL_GET_CARD_STATUS CTL_CODE(0x8001, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MCR_ISINSERTED CTL_CODE(0x8001, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MCR_INSERT CTL_CODE(0x8001, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MCR_REMOVE CTL_CODE(0x8001, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MCR_GET_OCR CTL_CODE(0x8001, 0x806, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MCR_SET_OCR CTL_CODE(0x8001, 0x807, METHOD_BUFFERED, FILE_ANY_ACCESS)

bool CMCRWrapper::m_bInserted = false;

CMCRWrapper::CMCRWrapper(CConfig config) 
	: m_insertRemoveErrorReported(false),
	m_findCardReaderFailedReported(false),
	m_errorOpeningMCRDriverReported(false),
	m_couldNotSetBezelColorReported(false)
{
	try
	{
		m_bPendingInsRemRequest = false;
		m_bInserted = false;
		m_track1DataIsAvailable = false;
		m_track2DataIsAvailable = false;
		m_rfidDataIsAvailable = false;
		m_cardDataReportBytes = 0;
		m_nextBezelStyle = CARD_REMOVED,
		m_currentBezelStyle = CARD_STYLE_INVALID; // Ensure that first CheckBezelStyle will update.
		m_nextRankId = CPlayerInfo::DEFAULT_RANK_ID;
		m_currentRankId = CPlayerInfo::DEFAULT_RANK_ID;
		m_lastBezelUpdateTicks = 0;
		m_lookForUSBTicks = CUtilities::GetTickCount();
		m_bErrorOpening = false;
		m_mcrIsCardInserted = false;
		m_card = NULL;
		m_mcrInserted = false;
		m_mcrRemoved = false;
		m_mcrTimeout = true;
		m_reconnectErrorCount = 0;
		m_waitingDataRead = false;
		m_waitingRemove = false;

		// Setup m_cardConfig;
		m_cardConfig.Setup(config);

		m_cardReaderLayer.LoadConfig();

		m_shutdown = false;
	}
	catch(...)
	{
		LogString(ERROR_LOG, "Exception catch during variable init in %s", static_cast<LPCTSTR>(__TFUNCTION__));
	}

	try
	{
		Open();
	}
	catch(...)
	{
		LogString(ERROR_LOG, "Exception catch during Open() in %s", static_cast<LPCTSTR>(__TFUNCTION__));
	}
}

CMCRWrapper::~CMCRWrapper(void)
{
	Close();
}

void CMCRWrapper::ShutdownCardReader()
{
	m_shutdown = true;
}

void CMCRWrapper::CardReaderProcessingLoop(const CConfig *config)
{
   bool isCardReaderCool = IsCardReaderReady();

	if ( !isCardReaderCool )
	{
		LogString(ERROR_LOG, "Could not Initialize card reader" );
	}
	  //	
	// Loop until shutdown
	while ( !m_shutdown )
	{
		switch (m_eventWaitType)
		{
		case CARD_WAIT_EVENT :
			ProcessWaitForEvent(config);
			CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
			CWatchDog::Instance().Checkin();
			break;
		default :
			CheckBezelStyle(); // Update bezel even if "ReadCardThread/InternalsFramework" thread sleeps.
			CUtilities::Sleep(CARD_READER_LOOP_SLEEP_PERIOD);
			CWatchDog::Instance().Checkin();
			break;
		}
	}
}

void CMCRWrapper::ProcessWaitForEvent( const CConfig *config )
{

	bool bTimeout = true;
	bool bInserted = false;
	bool bRemoved = false;
	bool track1 = config->CardReaderTrackNumber() == CARD_READER_TRACK_1;
	CCard *card = WaitForCardEvent( bInserted, bRemoved, bTimeout, track1 );

	{
		lock_guard<recursive_mutex> lock(m_csInternalsFramework);
		if (!bTimeout)
		{
			m_card = card;
			switch (m_eventWaitType)
			{
				case CARD_WAIT_EVENT:
					m_eventWaitType = CARD_WAIT_EVENT_DONE;
					m_mcrIsCardInserted = IsCardInserted();
					m_mcrInserted = bInserted;
					m_mcrRemoved = bRemoved;
					break;
				default:
					break;
			}
		}
	
		m_mcrTimeout = bTimeout;
	
	}
}

/// <summary>
/// Set event type for cardreader to wait on
/// </summary>
/// <param name="eventType">Event type to wait for (ie CARD_WAIT_INSERT)</param>
void CMCRWrapper::SetCardEventWaitType( CardEventType eventType)
{
	m_eventWaitType = eventType;
}
bool CMCRWrapper::IsOpenErrorAndCardInserted()
{
	return IsOpenError() && IsCardInserted();
}

CCard * CMCRWrapper::McrWaitForCardEvent( bool &bInserted, bool &bRemoved, bool &bTimeout, bool &bIsCardInserted )
{
	CCard *card = NULL;
	
	bTimeout = true;

	lock_guard<recursive_mutex> lock(m_csInternalsFramework);
	try
	{
		if (m_eventWaitType == CARD_WAIT_EVENT_DONE)
		{
			m_eventWaitType = CARD_WAIT_NONE;
			bInserted = m_mcrInserted;
			bRemoved = m_mcrRemoved;
			bIsCardInserted = 	m_mcrIsCardInserted;
			bTimeout = false;
			if (m_mcrInserted)
			{
				card = m_card;
			}
		}
		else
		{
			m_eventWaitType = CARD_WAIT_EVENT;
		}
	}
	catch(...)
	{
	}

	return card;
}

void CMCRWrapper::Open( void )
{
	const int OPENSTATUSRETRIES = 3;
	const int DELAYAFTERCONFIG = 3000;

	// Do necessary cleanup before (re)initializing card reader
	Close();
	
	bool deviceFound = false;

	if (!m_cardReaderLayer.IsReady())
	{
		if (m_cardReaderLayer.FindCardReader())
		{
			// Reset the error reporting on insert/Remove Error.
			m_insertRemoveErrorReported = false;
			m_findCardReaderFailedReported = false;
			m_errorOpeningMCRDriverReported = false;
			m_couldNotSetBezelColorReported = false;
			m_bPendingInsRemRequest = false;
			deviceFound = true;

			// Don't use the device found unless our buffers are big enough.
			// Assert that CARD_DATA_BUFFER_SIZE >= m_cardReaderLayer.m_context.GetFeatureReportByteLength().
			// Assert that sizeof(m_cardDataReport) >= m_cardReaderLayer.m_context.GetInputReportByteLength().
		}
		else
		{
			if (!m_findCardReaderFailedReported) 
			{
				LogString( ERROR_LOG, "Find Card Reader failed. " );
				m_findCardReaderFailedReported = true;
			}

			// If open failed then wait a bit before trying again, so thread does not run continuously.
			CUtilities::Sleep(CARD_READER_LOOP_SLEEP_PERIOD);
		}
	}

	if (m_cardReaderLayer.IsReady())
	{
		// Look for card status to see if card is in at startup
		bool bTimeout = true;
		m_bErrorOpening = false;
		CardStatusType status = CARD_STATUS_UNKNOWN;
		int bytesRead = 0;

		// Set count before we retry connecting after too many errors. This will also prevent a (very unlikely) recursive call of Open in ReadCardStatus
		m_reconnectErrorCount = 0;
		
		// Sometimes ReadCardStatus times out and needs a second chance to run
		CUtilities::Sleep(DELAYAFTERCONFIG);
		for (int i = 0; i < OPENSTATUSRETRIES && bTimeout; i++)
		{
			ReadCardStatus(CUtilities::GetTickCount(), status, bTimeout, false, bytesRead, NULL, 0);
		}
		if (status == CARD_STATUS_INSERTED)
		{
			// Card inserted
			m_bInserted = true;
			
			// Only set bezel if this is first connect attempt
			if (deviceFound)
			{
				SetBezelStyle(CARD_BAD_READ); // No data for internals.
			}
			//TRACE(L"Found card inserted\n");
		}
		else if (status == CARD_STATUS_REMOVED)
		{
			// Card removed
			m_bInserted = false;
			
			// Only set bezel if this is first connect attempt
			if (deviceFound)
			{
				SetBezelStyle(CARD_REMOVED);
			}
			//TRACE(L"card not inserted\n");
		}
		
	}
	else
	{
		m_bErrorOpening = true;
#ifdef LOGGING
		if (!m_errorOpeningMCRDriverReported)
		{
			LogString( ERROR_LOG, "Error opening MCR driver" );
			m_errorOpeningMCRDriverReported=true;
		}
#endif
	}
}


void CMCRWrapper::Close( void )
{
	// clean up HID cardreader
	if (m_cardReaderLayer.IsReady())
	{
		m_cardReaderLayer.Cleanup();
	}
}

/// <summary>
/// Get the inserted card, if any.
/// </summary>
/// <param name="bTimeout">OUT - true if a timeout occurred, false otherwise.</param>
/// <param name="track1">IN - The preferred track to get data from.</param>
/// <returns>A pointer for the inserted card, iif any.</returns>
CCard * CMCRWrapper::WaitForCardEvent(bool &bInserted, bool &bRemoved, bool &bTimeout, bool track1)
{
	CCard *card = nullptr;

	bTimeout = false;
	bool eventReady = false;
	bRemoved = false;
	bInserted = false;
	DWORD tickAtStart = CUtilities::GetTickCount();

	CheckBezelStyle();

	while (!eventReady && !bTimeout)
	{
		// loop until timeout or sentinel gets a card in/out
		WaitForCardInsertRemove(tickAtStart, bInserted, bRemoved, bTimeout);
		if (!bTimeout)
		{
			eventReady = true;

			// Insert or Remove event
			if (bRemoved)
			{
				m_removeCardEvent.signal(false);
				LogString(_T("GENERAL"), _T("DEBUG: MCR SetEvent Card Removed."));
			}
			else if (bInserted)
			{
				// Return timeout=false, so Internals will process the newly created card instance.
				card = GetInsertedCard(track1);
				m_insertCardEvent.signal(false);
				LogString(_T("GENERAL"), _T("DEBUG: MCR SetEvent Card Inserted."));
			}
		}

		CheckBezelStyle();
	}
	return card;
}

CCard *CMCRWrapper::GetCardFromRawData(const byte *readBuffer, int bytesRead) const
{
	if (readBuffer == NULL)
	{
		bytesRead = 0;
	}

	CCard *card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetUTCTime());
	card->SetAsInvalidRead();

	// SetAsInvalidRead clears the 'card time in' values so we need to put it back.
	card->SetCardInTime( CUtilities::GetCurrentTime(), CUtilities::GetUTCTime() );

	if ( bytesRead > 0 )
	{
		// Set the number in the card object from the mcr raw data.
		if (!card->IsValidRead())
		{
			card->SetCardInfoFromRawData(readBuffer, bytesRead, m_cardConfig);
		}
	}

	return card;
}

/// <remarks>
/// 2013-08-23 Typical operation, per 0x5b Read Card Data request:
///
/// If good data is available, UIC card readers
///     send a 0x66 Error Data report immediately (with error code 0x11).
///     followed by a 0x65 Card Data report after 60-70 milliseconds.
/// If good data is available, XST card readers
///     send a 0x65 Card Data report after 10-30 milliseconds.
///
/// If bad data is available, UIC card readers
///     send a 0x66 Error Data report immediately (with error code 17).
///     followed by another 0x66 Error Data report after 60-70 milliseconds (with error code 17) .
/// If bad data is available, XST card readers
///     send a 0x66 Error Data report after 10-30 milliseconds (with error code 7, 8, 9, or 10) .
///
/// If no data is available, UIC and XST card readers
///     send a 0x66 Error Data report immediately (with error code 17).
/// </remarks>
bool CMCRWrapper::ReadCardStatus(DWORD startTickCount, CardStatusType &status, bool &timedOut, bool track1, int &bytesRead, byte *byteBuffer, int byteBufferLen )
{
	timedOut = false;
	bool bDataReady = false;
	bool pendingOperationReady = false;
	bool bRemoved;

	try
	{
		lock_guard<recursive_mutex> lock(m_cs);
		
		// Only request data once per track per card insertion.
		// The card reader may have to send multiple other reports first.
		// If the card reader does not have a report ready to send then overlapped reads will pend.
		if ( byteBuffer != NULL)
		{
			SetToReadCardData(track1);
		}

		// cardreader returns partial insert and then full insert.  Keep looping until timeout or full event received.
		// Stop looping if track data is received and byteBuffer != NULL.
		while (!timedOut && !bDataReady)
		{
			// Loop until timeout or pending cardreader event received.  Event could be insert/remove or partial insert, or error event
			while (!timedOut && !pendingOperationReady)
			{
				CheckBezelStyle();
				
				CheckIfGetInputReportNeedsToBeCalled(pendingOperationReady);

				CheckIfThereIsAPendingRequest(pendingOperationReady, timedOut, startTickCount);

				CUtilities::Sleep(CARD_READER_LOOP_SLEEP_PERIOD);
			}

			CheckIfThereIsPendingOperationReadyToBeProcessed(pendingOperationReady,
				bRemoved,
				bDataReady,
				status,
				bytesRead,
				byteBufferLen,
				track1, 
				timedOut,
				byteBuffer);			
		}
	}
	catch (...)
	{
	}

	return (!timedOut && bDataReady);

}

void CMCRWrapper::SetToReadCardData(bool track1)
{
	
	// Want to ask for card data
	CardDataTrackType track = CARD_DATA_TRACK2;
	if (m_rfidDataIsAvailable)
	{
		track = CARD_DATA_RFID;
	}
	else if (track1)
	{
		track = CARD_DATA_TRACK1;
	}

	bool success = m_cardReaderLayer.ReadCardData(track);
	if (!success)
	{
		LogString( ERROR_LOG, "MCR Error: Could not send get card data.");
	}
}

void CMCRWrapper::CheckIfGetInputReportNeedsToBeCalled(
	bool &pendingOperationReady)
{
	bool success = false;
	if (!m_bPendingInsRemRequest)
	{
		// Call GetInputReport, wait for answer to be completed.
		m_cardDataReport[0] = 0;
		m_cardDataReport[1] = 0;
		m_cardDataReportBytes = CARD_DATA_REPORT_SIZE;
		success = m_cardReaderLayer.ReadReportData(m_cardDataReport, m_cardDataReportBytes);
		if (success && m_cardDataReportBytes > 0)
		{
#ifdef MCR_LOGGING
			LogString(_T("MCR"), _T("{FeatureRpt} [%s]"), static_cast<LPCTSTR>(CUtilities::ToHexString(m_cardDataReport, m_cardDataReportBytes)));
#endif
			pendingOperationReady = true;
		}
//		else if ( lastError == ERROR_IO_PENDING)
//		{
//			m_bPendingInsRemRequest = true;
//		}
		else
		{
			// something else happened
			//HandleCardReaderReportError(bDataReady, bRemoved, pendingOperationReady);
		}
	}
}

void CMCRWrapper::HandleCardReaderReportError(
	bool &bDataReady,
	bool &bRemoved,		
	bool &pendingOperationReady)
{
	// This gets logged thousands of times per minute when cardreader disappears. If the count gets to 1000, try to reconnect cardreader.
	m_reconnectErrorCount++;

	// Added logic check to make sure this error does not go crazy reporting and filling disk and crashing system
	if (!m_insertRemoveErrorReported)		 
	{
		LogString( ERROR_LOG, "MCR: Could not complete insert/remove HID cardreader report");
		m_insertRemoveErrorReported = true;  // the error has been reported stop reporting until the card reader is reconnected
	}

	// Handle errors that can occur while using "context->readHandle_".
	if (m_reconnectErrorCount > MCR_RECONNECT_ON_ERROR_COUNT)
	{
		// try to recover cardreader
		ReconectOnErrorCount(bDataReady, bRemoved, pendingOperationReady);
	}
}

void CMCRWrapper::ReconectOnErrorCount(
	bool &bDataReady,
	bool &bRemoved,		
	bool &pendingOperationReady)
{
	m_reconnectErrorCount = 0;
	m_cardReaderLayer.Cleanup();
	Open();

	// Set states
	if (m_bInserted)
	{
		// Card inserted
		bRemoved = false;
		bDataReady = true;
	}
	else
	{
		// Card removed
		bRemoved = true;
		bDataReady = true;
	}
	pendingOperationReady = false;
}

void CMCRWrapper::CheckIfThereIsAPendingRequest(bool &pendingOperationReady, bool &timedOut, DWORD startTickCount)
{
	if (m_bPendingInsRemRequest)
	{
		m_bPendingInsRemRequest = false;
	}
							
	//Check if pending operation timed out
	if (!pendingOperationReady && CUtilities::GetTickCount() - startTickCount > CARD_READ_TIMEOUT)
	{
		timedOut = true;
	}
}

void CMCRWrapper::CheckIfThereIsPendingOperationReadyToBeProcessed(bool &pendingOperationReady, 
	bool &bRemoved, 
	bool &bDataReady,
	CardStatusType &status,
	int &bytesRead,
	int byteBufferLen, 
	bool track1,
	bool timedOut,
	byte *byteBuffer)
{
	if (pendingOperationReady && !timedOut)
	{
		m_bPendingInsRemRequest = false;

		switch (m_cardDataReport[DATA_REPORT_EVENT])
		{
		case HID_CARDRDR_EVT_CARD_STATUS :
			ProcessEventCardStatus(bRemoved, byteBuffer, bDataReady, pendingOperationReady, status);
			break;
		case HID_CARDRDR_EVT_CARD_DATA :
			{
				ProcessEventCardData(byteBuffer, bDataReady, pendingOperationReady, byteBufferLen, bytesRead, track1);
			}
			break;
		case HID_CARDRDR_EVT_ERROR_DATA :
			{
				ProcessEventError();

				if (m_waitingDataRead)
				{
					m_waitingDataRead = false;
					ClearRFIDBuffer();
				}
				pendingOperationReady = false;
			}
			break;
		default:
			pendingOperationReady = false;
			break;
		}
	}
}

void CMCRWrapper::ProcessEventCardStatus(bool &bRemoved, byte *byteBuffer, bool &bDataReady, bool &pendingOperationReady, CardStatusType &status)
{
	if (m_cardDataReport[DATA_REPORT_VALUE] & DATA_REPORT_INSERTED)
	{
		// Card inserted
		LogString(_T("GENERAL"), _T("DEBUG: MCR Card Inserted."));
		m_bInserted = true;
		m_track1DataIsAvailable = 0 != (m_cardDataReport[CARD_DATA_REPORT_CARD_STATUS_TRACK_OFFSET] & CARD_DATA_REPORT_CARD_STATUS_TRACK1_AVAILABLE_FLAG);
		m_track2DataIsAvailable = 0 != (m_cardDataReport[CARD_DATA_REPORT_CARD_STATUS_TRACK_OFFSET] & CARD_DATA_REPORT_CARD_STATUS_TRACK2_AVAILABLE_FLAG);
		m_rfidDataIsAvailable = 0 != (m_cardDataReport[CARD_DATA_REPORT_CARD_STATUS_TRACK_OFFSET] & CARD_DATA_REPORT_CARD_STATUS_RFID_AVAILABLE_FLAG);
		bRemoved = false;

		// only set bDataReady = true if not waiting for card data
		if (!byteBuffer)
		{
			bDataReady = true;
		}
		else
		{
			pendingOperationReady = false; // not what is needed, go again
		}
		status = CARD_STATUS_INSERTED;
	}
	else if (m_cardDataReport[DATA_REPORT_VALUE] & DATA_REPORT_CARD_PRESENT)
	{
		// Only process for RFID/Mifare
		if (m_cardDataReport[CARD_DATA_REPORT_CARD_STATUS_TRACK_OFFSET] & CARD_DATA_REPORT_CARD_STATUS_RFID_AVAILABLE_FLAG)
		{
			// RFID Card inserted, keep m_bInserted = false
			LogString(_T("GENERAL"), _T("DEBUG: RFID Card Inserted."));
			m_bInserted = false;
			m_track1DataIsAvailable = false;
			m_track2DataIsAvailable = false;
			m_rfidDataIsAvailable = 0 != (m_cardDataReport[CARD_DATA_REPORT_CARD_STATUS_TRACK_OFFSET] & CARD_DATA_REPORT_CARD_STATUS_RFID_AVAILABLE_FLAG);
			m_waitingDataRead = true;

			bRemoved = false;
			bDataReady = true;
			status = CARD_STATUS_INSERTED;
		}
		else
		{
			pendingOperationReady = false; // Not what is needed, go again.
		}
	}
	else if(m_cardDataReport[DATA_REPORT_VALUE] & DATA_REPORT_REMOVED)
	{
		if (!m_cardReaderLayer.IsRfidReader())
		{
			// Card removed
			LogString("GENERAL", "DEBUG: MCR Card Removed.");
			m_bInserted = false;
			bRemoved = true;
			m_track1DataIsAvailable = false;
			m_track2DataIsAvailable = false;
			m_rfidDataIsAvailable = false;

			// only set bDataReady = true if not waiting for card data
			if (!byteBuffer)
			{
				bDataReady = true;
			}
			else
			{
				pendingOperationReady = false; // not what is needed, go again
			}
			status = CARD_STATUS_REMOVED;

			//Need to clear buffer data so a bad insert won't report this card OCR
			bool success = m_cardReaderLayer.ClearBuffer();
			if (!success)
			{
				LogString(ERROR_LOG, "MCR Info: Could not clear MCR buffer.");
			}
		}
		else
		{
			bDataReady = true;
			pendingOperationReady = false;
			m_waitingRemove = false;
		}
	}
	else
	{
		pendingOperationReady = false; // not what is needed, go again
	}
}

void CMCRWrapper::ProcessEventCardData( byte *byteBuffer, bool &bDataReady, bool &pendingOperationReady, int byteBufferLen, int &bytesRead, bool track1)
{
	if (byteBuffer)
	{
		USHORT dataLen = (USHORT) *(m_cardDataReport + CARD_DATA_SIZE_LOC);
		byte type = (byte)*(m_cardDataReport + CARD_DATA_TYPE_LOC);

		char buf[CARD_DATA_BUFFER_SIZE];
		if (type != RFIDDATA)
		{
			FormatMCRCardData(buf,
				m_cardDataReport + CARD_DATA_REPORT_CARD_DATA_OFFSET,
				m_cardDataReport[CARD_DATA_REPORT_CARD_DATA_LEN_OFFSET],
				track1 ? TRACK_1 : TRACK_2);
			memcpy(byteBuffer, buf, dataLen < byteBufferLen ? dataLen : byteBufferLen);
			bytesRead = dataLen < byteBufferLen ? dataLen : byteBufferLen;
			//((CSentinelApp *)AfxGetApp())->LogString( L"GENERAL", L"MCR: Read Card data %s", byteBuffer );

			bDataReady = true;
		}
		else
		{
			// Data is single block of RFID data
			FormatRFIDData(byteBuffer, byteBufferLen, bDataReady, bytesRead);
				
			m_bPendingInsRemRequest = bDataReady;
			pendingOperationReady = bDataReady;
			//((CSentinelApp *)AfxGetApp())->LogString( L"GENERAL", L"MCR: Read Card data %s", byteBuffer );
		}
	}
	else
	{
		pendingOperationReady = false;
	}
}

/// <summary>
/// Processes the 0x66 Error Data report received from the card reader.
/// </summary>
/// <remarks>
/// Error code 17 is normal information if the card reader indicated that data was NOT available for the track.
/// For UIC card readers, error code 17 is normal information on a bad read, when data is present, but the data read is not valid.
/// For XST card readers, error code 17 indicates an error if the card reader indicated that data WAS available for the track.
/// Error codes 7, 8, 9, 10 are normal information on a bad read, when data is present, but the data read is not valid.
/// </remarks>
void CMCRWrapper::ProcessEventError()
{
	// Most attempts to read card data result in at least 1 error. Allow multiple errors before failing
	LogString( ERROR_LOG, "MCR Event Error Code %d", (byte)m_cardDataReport[DATA_REPORT_VALUE] );
	
}

void CMCRWrapper::WaitForCardInsertRemove(DWORD startTickCount, bool &bInserted, bool &bRemoved, bool &bTimeout)
{
	const int LOOK_FOR_USB_INTERVAL = 15000;  // 15 seconds

	bTimeout = false;
	bInserted = false;
	bRemoved = false;
	int bytesRead = 0;

	try
	{

		//Every 15 seconds, if device state is TTL_NODATA try to reconnect to card reader hoping to find USB reader.
		if ( !IsCardReaderReady() && CUtilities::GetTickCount() - m_lookForUSBTicks > LOOK_FOR_USB_INTERVAL )
		{
			// open new connection
			Open();
			m_lookForUSBTicks = CUtilities::GetTickCount();
		}

		CardStatusType status(CARD_STATUS_UNKNOWN);
		
		ReadCardStatus(startTickCount, status, bTimeout, false, bytesRead, NULL, 0);
		
		// If this function is called as a result of a reconnect, card removed can't appear to timeout.
		if ( status == CARD_STATUS_REMOVED )
		{
			bTimeout = false;
			bRemoved = true;
		}
		else if ( status == CARD_STATUS_INSERTED )
		{
			if (bTimeout)
			{
				LogString("GENERAL", "DEBUG: MCR Card Inserted. bTimeout=true");
			}

			bInserted = true;
		}
			
	}
	catch (...)
	{
	}

}

void CMCRWrapper::SetBezelStyle(CARD_STYLE style, DWORD rankId)
{
	lock_guard<recursive_mutex> lock(m_cs);
	if (m_nextBezelStyle != style)
	{
		LogString(MCR_LOG, "Queuing bezel style from %d to %d, current rank id = %d, next rank id = %d",
			m_nextBezelStyle, style, m_nextRankId, rankId);
	}
	m_nextBezelStyle = style;
	m_nextRankId = rankId;
}

CARD_STYLE CMCRWrapper::GetBezelStyle()
{
	lock_guard<recursive_mutex> lock(m_cs);
	return m_nextBezelStyle;
}

void CMCRWrapper::CheckBezelStyle()
{
	lock_guard<recursive_mutex> lock(m_cs);

	CARD_STYLE style = m_nextBezelStyle;
	DWORD rankId = m_nextRankId;

	DWORD currentTicks = CUtilities::GetTickCount();
	if (m_currentBezelStyle != style || m_currentRankId != rankId ||
		currentTicks - m_lastBezelUpdateTicks >= BEZEL_UPDATE_TIMEOUT)
	{
		if (m_currentBezelStyle != style || m_currentRankId != rankId)
		{
			LogString(MCR_LOG, "Setting bezel style %d, rank id = %d", style, rankId);
		}

		m_lastBezelUpdateTicks = currentTicks;
		SetBezelStyleFeature(style, rankId);
		m_currentBezelStyle = style; // Don't retry failures.
		m_currentRankId = rankId;
	}
}

void CMCRWrapper::ClearRFIDBuffer()
{
	if (!m_waitingRemove && !m_waitingDataRead)
	{
		m_cardReaderLayer.ClearBuffer();
	}
}

void CMCRWrapper::SetBezelStyleFeature(CARD_STYLE style, DWORD rankId)
{
	bool success = m_cardReaderLayer.SetBezelStyle(style, rankId);
	if (!success)
	{
		// Probably don't want to log error message if lighted bezel isn't present.
		// 2011-12-02 Errors do not occur with serial card readers (which do not have lighted bezels).
		//TRACE(L"Could not set color.");
		if (!m_couldNotSetBezelColorReported)
		{
			LogString( ERROR_LOG, "MCR Info: Could not set bezel color. style=%d",
				(int)style);
			m_couldNotSetBezelColorReported = true;
		}
		else
		{
			Open();
		}
			
//		// Handle errors that can occur while using "context->deviceHandle_".
//		if (ERROR_INVALID_HANDLE == lastError)
//		{
//			Open();
//		}
	}
}

bool CMCRWrapper::IsTrackDataAvailable(bool track1) const
{
	bool available = (track1 && m_track1DataIsAvailable) || (!track1 && m_track2DataIsAvailable) || m_rfidDataIsAvailable;
	return available;
}

/// <summary>
/// Get the inserted card.
/// Clear out pending reports, which could be generated by previous processing for this card insertion.
/// </summary>
/// <param name="track1">IN - The preferred track to get data from.</param>
/// <returns>A pointer for the inserted card.</returns>
/// <remarks>
/// Only global global cards are valid on the non-preferred track.
/// </remarks>
CCard * CMCRWrapper::GetInsertedCard(bool track1)
{
	int bytesRead = 0;
	byte readBuffer[CARD_BUFFER_LEN];
	CCard *card(NULL);
	bool processedPendingEvents = false;

	// Check if card data for the preferred track is available.
	if (IsTrackDataAvailable(track1))
	{
		GetInsertedCardData(track1, readBuffer, CARD_BUFFER_LEN, bytesRead);
		LogString("GENERAL", "DEBUG: MCR Card Inserted. track1=%d bytesRead=%d", (int)track1, (int)bytesRead);
		processedPendingEvents = true;

		card = GetCardFromRawData(readBuffer, bytesRead);
	}

	// If not valid then check other track for a global global card.
	if (card == NULL || !card->IsValidRead())
	{
		// Check if card data for the non-preferred track is available.
		if (IsTrackDataAvailable(!track1))
		{
			if (card)
			{
				delete card;
				card = NULL;
			}

			GetInsertedCardData(!track1, readBuffer, CARD_BUFFER_LEN, bytesRead);
			LogString("GENERAL", "DEBUG: MCR Card Inserted2. !track1=%d bytesRead=%d", (int)!track1, (int)bytesRead);
			processedPendingEvents = true;

			card = GetCardFromRawData(readBuffer, bytesRead);

			if (card != NULL)
			{
				if (card->IsValidRead())
				{
					// Set invalid if not a global global card.
					if (!card->IsGlobalGlobal())
					{
						card->SetAsInvalidRead();
					}
				}
			}
		}
	}

	if (!processedPendingEvents)
	{
		// Clear out excess pending reports -- generally caused by disabling and enabling the card reader. 
		// GetInsertedCardData does this efficiently when called with a NULL buffer. 
		GetInsertedCardData(track1, NULL, 0, bytesRead);
		LogString("GENERAL", "DEBUG: MCR Card Inserted. No data");
	}

	if (card == NULL)
	{
		// Make a default card.
		card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetUTCTime());
	}

	return card;
}

void CMCRWrapper::GetInsertedCardData(bool track1, byte *byteBuffer, int byteBufferLen, int &bytesRead)
{
	bytesRead = 0;

	//since card was inserted, start tick count over
	DWORD startTickCount = CUtilities::GetTickCount();

	//((CSentinelApp *)AfxGetApp())->LogString( LERROR_LOG, L"MCR Info: Card Inserted, getting card data");
	try
	{
		bool bTimeout = false;
		CardStatusType status = CARD_STATUS_UNKNOWN;
		ReadCardStatus(startTickCount, status, bTimeout, track1, bytesRead, byteBuffer, byteBufferLen);
	}
	catch (...)
	{
	}

}
bool CMCRWrapper::IsCardInserted( void )
{
	// The Sentinel 3 declared this function static.  Accessing the S5 cardreader driver this way
	// doesn't seem to be supported.  
	// The function will return false if called before the card reader status is determined, but isn't a problem in this case.
	return m_bInserted;
}

void CMCRWrapper::FormatMCRCardData (char *buf, UCHAR cardDataReport[], USHORT cardDataLen, USHORT trk)
{
  // format a string
  static unsigned char tbl23[] =
  {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    ':', ';', '<', '=', '>', '?'
  };
  *buf = '\0';

  for (int i=0; i<cardDataLen; i++)
  {
    if (trk == 1)
	{
		// track 1 data is already been converted by card reader or hid driver
		buf[i] = cardDataReport[i];
	}
    else
		buf[i] = tbl23[cardDataReport[i] & 0x0F];
  }

  buf[cardDataLen] = 0;
}

void CMCRWrapper::FormatRFIDData(byte *buf,USHORT cardDataLen, bool &cardComplete, int &bytesRead)
{
	USHORT dataLen = (USHORT)*(m_cardDataReport + CARD_DATA_SIZE_LOC);
	WORD index = *(m_cardDataReport + CARD_DATA_INDEX_LOC);

	if (dataLen < CARD_DATA_FULL_LENGTH)
	{
		// No data or less than full length signifies no more data coming
		cardComplete = true;
	}
	if (dataLen > 0 && dataLen + CARD_DATA_LOC >= bytesRead && index * CARD_DATA_FULL_LENGTH + dataLen <= cardDataLen)
	{
		// copy new data
		memcpy(buf + CARD_DATA_FULL_LENGTH * index, m_cardDataReport + CARD_DATA_LOC, dataLen);
	}

	if (index > 0)
	{
		bytesRead = index * CARD_DATA_FULL_LENGTH;
	}
	bytesRead += dataLen;

	if (!cardComplete)
	{
		SetToReadCardData(false);
	}
	else
	{
		// Received last of data, check if buffer should be cleared
		m_waitingDataRead = false;
		ClearRFIDBuffer();
	}
}

/// <summary>
/// Check if error occurred when last opening the card reader.
/// </summary>
/// <returns>Returns true if error occurred when last opening the card reader, false otherwise.</returns>
bool CMCRWrapper::IsOpenError()
{
	
	return m_bErrorOpening;
}

/// <summary>
/// Check if card reader is open and ready for commands
/// </summary>
/// <param name="void"></param>
/// <returns>Returns true if card reader is open and ready for commandsk, false otherwise.</returns>
bool CMCRWrapper::IsCardReaderReady( void )
{
	bool isCardReaderReady = m_cardReaderLayer.IsReady();

	return isCardReaderReady;
}

void CMCRWrapper::RefreshConfig(const CConfig &config)
{
	lock_guard<recursive_mutex> lock(m_cs);
	m_cardConfig.Setup(config);
	m_cardReaderLayer.LoadConfig();
}

bool CMCRWrapper::IsRfidReader()
{
	return m_cardReaderLayer.IsRfidReader();
}
