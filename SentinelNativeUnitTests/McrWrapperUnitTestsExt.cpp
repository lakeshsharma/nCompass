#if 000//TODO-PORT

#include "stdafx.h"
#include "CppUnitTest.h"

#include "Hardware\MCRWrapper.h"
#include "..\CardReaderLayerDll\CardReaderSimulator.h"
#include "DirectoryManager.h"
#include "UnitTestingUtils.h"
#include "Hardware\CardReaderTestHelpers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(McrWrapperUnitTestsExt)
	{
		static const int WAITFORCRASH = 3000;
		static const int WAITFORLOOP = 3000;
		static const int SAFETY = 20000;

		static const LPCWSTR TESTDIR;

		struct ThreadData
		{
			enum Sentinel
			{
				RUNNING = 0,
				DONE,
				CRASHED
			};

			ThreadData(CConfig* _config, CMCRWrapper& _mcr) : config(_config), mcr(_mcr), sentinel(RUNNING) {}

			CConfig* config;
			CMCRWrapper& mcr;
			Sentinel sentinel;
		};

		// Need to run CardReaderProcessingLoop to cause state changes
		static UINT AFX_CDECL ReaderLoop(PVOID threadParam)
		{
			ThreadData *data = (ThreadData *)threadParam;

			m_cs.Lock(); // Make sure only one unit test is using the card processing loop
			try
			{
				data->mcr.CardReaderProcessingLoop(data->config);
				data->sentinel = ThreadData::DONE;
			}
			catch (...)
			{
				data->sentinel = ThreadData::CRASHED;
			}
			m_cs.Unlock();
		
			return 0;
		}

		ThreadData::Sentinel WaitForLoop(const ThreadData& threadData)
		{
			DWORD start = GetTickCount();
			DWORD safety = GetTickCount();

			while (ThreadData::RUNNING == threadData.sentinel && SAFETY > safety - start)
			{
				Sleep(1);
				safety = GetTickCount();
			}

			return threadData.sentinel;
		}

		static ::CCriticalSection m_cs;

		CConfig* m_config;

	public:

		void TestBezelColor(const CardReaderColor* pExpected)
		{
			const CardReaderColor* pColor = (const CardReaderColor*) CCardReaderSimulator::Instance().GetColors();
			Assert::IsTrue(pExpected->CompareHidData(*pColor), L"CardReaderColor");
		}

		McrWrapperUnitTestsExt()
		{
			m_config = NULL;
		}

		TEST_CLASS_CLEANUP(MCRWrapperClassCleanup)
		{
			CCardReaderSimulator::CleanupInstance();
		}

		TEST_METHOD_INITIALIZE(MCRWraperInitialize)
		{
			CDirectoryManager::MockInstance();
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
			
			CDirectoryManager::MockInstanceCleanup();
		}

		TEST_METHOD(McrWrapperWhenNotConnectedTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, false);

			CMCRWrapper mcr(*m_config);
			Assert::IsFalse(mcr.IsCardReaderReady(), L"Reports as ready when card reader is not connected.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when not connected.");

			// Make sure CardReaderProcessingLoop does not crash when not connected.
			ThreadData threadData(m_config, mcr);

			// Start reader loop
			CWinThread* thread = AfxBeginThread(ReaderLoop, &threadData);

			Sleep(WAITFORCRASH);

			mcr.ShutdownCardReader();

			if (ThreadData::RUNNING == WaitForLoop(threadData))
			{
				thread->ExitInstance();
			}

			Assert::IsFalse(ThreadData::CRASHED == threadData.sentinel, L"CardReaderProcessingLoop crashed when not connected.");
		}

		TEST_METHOD(McrWrapperWhenConnectedAndCardRemovedTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();

			CMCRWrapper mcr(*m_config);
			Assert::IsTrue(mcr.IsCardReaderReady(), L"MCRWrappert reports as not ready when card is removed.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when card is removed.");

			TestBezelColor(&CardReaderTestHelpers::colorNone);
		}

		TEST_METHOD(McrWrapperWhenConnectedAndCardInsertedTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			{
				simulator.PushEvtStatusInserted(true);
				CMCRWrapper mcr(*m_config);
				Assert::IsTrue(mcr.IsCardReaderReady(), L"Reports as not ready when card reader is inserted, 1st track.");
				Assert::IsTrue(mcr.IsCardInserted(), L"Reports card as not inserted when card is, 1st track.");
			}

			simulator.SetConnected(USB_VENDOR_ID_UIC, false);
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);

			{
				simulator.PushEvtStatusInserted(false);
				CMCRWrapper mcr(*m_config);
				Assert::IsTrue(mcr.IsCardReaderReady(), L"MCRWrappert reports as not ready when card reader is inserted, 2nd track.");
				Assert::IsTrue(mcr.IsCardInserted(), L"Reports card as not inserted when card is, 2nd track.");
			}
		}

		TEST_METHOD(McrWrapperWaitForNoCardTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();

			CMCRWrapper mcr(*m_config);
			Assert::IsTrue(mcr.IsCardReaderReady(), L"Reports as not ready when card reader.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when card when not.");

			ThreadData threadData(m_config, mcr);

			// Start reader loop
			CWinThread* thread = AfxBeginThread(ReaderLoop, &threadData);

			bool loopTimeout = false;
			bool cardInserted = false;
			bool bInserted = false;
			bool bRemoved = false;
			
			// Note that there is no PushEvtStatusInserted
			CCard* card = mcr.McrWaitForCardEvent(bInserted, bRemoved, loopTimeout, cardInserted);

			mcr.ShutdownCardReader();

			if (ThreadData::RUNNING == WaitForLoop(threadData))
			{
				thread->ExitInstance();
			}

			Assert::IsNull(card, L"Attempted to return a card when there is none.");
			Assert::IsTrue(loopTimeout, L"Card processing looped did not time out when no card.");
			Assert::IsFalse(cardInserted, L"Card reported as inserted when there is none.");
			Assert::IsFalse(bInserted);
			Assert::IsFalse(bRemoved);

			TestBezelColor(&CardReaderTestHelpers::colorNone);
		}

		TEST_METHOD(McrWrapperWaitForErrorTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtErrorData((UCHAR)ERROR_ERRORS_ENCOUNTERED);
			simulator.PushEvtStatusRemoved();

			CMCRWrapper mcr(*m_config);
			Assert::IsTrue(mcr.IsCardReaderReady(), L"Reports as not ready when card reader.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when card when not.");

			ThreadData threadData(m_config, mcr);

			// Start reader loop
			CWinThread* thread = AfxBeginThread(ReaderLoop, &threadData);

			bool loopTimeout = false;
			bool cardInserted = false;
			bool bInserted = false;
			bool bRemoved = false;

			// Note that there is no PushEvtStatusInserted
			CCard* card = mcr.McrWaitForCardEvent(bInserted, bRemoved, loopTimeout, cardInserted);

			mcr.ShutdownCardReader();

			if (ThreadData::RUNNING == WaitForLoop(threadData))
			{
				thread->ExitInstance();
			}

			Assert::IsNull(card, L"Attempted to return a card when there is none.");
			Assert::IsTrue(loopTimeout, L"Card processing looped did not time out when no card.");
			Assert::IsFalse(cardInserted, L"Card reported as inserted when there is none.");
			Assert::IsFalse(bInserted);
			Assert::IsFalse(bRemoved);
		}

		TEST_METHOD(McrWrapperWaitForInsert1stTrackTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();

			CMCRWrapper mcr(*m_config);
			Assert::IsTrue(mcr.IsCardReaderReady(), L"Reports as not ready when card reader.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when card when not.");

			ThreadData threadData(m_config, mcr);

			bool loopTimeout = true;
			bool cardInserted = false;

			UCHAR testData[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			// Start reader loop
			CWinThread* thread = AfxBeginThread(ReaderLoop, &threadData);

			// Insert card with 1st track
			simulator.PushEvtStatusInserted(true);
			simulator.PushEvtCardData(testData, _countof(testData), true);

			Sleep(WAITFORLOOP);

			DWORD start = GetTickCount();
			DWORD safety = GetTickCount();

			CCard* card = NULL;

			CTime cardInTimeLow = CUtilities::GetCurrentTime();
			CTime utcCardInTimeLow = CUtilities::GetUTCTime();
			bool bInserted = false;
			bool bRemoved = false;

			while (loopTimeout && !cardInserted && SAFETY > safety - start)
			{
				card = mcr.McrWaitForCardEvent(bInserted, bRemoved, loopTimeout, cardInserted);
				safety = GetTickCount();
			}

			CTime cardInTimeHigh = CUtilities::GetCurrentTime();
			CTime utcCardInTimeHigh = CUtilities::GetUTCTime();

			// End loop in case we fail
			mcr.ShutdownCardReader();

			if (ThreadData::RUNNING == WaitForLoop(threadData))
			{
				thread->ExitInstance();
			}

			Assert::IsTrue(cardInserted, L"Card reported as not inserted when there is.");
			Assert::IsNotNull(card, L"No card when there is.");

			CTime cardInTime = card->GetCardInTime();
			CTime utcCardInTime = card->GetUTCCardInTime();

			delete card;

			Assert::IsTrue(cardInTimeLow <= cardInTime, cardInTime.Format(L"Card In time too high, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(cardInTime <= cardInTimeHigh, cardInTime.Format(L"Card In time too low, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(utcCardInTimeLow <= utcCardInTime, utcCardInTime.Format(L"UTC Card In time too high, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(utcCardInTime <= utcCardInTimeHigh, utcCardInTime.Format(L"UTC Card In time too low, %m/%d/%Y %I:%M:%S %p"));

			Assert::IsFalse(loopTimeout, L"Card processing looped timed out when no card.");
		}

		TEST_METHOD(McrWrapperWaitForInsertNclTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();

			CMCRWrapper mcr(*m_config);
			Assert::IsTrue(mcr.IsCardReaderReady(), L"Reports as not ready when card reader.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when card when not.");

			m_config->SetNclCardsEnabled(true);

			ThreadData threadData(m_config, mcr);

			CCard* card = NULL;

			bool loopTimeout = true;
			bool cardInserted = false;

			UCHAR testData[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			// Start reader loop
			CWinThread* thread = AfxBeginThread(ReaderLoop, &threadData);

			// Insert card with 1st track
			simulator.PushEvtStatusInserted(true);
			simulator.PushEvtCardData(testData, _countof(testData), true);

			Sleep(WAITFORLOOP);

			DWORD start = GetTickCount();
			DWORD safety = GetTickCount();

			CTime cardInTimeLow = CUtilities::GetCurrentTime();
			CTime utcCardInTimeLow = CUtilities::GetUTCTime();
			bool bInserted = false;
			bool bRemoved = false;

			while (loopTimeout && !cardInserted && SAFETY > safety - start)
			{
				card = mcr.McrWaitForCardEvent(bInserted, bRemoved, loopTimeout, cardInserted);
				safety = GetTickCount();
			}

			CTime cardInTimeHigh = CUtilities::GetCurrentTime();
			CTime utcCardInTimeHigh = CUtilities::GetUTCTime();

			// End loop in case we fail
			mcr.ShutdownCardReader();

			if (ThreadData::RUNNING == WaitForLoop(threadData))
			{
				thread->ExitInstance();
			}

			Assert::IsTrue(cardInserted, L"Card reported as not inserted when there is.");
			Assert::IsNotNull(card, L"No card when there is.");

			CTime cardInTime = card->GetCardInTime();
			CTime utcCardInTime = card->GetUTCCardInTime();

			delete card;

			Assert::IsTrue(cardInTimeLow <= cardInTime, cardInTime.Format(L"Card In time too high, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(cardInTime <= cardInTimeHigh, cardInTime.Format(L"Card In time too low, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(utcCardInTimeLow <= utcCardInTime, utcCardInTime.Format(L"UTC Card In time too high, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(utcCardInTime <= utcCardInTimeHigh, utcCardInTime.Format(L"UTC Card In time too low, %m/%d/%Y %I:%M:%S %p"));

			Assert::IsFalse(loopTimeout, L"Card processing looped timed out when no card.");
		}

		TEST_METHOD(McrWrapperWaitForInsert2ndTrackTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();

			CMCRWrapper mcr(*m_config);
			Assert::IsTrue(mcr.IsCardReaderReady(), L"Reports as not ready when card reader.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when card when not.");

			ThreadData threadData(m_config, mcr);

			CCard* card = NULL;

			bool loopTimeout = true;
			bool cardInserted = false;

			UCHAR testData[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			// Start reader loop
			CWinThread* thread = AfxBeginThread(ReaderLoop, &threadData);

			// Insert card with 2nd track
			simulator.PushEvtStatusInserted(false);
			simulator.PushEvtCardData(testData, _countof(testData), false);

			Sleep(WAITFORLOOP);

			DWORD start = GetTickCount();
			DWORD safety = GetTickCount();

			CTime cardInTimeLow = CUtilities::GetCurrentTime();
			CTime utcCardInTimeLow = CUtilities::GetUTCTime();
			bool bInserted = false;
			bool bRemoved = false;

			while (loopTimeout && !cardInserted && SAFETY > safety - start)
			{
				card = mcr.McrWaitForCardEvent(bInserted, bRemoved, loopTimeout, cardInserted);
				safety = GetTickCount();
			}

			CTime cardInTimeHigh = CUtilities::GetCurrentTime();
			CTime utcCardInTimeHigh = CUtilities::GetUTCTime();

			// End loop in case we fail
			mcr.ShutdownCardReader();

			if (ThreadData::RUNNING == WaitForLoop(threadData))
			{
				thread->ExitInstance();
			}

			Assert::IsTrue(cardInserted, L"Card reported as not inserted when there is.");
			Assert::IsNotNull(card, L"No card when there is.");

			CTime cardInTime = card->GetCardInTime();
			CTime utcCardInTime = card->GetUTCCardInTime();

			delete card;

			Assert::IsTrue(cardInTimeLow <= cardInTime, cardInTime.Format(L"Card In time too high, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(cardInTime <= cardInTimeHigh, cardInTime.Format(L"Card In time too low, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(utcCardInTimeLow <= utcCardInTime, utcCardInTime.Format(L"UTC Card In time too high, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(utcCardInTime <= utcCardInTimeHigh, utcCardInTime.Format(L"UTC Card In time too low, %m/%d/%Y %I:%M:%S %p"));

			Assert::IsFalse(loopTimeout, L"Card processing looped timed out when no card.");
		}

		TEST_METHOD(McrWrapperWaitForInsertOasisTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();

			CMCRWrapper mcr(*m_config);
			Assert::IsTrue(mcr.IsCardReaderReady(), L"Reports as not ready when card reader.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when card when not.");

			ThreadData threadData(m_config, mcr);

			CCard* card = NULL;

			bool loopTimeout = true;
			bool cardInserted = false;

			// Oasis is detected by having more than 17 bytes of data.
			UCHAR testData[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };

			// Start reader loop
			CWinThread* thread = AfxBeginThread(ReaderLoop, &threadData);

			// Insert card with 2nd track
			simulator.PushEvtStatusInserted(false);
			simulator.PushEvtCardData(testData, _countof(testData), false);

			Sleep(WAITFORLOOP);

			DWORD start = GetTickCount();
			DWORD safety = GetTickCount();
			bool bInserted = false;
			bool bRemoved = false;

			while (loopTimeout && !cardInserted && SAFETY > safety - start)
			{
				card = mcr.McrWaitForCardEvent(bInserted, bRemoved, loopTimeout, cardInserted);
				safety = GetTickCount();
			}

			// End loop in case we fail
			mcr.ShutdownCardReader();

			if (ThreadData::RUNNING == WaitForLoop(threadData))
			{
				thread->ExitInstance();
			}

			Assert::IsTrue(cardInserted, L"Card reported as not inserted when there is.");
			Assert::IsNotNull(card, L"No card when there is.");

			delete card;

			Assert::IsFalse(loopTimeout, L"Card processing looped timed out when no card.");
		}

		TEST_METHOD(McrWrapperWaitForInsertUniversalTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();

			CMCRWrapper mcr(*m_config);
			Assert::IsTrue(mcr.IsCardReaderReady(), L"Reports as not ready when card reader.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when card when not.");

			m_config->SetUniverseEnabled(true);

			ThreadData threadData(m_config, mcr);

			CCard* card = NULL;

			bool loopTimeout = true;
			bool cardInserted = false;

			// Universal is expected to have exactly 17 bytes of data.
			UCHAR testData[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

			// Start reader loop
			CWinThread* thread = AfxBeginThread(ReaderLoop, &threadData);

			// Insert card with 2nd track
			simulator.PushEvtStatusInserted(false);
			simulator.PushEvtCardData(testData, _countof(testData), false);

			Sleep(WAITFORLOOP);

			DWORD start = GetTickCount();
			DWORD safety = GetTickCount();
			bool bInserted = false;
			bool bRemoved = false;

			CTime cardInTimeLow = CUtilities::GetCurrentTime();
			CTime utcCardInTimeLow = CUtilities::GetUTCTime();

			while (loopTimeout && !cardInserted && SAFETY > safety - start)
			{
				card = mcr.McrWaitForCardEvent(bInserted, bRemoved, loopTimeout, cardInserted);
				safety = GetTickCount();
			}

			CTime cardInTimeHigh = CUtilities::GetCurrentTime();
			CTime utcCardInTimeHigh = CUtilities::GetUTCTime();

			// End loop in case we fail
			mcr.ShutdownCardReader();

			if (ThreadData::RUNNING == WaitForLoop(threadData))
			{
				thread->ExitInstance();
			}

			Assert::IsTrue(cardInserted, L"Card reported as not inserted when there is.");
			Assert::IsNotNull(card, L"No card when there is.");

			CTime cardInTime = card->GetCardInTime();
			CTime utcCardInTime = card->GetUTCCardInTime();

			delete card;

			Assert::IsTrue(cardInTimeLow <= cardInTime, cardInTime.Format(L"Card In time too high, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(cardInTime <= cardInTimeHigh, cardInTime.Format(L"Card In time too low, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(utcCardInTimeLow <= utcCardInTime, utcCardInTime.Format(L"UTC Card In time too high, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(utcCardInTime <= utcCardInTimeHigh, utcCardInTime.Format(L"UTC Card In time too low, %m/%d/%Y %I:%M:%S %p"));

			Assert::IsFalse(loopTimeout, L"Card processing looped timed out when no card.");
		}

		TEST_METHOD(McrWrapperWaitForRemoveTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();

			CMCRWrapper mcr(*m_config);
			Assert::IsTrue(mcr.IsCardReaderReady(), L"Reports as not ready when card reader.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when card when not.");

			bool loopTimeout = true;
			bool cardInserted = false;

			DWORD start = 0;
			DWORD safety = 0;

			UCHAR testData[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			// Start reader loop
			ThreadData threadData(m_config, mcr);

			CWinThread* thread = AfxBeginThread(ReaderLoop, &threadData);

			// Insert card
			simulator.PushEvtStatusInserted(false);
			simulator.PushEvtCardData(testData, _countof(testData), false);

			Sleep(WAITFORLOOP);

			start = GetTickCount();
			safety = GetTickCount();
			bool bInserted = false;
			bool bRemoved = false;

			while (loopTimeout && !cardInserted && SAFETY > safety - start)
			{
				mcr.McrWaitForCardEvent(bInserted, bRemoved, loopTimeout, cardInserted);
				safety = GetTickCount();
			}

			simulator.PushEvtStatusRemoved();

			Sleep(WAITFORLOOP);

			start = GetTickCount();
			safety = GetTickCount();

			bool inserted = false;
			bool removed = false;
			loopTimeout = true;

			while (loopTimeout && !removed && SAFETY > safety - start)
			{
				mcr.McrWaitForCardEvent(inserted, removed, loopTimeout, cardInserted);
				safety = GetTickCount();
			}

			// End loop in case we fail
			mcr.ShutdownCardReader();

			if (ThreadData::RUNNING == WaitForLoop(threadData))
			{
				thread->ExitInstance();
			}

			Assert::IsTrue(removed, L"Card reported as not removed.");
			Assert::IsFalse(inserted, L"Card reported as inserted only removed.");
			Assert::IsFalse(cardInserted, L"Card reported as inserted when there is not.");
			Assert::IsFalse(loopTimeout, L"Card processing looped timed out when no card, 1st track.");
			TestBezelColor(&CardReaderTestHelpers::colorNone);
		}

		TEST_METHOD(McrWrapperWaitForDataReadErrorTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();

			CMCRWrapper mcr(*m_config);
			Assert::IsTrue(mcr.IsCardReaderReady(), L"Reports as not ready when card reader.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when card when not.");

			ThreadData threadData(m_config, mcr);

			CCard* card = NULL;

			bool loopTimeout = true;
			bool cardInserted = false;

			// Start reader loop
			CWinThread* thread = AfxBeginThread(ReaderLoop, &threadData);

			// Insert card
			simulator.PushEvtStatusInserted(false);
			simulator.SetReportError(ERROR_ERRORS_ENCOUNTERED);

			DWORD start = GetTickCount();
			DWORD safety = GetTickCount();
			bool bInserted = false;
			bool bRemoved = false;

			while (loopTimeout && !cardInserted && WAITFORLOOP > safety - start)
			{
				card = mcr.McrWaitForCardEvent(bInserted, bRemoved, loopTimeout, cardInserted);
				safety = GetTickCount();
			}

			// End loop in case we fail
			mcr.ShutdownCardReader();

			if (ThreadData::RUNNING == WaitForLoop(threadData))
			{
				thread->ExitInstance();
			}

			Assert::IsFalse(cardInserted, L"Card reported as inserted when there is not.");
			Assert::IsNull(card, L"No card when there is, 1st track.");
			Assert::IsTrue(loopTimeout, L"Card processing looped did not time out when no card.");
		}

		TEST_METHOD(McrWrapperWaitCardInsertedWitErrorTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();

			CMCRWrapper mcr(*m_config);
			Assert::IsTrue(mcr.IsCardReaderReady(), L"Reports as not ready when card reader.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when card when not.");

			ThreadData threadData(m_config, mcr);

			CCard* card = NULL;

			bool loopTimeout = true;
			bool cardInserted = false;

			UCHAR testData[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			// Start reader loop
			CWinThread* thread = AfxBeginThread(ReaderLoop, &threadData);

			// Set error onr Read Card Data
			simulator.SetFeatureError(HID_CARDRDR_CMD_READ_CARD_DATA, ERROR_ERRORS_ENCOUNTERED);

			// Insert card with 1st track
			simulator.PushEvtStatusInserted(true);
			simulator.PushEvtCardData(testData, _countof(testData), true);

			Sleep(WAITFORLOOP);

			DWORD start = GetTickCount();
			DWORD safety = GetTickCount();

			CTime cardInTimeLow = CUtilities::GetCurrentTime();
			CTime utcCardInTimeLow = CUtilities::GetUTCTime();
			bool bInserted = false;
			bool bRemoved = false;

			while (loopTimeout && !cardInserted && SAFETY > safety - start)
			{
				card = mcr.McrWaitForCardEvent(bInserted, bRemoved, loopTimeout, cardInserted);
				safety = GetTickCount();
			}

			CTime cardInTimeHigh = CUtilities::GetCurrentTime();
			CTime utcCardInTimeHigh = CUtilities::GetUTCTime();

			// End loop in case we fail
			mcr.ShutdownCardReader();

			if (ThreadData::RUNNING == WaitForLoop(threadData))
			{
				thread->ExitInstance();
			}

			Assert::IsTrue(cardInserted, L"Card reported as not inserted when there is.");
			Assert::IsNotNull(card, L"No card when there is.");

			CTime cardInTime = card->GetCardInTime();
			CTime utcCardInTime = card->GetUTCCardInTime();

			delete card;

			Assert::IsTrue(cardInTimeLow <= cardInTime, cardInTime.Format(L"Card In time too high, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(cardInTime <= cardInTimeHigh, cardInTime.Format(L"Card In time too low, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(utcCardInTimeLow <= utcCardInTime, utcCardInTime.Format(L"UTC Card In time too high, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(utcCardInTime <= utcCardInTimeHigh, utcCardInTime.Format(L"UTC Card In time too low, %m/%d/%Y %I:%M:%S %p"));

			Assert::IsFalse(loopTimeout, L"Card processing looped timed out when no card.");
		}

		TEST_METHOD(McrWrapperWaitCardInsertedAndQuickRemoveTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();

			CMCRWrapper mcr(*m_config);
			Assert::IsTrue(mcr.IsCardReaderReady(), L"Reports as not ready when card reader.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when card when not.");

			ThreadData threadData(m_config, mcr);

			CCard* card = NULL;

			bool loopTimeout = true;
			bool cardInserted = false;

			UCHAR testData[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			// Start reader loop
			CWinThread* thread = AfxBeginThread(ReaderLoop, &threadData);

			// Insert card and remove
			simulator.PushEvtStatusInserted(false);
			simulator.PushEvtCardData(testData, _countof(testData), false);
			simulator.PushEvtStatusRemoved();

			Sleep(WAITFORLOOP);

			DWORD start = GetTickCount();
			DWORD safety = GetTickCount();
			bool inserted = false;
			bool removed = false;

			while (loopTimeout && !cardInserted && SAFETY > safety - start)
			{
				mcr.McrWaitForCardEvent(inserted, removed, loopTimeout, cardInserted);
				safety = GetTickCount();
			}

			Sleep(WAITFORLOOP);

			inserted = false;
			removed = false;
			start = GetTickCount();
			safety = GetTickCount();
			loopTimeout = true;

			while (loopTimeout && !removed && SAFETY > safety - start)
			{
				mcr.McrWaitForCardEvent(inserted, removed, loopTimeout, cardInserted);
				safety = GetTickCount();
			}

			// End loop in case we fail
			mcr.ShutdownCardReader();

			if (ThreadData::RUNNING == WaitForLoop(threadData))
			{
				thread->ExitInstance();
			}

			Assert::IsTrue(removed, L"Card reported as not removed.");
			Assert::IsFalse(inserted, L"Card reported as inserted only removed.");
			Assert::IsFalse(cardInserted, L"Card reported when it should be remove.");
			Assert::IsFalse(loopTimeout, L"Card processing looped timed out when no card.");
		}

		/*
		TEST_METHOD(McrWrapperWaitForIOPendingTest)
		{
			CCard* card = NULL;

			bool loopTimeout = false;
			bool cardInserted = false;

			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();

			CMCRWrapper mcr;
			Assert::IsTrue(mcr.IsCardReaderReady(), L"Reports as not ready when card reader.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when card when not.");

			// Generate Card Insert with IO Pending
			UCHAR testData[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			ThreadData threadData(mcr);
			CWinThread* thread = NULL;

			// Start reader loop
			thread = AfxBeginThread(ReaderLoop, &threadData);

			simulator.PushEvtStatusInserted(false);
			simulator.SetReportError(ERROR_IO_PENDING, GetTickCount() + 30);
			simulator.PushEvtCardData(testData, _countof(testData), false);
			//mcr.McrWaitForCardInsert(loopTimeout, cardInserted);

			DWORD start = GetTickCount();
			DWORD safety = GetTickCount();

			// Provide Data
			while (!loopTimeout && !cardInserted && SAFETY > safety - start)
			{
				card = mcr.McrWaitForCardInsert(loopTimeout, cardInserted);
				safety = GetTickCount();
			}

			// End loop in case we fail
			mcr.ShutdownCardReader();

			if (ThreadData::RUNNING == WaitForLoop(threadData))
			{
				thread->ExitInstance();
			}

			Assert::IsTrue(cardInserted, L"Card reported not inserted when IO pending occurs.");
			Assert::IsNotNull(card, L"No card when IO pending occurs.");

			delete card;

			Assert::IsFalse(loopTimeout, L"Card processing looped timed out when no card, 1st track.");
		}
		*/

		TEST_METHOD(McrWrapperMcrWaitForInsertInsertTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			simulator.PushEvtStatusRemoved();

			CMCRWrapper mcr(*m_config);
			Assert::IsTrue(mcr.IsCardReaderReady(), L"Reports as not ready when card reader.");
			Assert::IsFalse(mcr.IsCardInserted(), L"Reports card as inserted when card when not.");

			ThreadData threadData(m_config, mcr);

			CCard* card = NULL;

			bool loopTimeout = true;
			bool cardInserted = false;

			UCHAR testData[] = { 0x3b, 0x30, 0x31, 0x32, 0x33, 0x34, 0x3f };

			// Start reader loop
			CWinThread* thread = AfxBeginThread(ReaderLoop, &threadData);

			// 1st insert of card
			simulator.PushEvtStatusInserted(false);
			simulator.PushEvtCardData(testData, _countof(testData), false);

			Sleep(WAITFORLOOP);

			DWORD start = GetTickCount();
			DWORD safety = GetTickCount();

			CTime cardInTimeLow = CUtilities::GetCurrentTime();
			CTime utcCardInTimeLow = CUtilities::GetUTCTime();
			bool inserted = false;
			bool removed = false;

			while (loopTimeout && !cardInserted && SAFETY > safety - start)
			{
				card = mcr.McrWaitForCardEvent(inserted, removed, loopTimeout, cardInserted);
				safety = GetTickCount();
			}

			bool secondInsert = false;
			inserted = false;
			removed = false;

			if (cardInserted && NULL != card && !loopTimeout)
			{
				secondInsert = true;

				Sleep(WAITFORLOOP);

				// 2nd insert of card
				simulator.PushEvtStatusInserted(false);
				simulator.PushEvtCardData(testData, _countof(testData), false);

				DWORD start = GetTickCount();
				DWORD safety = GetTickCount();

				loopTimeout = true;
				while (loopTimeout && SAFETY > safety - start)
				{
					mcr.McrWaitForCardEvent(inserted, removed, loopTimeout, cardInserted);
					safety = GetTickCount();
				}
			}

			CTime cardInTimeHigh = CUtilities::GetCurrentTime();
			CTime utcCardInTimeHigh = CUtilities::GetUTCTime();

			// End loop in case we fail
			mcr.ShutdownCardReader();

			if (ThreadData::RUNNING == WaitForLoop(threadData))
			{
				thread->ExitInstance();
			}


			if (secondInsert)
			{
				Assert::IsFalse(removed, L"Card reported as removed after 2nd insert.");
				Assert::IsTrue(inserted, L"Card reported as not inserted after 2nd insert.");
				Assert::IsTrue(cardInserted, L"CardInserted 'false' when it should be remove.");
			}
			Assert::IsTrue(secondInsert, L"First insert didn't show card inserted");

			CTime cardInTime = card->GetCardInTime();
			CTime utcCardInTime = card->GetUTCCardInTime();

			delete card;

			Assert::IsTrue(cardInTimeLow <= cardInTime, cardInTime.Format(L"Card In time too high, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(cardInTime <= cardInTimeHigh, cardInTime.Format(L"Card In time too low, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(utcCardInTimeLow <= utcCardInTime, utcCardInTime.Format(L"UTC Card In time too high, %m/%d/%Y %I:%M:%S %p"));
			Assert::IsTrue(utcCardInTime <= utcCardInTimeHigh, utcCardInTime.Format(L"UTC Card In time too low, %m/%d/%Y %I:%M:%S %p"));

			Assert::IsFalse(loopTimeout, L"Card processing looped timed out.");
		}

	};

	const LPCWSTR McrWrapperUnitTestsExt::TESTDIR = L"Test_McrWrapperExt";
	::CCriticalSection McrWrapperUnitTestsExt::m_cs;
}

#endif //TODO-PORT