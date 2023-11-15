#if 000//TODO-PORT
#include "stdafx.h"
#include "Util/FileInfo.h"
#include "TestUtil/AssertMsg.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
	struct
	{
		int line;
		const wchar_t* pPath;
		WIN32_FIND_DATAW findData;
		struct
		{
			const wchar_t* pPath;
			const wchar_t* pFileName;
			uint32_t attributes;
			uint64_t creationTime;
			uint64_t lastWriteTime;
			uint64_t fileSize;

		} expected;

		void Verify(const FileInfo& fileInfo, const wchar_t* pTest = nullptr)
		{
			Assert::AreEqual(expected.pPath, fileInfo.path.c_str(), AssertMsg::Format(line, L"path", pTest));
			Assert::AreEqual(expected.pFileName, fileInfo.filename.c_str(), AssertMsg::Format(line, L"filename", pTest));
			Assert::AreEqual(expected.attributes, fileInfo.attributes, AssertMsg::Format(line, L"attributes", pTest));
			Assert::AreEqual(expected.creationTime, fileInfo.creationTime, AssertMsg::Format(line, L"creationTime", pTest));
			Assert::AreEqual(expected.lastWriteTime, fileInfo.lastWriteTime, AssertMsg::Format(line, L"lastWriteTime", pTest));
			Assert::AreEqual(expected.fileSize, fileInfo.fileSize, AssertMsg::Format(line, L"fileSize", pTest));
		}

		void VerifyEmpty(const FileInfo& fileInfo, const wchar_t* pTest = nullptr)
		{
			Assert::AreEqual(L"", fileInfo.path.c_str(), AssertMsg::Format(line, L"path", pTest));
			Assert::AreEqual(L"", fileInfo.filename.c_str(), AssertMsg::Format(line, L"filename", pTest));
			Assert::AreEqual((uint32_t) 0, fileInfo.attributes, AssertMsg::Format(line, L"attributes", pTest));
			Assert::AreEqual((uint64_t) 0, fileInfo.creationTime, AssertMsg::Format(line, L"creationTime", pTest));
			Assert::AreEqual((uint64_t) 0, fileInfo.lastWriteTime, AssertMsg::Format(line, L"lastWriteTime", pTest));
			Assert::AreEqual((uint64_t) 0, fileInfo.fileSize, AssertMsg::Format(line, L"fileSize", pTest));
		}
	}
	data[] =
	{
		{ __LINE__, nullptr,
			{},
			{ L"", L"", 0, 0, 0, 0 } },
		{ __LINE__, L"",
			{ FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
			{ L"", L"", FILE_ATTRIBUTE_NORMAL, 0x0000000200000001, 0x0000000600000005, 0x0000000700000008 } },
		{ __LINE__, L"Path",
			{ FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"File", L"xxx" },
			{ L"Path/", L"File", FILE_ATTRIBUTE_NORMAL, 0x0000000200000001, 0x0000000600000005, 0x0000000700000008 } },
	};

	TEST_CLASS(Util_FindFileTests)
	{
		/// 	struct WIN32_FIND_DATA
		/// 	{
		/// 	  DWORD    dwFileAttributes;
		/// 	  FILETIME ftCreationTime;
		/// 	  FILETIME ftLastAccessTime;
		/// 	  FILETIME ftLastWriteTime;
		/// 	  DWORD    nFileSizeHigh;
		/// 	  DWORD    nFileSizeLow;
		/// 	  DWORD    dwReserved0;
		/// 	  DWORD    dwReserved1;
		/// 	  TCHAR    cFileName[MAX_PATH];
		/// 	  TCHAR    cAlternateFileName[16];
		///		};

		TEST_METHOD(Util_FileInfo_ctor)
		{
			for (size_t i = 0; i < _countof(data); i++)
			{
				FileInfo fileInfo(data[i].pPath, data[i].findData);
				data[i].Verify(fileInfo);
			}
		}

		TEST_METHOD(Util_FileInfo_ctor2)
		{
			for (size_t i = 0; i < _countof(data); i++)
			{
				if (data[i].pPath)
				{
					wstring path(data[i].pPath);
					FileInfo fileInfo(path, data[i].findData);
					data[i].Verify(fileInfo);
				}
			}
		}

		TEST_METHOD(Util_FileInfo_cctor)
		{
			for (size_t i = 0; i < _countof(data); i++)
			{
				FileInfo fileInfoOrg(data[i].pPath, data[i].findData);
				FileInfo fileInfo(fileInfoOrg);
				data[i].Verify(fileInfo, L"Copy destination");
				data[i].Verify(fileInfoOrg, L"Copy source");
			}
		}

		TEST_METHOD(Util_FileInfo_mctor)
		{
			for (size_t i = 0; i < _countof(data); i++)
			{
				FileInfo fileInfoOrg(data[i].pPath, data[i].findData);
				FileInfo fileInfo(move(fileInfoOrg));
				data[i].Verify(fileInfo, L"Move ctor destination");
				data[i].VerifyEmpty(fileInfoOrg, L"Move ctor source");
			}
		}

		TEST_METHOD(Util_FileInfo_assignment)
		{
			for (size_t i = 0; i < _countof(data); i++)
			{
				FileInfo fileInfoOrg(data[i].pPath, data[i].findData);

				WIN32_FIND_DATAW findData;
				ZeroMemory(&findData, sizeof(findData));
				FileInfo fileInfo(nullptr, findData);

				fileInfo = fileInfoOrg;
				data[i].Verify(fileInfo, L"assignment destination");
			}
		}

		TEST_METHOD(Util_FileInfo_move)
		{
			for (size_t i = 0; i < _countof(data); i++)
			{
				FileInfo fileInfoOrg(data[i].pPath, data[i].findData);

				WIN32_FIND_DATAW findData;
				ZeroMemory(&findData, sizeof(findData));
				FileInfo fileInfo(nullptr, findData);

				fileInfo = move(fileInfoOrg);
				data[i].Verify(fileInfo, L"Move destination");
				data[i].VerifyEmpty(fileInfoOrg, L"Move source");
			}
		}

		TEST_METHOD(Util_FileInfo_comparison)
		{
			struct
			{
				int line;
				const wchar_t* pPathA;
				WIN32_FIND_DATAW findDataA;
				const wchar_t* pPathB;
				WIN32_FIND_DATAW findDataB;
				bool result;
			}
			_data[] =
			{
				{ __LINE__, 
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 1 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					false },

				{ __LINE__,
					L"", {},
					L"", {},
					true },
				{ __LINE__, 
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					true },
				{ __LINE__, 
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 1 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					false },
				{ __LINE__, 
					L"", { FILE_ATTRIBUTE_NORMAL,{ 2, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					false },
				{ __LINE__, 
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 4, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					true },
				{ __LINE__, 
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 5 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					true },
				{ __LINE__, 
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 5 },{ 6, 6 }, 7, 8, 9, 10, L"", L"" },
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					false },
				{ __LINE__, 
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 5 },{ 5, 7 }, 7, 8, 9, 10, L"", L"" },
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					false },
				{ __LINE__, 
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 5 },{ 5, 6 }, 8, 8, 9, 10, L"", L"" },
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					false },
				{ __LINE__, 
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 5 },{ 5, 6 }, 7, 9, 9, 10, L"", L"" },
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					false },
				{ __LINE__, 
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 5 },{ 5, 6 }, 7, 8, 8, 10, L"", L"" },
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					true },
				{ __LINE__, 
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 5 },{ 5, 6 }, 7, 8, 9,  9, L"", L"" },
					L"", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"", L"" },
					true },
				{ __LINE__, 
					L"Path", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"File", L"xxx" },
					L"Path", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"File", L"yyy" },
					true },
				{ __LINE__, 
					L"path", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"file", L"xxx" },
					L"path", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"file", L"yyy" },
					true },
				{ __LINE__, 
					L"path", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"file", L"xxx" },
					L"path", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"File", L"yyy" },
					true },
				{ __LINE__, 
					L"path", { FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"file", L"xxx" },
					L"path", { FILE_ATTRIBUTE_NORMAL,{ 1, 2 },{ 3, 4 },{ 5, 6 }, 7, 8, 9, 10, L"file", L"yyy" },
					false },
			};

			for (size_t i = 0; i < _countof(_data); i++)
			{
				FileInfo fileInfoA(_data[i].pPathA, _data[i].findDataA);
				FileInfo fileInfoB(_data[i].pPathB, _data[i].findDataB);

				Assert::AreEqual(_data[i].result, fileInfoA == fileInfoB, AssertMsg::Format(_data[i].line));
			}
		}
	};
}
#endif //TODO-PORT