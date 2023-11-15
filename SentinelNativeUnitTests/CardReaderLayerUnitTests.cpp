#if 000//TODO-PORT

#include "stdafx.h"
#include "Hardware\CardReaderLayer.h"
#include "PlayerInfo.h"
#include "..\CardReaderLayerDll\CardReaderSimulator.h"
#include "UnitTestingUtils.h"
#include "Hardware\CardReaderTestHelpers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace TestUtil;
using namespace pugi;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(CardReaderLayerUnitTests)
	{
		static const LPCWSTR TESTDIR;

		static const int SAFETY = 20000;

		struct ThreadData
		{
			enum Sentinel
			{
				RUNNING = 0,
				DONE,
				CRASHED,
				TIMEDOUT
			};

			ThreadData(CCardReaderLayer& crl) : cardReaderLayer(crl), bytes(0), sentinel(RUNNING) {}

			CCardReaderLayer& cardReaderLayer;
			DWORD bytes;
			Sentinel sentinel;
		};

		ThreadData::Sentinel WaitForThread(const ThreadData& threadData)
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

	public:
		TEST_CLASS_CLEANUP(CardReaderLayerUnitTestsCleanup)
		{
			CCardReaderSimulator::CleanupInstance();
		}

		TEST_METHOD_INITIALIZE(CardReaderLayerInitialize)
		{
			CDirectoryManager::MockInstance();

			CString testDir;
			CUnitTestingUtils::GetTestDir(TESTDIR, testDir);

			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.Start(testDir);
		}

		TEST_METHOD_CLEANUP(CardReaderLayerCleanup)
		{
			CCardReaderSimulator::Instance().End();
			DeleteSentinelDirectories();
		}

		TEST_METHOD(CardReaderLayerDefaultTest)
		{
			CCardReaderLayer cardReaderLayer;
			Assert::IsFalse(cardReaderLayer.IsReady(), L"CardReaderLayer reports as 'ready' before finding device.");
		}

		void TestColors(int line, CCardReaderLayer& cardReaderLayer, const char* pExpectedXml)
		{
			CardReaderConfig expectedConfig(nullptr, pExpectedXml);
			std::shared_ptr<CardReaderConfig> cardReaderConfig = cardReaderLayer.GetCardReaderConfig();
		}

		TEST_METHOD(CardReaderLayer_LoadConfig_DIRECTORY_SENTINELDATA)
		{
			DeleteOnCloseFile configFile(DIRECTORY_SENTINELDATA, pCardReaderConfigFilename, CardReaderTestHelpers::pConfigXmlNoRanks);
			DeleteSentinelFiles(DIRECTORY_APPLICATIONS);

			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);

			CCardReaderLayer cardReaderLayer;
			cardReaderLayer.LoadConfig();
			CardReaderTestHelpers::Verify::AreEqual(__LINE__, CardReaderTestHelpers::pConfigXmlNoRanks, cardReaderLayer.GetCardReaderConfig());

			configFile.WriteFileContents(CardReaderTestHelpers::pConfigXmlNoRanksDiffEmployee);
			cardReaderLayer.LoadConfig();
			CardReaderTestHelpers::Verify::AreEqual(__LINE__, CardReaderTestHelpers::pConfigXmlNoRanksDiffEmployee, cardReaderLayer.GetCardReaderConfig());
		}

		TEST_METHOD(CardReaderLayer_LoadConfig_DIRECTORY_APPLICATIONS)
		{
			DeleteOnCloseFile cardReaderConfigData(DIRECTORY_SENTINELDATA, pCardReaderConfigFilename, CardReaderTestHelpers::pConfigXmlNoRanksMissingEmployee);
			DeleteOnCloseFile configFile(DIRECTORY_APPLICATIONS, pCardReaderConfigFilename, CardReaderTestHelpers::pConfigXml);

			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);

			CCardReaderLayer cardReaderLayer;
			cardReaderLayer.LoadConfig();
			CardReaderTestHelpers::Verify::AreEqual(__LINE__, CardReaderTestHelpers::pConfigXml, cardReaderLayer.GetCardReaderConfig());

			configFile.WriteFileContents(CardReaderTestHelpers::pConfigXmlNoRanksDiffEmployee);
			cardReaderLayer.LoadConfig();
			CardReaderTestHelpers::Verify::AreEqual(__LINE__, CardReaderTestHelpers::pConfigXmlNoRanksDiffEmployee, cardReaderLayer.GetCardReaderConfig());
		}

		TEST_METHOD(CardReaderLayerFindNotConnectedTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, false);
			Assert::IsFalse(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate UIC card reader as not connected.");
			simulator.SetConnected(USB_VENDOR_ID_XST, false);
			Assert::IsFalse(simulator.GetConnected(USB_VENDOR_ID_XST), L"Unable to simulate XST card reader as not connected.");

			CCardReaderLayer cardReaderLayer;
			bool found = cardReaderLayer.FindCardReader();
			Assert::IsFalse(found, L"Device found when none connected");
			Assert::IsFalse(cardReaderLayer.IsReady(), L"CardReaderLayer reports as 'ready' when no device connected.");
		}

		TEST_METHOD(CardReaderLayerFindUicTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate card reader as connected.");

			CCardReaderLayer cardReaderLayer;
			bool found = cardReaderLayer.FindCardReader();
			Assert::IsTrue(found, L"Unable to find UIC card reader");
		}

		TEST_METHOD(CardReaderLayerFindXstTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_XST, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_XST), L"Unable to simulate card reader as connected.");

			CCardReaderLayer cardReaderLayer;
			bool found = cardReaderLayer.FindCardReader();
			Assert::IsTrue(found, L"Unable to find XST card reader");
		}

		TEST_METHOD(CardReaderLayerEnableDisableTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate card reader as connected.");

			CCardReaderLayer cardReaderLayer;
			bool found = cardReaderLayer.FindCardReader();
			Assert::IsTrue(found, L"Unable to find card reader");

			DWORD result = ERROR_SUCCESS;
			result = cardReaderLayer.Enable();
			if (ERROR_SUCCESS != result)
			{
				CString errorMsg;
				CUnitTestingUtils::GetErrorMessage(result, errorMsg);

				CString msg(L"Enabling card reader failed with error");
				msg.AppendFormat(L": %d - %s", result, errorMsg);
				Assert::Fail(msg);
			}
			Assert::IsTrue(simulator.IsEnabled(), L"Card reader not reported as 'enabled' after enabling with no errors");

			result = cardReaderLayer.Disable();
			if (ERROR_SUCCESS != result)
			{
				CString errorMsg;
				CUnitTestingUtils::GetErrorMessage(result, errorMsg);

				CString msg(L"Disabling card reader failed with error");
				msg.AppendFormat(L": %d - %s", result, errorMsg);
				Assert::Fail(msg);
			}
			Assert::IsFalse(simulator.IsEnabled(), L"Card reader reported as 'enabled' after disabling with no errors");
		}

		TEST_METHOD(CardReaderLayerEnableErrorTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate card reader as connected.");

			simulator.SetFeatureError(HID_CARDRDR_CMD_ENABLE, ERROR_ERRORS_ENCOUNTERED);

			CCardReaderLayer cardReaderLayer;
			bool found = cardReaderLayer.FindCardReader();
			Assert::IsTrue(found, L"Unable to find card reader");
			Assert::IsFalse(simulator.IsEnabled(), L"Card reader not reported as 'enabled' after enabling during init fails.");

			simulator.SetFeatureError(HID_CARDRDR_CMD_ENABLE, ERROR_ERRORS_ENCOUNTERED);

			DWORD result = cardReaderLayer.Enable();
			Assert::IsFalse(ERROR_SUCCESS == result, L"Enabling reports as success when should be reporting error.");
			Assert::IsFalse(simulator.IsEnabled(), L"Card reader not reported as 'enabled' after enabling fails.");
		}

		TEST_METHOD(CardReaderLayerDisableErrorTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate card reader as connected.");

			simulator.SetFeatureError(HID_CARDRDR_CMD_DISABLE, ERROR_ERRORS_ENCOUNTERED);

			CCardReaderLayer cardReaderLayer;
			bool found = cardReaderLayer.FindCardReader();
			Assert::IsTrue(found, L"Unable to find card reader");
			Assert::IsTrue(simulator.IsEnabled(), L"Card reader not reported as 'enabled' when disabling fails during init.");

			simulator.SetFeatureError(HID_CARDRDR_CMD_DISABLE, ERROR_ERRORS_ENCOUNTERED);

			DWORD result = cardReaderLayer.Disable();
			Assert::IsFalse(ERROR_SUCCESS == result, L"Disabling reports as success when should be reporting error.");
			Assert::IsTrue(simulator.IsEnabled(), L"Card reader not reported as 'enabled' after disabling fails.");
		}

		TEST_METHOD(CardReaderLayerCleanupTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate card reader as connected.");

			CCardReaderLayer cardReaderLayer;
			bool found = cardReaderLayer.FindCardReader();
			Assert::IsTrue(found, L"Unable to find card reader");
			Assert::IsTrue(cardReaderLayer.IsReady(), L"Card reader not reported as read when device is fould");

			DWORD result = ERROR_SUCCESS;
			result = cardReaderLayer.Enable();
			if (ERROR_SUCCESS != result)
			{
				CString errorMsg;
				CUnitTestingUtils::GetErrorMessage(result, errorMsg);

				CString msg(L"Enabling card reader failed with error");
				msg.AppendFormat(L": %d - %s", result, errorMsg);
				Assert::Fail(msg);
			}
			Assert::IsTrue(simulator.IsEnabled(), L"Card reader not reported as 'enabled' after enabling with no errors");

			result = cardReaderLayer.Disable();
			if (ERROR_SUCCESS != result)
			{
				CString errorMsg;
				CUnitTestingUtils::GetErrorMessage(result, errorMsg);

				CString msg(L"Disabling card reader failed with error");
				msg.AppendFormat(L": %d - %s", result, errorMsg);
				Assert::Fail(msg);
			}
			Assert::IsFalse(simulator.IsEnabled(), L"Card reader reported as 'enabled' after disabling with no errors");

			cardReaderLayer.Cleanup();
			Assert::IsFalse(cardReaderLayer.IsReady(), L"CardReaderLayer reports as 'ready' after cleanup.");
		}

		void TestSetColor(int line, CCardReaderLayer& cardReaderLayer, CARD_STYLE style, DWORD rankId, const CardReaderColor* pExpected)
		{
			DWORD result = cardReaderLayer.SetBezelStyle(style, rankId);
			Assert::AreEqual((DWORD) ERROR_SUCCESS, result, AssertMsg::Format(line, nullptr, L"SetBezelStyle(%d, %u)", (int) style, rankId));

			const CardReaderColor* pColor = (const CardReaderColor*) CCardReaderSimulator::Instance().GetColors();
			Assert::IsTrue(pExpected->CompareHidData(*pColor), AssertMsg::Format(line, L"CardReaderColor", L"SetBezelStyle(%d, %u)", (int) style, rankId));
		}

		TEST_METHOD(CardReaderLayerSetColorTest)
		{
			DeleteOnCloseFile configFile(DIRECTORY_APPLICATIONS, pCardReaderConfigFilename, CardReaderTestHelpers::pConfigXml);

			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate card reader as connected.");

			CCardReaderLayer cardReaderLayer;
			bool found = cardReaderLayer.FindCardReader();
			Assert::IsTrue(found, L"Unable to find card reader");

			TestSetColor(__LINE__, cardReaderLayer, CARD_BAD_READ,       (DWORD) -1, &CardReaderTestHelpers::colorBadRead);
			TestSetColor(__LINE__, cardReaderLayer, CARD_BAD_READ,       (DWORD)  0, &CardReaderTestHelpers::colorBadRead);

			TestSetColor(__LINE__, cardReaderLayer, CARD_REMOVED,        (DWORD) -1, &CardReaderTestHelpers::colorNone);
			TestSetColor(__LINE__, cardReaderLayer, CARD_REMOVED,        (DWORD)  0, &CardReaderTestHelpers::colorNone);

			TestSetColor(__LINE__, cardReaderLayer, CARD_STYLE_INVALID,  (DWORD) -1, &CardReaderTestHelpers::colorNone);
			TestSetColor(__LINE__, cardReaderLayer, CARD_STYLE_INVALID,  (DWORD)  0, &CardReaderTestHelpers::colorNone);

			TestSetColor(__LINE__, cardReaderLayer, CARD_STYLE_PLAYER,   (DWORD) -1, &CardReaderTestHelpers::colorPlayer);
			TestSetColor(__LINE__, cardReaderLayer, CARD_STYLE_PLAYER,   (DWORD)  0, &CardReaderTestHelpers::colorRank0);

			TestSetColor(__LINE__, cardReaderLayer, CARD_STYLE_EMPLOYEE, (DWORD) -1, &CardReaderTestHelpers::colorEmployee);
			TestSetColor(__LINE__, cardReaderLayer, CARD_STYLE_EMPLOYEE, (DWORD)  0, &CardReaderTestHelpers::colorEmployee);
		}

		TEST_METHOD(CardReaderLayerReadCardDataErrorTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate card reader as connected.");

			CCardReaderLayer cardReaderLayer;
			bool found = cardReaderLayer.FindCardReader();
			Assert::IsTrue(found, L"Unable to find card reader");

			simulator.SetFeatureError(HID_CARDRDR_CMD_READ_CARD_DATA, ERROR_ERRORS_ENCOUNTERED);

			DWORD result = ERROR_SUCCESS;
			result = cardReaderLayer.ReadCardData(CARD_DATA_TRACK1);
			Assert::IsFalse(ERROR_SUCCESS == result, L"Read Card Data reports as success when should be reporting error on 1st track.");

			result = cardReaderLayer.ReadCardData(CARD_DATA_TRACK2);
			Assert::IsFalse(ERROR_SUCCESS == result, L"Read Card Data reports as success when should be reporting error on 2nd track.");
		}

		TEST_METHOD(CardReaderLayerReadReportDataErrorTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate card reader as connected.");

			CCardReaderLayer cardReaderLayer;
			bool found = cardReaderLayer.FindCardReader();
			Assert::IsTrue(found, L"Unable to find card reader");

			simulator.SetReportError(ERROR_ERRORS_ENCOUNTERED);

			UCHAR reportBuf[HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_DATA + 1];
			DWORD bytes = 0;

			DWORD result = cardReaderLayer.ReadReportData(reportBuf, &bytes);
			Assert::IsFalse(ERROR_SUCCESS == result, L"ReadReportData reports as success when should be reporting error.");
			Assert::IsTrue(0 == bytes, L"Bytes not 0 after error.");
		}

		TEST_METHOD(CardReaderLayerReadClearDataTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate card reader as connected.");

			CCardReaderLayer cardReaderLayer;
			bool found = cardReaderLayer.FindCardReader();
			Assert::IsTrue(found, L"Unable to find card reader");
			Assert::IsTrue(cardReaderLayer.IsReady(), L"Card reader not reported as read when device is fould");

			UCHAR testData[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			simulator.PushEvtCardData(testData, _countof(testData), false);

			DWORD result = ERROR_SUCCESS;

			UCHAR reportBuf[HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_DATA + _countof(testData)];
			DWORD bytes = 0;


			result = cardReaderLayer.ReadCardData(CARD_DATA_TRACK2);
			if (ERROR_SUCCESS != result)
			{
				CString errorMsg;
				CUnitTestingUtils::GetErrorMessage(result, errorMsg);

				CString msg(L"Command for Reading card data failed with error");
				msg.AppendFormat(L": %d - %s", result, errorMsg);
				Assert::Fail(msg);
			}

			result = cardReaderLayer.ReadReportData(reportBuf, &bytes);
			if (ERROR_SUCCESS != result)
			{
				CString errorMsg;
				CUnitTestingUtils::GetErrorMessage(result, errorMsg);

				CString msg(L"Reading card data failed with error");
				msg.AppendFormat(L": %d - %s", result, errorMsg);
				Assert::Fail(msg);
			}

			Assert::IsTrue(_countof(reportBuf) == bytes, L"Wrong number of bytes read.");
			Assert::IsTrue(HID_CARDRDR_EVT_CARD_DATA == reportBuf[HID_CARDRDR_DATA_REPORT_OFFSET_EVENT], L"'Data report' not returned as a 'card data event'.");
			Assert::IsTrue(HID_CARDRDR_DATA_REPORT_VALUE_NONE == reportBuf[HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_INDEX1], L"Garbage in unused part of 'data report'.");
			Assert::IsTrue(HID_CARDRDR_DATA_REPORT_VALUE_NONE == reportBuf[HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_INDEX2], L"Garbage in unused part of 'data report'.");
			Assert::IsTrue(_countof(testData) == reportBuf[HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_LENGTH], L"'Data report' returned wrong length of data.");
			Assert::IsTrue(HID_CARDRDR_DATA_REPORT_CARD_DATA_VALUE_TRACK2_USED == reportBuf[HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_TRACK], L"'Data report' returned wrong track used.");

			for (unsigned n = 0; n < 10; n++)
			{
				unsigned dataOffset = HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_DATA + n;
				if (testData[n] != reportBuf[dataOffset])
				{
					CString msg(L"Wrong report data returned");
					msg.AppendFormat(L": index = %d, expected = %d, set = %d", n, testData[n], reportBuf[dataOffset]);
					Assert::Fail(msg);
				}
			}

			result = cardReaderLayer.ClearBuffer();
			if (ERROR_SUCCESS != result)
			{
				CString errorMsg;
				CUnitTestingUtils::GetErrorMessage(result, errorMsg);

				CString msg(L"Clearing buffer failed with error");
				msg.AppendFormat(L": %d - %s", result, errorMsg);
				Assert::Fail(msg);
			}

			result = cardReaderLayer.ReadCardData(CARD_DATA_TRACK2);
			if (ERROR_SUCCESS != result)
			{
				CString errorMsg;
				CUnitTestingUtils::GetErrorMessage(result, errorMsg);

				CString msg(L"Command for Reading card data failed with error");
				msg.AppendFormat(L": %d - %s", result, errorMsg);
				Assert::Fail(msg);
			}

			result = cardReaderLayer.ReadReportData(reportBuf, &bytes);
			if (ERROR_SUCCESS != result)
			{
				CString errorMsg;
				CUnitTestingUtils::GetErrorMessage(result, errorMsg);

				CString msg(L"Reading card data after clear failed with error");
				msg.AppendFormat(L": %d - %s", result, errorMsg);
				Assert::Fail(msg);
			}

			Assert::IsTrue(0 == bytes, L"Bytes not 0 after clearing buffer.");
		}

		static UINT AFX_CDECL PendingThread( PVOID threadParam )
		{
			ThreadData *data = (ThreadData *)threadParam;

			try
			{
				if (data->cardReaderLayer.IsRequestPending(data->bytes))
				{
					data->sentinel = ThreadData::TIMEDOUT;
				}
				else
				{
					data->sentinel = ThreadData::DONE;
				}
			}
			catch (...)
			{
				data->sentinel = ThreadData::CRASHED;
			}
		
			return 0;
		}

		TEST_METHOD(CardReaderLayerIsRequestPendingTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate card reader as connected.");

			CCardReaderLayer cardReaderLayer;
			bool found = cardReaderLayer.FindCardReader();
			Assert::IsTrue(found, L"Unable to find card reader");
			Assert::IsTrue(cardReaderLayer.IsReady(), L"Card reader not reported as read when device is fould");

			UCHAR testData[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			simulator.PushEvtCardData(testData, _countof(testData), false);

			DWORD result = ERROR_SUCCESS;

			UCHAR reportBuf[HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_DATA + _countof(testData)];
			DWORD bytes = 0;

			result = cardReaderLayer.ReadCardData(CARD_DATA_TRACK2);
			if (ERROR_SUCCESS != result)
			{
				CString errorMsg;
				CUnitTestingUtils::GetErrorMessage(result, errorMsg);

				CString msg(L"Command for Reading card data failed with error");
				msg.AppendFormat(L": %d - %s", result, errorMsg);
				Assert::Fail(msg);
			}

			ThreadData threadData(cardReaderLayer);
			CWinThread* thread = AfxBeginThread(PendingThread, &threadData);
			
			simulator.SetReportError(ERROR_IO_PENDING, GetTickCount() + 30);

			result = cardReaderLayer.ReadReportData(reportBuf, &bytes);

			if (ThreadData::RUNNING == WaitForThread(threadData))
			{
				thread->ExitInstance();
			}

			Assert::IsTrue(ThreadData::DONE == threadData.sentinel, L"IsRequestPending timedout or crashed.");
			//Assert::IsTrue(_countof(testData) == threadData.bytes, L"IsRequestPending did not return correct number of bytes.");
		}

	};

	const LPCWSTR CardReaderLayerUnitTests::TESTDIR = L"Test_CardReaderLayer";
}

#endif //TODO-PORT