#if 000//TODO-PORT

#include "stdafx.h"
#include "Hardware\CardReaderLayer.h"
#include "..\CardReaderLayerDll\CardReaderSimulator.h"
#include "UnitTestingUtils.h"
#include "TestUtil.h"
#include "DirectoryManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(CardReaderLayerIsRequestPendingUnitTest)
	{
		static const LPCWSTR TESTDIR;

	public:

		TEST_CLASS_CLEANUP(CardReaderLayerClassCleanup)
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
			CDirectoryManager::MockInstanceCleanup();
		}

		TEST_METHOD(CardReaderLayerIsRequestPendingNoneTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate card reader as connected.");

			CCardReaderLayer cardReaderLayer;
			bool found = cardReaderLayer.FindCardReader();
			Assert::IsTrue(found, L"Unable to find card reader");

			//CWinThread* thread = AfxBeginThread(SendWait, NULL);

			DWORD bytes = 0;
			bool requestPending = cardReaderLayer.IsRequestPending(bytes);
			Assert::IsFalse(requestPending, L"Request pending when none yet sent.");
		}

	};

	const LPCWSTR CardReaderLayerIsRequestPendingUnitTest::TESTDIR = L"Test_CardReaderLayerIsRequestPending";
}

#endif //TODO-PORT