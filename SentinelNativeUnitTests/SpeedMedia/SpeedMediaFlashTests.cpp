#if 000//TODO-PORT
#include "stdafx.h"
#include "FilePacker.h"
#include "Util\FileSys.h"
#include "TestUtil\TestUtil.h"
#include "TestUtil\FileHook.h"
#include "Util\OutputDebugFormat.h"
#include "SpeedMedia\SpeedMediaFlash.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(SpeedMediaFlashFileTests)
	{
		TEST_METHOD_INITIALIZE(SpeedMediaFlashFileTests_Init)
		{
			TestUtil::CreateSentinelDirectory(DIRECTORY_SMCONTENT);
			TestUtil::CreateSentinelDirectory(DIRECTORY_UPGRADE);
		}

		TEST_METHOD_CLEANUP(SpeedMediaFlashFileTests_Cleanup)
		{
			FileHook::ClearFileHooks();
			TestUtil::DeleteSentinelDirectories();
		}

		TEST_METHOD(SpeedMediaFlashFile_ctor)
		{
			CString dataLocation = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SMCONTENT);

			const DWORD numBlocks = 3;
			CString filename(_T("filename"));
			const char* pGroupAddress = "239.10.10.7";

			CSpeedMediaFlash flash(BLOCKSIZE * numBlocks, numBlocks, 1, 10551, 443, 1, filename, CString(pGroupAddress), FileTypeMisc);

			Assert::AreEqual(pGroupAddress, flash.m_groupAddress.c_str(), L"flash.m_groupAddress");
			Assert::AreEqual((int) FileTypeMisc, (int) flash.m_mediaType, L"flash.m_mediaType");

			Assert::IsNotNull(flash.m_critSection.get(), L"flash.m_critSection");
			Assert::AreEqual(numBlocks, flash.m_blockCount, L"flash.m_blockCount");
			Assert::AreEqual(numBlocks, flash.m_missingBlockCount, L"flash.m_missingBlockCount");
			Assert::AreEqual((DWORD) BLOCKSIZE, flash.m_blockSize, L"flash.m_blockSize");
			Assert::AreEqual((LPCTSTR) dataLocation, flash.m_dataLocation, L"flash::m_dataLocation");
			Assert::AreEqual(BLOCKSIZE * numBlocks, flash.m_fileSize, L"flash.m_fileSize");
			Assert::AreEqual((int) FLASH_SPEEDMEDIA, (int) flash.m_type, L"flash.m_type");
			Assert::AreEqual((DWORD) 1, flash.m_sessionID, L"flash.m_sessionID");
			Assert::AreEqual((DWORD) 10551, (DWORD) flash.m_port, L"flash.m_port");
			Assert::AreEqual((DWORD) 443, flash.m_expectedChecksum, L"flash.m_expectedChecksum");
			Assert::AreEqual((DWORD) 0, flash.m_actualChecksum, L"flash.m_actualChecksum");
			Assert::AreEqual((DWORD) 443, flash.m_expectedChecksum, L"flash.m_sessionID");
			Assert::IsFalse(flash.m_isFlashFileReady, L"flash.m_isFlashFileReady");
			Assert::IsTrue(flash.m_autoBurn, L"flash.m_autoBurn");
			Assert::AreEqual((DWORD) 0, (DWORD) flash.m_hSessionKey, L"flash.m_hSessionKey");
			Assert::IsFalse(flash.m_isCleared, L"flash.m_isCleared");
			Assert::AreEqual((DWORD) 0, (DWORD) flash.m_percentComplete, L"flash.m_percentComplete");
			Assert::AreEqual((DWORD) 1, (DWORD) flash.m_nextBlockBegin, L"flash.m_nextBlockBegin");
			Assert::AreEqual((DWORD) 0, (DWORD) flash.m_ipAddress, L"flash.m_ipAddress");
			Assert::IsFalse(flash.m_restartServiceWindow, L"flash.m_restartServiceWindow");
			Assert::AreEqual((DWORD) (numBlocks + 1), (DWORD) flash.m_blockArray.GetCount(), L"flash.m_blockArray.GetCount()");
		}

		TEST_METHOD(SpeedMediaFlashFile_CreateFile)
		{
			try
			{
				unique_ptr<CSpeedMediaFlash> flash(new CSpeedMediaFlash(64, 8, 2134, 0, 5, 0, CString(L"1234.txt"), CString(L""), FileTypeMisc));

				bool result = flash->CreateFile();
				Assert::IsTrue(result, L"CreateFile");
				Assert::IsTrue(flash->IsFileOpen(), L"IsFileOpen");
				Assert::IsTrue(FileSys::FileExists(flash->m_filename), L"FileExists");
			}
			catch (...)
			{
				Assert::Fail(L"Unexpected exception");
			}
		}
	};
}
#endif //TODO-PORT