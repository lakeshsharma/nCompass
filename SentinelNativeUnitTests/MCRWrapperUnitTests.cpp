#if 000//TODO-PORT
#include "stdafx.h"
#include "Sentinel.h"
#include "ProcessDump\WebDiagnostics.h"
#include "Hardware\MCRWrapper.h"
#include "Hardware\CardReaderTestHelpers.h"
#include "..\CardReaderLayerDll\CardReaderSimulator.h"
#include "UnitTestingUtils.h"
#include "TestUtil.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

const int WAITTIME = 30000;
const int MCRSLEEP = 1000;

namespace SentinelNativeUnitTests
{
	UINT AFX_CDECL ReconnectControl( PVOID threadParam );
	UINT AFX_CDECL ReaderLoop( PVOID threadParam );
	UINT AFX_CDECL CardLoop( PVOID threadParam );

	typedef struct  {
		HANDLE event1;
		HANDLE event2;
		HANDLE event3;
	} EventsType;

	typedef struct
	{
		CConfig* config;
		CMCRWrapper *mcr;
	} MCRWrapperAndConfigType;

	EventsType events;

	TEST_CLASS(MCRWrapperUnitTests)
	{
		static const LPCWSTR TESTDIR;

		CConfig* m_config;

	public:

		void SetupCardReader(CCardReaderLayer& cardReaderLayer)
		{
			CCardReaderSimulator::Instance().SetConnected(USB_VENDOR_ID_UIC, true);
			bool found = cardReaderLayer.FindCardReader();
			Assert::IsTrue(found, L"Unable to find card reader");
		}
		
		TEST_CLASS_INITIALIZE(MCRWrapperInit)
		{
			events.event1 = CreateEvent(NULL, false, false, NULL );
			events.event2 = CreateEvent(NULL, false, false, NULL );
			events.event3 = CreateEvent(NULL, false, false, NULL);
		}

		TEST_CLASS_CLEANUP(MCRWrappeCleanup)
		{
			// Delete events
			CloseHandle(events.event1);
			CloseHandle(events.event2);
			CloseHandle(events.event3);

			CCardReaderSimulator::CleanupInstance();
		}

		TEST_METHOD_INITIALIZE(MCRWraperInitialize)
		{
			CUnitTestingUtils::SetNvramInvalid();
			m_config = new CConfig;
			m_config->BuildYourself();
			m_config->SetNclCardsEnabled(false);
			m_config->SetRcclCardsEnabled(false);
			m_config->SetUniverseEnabled(false);

			CString testDir;
			CUnitTestingUtils::GetTestDir(TESTDIR, testDir);

			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.Start(testDir);
		}

		TEST_METHOD_CLEANUP(MCRWrapperCleanup)
		{
			CCardReaderSimulator::Instance().End();

			m_config->SetNclCardsEnabled(false);
			m_config->SetRcclCardsEnabled(false);
			m_config->SetUniverseEnabled(false);
			delete m_config;
			m_config = NULL;
		}

		TEST_METHOD(MCRWrapperWebDiagnostics)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			bool connected = false;

			// UIC connected
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();
			unique_ptr<CMCRWrapper> reader(new CMCRWrapper(*m_config));
			connected = reader->IsCardReaderReady();

			bool hasSpeedMediaInfo(false);
			const CSpeedMediaInfo speedMediaInfo;
			LPCTSTR speedMediaInfoCacheList(_T(""));
			const CWebDiagnosticsCommonInfo commonInfo(CTime(2014, 1, 27, 10, 11, 12),
				CTime(2014, 1, 27, 13, 14, 15),
				941,
				5141,
				_T("1.12.2.0"),
				_T("1.21.0"),
				hasSpeedMediaInfo,
				speedMediaInfo,
				speedMediaInfoCacheList,
				161028 * 1024,
				0,
				8);

			//fstream os("C:\\Temp\\~generated.html", ios_base::out | ios_base::binary | ios_base::trunc);
			stringstream os;
			CWebDiagnostics::SaveCardReaderData(os, commonInfo, reader.get());
			//os.close();

			//string generatedHtml(os.str());
			//Assert::AreEqual(expectedHtml.c_str(), generatedHtml.c_str());

			reader->ShutdownCardReader();
			Sleep(MCRSLEEP);
		}

		
		TEST_METHOD(MCRWrapperOpenTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			bool connected = false;

			// UIC connected
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();
			CMCRWrapper *reader = new CMCRWrapper(*m_config);
			connected = reader->IsCardReaderReady();
			reader->ShutdownCardReader();
			Sleep(MCRSLEEP);
			delete reader;
			Assert::IsTrue( connected,  L"UIC connected error" );

			//UIC disconnected
			simulator.SetConnected(USB_VENDOR_ID_UIC, false);
			reader = new CMCRWrapper(*m_config);
			connected = reader->IsCardReaderReady();
			reader->ShutdownCardReader();
			Sleep(MCRSLEEP);
			delete reader;
			Assert::IsFalse( connected,  L"UIC disconnected error" );

			// XST connected
			simulator.SetConnected(USB_VENDOR_ID_XST, true);
			simulator.PushEvtStatusRemoved();
			reader = new CMCRWrapper(*m_config);
			connected = reader->IsCardReaderReady();
			reader->ShutdownCardReader();
			Sleep(MCRSLEEP);
			delete reader;
			Assert::IsTrue( connected,  L"XST connected error" );

			// XST disconnected
			simulator.SetConnected(USB_VENDOR_ID_XST, false);
			reader = new CMCRWrapper(*m_config);
			connected = reader->IsCardReaderReady();
			reader->ShutdownCardReader();
			Sleep(MCRSLEEP);
			delete reader;
			Assert::IsFalse( connected, L"XST disconnected error" );
		}

	//	TEST_METHOD(MCRWrapperReconnectTest)
	//	{
	//		CWinThread * controlLoopThrd;
	//		CWinThread *CardLoopThrd;
	//		CWinThread * controlThrd;

	//		CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

	//		CString testDir;
	//		CUnitTestingUtils::GetTestDir(TESTDIR, testDir);
	//		simulator.Start(testDir);

	//		// UIC connected
	//		simulator.SetConnected(USB_VENDOR_ID_UIC, true);
	//		simulator.PushEvtStatusRemoved();
	//		CMCRWrapper *reader = new CMCRWrapper(*m_config);
	//		bool connected = reader->IsCardReaderReady();
	//		Assert::IsTrue( connected,  L"UIC connected error" );
	//		
	//		MCRWrapperAndConfigType mcrAndConfig;
	//		mcrAndConfig.config = m_config;
	//		mcrAndConfig.mcr = reader;

	//		// Start reader loop
	//		controlLoopThrd = AfxBeginThread(ReaderLoop, &mcrAndConfig);

	//		// Start card loop that keeps looking for insert
	//		CardLoopThrd = AfxBeginThread(CardLoop, &mcrAndConfig);

	//		// Start TeconnectTesteControlThread
	//		// 1. Disconnect UIC
	//		// 2. Wait for signal
	//		// 3. Connect UIC
	//		// 4. Wait for signal
	//		// 5. Disconnect UIC
	//		// 6. Wait for signal
	//		// 7. Connect XST
	//		controlThrd = AfxBeginThread(ReconnectControl, &events);

	//		// Wait for control thread to reconnect card reader
	//		WaitForSingleObject(events.event2, WAITTIME);
	//		ResetEvent(events.event2);

	//		Assert::IsTrue( reader->IsCardReaderReady(),  L"UIC Card reader did not reconnect" );

	//		// Signal control thread for next step
	//		SetEvent(events.event1);
	//		WaitForSingleObject(events.event2, WAITTIME);
	//		ResetEvent(events.event2);

	//		Assert::IsFalse( reader->IsCardReaderReady(),  L"UIC Card reader still connected" );

	//		SetEvent(events.event1);
	//		WaitForSingleObject(events.event3, WAITTIME);
	//		Assert::IsTrue( reader->IsCardReaderReady(),  L"XST Card reader did not reconnect" );

	//		reader->ShutdownCardReader();
	//	}

	//	
	};

	//UINT AFX_CDECL ReconnectControl( PVOID threadParam )
	//{
	//	CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
	//	EventsType *events = (EventsType *)threadParam;

	//	simulator.SetConnected( USB_VENDOR_ID_UIC, true);
	//	simulator.PushEvtStatusRemoved();

	//	// Signal test for next step
	//	SetEvent( events->event2 );

	//	WaitForSingleObject(events->event1, WAITTIME);
	//	ResetEvent(events->event1);

	//	simulator.SetConnected( USB_VENDOR_ID_UIC, false);
	//	Sleep(MCRSLEEP);

	//	SetEvent( events->event2 );

	//	WaitForSingleObject(events->event1, WAITTIME);

	//	simulator.SetConnected(USB_VENDOR_ID_XST, true);
	//	simulator.PushEvtStatusRemoved();
	//	Sleep(MCRSLEEP*3);

	//	SetEvent(events->event3);

	//	return 0;

	//}

	// Need to run CardReaderProcessingLoop to cause state changes
	UINT AFX_CDECL ReaderLoop( PVOID threadParam )
	{
		MCRWrapperAndConfigType *data = (MCRWrapperAndConfigType *)threadParam;
		CMCRWrapper *mcr = data->mcr;

		mcr->CardReaderProcessingLoop(data->config);
		
		return 0;
	}

	UINT AFX_CDECL CardLoop( PVOID threadParam )
	{
		// The purpose of this thread is to loop indefinitely waiting for card insert, necessary to keep prompting mcrwrapper to read device
		MCRWrapperAndConfigType *data = (MCRWrapperAndConfigType *)threadParam;
		
		CMCRWrapper *mcr = data->mcr;

		bool bTimeout = true;
		bool bIsCardInserted = false;
		bool bInserted = false;
		bool bRemoved = false;

		while (bTimeout && !bIsCardInserted)
			mcr->McrWaitForCardEvent(bInserted, bRemoved, bTimeout, bIsCardInserted);
		
		return 0;
	}

	const LPCWSTR MCRWrapperUnitTests::TESTDIR = L"Test_MCRWrapper";
}

#endif //TODO-PORT