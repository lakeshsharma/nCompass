#if 000//TODO-PORT
#include "stdafx.h"
#include "Util\FindFile.h"
#include "TestUtil\TestUtil.h"
#include "Hash.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(Util_FindFileTests)
	{
		TEST_METHOD_INITIALIZE(Util_FindFileTests_Init)
		{
			TestUtil::CreateSentinelDirectory(DIRECTORY_SENTINELDATA);
		}

		TEST_METHOD_CLEANUP(Util_FindFileTests_Cleanup)
		{
			TestUtil::DeleteSentinelDirectories();
		}

		TEST_METHOD(Util_FindFile_GetFiles)
		{
			CString pathname = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA);

			unordered_set<const wchar_t*, wstr_hash_sdbm, wstr_equal_to> actualFiles;
			
			actualFiles.insert(L"1");
			actualFiles.insert(L"2");
			actualFiles.insert(L"3");

			for (auto& actualFile : actualFiles)
			{
				TestUtil::WriteFileContents(DIRECTORY_SENTINELDATA, actualFile, "1");
			}

			vector<FileInfo> files = FindFiles(pathname, nullptr, false);

			for (auto& file : files)
			{
				auto it = actualFiles.find(file.filename.c_str());
				Assert::IsTrue(it != actualFiles.end(), L"it != actualFiles.end()");
				actualFiles.erase(it);
			}

			Assert::IsTrue(actualFiles.empty(), L"actualFiles.empty()");
		}
	};
}
#endif //TODO-PORT