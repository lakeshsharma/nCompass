#include "stdafx.h"
#include "FileHook.h"
#include "Util\FileSys.h"
#include "Util\Path.h"
#include "Util\OutputDebugFormat.h"
#include "Detours\include\detours.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

namespace FileHook
{
	typedef HANDLE (WINAPI* _CreateFileW)(
		LPCWSTR               pFileName,
		DWORD                 desiredAccess,
		DWORD                 shareMode,
		LPSECURITY_ATTRIBUTES pSecurityAttributes,
		DWORD                 creationDisposition,
		DWORD                 flagsAndAttributes,
		HANDLE                hTemplateFile);

	typedef BOOL (WINAPI* _CloseHandle)(
		HANDLE hObject);

	typedef BOOL (WINAPI* _ReadFile)(
		HANDLE       hFile,
		LPVOID       pBuffer,
		DWORD        numberOfBytesToRead,
		LPDWORD      numberOfBytesRead,
		LPOVERLAPPED pOverlapped);

	typedef BOOL (WINAPI* _WriteFile)(
		HANDLE       hFile,
		LPCVOID      pBuffer,
		DWORD        numberOfBytesToWrite,
		LPDWORD      numberOfBytesWritten,
		LPOVERLAPPED pOverlapped);

	typedef DWORD (WINAPI* _SetFilePointer)(
		HANDLE hFile,
		LONG   distanceToMove,
		PLONG  pDistanceToMoveHigh,
		DWORD  moveMethod);

	typedef BOOL (WINAPI* _SetEndOfFile)(
		HANDLE hFile);

	typedef BOOL (WINAPI* _DeleteFileW)(
		LPCWSTR pFileName);

	///////////////////////////////////////////////////////////////////////////

	static _CreateFileW    CreateFileW_True    = CreateFileW;
	static _CloseHandle    CloseHandle_True    = CloseHandle;
	static _ReadFile       ReadFile_True       = ReadFile;
	static _WriteFile      WriteFile_True      = WriteFile;
	static _SetFilePointer SetFilePointer_True = SetFilePointer;
	static _SetEndOfFile   SetEndOfFile_True   = SetEndOfFile;
	static _DeleteFileW    DeleteFileW_True    = DeleteFileW;

	static ApisToHook apisHooked = (ApisToHook) 0;

	//////////////////////////////////////////////////////////////////////////

	struct FileHookFile
	{
		CString filename;
		HANDLE  hHandle;
		void*	pParam;
		bool	alwaysClose;

		DWORD createFileWError;
		DWORD closeHandleError;
		DWORD readFileError;
		DWORD writeFileError;
		DWORD setFilePointerError;
		DWORD setEndOfFileError;
		DWORD deleteFileError;

		CreateFileWFunc    createFileWFunc;
		CloseHandleFunc    closeHandleFunc;
		ReadFileFunc       readFileFunc;
		WriteFileFunc      writeFileFunc;
		SetFilePointerFunc setFilePointerFunc;
		SetEndOfFileFunc   setEndOfFileFunc;
		DeleteFileWFunc    deleteFileFunc;

		FileHookFile(LPCWSTR pFileName)
			: filename(pFileName)
			, hHandle(INVALID_HANDLE_VALUE)
			, pParam(nullptr)
			, alwaysClose(true)
			, createFileWError(0)
			, closeHandleError(0)
			, readFileError(0)
			, writeFileError(0)
			, setFilePointerError(0)
			, setEndOfFileError(0)
			, deleteFileError(0)
			, createFileWFunc(nullptr)
			, closeHandleFunc(nullptr)
			, readFileFunc(nullptr)
			, writeFileFunc(nullptr)
			, setFilePointerFunc(nullptr)
			, setEndOfFileFunc(nullptr)
			, deleteFileFunc(nullptr)
		{
		}
	};

	static ::CCriticalSection filenameMapCS;
	static map<CString, shared_ptr<FileHookFile>> filenameMap;

	static ::CCriticalSection handleMapCS;
	static map<HANDLE, shared_ptr<FileHookFile>> handleMap;

	static ::CCriticalSection setFuncErrorCS;

	static shared_ptr<FileHookFile> GetOrCreate(LPCWSTR pFilename)
	{
		CSingleLock lock(&filenameMapCS, TRUE);

		auto it = filenameMap.find(pFilename);
		if (it != filenameMap.end())
			return it->second;

		auto file = make_shared<FileHookFile>(pFilename);
		filenameMap.insert(make_pair(CString(pFilename), file));
		return file;
	}

	static shared_ptr<FileHookFile> GetFileHookFile(LPCWSTR pFileName)
	{
		CSingleLock lock(&filenameMapCS, TRUE);
		auto it = filenameMap.find(pFileName);
		return it != filenameMap.end() ? it->second : shared_ptr<FileHookFile>();
	}

	static shared_ptr<FileHookFile> GetFileHookFile(HANDLE hHandle)
	{
		CSingleLock lock(&handleMapCS, TRUE);
		auto it = handleMap.find(hHandle);
		return it != handleMap.end() ? it->second : shared_ptr<FileHookFile>();
	}

	static void ClearHandle(shared_ptr<FileHookFile> file)
	{
		if (file)
		{
			CSingleLock lock(&handleMapCS, TRUE);
			if (FileSys::IsValidHandle(file->hHandle))
			{
				auto it = handleMap.find(file->hHandle);
				if (it != handleMap.end())
				{
					handleMap.erase(it);
				}
				file->hHandle = INVALID_HANDLE_VALUE;
			}
		}
	}

	static bool SetHandle(shared_ptr<FileHookFile> file, HANDLE hHandle)
	{
		bool success = false;
		if (file)
		{
			CSingleLock lock(&handleMapCS, TRUE);
			ClearHandle(file);
			file->hHandle = hHandle;
			if (FileSys::IsValidHandle(hHandle))
			{
				handleMap[hHandle] = file;
				success = true;
			}
		}
		return success;
	}

	//////////////////////////////////////////////////////////////////////////

	static HANDLE WINAPI CreateFileW_Hook(
		LPCWSTR               pFileName,
		DWORD                 desiredAccess,
		DWORD                 shareMode,
		LPSECURITY_ATTRIBUTES pSecurityAttributes,
		DWORD                 creationDisposition,
		DWORD                 flagsAndAttributes,
		HANDLE                hTemplateFile)
	{
		if (apisHooked & HookCreateFileW)
		{
			shared_ptr<FileHookFile> file = GetFileHookFile(pFileName);
			if (file)
			{
				if (FileSys::IsValidHandle(file->hHandle))
				{
					SetLastError(ERROR_NOT_SUPPORTED);
					return INVALID_HANDLE_VALUE;
				}

				CSingleLock(&setFuncErrorCS, TRUE);

				if (file->createFileWFunc != nullptr)
				{
					auto result = file->createFileWFunc(
						pFileName, 
						desiredAccess,
						shareMode,
						creationDisposition,
						flagsAndAttributes);

					if (result.haltProcessing)
					{
						SetHandle(file, result.returnHandle);
						SetLastError(result.lastError);
						return result.returnHandle;
					}
				}
				
				if (file->createFileWError != 0)
				{
					SetLastError(file->createFileWError);
					return INVALID_HANDLE_VALUE;
				}
			}
		}

		HANDLE hFile = CreateFileW_True(
			pFileName,
			desiredAccess,
			shareMode,
			pSecurityAttributes,
			creationDisposition,
			flagsAndAttributes,
			hTemplateFile);

		if ((apisHooked & HookCreateFileW) && FileSys::IsValidHandle(hFile))
		{
			SetHandle(GetFileHookFile(pFileName), hFile);
		}

		return hFile;
	}

	static BOOL WINAPI CloseHandle_Hook(
		HANDLE hObject)
	{
		if (apisHooked & HookCloseHandle)
		{
			shared_ptr<FileHookFile> file = GetFileHookFile(hObject);
			if (file)
			{
				CSingleLock(&setFuncErrorCS, TRUE);

				if (file->closeHandleFunc != nullptr)
				{
					bool alwaysClose = file->alwaysClose;
					auto result = file->closeHandleFunc(file->filename, file->hHandle, alwaysClose);
					ClearHandle(file);
					if (result.haltProcessing)
					{
						if (alwaysClose)
						{
							CloseHandle_True(hObject);
						}
						SetLastError(result.lastError);
						return result.returnBool;
					}
				}
				
				if (file->closeHandleError)
				{
					ClearHandle(file);
					if (file->alwaysClose)
					{
						CloseHandle_True(hObject);
					}
					SetLastError(file->closeHandleError);
					return FALSE;
				}
			}
		}

		return CloseHandle_True(
			hObject);
	}

	static BOOL WINAPI ReadFile_Hook(
		HANDLE       hFile,
		LPVOID       pBuffer,
		DWORD        bytesToRead,
		LPDWORD      pBytesRead,
		LPOVERLAPPED pOverlapped)
	{
		if (apisHooked & HookReadFile)
		{
			shared_ptr<FileHookFile> file = GetFileHookFile(hFile);
			if (file)
			{
				CSingleLock(&setFuncErrorCS, TRUE);

				if (file->readFileFunc != nullptr)
				{
					auto result = file->readFileFunc(
						file->filename,
						hFile,
						pBuffer,
						bytesToRead,
						pBytesRead);

					if (result.haltProcessing)
					{
						SetLastError(result.lastError);
						return result.returnBool;
					}
				}
				
				if (file->readFileError)
				{
					SetLastError(file->readFileError);
					return FALSE;
				}
			}
		}

		return ReadFile_True(
			hFile,
			pBuffer,
			bytesToRead,
			pBytesRead,
			pOverlapped);
	}

	static BOOL WINAPI WriteFile_Hook(
		HANDLE       hFile,
		LPCVOID      pBuffer,
		DWORD        bytesToWrite,
		LPDWORD      pBytesWritten,
		LPOVERLAPPED pOverlapped)
	{
		if (apisHooked & HookWriteFile)
		{
			shared_ptr<FileHookFile> file = GetFileHookFile(hFile);
			if (file)
			{
				CSingleLock(&setFuncErrorCS, TRUE);

				if (file->writeFileFunc != nullptr)
				{
					auto result = file->writeFileFunc(
						file->filename,
						hFile,
						pBuffer,
						bytesToWrite,
						pBytesWritten);

					if (result.haltProcessing)
					{
						SetLastError(result.lastError);
						return result.returnBool;
					}
				}
				
				if (file->writeFileError)
				{
					SetLastError(file->writeFileError);
					return FALSE;
				}
			}
		}

		return WriteFile_True(
			hFile,
			pBuffer,
			bytesToWrite,
			pBytesWritten,
			pOverlapped);
	}

	static DWORD WINAPI SetFilePointer_Hook(
		HANDLE hFile,
		LONG   distance,
		PLONG  pDistanceHigh,
		DWORD  moveMethod)
	{
		if (apisHooked & HookSetFilePointer)
		{
			shared_ptr<FileHookFile> file = GetFileHookFile(hFile);
			if (file)
			{
				CSingleLock(&setFuncErrorCS, TRUE);

				if (file->setFilePointerFunc != nullptr)
				{
					auto result = file->setFilePointerFunc(
						file->filename,
						hFile,
						distance,
						pDistanceHigh,
						moveMethod);

					if (result.haltProcessing)
					{
						SetLastError(result.lastError);
						return result.returnDword;
					}
				}
				
				if (file->setFilePointerError)
				{
					SetLastError(file->setFilePointerError);
					return INVALID_SET_FILE_POINTER;
				}
			}
		}

		return SetFilePointer_True(
			hFile,
			distance,
			pDistanceHigh,
			moveMethod);
	}

	static BOOL WINAPI SetEndOfFile_Hook(
		HANDLE hFile)
	{
		if (apisHooked & HookSetEndOfFile)
		{
			shared_ptr<FileHookFile> file = GetFileHookFile(hFile);
			if (file)
			{
				CSingleLock(&setFuncErrorCS, TRUE);

				if (file->setEndOfFileFunc != nullptr)
				{
					auto result = file->setEndOfFileFunc(file->filename, hFile);
					if (result.haltProcessing)
					{
						SetLastError(result.lastError);
						return result.returnBool;
					}
				}
				
				if (file->setEndOfFileError)
				{
					SetLastError(file->setEndOfFileError);
					return FALSE;
				}
			}
		}

		return SetEndOfFile_True(
			hFile);
	}

	static BOOL WINAPI DeleteFileW_Hook(
		LPCWSTR pFileName)
	{
		if (apisHooked & HookDeleteFileW)
		{
			shared_ptr<FileHookFile> file = GetFileHookFile(pFileName);
			if (file)
			{
				CSingleLock(&setFuncErrorCS, TRUE);

				if (file->deleteFileFunc != nullptr)
				{
					auto result = file->deleteFileFunc(file->filename);
					if (result.haltProcessing)
					{
						SetLastError(result.lastError);
						return result.returnBool;
					}
				}
				
				if (file->deleteFileError)
				{
					SetLastError(file->deleteFileError);
					return FALSE;
				}
			}
		}

		return DeleteFileW_True(
			pFileName);
	}

	///////////////////////////////////////////////////////////////////////////

	LONG CreateFileHooks(ApisToHook apisToHook)
	{
		if (apisHooked)
			return ERROR_INVALID_OPERATION;

		apisToHook = (ApisToHook) (apisToHook | HookCreateFileW | HookCloseHandle);

		LONG result = DetourTransactionBegin();
		result = DetourUpdateThread(GetCurrentThread());
		result = DetourAttach(&(PVOID&) CreateFileW_True, CreateFileW_Hook);
		result = DetourAttach(&(PVOID&) CloseHandle_True, CloseHandle_Hook);

		if (apisToHook & HookReadFile)
			DetourAttach(&(PVOID&) ReadFile_True, ReadFile_Hook);

		if (apisToHook & HookWriteFile)
			DetourAttach(&(PVOID&) WriteFile_True, WriteFile_Hook);

		if (apisToHook & HookSetFilePointer)
			DetourAttach(&(PVOID&) SetFilePointer_True, SetFilePointer_Hook);

		if (apisToHook & HookSetEndOfFile)
			DetourAttach(&(PVOID&) SetEndOfFile_True, SetEndOfFile_Hook);

		if (apisToHook & HookDeleteFileW)
			DetourAttach(&(PVOID&) DeleteFileW_True, DeleteFileW_Hook);

		result = DetourTransactionCommit();
		if (result == NO_ERROR)
		{
			apisHooked = apisToHook;
		}
		return result;
	}

	LONG ClearFileHooks()
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourDetach(&(PVOID&) CreateFileW_True, CreateFileW_Hook);
		DetourDetach(&(PVOID&) CloseHandle_True, CloseHandle_Hook);

		if (apisHooked & HookReadFile)
			DetourDetach(&(PVOID&) ReadFile_True, ReadFile_Hook);

		if (apisHooked & HookWriteFile)
			DetourDetach(&(PVOID&) WriteFile_True, WriteFile_Hook);

		if (apisHooked & HookSetFilePointer)
			DetourDetach(&(PVOID&) SetFilePointer_True, SetFilePointer_Hook);

		if (apisHooked & HookSetEndOfFile)
			DetourDetach(&(PVOID&) SetEndOfFile_True, SetEndOfFile_Hook);

		if (apisHooked & HookDeleteFileW)
			DetourDetach(&(PVOID&) DeleteFileW_True, DeleteFileW_Hook);

		LONG result = DetourTransactionCommit();

		apisHooked = (ApisToHook) 0;
		handleMap.clear();
		filenameMap.clear();

		return result;
	}

	bool RegisterFile(LPCWSTR pFilename, void* pParam, bool alwaysClose)
	{
		auto file = GetOrCreate(pFilename);
		if (file)
		{
			file->pParam = pParam;
			file->alwaysClose = alwaysClose;
		}
		return (bool) file;
	}

	bool CreateFileW_SetFilter(LPCWSTR pFilename, CreateFileWFunc func, DWORD error)
	{
		bool success = false;
		auto file = GetOrCreate(pFilename);
		if (file)
		{
			CSingleLock(&setFuncErrorCS, TRUE);
			file->createFileWError = error;
			file->createFileWFunc = func;
			success = true;
		}
		return success;
	}

	bool CloseHandle_SetFilter(LPCWSTR pFilename, CloseHandleFunc func, DWORD error, bool alwaysClose)
	{
		bool success = false;
		auto file = GetOrCreate(pFilename);
		if (file)
		{
			CSingleLock(&setFuncErrorCS, TRUE);
			file->closeHandleError = error;
			file->closeHandleFunc = func;
			file->alwaysClose = alwaysClose;
			success = true;
		}
		return success;
	}

	bool ReadFile_SetFilter(LPCWSTR pFilename, ReadFileFunc func, DWORD error)
	{
		bool success = false;
		auto file = GetOrCreate(pFilename);
		if (file)
		{
			CSingleLock(&setFuncErrorCS, TRUE);
			file->readFileError = error;
			file->readFileFunc = func;
			success = true;
		}
		return success;
	}

	bool WriteFile_SetFilter(LPCWSTR pFilename, WriteFileFunc func, DWORD error)
	{
		bool success = false;
		auto file = GetOrCreate(pFilename);
		if (file)
		{
			CSingleLock(&setFuncErrorCS, TRUE);
			file->writeFileError = error;
			file->writeFileFunc = func;
			success = true;
		}
		return success;
	}

	bool SetFilePointer_SetFilter(LPCWSTR pFilename, SetFilePointerFunc func, DWORD error)
	{
		bool success = false;
		auto file = GetOrCreate(pFilename);
		if (file)
		{
			CSingleLock(&setFuncErrorCS, TRUE);
			file->setFilePointerError = error;
			file->setFilePointerFunc = func;
			success = true;
		}
		return success;
	}

	bool SetEndOfFile_SetFilter(LPCWSTR pFilename, SetEndOfFileFunc func, DWORD error)
	{
		bool success = false;
		auto file = GetOrCreate(pFilename);
		if (file)
		{
			CSingleLock(&setFuncErrorCS, TRUE);
			file->setEndOfFileError = error;
			file->setEndOfFileFunc = func;
			success = true;
		}
		return success;
	}

	bool DeleteFileW_SetFilter(LPCWSTR pFilename, DeleteFileWFunc func, DWORD error)
	{
		bool success = false;
		auto file = GetOrCreate(pFilename);
		if (file)
		{
			CSingleLock(&setFuncErrorCS, TRUE);
			file->deleteFileError = error;
			file->deleteFileFunc = func;
			success = true;
		}
		return success;
	}
};