#pragma once

#include "../Card.h"
#include "../Utilities.h"
#include "Config/Config.h"
#include "Config/CardConfig.h"
#include "PlayerInfo.h"
//#include "../../CardReaderLayerDll/CardReaderLayerAPI.h"
#include "../binsem.h"
#include "CardReaderLayer.h"


static const DWORD CARD_READER_LOOP_SLEEP_PERIOD = 9; // Millisecond Sleep period while waiting for other threads.

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class MCRWrapperUnitTests_RawData; }

enum CARD_EVENTS
{
	CARD_INSERT_EVENT = 0,
	CARD_REMOVE_EVENT,
	NUMBER_OF_CARD_EVENTS,	// Keep last for the count
};

typedef enum CARD_WAIT_EVENTS
{
	CARD_WAIT_NONE,
	CARD_WAIT_EVENT,
	CARD_WAIT_EVENT_DONE,
} CardEventType;

typedef enum CARD_STATUS
{
	CARD_STATUS_UNKNOWN,
	CARD_STATUS_INSERTED,
	CARD_STATUS_REMOVED,
} CardStatusType;

class CMCRWrapper
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CMCRWrapper(CConfig config);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CMCRWrapper(void);

	/// <summary>
	/// Check if card reader is open and ready for commands
	/// </summary>
	/// <param name="void"></param>
	/// <returns>Returns true if card reader is open and ready for commands</returns>
	bool IsCardReaderReady( void );

	/// <summary>
	/// Check if card is inserted
	/// </summary>
	/// <returns>Returns true if card is currently inserted.</returns>
	/// <remarks>
	/// This method is static so it can be called directly from CUIProtocol::ProcessPBCompleteFromInternals() after sleeping/waiting as needed.
	/// </remarks>
	static bool IsCardInserted( void );

	/// <summary>
	/// Monitors card reader looking for state changes (i.e. card insert/remove actions)
	/// </summary>
	/// <param name="config">IN - pointer to configuration values that can change at any time.</param>
	/// <returns></returns>
	void CardReaderProcessingLoop(const CConfig *config);

	/// <summary>
	/// Tells CMCRWrapper to stop card reader processing loop
	/// </summary>
	/// <returns></returns>
	void ShutdownCardReader( );

	/// <summary>
	/// Determines if an open error exists and a card inserted
	/// </summary>
	/// <returns>Returns true if there was an error opening card reader and a card was inserted when card reader went offline.</returns>
	bool IsOpenErrorAndCardInserted();

	/// <summary>
	/// SetBezelStyle can be called asynchronously.
	/// </summary>
	/// <param name="style">Card style to be displayed</param>
	/// <param name="rankId">IN - player rank identifier.  Ignored if style != CARD_STYLE_PLAYER.</param>
	void SetBezelStyle(CARD_STYLE style, DWORD rankId = CPlayerInfo::INVALID_RANK_ID);

	/// <summary>
	/// Gets the bezel style
	/// </summary>
	/// <returns>Returns the bezel style.</returns>
	CARD_STYLE GetBezelStyle();

	/// <summary>
	/// Tells Reader Processing Loop to look for card event
	/// </summary>
	/// <param name="bInserted"></param>
	/// <param name="bRemoved"></param>
	/// <param name="bTimeout"></param>
	/// <param name="bIsCardInserted"></param>
	/// <returns>Returns inserted card or NULL</returns>
	CCard *McrWaitForCardEvent(bool &bInserted, bool &bRemoved, bool &bTimeout, bool &bIsCardInserted);

	/// <summary>
	/// Refreshes the current card configuration.
	/// </summary>
	/// <param name="config">The configuration.</param>
	void RefreshConfig(const CConfig &config);

	///	<summary>
	/// Writes HTML.
	///	</summary>
	/// <param name="os">The ostream to write to.</param>
	///	<remarks>
	/// This is implemented in ProcessDump\WebDiagnosticsCardReader.cpp
	///	</remarks>
	void WriteHtml(std::ostream& os) const;

	bool IsRfidReader();

private:
	static const size_t CARD_DATA_REPORT_SIZE = HID_CARDRDR_DATA_REPORT_SIZE; // MUST be a least m_cardReaderLayer.m_context.GetInputReportByteLength() bytes.
	static const int CARD_DATA_REPORT_CARD_DATA_LEN_OFFSET = HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_LENGTH;
	static const int CARD_DATA_REPORT_CARD_DATA_OFFSET = HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_DATA;
	static const int CARD_DATA_REPORT_CARD_STATUS_TRACK_OFFSET = HID_CARDRDR_DATA_REPORT_CARD_STATUS_TRACK_OFFSET;
	static const int CARD_DATA_REPORT_CARD_STATUS_TRACK1_AVAILABLE_FLAG = HID_CARDRDR_DATA_REPORT_CARD_STATUS_TRACK1_AVAILABLE_FLAG;
	static const int CARD_DATA_REPORT_CARD_STATUS_TRACK2_AVAILABLE_FLAG = HID_CARDRDR_DATA_REPORT_CARD_STATUS_TRACK2_AVAILABLE_FLAG;
	static const int CARD_DATA_REPORT_CARD_STATUS_RFID_AVAILABLE_FLAG = HID_CARDRDR_DATA_REPORT_CARD_STATUS_RFID_AVAILABLE_FLAG;
	static const USHORT TRACK_1 = 1;
	static const USHORT TRACK_2 = 2;
	static const USHORT TRACK_RFID = 8;

	binsem m_insertCardEvent;
	binsem m_removeCardEvent;
	UCHAR m_cardDataReport[CARD_DATA_REPORT_SIZE];
	int m_cardDataReportBytes; // Number of valid bytes in m_cardDataReport.
	CCardConfig m_cardConfig;
	static bool m_bInserted;			// Indicates when a card is inserted. Static so it can be called from IsCardInserted().
	bool m_track1DataIsAvailable;
	bool m_track2DataIsAvailable;
	bool m_rfidDataIsAvailable;
	volatile CARD_STYLE m_nextBezelStyle;
	CARD_STYLE m_currentBezelStyle;
	volatile DWORD m_nextRankId;
	DWORD m_currentRankId;
	DWORD m_lastBezelUpdateTicks;
	std::recursive_mutex m_cs;
	DWORD m_lookForUSBTicks;
	bool m_bErrorOpening;
	std::recursive_mutex m_csInternalsFramework;
	bool m_shutdown;
	bool m_bPendingInsRemRequest; // is there a pending insert/remove request.
	void Open( void );
	void Close( void );
    bool m_isUniverseEnabled;
//	byte m_universePropCodes[SIZE_OF_VALID_CARDS_PREFIX][SIZE_OF_VALID_PROP_CODE];
//	byte m_universePrefixes[SIZE_OF_VALID_CARDS_PREFIX];
	int m_universePrefixCount;
	int m_reconnectErrorCount;
	bool m_insertRemoveErrorReported;	
	bool m_findCardReaderFailedReported;
	bool m_errorOpeningMCRDriverReported;
	bool m_couldNotSetBezelColorReported;

	/// <summary>
	/// Get information after insert/remove event.
	/// </summary>
	/// <param name="bTimeout">OUT - true if a timeout occurred, false otherwise.</param>
	/// <param name="track1">IN - The preferred track to get data from.</param>
	/// <returns>A pointer for the inserted card, iif any.</returns>
	CCard *WaitForCardEvent(bool &bInserted, bool &bRemoved, bool &bTimeout, bool track1);
	/// <summary>
	/// Check is error occurred while opening card reader
	/// </summary>
	/// <returns>Returns true if error occurred while opening card reader</returns>
	bool IsOpenError();
	/// <summary>
	/// Set event type for cardreader to wait on
	/// </summary>
	/// <param name="eventType">Event type to wait for (ie CARD_WAIT_INSERT)</param>
	/// <returns></returns>
	void SetCardEventWaitType( CardEventType eventType);

	void WaitForCardInsertRemove(DWORD startTickCount, bool &bInserted, bool &bRemoved, bool &bTimeout);
	bool IsTrackDataAvailable(bool track1) const;
	CCard * GetInsertedCard(bool track1);
	void GetInsertedCardData(bool track1, byte *byteBuffer, int byteBufferLen, int &bytesRead);
	void FormatMCRCardData (char *buf, UCHAR cardDataReport[], USHORT cardDataLen, USHORT trk);
	void FormatRFIDData(byte *buf, USHORT cardDataLen, bool &cardComplete, int &bytesRead);
	void CheckBezelStyle();
	void SetBezelStyleFeature(CARD_STYLE style, DWORD m_rankId);

	/// <summary>
	/// Gets a new CCard instance, which is set according the readBuffer and the current configuration.
	/// </summary>
	/// <param name="readBuffer">IN - The read buffer from the mcr raw data read.</param>
	/// <param name="bytesRead">IN - The number of bytes read and stored in readbuffer.</param>
	/// <returns>A pointer to a new CCard instance.</returns>
	/// <remarks>
	/// The caller is responsible for deleting the returned value.
	/// </remarks>
	CCard *GetCardFromRawData(const byte *readBuffer, int bytesRead) const;

	bool ReadCardStatus( DWORD startTickCount, CardStatusType &status, bool &timedOut, bool track1, int &bytesRead, byte *byteBuffer = NULL, int byteBufferLen = 0 );
	void SetToReadCardData(bool track1);
	void CheckIfGetInputReportNeedsToBeCalled(
		
		bool &pendingOperationReady);
	void HandleCardReaderReportError(
		bool &bDataReady,
		bool &bRemoved,		
		bool &pendingOperationReady);
	void ReconectOnErrorCount(
		bool &bDataReady,
		bool &bRemoved,		
		bool &pendingOperationReady);
	void CheckIfThereIsAPendingRequest(bool &pendingOperationReady, bool &timedOut, DWORD startTickCount);
	void CheckIfThereIsPendingOperationReadyToBeProcessed(bool &pendingOperationReady, 
		bool &bRemoved, 
		bool &bDataReady,
		CardStatusType &status,
		int &bytesRead,
		int byteBufferLen, 
		bool track1,
		bool timedOut,
		byte *byteBuffer);
	void ProcessEventCardStatus( bool &bRemoved, byte *byteBuffer, bool &bDataReady, bool &pendingOperationReady, CardStatusType &status );
	void ProcessEventCardData( byte *byteBuffer, bool &bDataReady, bool &pendingOperationReady, int byteBufferLen, int &bytesRead, bool track1 );
	void ProcessEventError(  );
	void ProcessWaitForEvent( const CConfig *config );
	void ClearRFIDBuffer();
	CCardReaderLayer m_cardReaderLayer;
	volatile bool m_mcrIsCardInserted;
	CCard * volatile m_card;
	volatile bool m_mcrInserted;
	volatile bool m_mcrRemoved;
	volatile bool m_mcrTimeout;
	CardEventType m_eventWaitType;
	bool m_waitingDataRead;
	bool m_waitingRemove;

	
	// Grant "friend" access for testing.
    friend class SentinelNativeUnitTests::MCRWrapperUnitTests_RawData;
};

