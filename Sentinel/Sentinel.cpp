#include "stdafx.h"
#include "Sentinel.h"

// Sentinel includes.
#include "Logging/Logger.h"
#include "Logging/LogString.h"
#include "SASProtocol/SASProtocol.h"
#include "PollerProtocol/PollerProtocol.h"
#include "Hardware/WatchDog.h"
#include "UI/ScreenDefs.h"
#include "GlobalDefs.h"
#include "SaveTestTransactions.h"
#include "ProgressiveProtocol/ProgressiveProtocol.h"
#include "Platform.h"
#include "Diagnostics.h"
#include "PollerProtocol/MobileConnectProtocol/RadBeacon.h"
#include "Utilities.h"
#include <activemq/library/ActiveMQCPP.h> 

using namespace std;

// Early comment
//int main(int argc, char *argv[])
int main()
{
#ifdef LEAK_CHECK
	InitMemoryLeakChecking();
#endif

    binsem::AllocateBinSemList();
	CSentinelApp app;

	return 0;
}

CSentinelApp::CSentinelApp()
{
	m_internals = NULL;
	m_egmProtocol = NULL;
	m_systemProtocol = NULL;
	m_progressiveProtocol = NULL;
	m_uiProtocol = nullptr;

	m_endCalibrationTask = false;

	InitInstance();
}

// The one and only CSentinelApp object

BOOL CSentinelApp::InitInstance()
{
	if (CUtilities::FileExists("/sdboot/firmware/layers/layer-0.squashfs") &&
		CUtilities::FileExists("/sdboot/firmware/layers/layer-1.squashfs") &&
		CUtilities::FileExists("/sdboot/firmware/layers/layer-2.squashfs") &&
		CUtilities::FileExists("/sdboot/firmware/layers/layer-3.squashfs") && 
		CUtilities::FileExists("/sdboot/firmware/layers/layer-4.squashfs") &&
		CUtilities::FileExists("/sdboot/firmware/layers/layer-000.squashfs") &&
		CUtilities::FileExists("/sdboot/firmware/layers/layer-001.squashfs") &&
		CUtilities::FileExists("/sdboot/firmware/layers/layer-002.squashfs") &&
		CUtilities::FileExists("/sdboot/firmware/layers/layer-003.squashfs") &&
		CUtilities::FileExists("/sdboot/firmware/layers/layer-004.squashfs"))
	{
		CUtilities::DeleteFile("/sdboot/firmware/layers/layer-0.squashfs");
		CUtilities::DeleteFile("/sdboot/firmware/layers/layer-1.squashfs");
		CUtilities::DeleteFile("/sdboot/firmware/layers/layer-2.squashfs");
		CUtilities::DeleteFile("/sdboot/firmware/layers/layer-3.squashfs");
		CUtilities::DeleteFile("/sdboot/firmware/layers/layer-4.squashfs");
		system("cd /sdboot/firmware && find . -type f -not -path ./manifest.sha1 -exec sha1sum '{}' + | sort > manifest.sha1");
		system("/sbin/reboot");
	}

   string firmwareVersionStr;
	Start1(firmwareVersionStr);
	if (Start2())
	{
		Start3();
		Start4(firmwareVersionStr);
		
		// Delete .undocount and undo folder if it exists (These are temporary, will eventually be a script);
		unlink("/sdboot/.undocount");
		CUtilities::DeleteDirectory(CDirectoryManager::Instance().GetUpgradeUndoPath());

		m_internals->CalcImageChecksum();

		m_uiProtocol->RestartServiceWindow(false, true);

		Main();
		Shutdown();
	}

	return FALSE;
}

void InstallSignalHandlers()
{
	signal(SIGABRT, GlobalSignalHandler);
	signal(SIGBUS, GlobalSignalHandler);   
	signal(SIGFPE, GlobalSignalHandler);   
	signal(SIGILL, GlobalSignalHandler);  

	//SIGPIPE happens when e.g. the SpeedMedia cache closes a connection in the middle of a write.
	//When that happens, we don't want a SIGPIPE ("Broken Pipe") signal to be raised but rather
	//have the socket call fail and the failure bubble up through the normal call hierarchy.
	//So we set the signal to be ignored here.
	
	//signal(SIGPIPE, GlobalSignalHandler);   
    signal(SIGPIPE, SIG_IGN);

	signal(SIGSEGV, GlobalSignalHandler);  
	signal(SIGSTKFLT, GlobalSignalHandler);   
	signal(SIGSYS, GlobalSignalHandler);   
	signal(SIGTERM, GlobalSignalHandler);   
	signal(SIGTSTP, GlobalSignalHandler);   
	signal(SIGURG, GlobalSignalHandler);   
	signal(SIGXFSZ, GlobalSignalHandler);   
}

/// <summary>
/// Starts the first part of the Sentinel application instance.
/// </summary>
/// <param name="firmwareVersionString">The firmware version string.</param>
void CSentinelApp::Start1(string& firmwareVersionString)
{
	S7_Result s7result = S7LITE_DLL_Init();
	
	// Wait for OS to be ready before starting Sentinel;
	CPlatform::Instance().WaitForNeededServices();

	// This will wait for all the drives to become available before we do anything else; 
	CDirectoryManager::Instance();

#if 1//TODO-PORT - When should logger be initialized and should we allow time to detect a debug.txt file?

	//Set the time offset from NVRAM before instantiating logger 
	//so filename of the logger  with timestamp will be correct.
	CNVRAMConfig nvramConfig;
	short timezoneOffset;
	nvramConfig.Get(TIME_ZONE_OFFSET, (BYTE*)&timezoneOffset, sizeof(timezoneOffset));
	if (CUtilities::IsTimezoneValid(timezoneOffset))
	{
		CUtilities::SetTzOffset(timezoneOffset);
	}

	// Enable logging based on REMOTE_DEBUG file's presence or the configuration stored on NVRAM
	bool remoteLoggingEnabled;
	nvramConfig.Get(REMOTE_DEBUGGING_ENABLED_OFFSET, (BYTE*)&remoteLoggingEnabled, sizeof(remoteLoggingEnabled));
	if (!remoteLoggingEnabled)
	{
		remoteLoggingEnabled = CUtilities::FileExists(CDirectoryManager::REMOTE_DEBUG_FILE);
	}

	CEEPROMConfig eepromConfig;
	int slotMastID(0);
	eepromConfig.Get(EEPROM_SLOTMAST_ID_OFFSET, (BYTE *)&slotMastID, sizeof(slotMastID));
	CSmartDataLogger::Instance().SetSlotMastId(slotMastID);
	CSmartDataLogger::Instance().SetRemoteLoggingEnabledFlag(remoteLoggingEnabled);
	CheckSystemTimeAndInitLogger(nvramConfig);

	// The following must occur after CSmartDataLogger has been instantiated since the handlers will
	// attempt to log, which could cause multiple logger instances to be created with strange results.
	InstallSignalHandlers(); 

	LogString(GENERAL, _T("StartupTime=%s"), CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime()).c_str());
	
#endif//TODO-PORT

	LogS5DLLStatus(s7result, "S7_DLL_Init");
	
	std::size_t firmwareVersionSize(512);
	unique_ptr<char> firmwareVersionBuffer(new char[firmwareVersionSize]);
	s7result = S7LITE_Firmware_Version(firmwareVersionBuffer.get(), &firmwareVersionSize);
	firmwareVersionString = string(firmwareVersionBuffer.get());
	LogS5DLLStatus(s7result, "S7LITE_Firmware_Version");
	LogString(GENERAL, _T("Firmware_Version %s"), firmwareVersionString.c_str());

#ifdef TEST_SAVED_TRANSACTIONS
	CSaveTestTransactions::SaveTestTransactions();

	// For SIT testing, provide a Sentinel.exe version that reboots after saving the transactions.
	// For development, disable the following lines to avoid rebooting after the transactions are saved.
	S5_FPGA_SysReset(1);
	S5_FPGA_SysReset(2);
	exit(0); // Shouldn't get here.
	AfxMessageBox(L"Test Transactions saved."); // Shouldn't get here either.
#endif // TEST_SAVED_TRANSACTIONS.

	// Register the main application thread with the Watchdog.
    CWatchDog::Instance().RegisterThread(THREAD_MAIN, CUtilities::GetCurrentThreadId(), APP_MAIN_THREAD_TIMEOUT, pthread_self());
}

BOOL CSentinelApp::Start2()
{
	BOOL success(TRUE);

	return success;
}

void CSentinelApp::Start3()
{
    CMemoryStorage::UpgradeNVRAM();
    CWatchDog::Instance().Checkin();
}

void CSentinelApp::Start4(const std::string& firmwareVersion)
{
	LogString(GENERAL, "Inside Start4()");

	// Determine if we are to run in tft mode or media window mode
	bool isTFTMode = CDisplayManager::IsHardwareTFT();

	CPlatform::Instance().GetPlatformName();

	// Start internals
	time_t startupTime = CUtilities::GetCurrentTime(); // Don't get startup time too early.
	
	cout << "Start4 " << CUtilities::FormatDateTimeString(startupTime, "%m/%d/%Y %I:%M:%S %p") << " sizeOfSYSTEMTIME=" << sizeof(SYSTEMTIME) << std::endl;

	m_internals = new CInternalsFramework(this, startupTime, isTFTMode, firmwareVersion);
        //Moved Start function to start various task hndlers out of constructor. 
        //Helpful in handling test scenarios who dont need to start too many handlers
	m_internals->Start();
	CWatchDog::Instance().Checkin();

	// Start GCF
	LogString(GENERAL, "Calling CSentinelApp::StartGCF()");
	StartGCF();
	CWatchDog::Instance().Checkin();

	// Start SCF
	m_systemProtocol = new CPollerProtocol(m_internals->GetMobilePairingInformation(), *m_internals->GetSCFQueue());
	m_systemProtocol->Start();
	CWatchDog::Instance().Checkin();

	// all protocols using ActiveMQ below this. 
	activemq::library::ActiveMQCPP::initializeLibrary();

	// Start Progresive Protocol
	m_progressiveProtocol = new CProgressiveProtocol(
		*m_internals->GetProgressiveQueue(), firmwareVersion);
	m_progressiveProtocol->Init();
	m_progressiveProtocol->Start();
    CWatchDog::Instance().Checkin();

	cout << "new CUIProtocol\n";
	m_uiProtocol = new CUIProtocol(m_internals->GetUIQueue());
	m_internals->SetUiStateObject(m_uiProtocol->GetUiStateObject());
	CWatchDog::Instance().Checkin();

	cout << "SendStartupMessageToUI\n";
	m_internals->SendStartupMessageToUI();
	CWatchDog::Instance().Checkin();


	cout << "InitialzePersisentObjects\n";
	m_internals->InitialzePersisentObjects();
	CWatchDog::Instance().Checkin();


	cout << "StartProcessingCards\n";
	m_internals->StartProcessingCards();
	CWatchDog::Instance().Checkin();

	cout << "ProcessInitialPersonalBanker\n";
	//// process any of the personal banker stuff that needs to be sent.
	m_internals->ProcessInitialPersonalBanker();
	CWatchDog::Instance().Checkin();

	cout << "SetSprinkles\n";
	CMemoryStorage::SetSprinkles();
}

void CSentinelApp::Main()
{
	CWatchDog::Instance().Checkin();

	while( !CWatchDog::Instance().IsTimeToShutdown() )
	{
		CUtilities::Sleep(100);

		CWatchDog::Instance().Checkin();
	}

}

void CSentinelApp::Shutdown()
{
	#ifdef LEAK_CHECK
	cout << "at beginning of shutdown, live pointers: " << live_pointers << " malloc_seq:" << malloc_seq << "\n";
	#endif
	  
	if (m_internals != nullptr)
	{
		CDisplayManager::Instance().PrepareDisplayForShutdown();
	}

	if (m_systemProtocol != NULL)
	{
		m_systemProtocol->LowerTimeoutsForShutdown();
	}

	// Delete the InternalsFramework... it calls the destruction of the scf gcf and ui
	delete m_internals;

	delete m_systemProtocol;

    delete m_uiProtocol;

	delete m_progressiveProtocol;

	CWatchDog::Instance().Checkin();

	CSmartDataLogger::Instance().FlushToNVRAM();
	CSmartDataLogger::Instance().FlushAllBuffers();

	// We have shutdown without timing out. Tell the watchdog to shutdown now.
	CWatchDog::CleanupInstance();

#ifdef LEAK_CHECK
    CDisplayManager::CleanupInstance();
	CWatchDog::CleanupInstance();
	CSmartDataLogger::CleanupInstance();
	CPlatform::CleanupInstance();
    CDiagnostics::CleanupInstance();
    binsem::DisposeBinSemList();

	//will cause a crash inside decaf at the moment, if uncommented:	
	//activemq::library::ActiveMQCPP::shutdownLibrary();

	cout << "after shut-down, live pointers: " << live_pointers << " malloc_seq:" << malloc_seq << "\n";

	ShowMemoryLeakInfo();
#endif

	activemq::library::ActiveMQCPP::shutdownLibrary();

}

bool CSentinelApp::StartGCF(void)
{
	bool bProtocolStarted = false;

	if ( m_egmProtocol == NULL )
	{
		switch ( m_internals->GetEGMProtocolType() )
		{
		case IGT_SAS_3xx:
		case SAS5xx:
		case SAS6xx:
			m_egmProtocol = new CSASProtocol(*m_internals->GetGCFQueue());
			m_egmProtocol->Init();
			m_egmProtocol->Start();

			break;
		default:
			m_egmProtocol = NULL;
			break;
		}
		bProtocolStarted = (m_egmProtocol != NULL );
	}

	return bProtocolStarted;
}


void CSentinelApp::StartProtocol(ProtocolType protocolType)
{
	switch (protocolType)
	{
	case PollerProtocol:
	   break;
	case EGMProtocol:
	   StartGCF();
	   break;
	case ProgressiveProtocol:
	   break;
	case UIProtocol:
	   break;
	default:
	   break;
	}
	
}

CQueueList * CSentinelApp::GetInternalsToGCFQueue(void)
{
	CQueueList *retVal = nullptr;
	if ( m_egmProtocol != nullptr )
	{
		retVal = m_egmProtocol->GetInternalsQueue();
	}
	return retVal;
}
CQueueList * CSentinelApp::GetInternalsToSCFQueue( void )
{
	CQueueList *retVal = nullptr;
	if ( m_systemProtocol != nullptr )
		retVal = m_systemProtocol->GetInternalsQueue();
	return retVal;
}

CQueueList * CSentinelApp::GetInternalsToProgressiveQueue(void)
{
	CQueueList *retVal = nullptr;
	if (m_progressiveProtocol != nullptr)
		retVal = m_progressiveProtocol->GetInternalsQueue();
	return retVal;
}

CInternalMessageQueueList * CSentinelApp::GetInternalsToUIQueue( void )
{
	CInternalMessageQueueList *retVal = nullptr;
	if (m_uiProtocol != nullptr)
		retVal = m_uiProtocol->GetInternalsQueue();
	return retVal;
}

void CSentinelApp::SendMessageToProtocol(ProtocolType protocolType, CInternalMessage *msg, int priorityLevel)
{
	if (msg != nullptr)
	{
		CQueueList *protocolInboundQueue = GetInboundQueueForProtocol(protocolType);
		if (protocolInboundQueue != nullptr)
		{
			protocolInboundQueue->Add(msg, priorityLevel);
		}
		else
		{
			delete msg;
		}
    }
}

CQueueList* CSentinelApp::GetInboundQueueForProtocol(ProtocolType protocolType)
{
	CQueueList *protocolInboundQueue = nullptr;
	
	switch (protocolType)
	{
	case PollerProtocol:
	   protocolInboundQueue = GetInternalsToSCFQueue();
	   break;
	case EGMProtocol:
	   protocolInboundQueue = GetInternalsToGCFQueue();
	   break;
	case ProgressiveProtocol:
	   protocolInboundQueue = GetInternalsToProgressiveQueue();
	   break;
	case UIProtocol:
	   protocolInboundQueue = GetInternalsToUIQueue();
	   break;
	default:
	   protocolInboundQueue = nullptr;
	   break;
	}
	
	return protocolInboundQueue;
}

void CSentinelApp::RemoveProtocol(ProtocolType protocolType) 
{
	
	switch (protocolType)
	{
	case PollerProtocol:
	  RemoveSCFProtocol();
	  break;
	case EGMProtocol:
	  RemoveGCFProtocol();
	  break;
	case ProgressiveProtocol:
	  RemoveProgressiveProtocol();
	  break;
	case UIProtocol:
	  RemoveUIProtocol();
	  break;
	  break;
	default:
	  break;
	}
}

void CSentinelApp::WaitForProtocol(ProtocolType protocolType)
{
	while (GetInboundQueueForProtocol(protocolType) == nullptr)
	{
		CWatchDog::Instance().Checkin();
		CUtilities::Sleep(100);
	}
}

void CSentinelApp::ClearInboundQueueStarvation(ProtocolType protocolType, MessagePersistType persistType)
{
	CQueueList *protocolInboundQueue = GetInboundQueueForProtocol(protocolType);
	if (protocolInboundQueue != nullptr)
	{
		protocolInboundQueue->ClearQueueStarvation(persistType);
	}
	else
	{
		LogString(ERROR_LOG, "ClearQueueStarvation for persist type %d was not called because protocol %d was not initialized.", persistType, protocolType);
	}
}

bool CSentinelApp::IsQueueStarved(ProtocolType protocolType, MessagePersistType persistType)
{
    bool retVal = false;
    if (protocolType == UIProtocol)
	{
		CInternalMessageQueueList *uiInboundQueue = GetInternalsToUIQueue();
		
		if (uiInboundQueue != nullptr)
		{
			bool queueIsCleared(false);
			retVal = uiInboundQueue->IsQueueStarved(persistType, queueIsCleared);
		}
		else
		{
			LogString(ERROR_LOG, "IsQueueStarved not called because protocol %d was not initialized.", protocolType);
		}
	}
	else
	{
		LogString(ERROR_LOG, "IsQueueStarved is not supported with protocol %d", protocolType);
	}

    return retVal;
}

bool CSentinelApp::IsProtocolInitialized(ProtocolType protocolType)
{
    bool retVal = false;
    if (protocolType == UIProtocol)
	{
		CInternalMessageQueueList *uiInboundQueue = GetInternalsToUIQueue();
		
		if (uiInboundQueue != nullptr)
		{
            retVal = true;
		}
		else
		{
			LogString(ERROR_LOG, "IsProtocolInitialized protocol %d was not initialized.", protocolType);
		}
	}
	else
	{
		LogString(ERROR_LOG, "IsProtocolInitialized is not supported with protocol %d", protocolType);
	}

    return retVal;
}

void CSentinelApp::FlushAllPersistedButTypeFromInboundQueue(ProtocolType protocolType, int flushTypeIDToNotFlush)
{
    if (protocolType == UIProtocol)
	{
		CInternalMessageQueueList *uiInboundQueue = GetInternalsToUIQueue();
		
		if (uiInboundQueue != nullptr)
		{
            uiInboundQueue->FlushAllPersistedButType(flushTypeIDToNotFlush);
		}
		else
		{
			LogString(ERROR_LOG, "FlushAllPersistedButTypeFromInboundQueue: Protocol %d was not initialized.", protocolType);
		}
	}
	else
	{
		LogString(ERROR_LOG, "FlushAllPersistedButTypeFromInboundQueue is not supported with protocol %d", protocolType);
	}
}

void CSentinelApp::SetInboundQueueEqualPriorityInterrupt(ProtocolType protocolType, bool isEqualPriorityInterruptible)
{
	if (protocolType == UIProtocol)
	{
		CInternalMessageQueueList *uiInboundQueue = GetInternalsToUIQueue();
		
		if (uiInboundQueue != nullptr)
		{
			uiInboundQueue->SetInterruptible(isEqualPriorityInterruptible);
		}
		else
		{
			LogString(ERROR_LOG, "SetInterruptible not called because protocol %d was not initialized.", protocolType);
		}
	}
	else
	{
		LogString(ERROR_LOG, "SetInterruptible is not supported with protocol %d", protocolType);
	}
}

bool CSentinelApp::InboundQueueForProtocolIsReady(ProtocolType protocolType)
{
	return GetInboundQueueForProtocol(protocolType) != nullptr;
}

void CSentinelApp::RemoveGCFProtocol( void )
{
   CSystemProtocol *egmProtocol = m_egmProtocol;
   
   m_egmProtocol = NULL;
   
   // This destructor should block until all threads are closed in the protocol.
   delete egmProtocol;
}

void CSentinelApp::RemoveSCFProtocol( void )
{
   CSystemProtocol *systemProtocol = m_systemProtocol;
   
   m_systemProtocol = NULL;
   
   if (systemProtocol != NULL)
   {
	   delete systemProtocol;
   }
}

void CSentinelApp::RemoveUIProtocol( void )
{
	if (m_uiProtocol != nullptr)
	{
		delete m_uiProtocol;
	   m_uiProtocol = nullptr;
	}
}

void CSentinelApp::RemoveProgressiveProtocol(void)
{
	CSystemProtocol *progressiveProtocol = m_progressiveProtocol;

   m_progressiveProtocol = NULL;

    if (progressiveProtocol != NULL) 
    {
       delete progressiveProtocol;
    }
}

template<typename T, typename... Targs>
void CSentinelApp::LogS5String(std::stringstream ss, const char* format, T value, Targs... Fargs)
{
	for ( ; *format != '\0'; format++) {
		if (*format == '%') {
			ss << value;
			LogS5String(format+1, Fargs...);
			return;
		}
		ss << *format;
	}
}

void CSentinelApp::LogS5String(std::stringstream ss, const char* format)
{
	ss << format;
}

// Tianon talked to Tedd (2019-05-15):
//   Some systems come up with a system time back in 1600 (or earlier) that when cast to a Unix timestamp overflow and cause issues.
//   This function checks for that and sets them statically to something more reasonable that can cast to a Unix timestamp properly and allow us to talk to OneLink and set the clock properly.
void CSentinelApp::CheckSystemTimeAndInitLogger(CNVRAMConfig& nvramConfig)
{
	bool loggerInitialized(false);
	bool setTimeAPIfailed(false);
	if (CPlatform::Instance().IsLiteDevice())
	{
		DWORD systemTimeSprinkle(0);
		SYSTEMTIME systemTimeFromNVRAM;
		nvramConfig.Get(SYSTEM_TIME_SPRINKLE_OFFSET, (BYTE*)&systemTimeSprinkle, sizeof(systemTimeSprinkle));
		nvramConfig.Get(SYSTEM_TIME_OFFSET, (BYTE*)&systemTimeFromNVRAM, sizeof(systemTimeFromNVRAM));
		SYSTEMTIME currentUTCSystemTime = CUtilities::GetUTCSystemTime(CUtilities::GetCurrentTime());
		cout << "currentUTCSystemTime = " << CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(currentUTCSystemTime) << endl;
		cout << "systemTimeFromNVRAM  = " << CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(systemTimeFromNVRAM) << endl;
		//Checks to see if sprinkle_value is there
		if (NVRAM_SYSTEM_TIME_SPRINKLE_VALUE == systemTimeSprinkle)
		{
			if (-1 == CUtilities::CompareSysTimes(currentUTCSystemTime, systemTimeFromNVRAM))
			{
				//This means the time moved back
				if (S7DLL_STATUS_OK == S7LITE_Rtc_SetTime(systemTimeFromNVRAM))
				{
					CSmartDataLogger::Instance().Initialize();
					loggerInitialized = true;
					LogString(GENERAL, _T("currentUTCSystemTime=%s systemTimeFromNVRAM=%s"),
						CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(currentUTCSystemTime).c_str(),
						CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(systemTimeFromNVRAM).c_str());
					LogString(GENERAL, "Current time is older than what is stored in NVRAM, Set real time clock to %s", CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(systemTimeFromNVRAM).c_str());
					LogString(GENERAL, "New Current Time is %s", CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime()).c_str());
				}
				else
				{
					setTimeAPIfailed = true;
				}
			}
			else if (1 == CUtilities::CompareSysTimes(currentUTCSystemTime, systemTimeFromNVRAM))
			{   //this means current system is greater than what is stored in NVRAM which is normal.
				//We will only use the time in NVRAM if the time difference is more than 60 minutes.
				int64_t differenceInmilliseconds(CUtilities::GetSystemTimespanInMilliseconds(systemTimeFromNVRAM, currentUTCSystemTime));
				if (differenceInmilliseconds > SIXTY_MINUTES_IN_MS)
				{
					if (S7DLL_STATUS_OK == S7LITE_Rtc_SetTime(systemTimeFromNVRAM))
					{
						CSmartDataLogger::Instance().Initialize();
						loggerInitialized = true;
						LogString(GENERAL, "Current system is greater than what is stored in NVRAM DifferenceInMinutes=%lld \
						Setting time to NVRAM stored time. New Current Time is %s",
							(differenceInmilliseconds / MILLISECONDS_PER_MINUTE),
							CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime()).c_str());
					}
					else
					{
						setTimeAPIfailed = true;
					}
				}
			}
		}
	}

	if (CUtilities::GetUTCTime() < LOCAL_CERT_TIME_ACTIVATION)
	{		
		if (S7DLL_STATUS_OK != S7LITE_Rtc_SetTime(CUtilities::GetUTCSystemTime(LOCAL_CERT_TIME_ACTIVATION)))
		{
			setTimeAPIfailed = true;
		}
	}

	if (!loggerInitialized)
	{
		CSmartDataLogger::Instance().Initialize();
		loggerInitialized = true;
	}

	if (setTimeAPIfailed)
	{
		LogString(ERROR_LOG, "CheckSystemTimeAndInitLogger-S7LITE_Rtc_SetTime failed");
	}
}
