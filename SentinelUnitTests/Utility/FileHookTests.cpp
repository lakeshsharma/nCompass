#include "stdafx.h"
#include "Util\FileSys.h"
#include "TestUtil\TestUtil.h"
#include "TestUtil\FileHook.h"
#include "Util\OutputDebugFormat.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
	struct FileHandle
	{
		HANDLE hFile;
		FileHandle(HANDLE hFile)
			: hFile(hFile)
		{
		}

		~FileHandle()
		{
			CloseHandle(hFile);
		}

		bool IsValid() const
		{
			return FileSys::IsValidHandle(hFile);
		}

		operator HANDLE() const
		{
			return hFile;
		}
	};

	/*
	For every type of hook (except Write) there is a test with hooks in place, but
	which do nothing. The Write case is covered in the Read.
	*/

	TEST_CLASS(FileHookTests)
	{
		TEST_METHOD_INITIALIZE(FileHookTests_Init)
		{
			TestUtil::CreateSentinelDirectory(DIRECTORY_SENTINELDATA);
		}

		TEST_METHOD_CLEANUP(FileHookTests_Cleanup)
		{
			FileHook::ClearFileHooks();
			TestUtil::DeleteSentinelDirectories();
		}

		TEST_METHOD(FileHookTests_CreateFileW)
		{
			CString filename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + L"1";
			Assert::AreEqual(NO_ERROR, FileHook::CreateFileHooks(FileHook::HookAll), L"CreateFileHooks");

			FileHandle fileHandle(CreateFile(filename, GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nullptr));
			DWORD err = GetLastError();
			Assert::IsTrue(fileHandle.IsValid(), AssertMsg::FormatOnly(L"CreateFile failed: %u", err));
		}

		TEST_METHOD(FileHookTests_CreateFileW_error)
		{
			CString filename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + L"1";
			Assert::AreEqual(NO_ERROR, FileHook::CreateFileHooks(FileHook::HookAll), L"CreateFileHooks");

			bool result = FileHook::CreateFileW_SetFilter(filename, nullptr, 1234567);
			Assert::IsTrue(result, L"FileHook::CreateFileW_SetFilter");

			HANDLE h = CreateFile(filename, GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nullptr);
			DWORD err = GetLastError();

			Assert::IsFalse(FileSys::IsValidHandle(h), AssertMsg::FormatOnly(L"CreateFile failed: %u", err));
			Assert::AreEqual((DWORD) 1234567, err, L"GetLastError");
		}

		TEST_METHOD(FileHookTests_CreateFileW_func)
		{
			CString filename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + L"1";
			Assert::AreEqual(NO_ERROR, FileHook::CreateFileHooks(FileHook::HookAll), L"CreateFileHooks");

			bool result = FileHook::CreateFileW_SetFilter(filename, [](
				LPCWSTR, DWORD, DWORD, DWORD, DWORD)->FileHook::FuncReturn
			{
				return FileHook::FuncReturn((HANDLE) 1, 1234567, true);
			});
			Assert::IsTrue(result, L"FileHook::CreateFileW_SetFilter");

			HANDLE h = CreateFile(filename, GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nullptr);
			DWORD err = GetLastError();
			Assert::AreEqual((INT_PTR) 1, (INT_PTR) h, AssertMsg::FormatOnly(L"CreateFile failed: %u", err));
			Assert::AreEqual((DWORD) 1234567, err, L"GetLastError");
		}

		// This also doubles as FileHookTests_SetCondition_Write
		TEST_METHOD(FileHookTests_ReadFile)
		{
			CString filename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + L"1";
			Assert::AreEqual(NO_ERROR, FileHook::CreateFileHooks(FileHook::HookAll), L"CreateFileHooks");

			FileHandle fileHandle(CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nullptr));
			DWORD err = GetLastError();
			Assert::IsTrue(fileHandle.IsValid(), AssertMsg::FormatOnly(L"CreateFile failed: %u", err));

			DWORD bytesWritten;
			BOOL result = WriteFile(fileHandle, "1", 1, &bytesWritten, nullptr);
			Assert::IsTrue(result, L"WriteFile result");
			Assert::AreEqual((DWORD) 0, err, L"GetLastError");

			char ch;
			DWORD bytesRead;
			result = ReadFile(fileHandle, &ch, 1, &bytesRead, nullptr);
			err = GetLastError();
			Assert::IsTrue(result, L"ReadFile result");
			Assert::AreEqual((DWORD) 0, err, L"GetLastError");
		}

		TEST_METHOD(FileHookTests_ReadFile_error)
		{
			CString filename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + L"1";
			Assert::AreEqual(NO_ERROR, FileHook::CreateFileHooks(FileHook::HookAll), L"CreateFileHooks");

			bool setResult = FileHook::ReadFile_SetFilter(filename, nullptr, ERROR_ACCESS_DENIED);
			Assert::IsTrue(setResult, L"FileHook::ReadFile_SetFilter");

			FileHandle fileHandle(CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nullptr));
			DWORD err = GetLastError();
			Assert::IsTrue(fileHandle.IsValid(), AssertMsg::FormatOnly(L"CreateFile failed: %u", err));

			DWORD bytesWritten;
			BOOL result = WriteFile(fileHandle, "1", 1, &bytesWritten, nullptr);
			Assert::IsTrue(result, AssertMsg::FormatOnly(L"WriteFile result: %u", err));
			Assert::AreEqual((DWORD) 0, err, L"GetLastError");

			char ch;
			DWORD bytesRead;
			result = ReadFile(fileHandle, &ch, 1, &bytesRead, nullptr);
			err = GetLastError();
			Assert::IsFalse(result, AssertMsg::FormatOnly(L"ReadFile result. %u", err));
			Assert::AreEqual((DWORD) ERROR_ACCESS_DENIED, err, L"GetLastError");
		}

		TEST_METHOD(FileHookTests_ReadFile_func)
		{
			CString filename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + L"1";
			Assert::AreEqual(NO_ERROR, FileHook::CreateFileHooks(FileHook::HookAll), L"CreateFileHooks");

			bool setResult = FileHook::ReadFile_SetFilter(filename, [](
				LPCWSTR, HANDLE, LPVOID, DWORD, PDWORD)->FileHook::FuncReturn
			{
				return FileHook::FuncReturn(FALSE, ERROR_ACCESS_DENIED, true);
			});
			Assert::IsTrue(setResult, L"FileHook::ReadFile_SetFilter");

			FileHandle fileHandle(CreateFile(filename, GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nullptr));
			DWORD err = GetLastError();
			Assert::IsTrue(fileHandle.IsValid(), AssertMsg::FormatOnly(L"CreateFile failed: %u", err));

			DWORD bytesWritten;
			BOOL result = WriteFile(fileHandle, "1", 1, &bytesWritten, nullptr);
			err = GetLastError();

			Assert::IsTrue(result, AssertMsg::FormatOnly(L"WriteFile result: %u", err));
			Assert::AreEqual((DWORD) 0, err, L"GetLastError");

			char ch;
			DWORD bytesRead;
			result = ReadFile(fileHandle, &ch, 1, &bytesRead, nullptr);
			err = GetLastError();
			Assert::IsFalse(result, L"ReadFile result");
		}

		TEST_METHOD(FileHookTests_WriteFile_error)
		{
			CString filename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + L"1";
			Assert::AreEqual(NO_ERROR, FileHook::CreateFileHooks(FileHook::HookAll), L"CreateFileHooks");

			bool setResult = FileHook::WriteFile_SetFilter(filename, nullptr, ERROR_HANDLE_DISK_FULL);
			Assert::IsTrue(setResult, L"FileHook::WriteFile_SetFilter");

			FileHandle fileHandle(CreateFile(filename, GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nullptr));
			DWORD err = GetLastError();
			Assert::IsTrue(fileHandle.IsValid(), AssertMsg::FormatOnly(L"CreateFile failed: %u", err));

			DWORD bytesWritten;
			BOOL result = WriteFile(fileHandle, "1", 1, &bytesWritten, nullptr);
			err = GetLastError();
			Assert::IsFalse(result, L"WriteFile result");
			Assert::AreEqual((DWORD) ERROR_HANDLE_DISK_FULL, err, L"GetLastError");
		}

		TEST_METHOD(FileHookTests_WriteFile_func)
		{
			CString filename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + L"1";
			Assert::AreEqual(NO_ERROR, FileHook::CreateFileHooks(FileHook::HookAll), L"CreateFileHooks");

			bool setResult = FileHook::WriteFile_SetFilter(filename, [](
				LPCWSTR, HANDLE, LPCVOID, DWORD, PDWORD)->FileHook::FuncReturn
			{
				return FileHook::FuncReturn(FALSE, ERROR_HANDLE_DISK_FULL, true);
			});
			Assert::IsTrue(setResult, L"FileHook::WriteFile_SetFilter");

			FileHandle fileHandle(CreateFile(filename, GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nullptr));
			DWORD err = GetLastError();
			Assert::IsTrue(fileHandle.IsValid(), AssertMsg::FormatOnly(L"CreateFile failed: %u", err));

			DWORD bytesWritten;
			BOOL result = WriteFile(fileHandle, "1", 1, &bytesWritten, nullptr);
			err = GetLastError();
			Assert::IsFalse(result, L"WriteFile result");
			Assert::AreEqual((DWORD) ERROR_HANDLE_DISK_FULL, err, L"GetLastError");
		}

		TEST_METHOD(FileHookTests_SetFilePointer)
		{
			CString filename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + L"1";
			Assert::AreEqual(NO_ERROR, FileHook::CreateFileHooks(FileHook::HookAll), L"CreateFileHooks");

			FileHandle fileHandle(CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nullptr));
			DWORD err = GetLastError();
			Assert::IsTrue(fileHandle.IsValid(), AssertMsg::FormatOnly(L"CreateFile failed: %u", err));

			DWORD result = SetFilePointer(fileHandle, 8, nullptr, FILE_BEGIN);
			Assert::AreEqual((DWORD) 8, result, L"SetFilePointer result");
		}

		TEST_METHOD(FileHookTests_SetFilePointer_error)
		{
			CString filename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + L"1";
			Assert::AreEqual(NO_ERROR, FileHook::CreateFileHooks(FileHook::HookAll), L"CreateFileHooks");

			bool setResult = FileHook::SetFilePointer_SetFilter(filename, nullptr, INVALID_SET_FILE_POINTER);
			Assert::IsTrue(setResult, L"FileHook::SetFilePointer_SetFilter");

			FileHandle fileHandle(CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nullptr));
			DWORD err = GetLastError();
			Assert::IsTrue(fileHandle.IsValid(), AssertMsg::FormatOnly(L"CreateFile failed: %u", err));

			DWORD result = SetFilePointer(fileHandle, 8, nullptr, FILE_BEGIN);
			Assert::AreEqual((DWORD) INVALID_SET_FILE_POINTER, result, L"SetFilePointer result");
		}

		TEST_METHOD(FileHookTests_SetFilePointer_func)
		{
			CString filename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + L"1";
			Assert::AreEqual(NO_ERROR, FileHook::CreateFileHooks(FileHook::HookAll), L"CreateFileHooks");

			bool setResult = FileHook::SetFilePointer_SetFilter(filename, [](
				LPCWSTR, HANDLE, LONG, PLONG, DWORD)->FileHook::FuncReturn
			{
				return FileHook::FuncReturn(INVALID_SET_FILE_POINTER, INVALID_SET_FILE_POINTER, true);
			});
			Assert::IsTrue(setResult, L"FileHook::SetFilePointer_SetFilter");

			FileHandle fileHandle(CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nullptr));
			DWORD err = GetLastError();
			Assert::IsTrue(fileHandle.IsValid(), AssertMsg::FormatOnly(L"CreateFile failed: %u", err));

			DWORD result = SetFilePointer(fileHandle, 8, nullptr, FILE_BEGIN);
			Assert::AreEqual((DWORD) INVALID_SET_FILE_POINTER, result, L"SetFilePointer result");
		}

		TEST_METHOD(FileHookTests_SetEndOfFile)
		{
			CString filename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + L"1";
			Assert::AreEqual(NO_ERROR, FileHook::CreateFileHooks(FileHook::HookAll), L"CreateFileHooks");

			FileHandle fileHandle(CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nullptr));
			DWORD err = GetLastError();
			Assert::IsTrue(fileHandle.IsValid(), AssertMsg::FormatOnly(L"CreateFile failed: %u", err));

			DWORD result = SetFilePointer(fileHandle, 8, nullptr, FILE_BEGIN);
			Assert::AreEqual((DWORD) 8, result, L"SetFilePointer result");

			BOOL result2 = SetEndOfFile(fileHandle);
			Assert::IsTrue(result2, L"SetEndOfFile result");
		}

		TEST_METHOD(FileHookTests_SetEndOfFile_error)
		{
			CString filename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + L"1";
			Assert::AreEqual(NO_ERROR, FileHook::CreateFileHooks(FileHook::HookAll), L"CreateFileHooks");

			bool setResult = FileHook::SetEndOfFile_SetFilter(filename, nullptr, ERROR_HANDLE_DISK_FULL);
			Assert::IsTrue(setResult, L"FileHook::SetEndOfFile_SetFilter");

			FileHandle fileHandle(CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nullptr));
			DWORD err = GetLastError();
			Assert::IsTrue(fileHandle.IsValid(), AssertMsg::FormatOnly(L"CreateFile failed: %u", err));

			DWORD result = SetFilePointer(fileHandle, 8, nullptr, FILE_BEGIN);
			Assert::AreEqual((DWORD) 8, result, L"SetFilePointer result");

			BOOL result2 = SetEndOfFile(fileHandle);
			err = GetLastError();
			Assert::IsFalse(result2, L"SetEndOfFile result");
			Assert::AreEqual((DWORD) ERROR_HANDLE_DISK_FULL, err, L"SetEndOfFile GetLastError");
		}

		TEST_METHOD(FileHookTests_SetEndOfFile_func)
		{
			CString filename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + L"1";
			Assert::AreEqual(NO_ERROR, FileHook::CreateFileHooks(FileHook::HookAll), L"CreateFileHooks");

			bool setResult = FileHook::SetEndOfFile_SetFilter(filename, [](
				LPCWSTR, HANDLE)->FileHook::FuncReturn
			{
				return FileHook::FuncReturn(FALSE, ERROR_HANDLE_DISK_FULL, true);
			});
			Assert::IsTrue(setResult, L"FileHook::SetEndOfFile_SetFilter");

			FileHandle fileHandle(CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, nullptr));
			DWORD err = GetLastError();
			Assert::IsTrue(fileHandle.IsValid(), AssertMsg::FormatOnly(L"CreateFile failed: %u", err));

			DWORD result = SetFilePointer(fileHandle, 8, nullptr, FILE_BEGIN);
			Assert::AreEqual((DWORD) 8, result, L"SetFilePointer result");

			BOOL result2 = SetEndOfFile(fileHandle);
			err = GetLastError();
			Assert::IsFalse(result2, L"SetEndOfFile result");
			Assert::AreEqual((DWORD) ERROR_HANDLE_DISK_FULL, err, L"SetEndOfFile GetLastError");
		}
	};
}