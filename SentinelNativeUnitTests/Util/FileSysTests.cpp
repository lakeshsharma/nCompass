#if 000//TODO-PORT
#include "stdafx.h"
#include "Util\FileSys.h"
#include "Util\String.h"
#include "TestUtil\TestUtil.h"
#include "TestUtil\FileHook.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TestUtil;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(Util_FileSysTests)
	{
	public:
		TEST_METHOD_CLEANUP(Util_FileSys_Cleanup)
		{
			FileHook::ClearFileHooks();
			TestUtil::DeleteSentinelDirectories();
		}

		TEST_METHOD(Util_FileSys_FileExists)
		{
			DeleteOnCloseDir deleteOnCloseDir(L"FileExistDir");
			DeleteOnCloseFile deleteOnCloseFile(deleteOnCloseDir, L"FileExistDir", "");

			CString pathname = deleteOnCloseFile.GetPathname();
			bool result = FileSys::FileExists(pathname);
			Assert::IsTrue(result, AssertMsg::FormatOnly(L"FileSys::FileExists(%s)", (LPCTSTR) pathname));
		}

		TEST_METHOD(Util_FileSys_FileExists_fail)
		{
			DeleteOnCloseDir deleteOnCloseDir(L"FileExistDir");

			CString pathname = deleteOnCloseDir.GetName() + L"File";
			bool result = FileSys::FileExists(pathname);
			Assert::IsFalse(result, AssertMsg::FormatOnly(L"FileSys::FileExists(%s)", (LPCTSTR) pathname));
		}

		TEST_METHOD(Util_FileSys_DirExists)
		{
			DeleteOnCloseDir deleteOnCloseDir(L"DirExist");

			CString path = deleteOnCloseDir.GetName();
			bool result = FileSys::DirExists(path);
			Assert::IsTrue(result, AssertMsg::FormatOnly(L"FileSys::FileExists(%s)", (LPCTSTR) path));
		}

		TEST_METHOD(Util_FileSys_DirExists_fail)
		{
			DeleteOnCloseDir deleteOnCloseDir(L"DirExist");

			CString path = deleteOnCloseDir.GetName() + L"Dir";
			bool result = FileSys::DirExists(path);
			Assert::IsFalse(result, AssertMsg::FormatOnly(L"FileSys::FileExists(%s)", (LPCTSTR) path));
		}

		TEST_METHOD(Util_FileSys_IsValidHandle)
		{
			struct
			{
				int line;
				HANDLE handle;
				bool result;
			}
			data[] =
			{
				{ __LINE__, (HANDLE)  0, false },
				{ __LINE__, (HANDLE) -1, false },
				{ __LINE__, (HANDLE)  1, true  }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				bool result = FileSys::IsValidHandle(data[i].handle);
				Assert::AreEqual(data[i].result, result, AssertMsg::Format(data[i].line, nullptr));
			}
		}
	};
}
#endif //TODO-PORT