#if 000//TODO-PORT
#include "stdafx.h"
#include "ProcessDump\WebDiagnostics.h"
#include "Hardware\MCRWrapper.h"
#include "Hardware\CardReaderLayer.h"
#include "UnitTestingUtils.h"
#include "Util\Convert.h"
#include "TestUtil.h"
#include "Hardware\CardReaderTestHelpers.h"
#include "..\CardReaderLayerDll\CardReaderSimulator.h"

using namespace pugi;
using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace TestUtil;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(WebDiagnostics_CardReader)
	{
		static const LPCWSTR TESTDIR;

		TEST_CLASS_CLEANUP(CardReaderLayerUnitTestsCleanup)
		{
			CCardReaderSimulator::CleanupInstance();
		}

		TEST_METHOD_INITIALIZE(CardReaderLayerInitialize)
		{
			WriteFileContents(DIRECTORY_APPLICATIONS, pCardReaderConfigFilename, CardReaderTestHelpers::pConfigXml);

			CString testDir;
			CUnitTestingUtils::GetTestDir(TESTDIR, testDir);

			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.Start(testDir);
		}

		TEST_METHOD_CLEANUP(CardReaderLayerCleanup)
		{
			CCardReaderSimulator::Instance().End();
			CDirectoryManager::MockInstanceCleanup();
			DeleteSentinelDirectories();
		}

		TEST_METHOD(WebDiagnostics_CardReader_Base)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);

			CCardReaderLayer cardReaderLayer;
			cardReaderLayer.LoadConfig();
			CardReaderTestHelpers::Verify::AreEqual(__LINE__, CardReaderTestHelpers::pConfigXml, cardReaderLayer.GetCardReaderConfig());
		}

	};
	const LPCWSTR WebDiagnostics_CardReader::TESTDIR = L"Test_CardReaderLayer";
}
#endif //TODO-PORT