#include "stdafx.h"
#include "CardReaderLayer.h"
#include "CardReader.h"
#include "Logging/LogString.h"
#include "EnumMap.h"

using namespace std;

static const DWORD CARD_READER_EVENT_WAIT_PERIOD = 10; // Timeout while waiting for card data report OLAP completion.
static const USHORT TRACK_1 = 1;
static const USHORT TRACK_2 = 2;
static const USHORT TRACK_RFID = 8;
static const int CARDREADERSTARTUPDELAY = 500;


static EnumMap<MACHINE_STATE>::EnumPair MACHINE_STATE_types[] =
{
	{ IN_THE_CLEAR,			"InTheClear"  },
	{ SOCIAL_DISTANCE,		"SocialDistance"   },
	{ CLEANING_IN_PROGRESS,	"CleaningInProgress"  }
};

EnumMap<MACHINE_STATE> MachineStateEnumMap(MACHINE_STATE_types);

static EnumMap<CARD_STYLE>::EnumPair CARD_STYLE_types[] =
{
	{ CARD_REMOVED,		   "Removed" },
	{ CARD_STYLE_PLAYER,   "Player" },
	{ CARD_BAD_READ,	   "BadRead" },
	{ CARD_STYLE_EMPLOYEE, "Employee" },
	{ CARD_STYLE_SOCIAL_DISTANCE, "SocialDistance" },
	{ CARD_STYLE_CLEANING_IN_PROGRESS, "CleaningInProgress" }
};

EnumMap<CARD_STYLE> CardStyleEnumMap(CARD_STYLE_types);

CCardReaderLayer::CCardReaderLayer(void) :
	m_insertRemoveErrorReported(false)
{
	m_rfidReader = false;
}

CCardReaderLayer::~CCardReaderLayer(void)
{
	Cleanup();
}

void CCardReaderLayer::Cleanup(void)
{
	m_context.Cleanup();
}

void CCardReaderLayer::LoadConfig()
{
	try
	{
		string pathname = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + pCardReaderConfigFilename;
		if (CUtilities::FileExists(pathname))
		{
			lock_guard<recursive_mutex> lock(m_cardReaderConfigCriticalSection);
			m_cardReaderConfig.reset(new CardReaderConfig(pathname));
		}
		else
		{
			string pathname = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_APPLICATIONS) + pCardReaderConfigFilename;
			if (CUtilities::FileExists(pathname))
			{
				lock_guard<recursive_mutex> lock(m_cardReaderConfigCriticalSection);
				m_cardReaderConfig.reset(new CardReaderConfig(pathname));
			}
 		}
	}
	catch (...)
	{
		m_cardReaderConfig.reset(new CardReaderConfig(""));
	}
}

bool CCardReaderLayer::IsReady()
{
	return m_context.IsReady();
}

bool CCardReaderLayer::IsRfidReader()
{
	return m_rfidReader;
}

// No longer using overlapped IO
//bool CCardReaderLayer::IsRequestPending(DWORD &numberOfBytesTransferred)
//{
//	numberOfBytesTransferred = 0;
//	bool requestPending = true;
//
//	// data not ready, look if call is pending or if call failed
//	// this should probably be a WaitForEvent(), but sample code does it this way.
//	DWORD waitResult = WaitForSingleObject(m_olap.hEvent, CARD_READER_EVENT_WAIT_PERIOD);
//	if (waitResult != WAIT_TIMEOUT)
//	{
//		requestPending = false;
//
//		if (waitResult == WAIT_OBJECT_0)
//		{
//			// Overlapped read completed.
//			BOOL ok = GetOverlappedResult(m_context.GetReadHandle(), &m_olap, &numberOfBytesTransferred, FALSE);
//#ifdef MCR_LOGGING
//			if (!ok)
//			{
//				DWORD errorCode = GetLastError();
//				LogString(MCR_LOG, _T("Error %u for GetOverlappedResult for {FeatureRpt}"), (unsigned)errorCode);
//				SetLastError(errorCode); // Restore error code.
//			}
//#endif
//		}
//		else
//		{
//			// An abnormal completion.
//#ifdef MCR_LOGGING
//			LogString(MCR_LOG, _T("{FeatureRpt} waitResult=%u"), (unsigned)waitResult);
//#endif
//		}
//	}
//
//	return requestPending;
//}

bool CCardReaderLayer::SetFeature(PUCHAR reportBuf)
{
	USHORT bufferLength = HID_FEATURE_DATAB_LOCK_SIZE;
	bool result = CardReaderLayerSetFeature(m_context.GetDeviceHandle(), reportBuf, bufferLength);
#ifdef MCR_LOGGING
	if (result)
	{
		LogString(MCR_LOG, _T("{SetFeature} [%s]"),
					 static_cast<LPCTSTR>(CUtilities::ToHexString(reportBuf, m_context.GetFeatureReportByteLength())));
	}
#endif
	if (!result)
	{
		m_insertRemoveErrorReported = true; // The error has been reported, so stop reporting until the card reader is reconnected.
		LogString(ERROR_LOG, "MCR Error: While attempting to SetFeature = %s.",
					 CUtilities::ToHexString(reportBuf, m_context.GetFeatureReportByteLength()).c_str());
	}
	return result;
}

bool CCardReaderLayer::ReadReportData(PUCHAR reportBuf, int &bytes)
{
#ifdef MCR_LOGGING
	memset(reportBuf, LOGGING_DIAG_VALUE, m_context.GetInputReportByteLength());
#endif
	bool result = CardReaderLayerGetInputReport(m_context.GetDeviceHandle(), reportBuf, HID_FEATURE_DATAB_LOCK_SIZE, bytes);
	// Error code 6 (ERROR_INVALID_HANDLE) can also occur when the device is not connected.
	if (result && m_context.GetDeviceHandle() != nullptr && bytes > 0)
	{
		LogString(MCR_LOG, "MCR Data: GetInputReport = %s. Bytes Read = %d",
					  CUtilities::ToHexString(reportBuf, bytes).c_str(), bytes);
	}
	else if (!result)
	{
		m_insertRemoveErrorReported = true; // The error has been reported, so stop reporting until the card reader is reconnected.
		LogString(ERROR_LOG, "MCR Error: While attempting to get the GetInputReport = %s. Bytes Read = %d",
					 CUtilities::ToHexString(reportBuf, bytes).c_str(), bytes);
	}
	return result;
}

bool CCardReaderLayer::FindUSBCardReader()
{
	vector<CardReader> readers = CardReader::GetCardReaders(m_cardReadersPathname);
	auto it = find_if(readers.cbegin(), readers.cend(), [this](const CardReader& reader)
	{
		return m_context.FindDevice(reader.GetVendorId(), reader.GetProductId());
	});

	bool found = it != readers.cend();
	if (found)
	{
		m_rfidReader = it->GetType() == CardReaderType::RFID;
		lock_guard<std::recursive_mutex> cs(m_cardReaderConfigCriticalSection);
		m_cardReader.reset(new CardReader(*it));
	}
	return found;
}

bool CCardReaderLayer::InitCardReader()
{
	//TRACE(L"Found cardreader...configuring now.\n");

	bool success = Disable();

	if (!success)
	{
		LogString(ERROR_LOG, "MCR Error: Could not disable card reader");
	}

	success = Enable();
	if (!success)
	{
		LogString(ERROR_LOG, "MCR Error: Could not enable card reader");
		success = false;
	}

	//TRACE(L"Finished configuring card reader.\n");

	return success;
}

bool CCardReaderLayer::FindCardReader()
{
	static bool cardReaderNotFoundReported = false;

	bool cr_found = FindUSBCardReader();

	if (!cr_found)
	{
		if (!cardReaderNotFoundReported) 
		{
			LogString(ERROR_LOG, "MCR Info: Card reader not found." );
			cardReaderNotFoundReported = true;
		}
	}
	else
	{
		// Give card reader a chance to finish startup stuff
		CUtilities::Sleep(CARDREADERSTARTUPDELAY);
		LogString(GENERAL, "MCR Info: Card reader found.");
		cardReaderNotFoundReported = false;
		m_insertRemoveErrorReported = false; // Allow error reporting for ReadReportData() again.
		LoadConfig();
		InitCardReader();
	}

	return cr_found;
}

DWORD CCardReaderLayer::Enable()
{
	UCHAR feature[HID_FEATURE_DATAB_LOCK_SIZE];
#ifdef MCR_LOGGING
	memset(feature, LOGGING_DIAG_VALUE, sizeof(feature));
#endif
	feature[FEATURE_COMMAND] = HID_CARDRDR_CMD_ENABLE;
    return SetFeature(feature);
}

DWORD CCardReaderLayer::Disable()
{
	UCHAR feature[HID_FEATURE_DATAB_LOCK_SIZE];
#ifdef MCR_LOGGING
	memset(feature, LOGGING_DIAG_VALUE, sizeof(feature));
#endif
	feature[FEATURE_COMMAND] = HID_CARDRDR_CMD_DISABLE;
    return SetFeature(feature);
}

bool CCardReaderLayer::ReadCardData(CardDataTrackType track)
{
	UCHAR feature[HID_FEATURE_DATAB_LOCK_SIZE];
#ifdef MCR_LOGGING
	memset(feature, LOGGING_DIAG_VALUE, sizeof(feature));
#endif
	feature[FEATURE_COMMAND] = HID_CARDRDR_CMD_READ_CARD_DATA;
	switch (track)
	{
	case CARD_DATA_RFID:
		feature[FEATURE_DATA] = TRACK_RFID;
		break;
	case CARD_DATA_TRACK1:
		feature[FEATURE_DATA] = TRACK_1;
		break;
	case CARD_DATA_TRACK2:
		feature[FEATURE_DATA] = TRACK_2;
		break;
	default:
		feature[FEATURE_DATA] = TRACK_2;
		break;
	}
    return SetFeature(feature);
}

bool CCardReaderLayer::ClearBuffer()
{
	UCHAR feature[HID_FEATURE_DATAB_LOCK_SIZE];
#ifdef MCR_LOGGING
	memset(feature, LOGGING_DIAG_VALUE, sizeof(feature));
#endif
	feature[FEATURE_COMMAND] = HID_CARDRDR_CMD_CLEAR_BUFFER;
	feature[FEATURE_DATA] = (1<<0) | (1<<1);   // track (bitmask)
    return SetFeature(feature);
}

bool CCardReaderLayer::SetBezelStyle(CARD_STYLE style, DWORD rankId)
{
	lock_guard<recursive_mutex> cs(m_cardReaderConfigCriticalSection);

	const CardReaderColor* pColor = nullptr;

	if(!m_cardReaderConfig)
	{
		LogString(ERROR_LOG, "MCR Error: Card Reader Config NULL.");
		return false;
	}

	switch (style)
	{
		default:
		case CARD_REMOVED:
			pColor = m_cardReaderConfig->GetCardTypeColor(CardReaderCardType::None);
			break;

		case CARD_STYLE_PLAYER:
			pColor = m_cardReaderConfig->GetRankColor(rankId);
			if (pColor == nullptr)
			{
				pColor = m_cardReaderConfig->GetCardTypeColor(CardReaderCardType::Player);
			}
			break;

		case CARD_STYLE_EMPLOYEE:
			pColor = m_cardReaderConfig->GetCardTypeColor(CardReaderCardType::Employee);
			break;

		case CARD_BAD_READ: 
			pColor = m_cardReaderConfig->GetCardTypeColor(CardReaderCardType::BadRead);
			break;

		case CARD_STYLE_SOCIAL_DISTANCE:
			pColor = m_cardReaderConfig->GetCardTypeColor(CardReaderCardType::SocialDistance);
			break;

		case CARD_STYLE_CLEANING_IN_PROGRESS:
			pColor = m_cardReaderConfig->GetCardTypeColor(CardReaderCardType::CleaningInProgress);
			break;				
	}

	UCHAR feature[HID_FEATURE_DATAB_LOCK_SIZE];
#ifdef MCR_LOGGING
	memset(feature, LOGGING_DIAG_VALUE, sizeof(feature));
#endif
	feature[0] = HID_CARDRDR_CMD_LIGHT_CONTROL;
	if (pColor != nullptr)
	{
		memcpy(&feature[1], pColor, CardReaderColor::HidDataLen);
	}
	else
	{
		ZeroMemory(&feature[1], CardReaderColor::HidDataLen);
	}

	CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
	
	return SetFeature(feature);
}

shared_ptr<CardReaderConfig> CCardReaderLayer::GetCardReaderConfig() const
{
	lock_guard<recursive_mutex> cs(m_cardReaderConfigCriticalSection);
	return m_cardReaderConfig;
}

