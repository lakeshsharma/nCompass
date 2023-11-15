#pragma once

namespace FileHook
{

	///	<summary>
	///	</summary>
	///	<param name=""></param>
	///	<returns></returns>
	///	<remarks>
	/// Create and Close are always hooked
	///	</remarks>
	enum ApisToHook
	{
		HookCreateFileW    = 0x0001,
		HookCloseHandle    = 0x0002,
		HookReadFile       = 0x0004,
		HookWriteFile      = 0x0008,
		HookSetFilePointer = 0x0010,
		HookSetEndOfFile   = 0x0028,
		HookDeleteFileW    = 0x0048,
		// Create and Close are always hooked
		HookAll            = HookReadFile | HookWriteFile | HookSetFilePointer | HookSetEndOfFile | HookDeleteFileW
	};

	///	<summary>
	///	Creates the hooks.
	///	</summary>
	///	<remarks>
	///	This must be called first. Do NOT call this in TEST_CLASS_INITIALIZE, but
	///	either in TEST_METHOD_INITIALIZE or at the start of the test method itself.
	///	</remarks>
	LONG CreateFileHooks(ApisToHook apis);

	///	<summary>
	///	Clears the file hooks AND all registered files and filters.
	///	</summary>
	///	<remarks>
	/// If hooks are used anywhere in a test class method, THIS <b>MUST</b> BE CALLED
	///	IN TEST_METHOD_CLEANUP.
	///	</remarks>
	LONG ClearFileHooks();

	///	<summary>
	/// FuncReturn struct.
	///	</summary>
	///	<remarks>
	///	</remarks>
	struct FuncReturn
	{
		///	<summary>
		///	</summary>
		///	<remarks>
		///	</remarks>
		union
		{
			HANDLE returnHandle;
			DWORD  returnDword;
			BOOL   returnBool;
		};

		///	<summary>
		///	</summary>
		///	<remarks>
		///	</remarks>
		DWORD lastError;

		///	<summary>
		///	</summary>
		///	<remarks>
		///	</remarks>
		bool haltProcessing;

		///	<summary>
		/// FuncReturn constructor.
		///	</summary>
		///	<param name=""></param>
		///	<returns></returns>
		///	<remarks>
		///	</remarks>
		FuncReturn(HANDLE returnHandle, DWORD lastError, bool haltProcessing)
			: returnHandle(returnHandle)
			, lastError(lastError)
			, haltProcessing(haltProcessing)
		{
		}

		///	<summary>
		/// FuncReturn constructor.
		///	</summary>
		///	<param name=""></param>
		///	<returns></returns>
		///	<remarks>
		///	</remarks>
		FuncReturn(DWORD returnDword, DWORD lastError, bool haltProcessing)
			: returnDword(returnDword)
			, lastError(lastError)
			, haltProcessing(haltProcessing)
		{
		}

		///	<summary>
		/// FuncReturn constructor.
		///	</summary>
		///	<param name=""></param>
		///	<returns></returns>
		///	<remarks>
		///	</remarks>
		FuncReturn(BOOL returnBool, DWORD lastError, bool haltProcessing)
			: returnBool(returnBool)
			, lastError(lastError)
			, haltProcessing(haltProcessing)
		{
		}
	};

	///	<summary>
	///	</summary>
	///	<param name=""></param>
	///	<returns></returns>
	///	<remarks>
	///	</remarks>
	typedef std::function<FuncReturn(
		LPCTSTR pFileName,
		DWORD   desiredAccess,
		DWORD   shareMode,
		DWORD   creationDisposition,
		DWORD   flagsAndAttributes)> CreateFileWFunc;

	///	<summary>
	///	</summary>
	///	<param name="alwaysClose">The value of the alwaysClose parameter when
	///		CloseHandle_SetFilter was called. Changing this value will cause the
	///		desired behavior, but will not change the original value.</param>
	///	<returns></returns>
	///	<remarks>
	///	</remarks>
	typedef std::function<FuncReturn(
		LPCWSTR pFileName,
		HANDLE  hObject,
		bool&   alwaysClose)> CloseHandleFunc;

	///	<summary>
	///	</summary>
	///	<param name=""></param>
	///	<returns></returns>
	///	<remarks>
	///	</remarks>
	typedef std::function<FuncReturn(
		LPCWSTR pFileName,
		HANDLE	hFile,
		LPVOID  pBuffer,
		DWORD   bytesToRead,
		PDWORD  pBytesRead)> ReadFileFunc;

	///	<summary>
	///	</summary>
	///	<param name=""></param>
	///	<returns></returns>
	///	<remarks>
	///	</remarks>
	typedef std::function<FuncReturn(
		LPCWSTR pFileName,
		HANDLE  hFile,
		LPCVOID pBuffer,
		DWORD   bytesToWrite,
		PDWORD  pBytesWritten)> WriteFileFunc;

	///	<summary>
	///	</summary>
	///	<param name=""></param>
	///	<returns></returns>
	///	<remarks>
	///	</remarks>
	typedef std::function<FuncReturn(
		LPCWSTR pFileName,
		HANDLE  hFile,
		LONG    distance,
		PLONG   pDistanceHigh,
		DWORD   moveMethod)> SetFilePointerFunc;

	///	<summary>
	///	</summary>
	///	<param name=""></param>
	///	<returns></returns>
	///	<remarks>
	///	</remarks>
	typedef std::function<FuncReturn(
		LPCWSTR pFileName,
		HANDLE  hFile)> SetEndOfFileFunc;

	///	<summary>
	///	</summary>
	///	<param name=""></param>
	///	<returns></returns>
	///	<remarks>
	///	</remarks>
	typedef std::function<FuncReturn(
		LPCWSTR pFileName)> DeleteFileWFunc;

	///	<summary>
	///	</summary>
	///	<param name="pFilename">The filename to register.</param>
	///	<param name="pParam">The parameter to return if a func is set and called.</param>
	///	<returns></returns>
	///	<remarks>
	/// pParam is not yet implemented.
	///	</remarks>
	bool RegisterFile(LPCWSTR pFilename, void* pParam = nullptr, bool alwaysClose = true);

	///	<summary>
	///	</summary>
	///	<param name=""></param>
	///	<returns></returns>
	///	<remarks>
	///	</remarks>
	bool CreateFileW_SetFilter(LPCWSTR pFilename, CreateFileWFunc func, DWORD error = 0);

	///	<summary>
	///	</summary>
	///	<param name=""></param>
	///	<param name="alwaysClose">Causes the true CloseHandle to be called regardless
	///		of the filter results. The last error code will be set to that specified
	///		by the filter.</param>
	///	<returns></returns>
	///	<remarks>
	///	</remarks>
	bool CloseHandle_SetFilter(LPCWSTR pFilename, CloseHandleFunc func, DWORD error = 0, bool alwaysClose = true);

	///	<summary>
	///	</summary>
	///	<param name=""></param>
	///	<returns></returns>
	///	<remarks>
	///	</remarks>
	bool ReadFile_SetFilter(LPCWSTR pFilename, ReadFileFunc func, DWORD error = 0);

	///	<summary>
	///	</summary>
	///	<param name=""></param>
	///	<returns></returns>
	///	<remarks>
	///	</remarks>
	bool WriteFile_SetFilter(LPCWSTR pFilename, WriteFileFunc func, DWORD error = 0);

	///	<summary>
	///	</summary>
	///	<param name=""></param>
	///	<returns></returns>
	///	<remarks>
	///	</remarks>
	bool SetFilePointer_SetFilter(LPCWSTR pFilename, SetFilePointerFunc func, DWORD error = 0);

	///	<summary>
	///	</summary>
	///	<param name=""></param>
	///	<returns></returns>
	///	<remarks>
	///	</remarks>
	bool SetEndOfFile_SetFilter(LPCWSTR pFilename, SetEndOfFileFunc func, DWORD error = 0);

	///	<summary>
	///	</summary>
	///	<param name=""></param>
	///	<returns></returns>
	///	<remarks>
	///	</remarks>
	bool DeleteFileW_SetFilter(LPCWSTR pFilename, DeleteFileWFunc func, DWORD error = 0);
};