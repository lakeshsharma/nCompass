#if 000//TODO-PORT
#include "stdafx.h"
#include <Util/FileSys.h>
#include <Util/FileCopy.h>
#include <Util/String.h>
#include "TestUtil/TestUtil.h"
#include "TestUtil/FileHook.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace TestUtil;

namespace SentinelNativeUnitTests
{
	static ofstream ofs_deb;

	struct File
	{
		LPCTSTR pName;			// If starts with '!' return directory
		int		len;			// if -1, file doesn't exist
		DWORD   attributes;		// See CreateFile dwFlagsAndAttributes

		CString	pathName;
		string	data;

		File(LPCTSTR pName, int len, DWORD attributes)
			: pName(pName)
			, len(len)
			, attributes(attributes)
		{
		}

		CString ToString() const
		{
			CString s;
			s.Format(L"'%s' len=%d attr=%0x", pName ? pName : L"<null>", len, attributes);
			return s;
		}

		LPCTSTR Create()
		{
			pathName.Empty();
			data.resize(0);

			if (pName == nullptr || *pName == 0)
				return pName;

			pathName = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA);

			if (*pName != L'!')
			{
				pathName += pName;
				ofstream ofs(pathName);

				if (len >= 0)
				{
					data.reserve((size_t) len);

					for (size_t i = 0; i < (size_t) len; i++)
					{
						data += (char) ((rand() % ('z' - '!')) + '!');
					}

					ofs << data;
				}
			}

			return pathName;
		}

		void Verify(const string& expectedData) const
		{
			Assert::IsTrue(FileSys::FileExists(pathName), AssertMsg::FormatOnly(L"FileExists(%s)", (LPCTSTR) pathName));

			ifstream ifs(pathName);
			Assert::IsTrue((bool) ifs, AssertMsg::FormatOnly(L"ifstream ifs(%s)", (LPCTSTR) pathName));
			string actualData;
			ifs >> actualData;

			Assert::IsTrue(expectedData == actualData, AssertMsg::FormatOnly(L"expectedData == actualData: %s", (LPCTSTR) pathName));
		}

		bool IsFileNameValid() const
		{
			return pName != nullptr && *pName != 0 && *pName != '!';
		}
	};

	TEST_CLASS(Util_FileCopyTests)
	{
		TEST_METHOD_INITIALIZE(Util_FileCopyTests_init)
		{
			TestUtil::CreateSentinelDirectory(DIRECTORY_SENTINELDATA);
		}

		TEST_METHOD_CLEANUP(Util_FileCopyTests_cleanup)
		{
			FileHook::ClearFileHooks();
			TestUtil::DeleteSentinelDirectories();
		}

		TEST_METHOD(Util_FileCopy_Copy_srcnull)
		{
			File srcFile(nullptr, 9, 0);
			File dstFile(L"2", 9, 0);

			try
			{
				FileCopy::Copy(srcFile.Create(), dstFile.Create(), 4);
				dstFile.Verify(dstFile.data);
				Assert::Fail(L"Expected exception");
			}
			catch (CFileException* pE)
			{
				pE->Delete();
			}
		}

		TEST_METHOD(Util_FileCopy_Copy_srcempty)
		{
			File srcFile(L"", 9, 0);
			File dstFile(L"2", 9, 0);

			try
			{
				FileCopy::Copy(srcFile.Create(), dstFile.Create(), 4);
				dstFile.Verify(dstFile.data);
				Assert::Fail(L"Expected exception");
			}
			catch (CFileException* pE)
			{
				pE->Delete();
			}
		}

		TEST_METHOD(Util_FileCopy_Copy_srcdir)
		{
			File srcFile(L"!", 9, 0);
			File dstFile(L"2", 9, 0);

			try
			{
				FileCopy::Copy(srcFile.Create(), dstFile.Create(), 4);
				dstFile.Verify(dstFile.data);
				Assert::Fail(L"Expected exception");
			}
			catch (CFileException* pE)
			{
				pE->Delete();
			}
		}

		TEST_METHOD(Util_FileCopy_Copy_dstnull)
		{
			File srcFile(L"1", 9, 0);
			File dstFile(nullptr, 9, 0);

			try
			{
				FileCopy::Copy(srcFile.Create(), dstFile.Create(), 4);
				dstFile.Verify(dstFile.data);
				Assert::Fail(L"Expected exception");
			}
			catch (CFileException* pE)
			{
				pE->Delete();
			}
		}

		TEST_METHOD(Util_FileCopy_Copy_dstempty)
		{
			File srcFile(L"1", 9, 0);
			File dstFile(L"", 9, 0);

			try
			{
				FileCopy::Copy(srcFile.Create(), dstFile.Create(), 4);
				dstFile.Verify(dstFile.data);
				Assert::Fail(L"Expected exception");
			}
			catch (CFileException* pE)
			{
				pE->Delete();
			}
		}

		TEST_METHOD(Util_FileCopy_Copy_dstdir)
		{
			File srcFile(L"1", 9, 0);
			File dstFile(L"!", 9, 0);

			try
			{
				FileCopy::Copy(srcFile.Create(), dstFile.Create(), 4);
				dstFile.Verify(dstFile.data);
				Assert::Fail(L"Expected exception");
			}
			catch (CFileException* pE)
			{
				pE->Delete();
			}
		}

		TEST_METHOD(Util_FileCopy_Copy_same)
		{
			File srcFile(L"1", 9, 0);
			LPCTSTR pSrc = srcFile.Create();

			try
			{
				FileCopy::Copy(pSrc, pSrc, 4);
				srcFile.Verify(srcFile.data);
				Assert::Fail(L"Expected exception");
			}
			catch (CFileException* pE)
			{
				pE->Delete();
			}
		}

		TEST_METHOD(Util_FileCopy)
		{
			File srcFile(L"1", 9, 0);
			File dstFile(L"2", 5, 0);

			try
			{
				FileCopy::Copy(srcFile.Create(), dstFile.Create(), 4);
				dstFile.Verify(srcFile.data);
			}
			catch (CFileException* pE)
			{
				Assert::Fail(L"unexpected exception");
				pE->Delete();
			}
		}

		TEST_METHOD(Util_FileCopy_Copy_hidden)
		{
			File srcFile(L"1", 9, FILE_ATTRIBUTE_HIDDEN);
			File dstFile(L"2", 5, FILE_ATTRIBUTE_HIDDEN);

			try
			{
				FileCopy::Copy(srcFile.Create(), dstFile.Create(), 4);
				dstFile.Verify(srcFile.data);
			}
			catch (CFileException* pE)
			{
				Assert::Fail(L"unexpected exception");
				pE->Delete();
			}
		}

		TEST_METHOD(Util_FileCopy_Copy_readonly)
		{
			File srcFile(L"1", 9, FILE_ATTRIBUTE_READONLY);
			File dstFile(L"2", 5, FILE_ATTRIBUTE_READONLY);

			try
			{
				FileCopy::Copy(srcFile.Create(), dstFile.Create(), 4);
				dstFile.Verify(srcFile.data);
			}
			catch (CFileException* pE)
			{
				Assert::Fail(L"unexpected exception");
				pE->Delete();
			}
		}

		TEST_METHOD(Util_FileCopy_Copy_len0)
		{
			File srcFile(L"1", 0, 0);
			File dstFile(L"2", 5, 0);

			try
			{
				FileCopy::Copy(srcFile.Create(), dstFile.Create(), 4);
				dstFile.Verify(srcFile.data);
			}
			catch (CFileException* pE)
			{
				Assert::Fail(L"unexpected exception");
				pE->Delete();
			}
		}

		TEST_METHOD(Util_FileCopy_Copy_len4)
		{
			File srcFile(L"1", 4, 0);
			File dstFile(L"2", 5, 0);

			try
			{
				FileCopy::Copy(srcFile.Create(), dstFile.Create(), 4);
				dstFile.Verify(srcFile.data);
			}
			catch (CFileException* pE)
			{
				Assert::Fail(L"unexpected exception");
				pE->Delete();
			}
		}

		TEST_METHOD(Util_FileCopy_Copy_len8)
		{
			File srcFile(L"1", 8, 0);
			File dstFile(L"2", 5, 0);

			try
			{
				FileCopy::Copy(srcFile.Create(), dstFile.Create(), 4);
				dstFile.Verify(srcFile.data);
			}
			catch (CFileException* pE)
			{
				Assert::Fail(L"unexpected exception");
				pE->Delete();
			}
		}
	};
}
#endif //TODO-PORT