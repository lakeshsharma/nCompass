#pragma once
#include "ProcessDump/OperatingSystem.h"
#include "DirectoryManager.h"

#if 000//TODO-PORT - Probably do not need stdio (non-binary text) file support on Linux.
class CStdioMemFile : public CStdioFile, public CMemFile
{
	/// <summary>
	/// Non-functional override for CStdioFile::ReadString(LPTSTR, ...).
	/// </summary>
	/// <param name="lpsz">IN/OUT - Specifies a pointer to a user-supplied buffer that will receive a null-terminated text string.</param>
	/// <param name="nMax">IN - Specifies the maximum number of characters to read, not counting the terminating null character.</param>
	/// <returns>NULL</returns>
	virtual LPTSTR ReadString(LPTSTR lpsz, UINT nMax);

	/// <summary>
	/// Reads a string from a CMemFile.
	/// Override CStdioFile::ReadString(CString) to read from CMemFile.
	/// </summary>
	/// <param name="rString">IN/OUT - A reference to a CString object that will contain the string when the function returns.</param>
	/// <returns>FALSE if end-of-file was reached without reading any data, otherwise TRUE.</returns>
	virtual BOOL ReadString(CString& rString);

	/// <summary>
	/// Writes the string to a CMemFile.
	/// Override for CStdioFile::WriteString() to write to CMemFile.
	/// </summary>
	/// <param name="lpsz">IN - Specifies a pointer to a buffer containing a null-terminated text string.</param>
	virtual void WriteString(LPCTSTR lpsz);
};

// Forward declaration.
class CStdioTestFile;
#endif//TODO-PORT - Probably do not need stdio (non-binary text) file support on Linux.
// Forward declaration.
class CTestFile;

class IOperatingSystemTest
{
public:
	/// <summary>
	/// Opens a CMemFile file.
	/// </summary>
	/// <param name="file">IN/OUT - The file.</param>
	/// <param name="lpszFileName">IN - A string that is the path to the desired file. The path can be relative, absolute, or a network name (UNC).< / param>
	/// <param name="nOpenFlags">IN - A UINT that defines the file's sharing and access mode. It specifies the action to take when opening the file.
	///      You can combine options by using the bitwise-OR ( | ) operator. One access permission and one share option are required;
	///      the modeCreate and modeNoInherit modes are optional. See the CFile constructor for a list of mode options.</param>
	/// <returns>true if the open was successful; otherwise false.< / returns>
	virtual bool FileOpen(CTestFile &file, const std::string &lpszFileName, std::ios::openmode nOpenFlags) = 0;

	/// <summary>
	/// Closes a CMemFile file.
	/// </summary>
	/// <param name="file">IN/OUT - The file.</param>
	/// <param name="lpszFileName">IN - A string that is the path to the desired file. The path can be relative, absolute, or a network name (UNC).</param>
	virtual void FileClose(CTestFile &file, const std::string &lpszFileName) = 0;

	/// <summary>
	/// Finalizes a CTestFile file.  Normally called from a destructor.
	/// </summary>
	/// <param name="file">IN/OUT - The file.</param>
	/// <param name="lpszFileName">IN - A string that is the path to the desired file. The path can be relative, absolute, or a network name (UNC).</param>
	virtual void FileFinalize(CTestFile &file, const std::string &lpszFileName) = 0; // Normally called from a destructor.

};

// Provide generic std::iostream file access for use with IOperatingSystemTest.
class CTestFile : public std::stringstream
{
public:
	CTestFile(IOperatingSystemTest &operatingSystemTest) :
		m_operatingSystemTest(operatingSystemTest)
	{
	}

	virtual ~CTestFile()
	{
		m_operatingSystemTest.FileFinalize(*this, m_fileName);
	}

	virtual BOOL Open(
		const std::string &lpszFileName,
		std::ios::openmode nOpenFlags
		)
	{
		m_fileName = lpszFileName;
		bool ok = m_operatingSystemTest.FileOpen(*this, lpszFileName, nOpenFlags);
		return ok;
	}

	virtual void Close()
	{
		m_operatingSystemTest.FileClose(*this, m_fileName);
	}

	IOperatingSystemTest &m_operatingSystemTest;
	std::string m_fileName;
};

#if 000//TODO-PORT - Probably do not need stdio (non-binary text) file support on Linux.
/// <summary>
/// Provide generic file access for use with IOperatingSystemTest.
/// </summary>
class CStdioTestFile : public CStdioMemFile
{
public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CStdioTestFile"/> class.
	/// </summary>
	/// <param name="operatingSystemTest">IN/OUT - The operating system to use for testing.</param>
	CStdioTestFile(IOperatingSystemTest &operatingSystemTest);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CStdioTestFile"/> class.
	/// </summary>
	virtual ~CStdioTestFile();

	/// <summary>
	/// Opens the file instance.
	/// Partially functional override for CFile::Open() -- pError object is not changed.
	/// </summary>
	/// <param name="lpszFileName">IN - A string that is the path to the desired file. The path can be relative, absolute, or a network name (UNC).< / param>
	/// <param name="nOpenFlags">IN - A UINT that defines the file's sharing and access mode. It specifies the action to take when opening the file.
	///      You can combine options by using the bitwise-OR ( | ) operator. One access permission and one share option are required;
	///      the modeCreate and modeNoInherit modes are optional. See the CFile constructor for a list of mode options.</param>
	/// <param name="pError">IN - A pointer to an existing file-exception object. The object is not changed.</param>
	/// <returns>Non-zero if successful, FALSE otherwise.</returns>
	virtual BOOL Open(
		LPCTSTR lpszFileName,
		UINT nOpenFlags,
		CFileException* pError = NULL);

	/// <summary>
	/// Closes this instance.
	/// </summary>
	virtual void Close();

	// Public variables.
	IOperatingSystemTest &m_operatingSystemTest;
	CString m_fileName;
};
#endif//TODO-PORT - Probably do not need stdio (non-binary text) file support on Linux.

/// <summary>
/// Operating system test environment.
/// Start with the default COperatingSystem and override the parts where we need other behavior(s).
/// </summary>
class COperatingSystemTest : public COperatingSystem, public IOperatingSystemTest
{
public:
	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="COperatingSystemTest"/> class.
	/// </summary>
	COperatingSystemTest();

	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="COperatingSystemTest"/> class.
	/// </summary>
	/// <param name="fileName1">IN - The name for the first file.</param>
	/// <param name="nOpenFlagsExpected1">IN - The open flags expected for the first file.</param>
	/// <param name="fileContents1">IN - A pointer to the contents for the first file.</param>
	/// <param name="fileLength1">IN - The length of the contents for the first file.</param>
	/// <param name="fileName2">IN - The name for the second file.</param>
	/// <param name="nOpenFlagsExpected2">IN - The open flags expected for the second file.</param>
	/// <param name="fileContents2">IN - A pointer to the contents for the second file.</param>
	/// <param name="fileLength2">IN - The length of the contents for the second file.</param>
	COperatingSystemTest(
		const std::string &fileName1,
		std::ios::openmode nOpenFlagsExpected1,
		const void* fileContents1,
		UINT fileLength1,
		const std::string &fileName2 = _T(""),
		std::ios::openmode nOpenFlagsExpected2 = std::ios::in,
		const void* fileContents2 = NULL,
		UINT fileLength2 = 0);

	/// <summary>
	/// Three or four file constructor. Initializes a new instance of the <see cref="COperatingSystemTest"/> class.
	/// </summary>
	/// <param name="fileName1">IN - The name for the first file.</param>
	/// <param name="nOpenFlagsExpected1">IN - The open flags expected for the first file.</param>
	/// <param name="fileContents1">IN - A pointer to the contents for the first file.</param>
	/// <param name="fileLength1">IN - The length of the contents for the first file.</param>
	/// <param name="fileName2">IN - The name for the second file.</param>
	/// <param name="nOpenFlagsExpected2">IN - The open flags expected for the second file.</param>
	/// <param name="fileContents2">IN - A pointer to the contents for the second file.</param>
	/// <param name="fileLength2">IN - The length of the contents for the second file.</param>
	/// <param name="fileName3">IN - The name for the third file.</param>
	/// <param name="nOpenFlagsExpected3">IN - The open flags expected for the third file.</param>
	/// <param name="fileContents3">IN - A pointer to the contents for the third file.</param>
	/// <param name="fileLength3">IN - The length of the contents for the third file.</param>
	/// <param name="fileName4">IN - The name for the fourth file.</param>
	/// <param name="nOpenFlagsExpected4">IN - The open flags expected for the fourth file.</param>
	/// <param name="fileContents4">IN - A pointer to the contents for the fourth file.</param>
	/// <param name="fileLength4">IN - The length of the contents for the fourth file.</param>
	COperatingSystemTest(
		const std::string &fileName1,
		std::ios::openmode nOpenFlagsExpected1,
		const void* fileContents1,
		UINT fileLength1,
		const std::string &fileName2,
		std::ios::openmode nOpenFlagsExpected2,
		const void* fileContents2,
		UINT fileLength2,
		const std::string &fileName3,
		std::ios::openmode nOpenFlagsExpected3,
		const void* fileContents3,
		UINT fileLength3,
		const std::string &fileName4 = _T(""),
		std::ios::openmode nOpenFlagsExpected4 = std::ios::in,
		const void* fileContents4 = NULL,
		UINT fileLength4 = 0);

	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="COperatingSystemTest"/> class.
	/// </summary>
	/// <param name="fileName1">IN - The name for the first file.</param>
	/// <param name="nOpenFlagsExpected1A">IN - The primary open flags expected for the first file.</param>
	/// <param name="nOpenFlagsExpected1B">IN - The secondary open flags expected for the first file.</param>
	/// <param name="fileContents1">IN - A pointer to the contents for the first file.</param>
	/// <param name="fileLength1">IN - The length of the contents for the first file.</param>
	/// <param name="fileName2">IN - The name for the second file.</param>
	/// <param name="nOpenFlagsExpected2A">IN - The primary open flags expected for the second file.</param>
	/// <param name="nOpenFlagsExpected2B">IN - The secondary open flags expected for the second file.</param>
	/// <param name="fileContents2">IN - A pointer to the contents for the second file.</param>
	/// <param name="fileLength2">IN - The length of the contents for the second file.</param>
	/// <remarks>
	/// Either the primary or secondary flags may be used for opening the file.
	/// </remarks>
	COperatingSystemTest(
		const std::string &fileName1,
		std::ios::openmode nOpenFlagsExpected1A,
		std::ios::openmode nOpenFlagsExpected1B,
		const void* fileContents1,
		UINT fileLength1,
		const std::string &fileName2 = _T(""),
		std::ios::openmode nOpenFlagsExpected2A = (std::ios::openmode) - 1,
		std::ios::openmode nOpenFlagsExpected2B = (std::ios::openmode) - 1,
		const void* fileContents2 = NULL,
		UINT fileLength2 = 0);

	/// <summary>
	/// SAS UART constructor. Initializes a new instance of the <see cref="COperatingSystemTest"/> class.
	/// </summary>
	COperatingSystemTest(UINT sasUartNumber);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="COperatingSystemTest"/> class.
	/// </summary>
	virtual ~COperatingSystemTest();

	/// <summary>
	/// Creates a test file instance compatible with std::iostream.
	/// </summary>
	/// <returns>A pointer to a test file instance compatible with std::iostream.</returns>
	virtual std::shared_ptr<std::iostream> CreateFile(const std::string &filename, std::ios_base::openmode mode);

	/// <summary>
	/// Gets the length of the file.
	/// </summary>
	/// <param name="file">IN - The file.</param>
	/// <returns>The length of the file.</returns>
	/// <remarks>
	/// Can return 0 if the file failbit is set.
	/// </remarks>
	static std::ios::pos_type GetLength(const std::shared_ptr<std::istream> &file);

	std::ios::pos_type GetLength(CTestFile &file);


#if 000//TODO-PORT - Probably do not need stdio (non-binary text) file support on Linux.
	/// <summary>
	/// Creates a test file instance compatible with CStdioFile.
	/// </summary>
	/// <returns>A pointer to a test file instance compatible with CStdioFile.</returns>
	/// <remarks>
	/// The caller is responsible for deleting the returned instance.
	/// </remarks>
	virtual CStdioFile* CreateStdioFile();
#endif//TODO-PORT - Probably do not need stdio (non-binary text) file support on Linux.

	/// <summary>
	/// Deletes the file.
	/// </summary>
	/// <param name="filename">IN - The filename.</param>
	/// <param name="pErrorCode">IN/OUT - a pointer to an error code.</param>
	/// <param name="bWaitForDisk">IN - true to wait for the disk to spin up, false to not wait.</param>
	/// <returns>true if successful, false otherwise.</returns>
	virtual bool DeleteFile(LPCTSTR filename, DWORD *pErrorCode = NULL, bool bWaitForDisk = false);

	/// <summary>
	/// Changes the file contents for the first file.
	/// </summary>
	/// <param name="fileContents1">IN - The new file contents.</param>
	/// <param name="fileLength1">IN - The length of the new file contents.</param>
	void ChangeFileContents(const void* fileContents1, UINT fileLength1);

	/// <summary>
	/// Gets the current local time as CTime.
	/// </summary>
	/// <returns>The current local time as CTime.</returns>
	virtual time_t GetCurrentTime();

	/// <summary>
	/// Gets the current UTC time as CTime.
	/// </summary>
	/// <returns>The current UTC time as CTime.</returns>
	virtual time_t GetUTCTime();

	/// <summary>
	/// Retrieves the number of milliseconds that have elapsed since the system was started, wrapping after 49.7 days.
	/// </summary>
	/// <returns>The number of milliseconds that have elapsed since the system was started, wrapping after 49.7 days.</returns>
	/// <remarks>
	/// Cannot add a GetTickCount() function to this class because of conflicts with existing GetCurrentTime() functions and macros.
	/// </remarks>
	virtual DWORD GetTickCount32();

	/// <summary>
	/// By default, incremements m_tickCount32 by dwMilliseconds and returns.
	/// </summary>
	/// <param name="dwMilliseconds">The number of milliseconds to elapse before returning.</param>
	/// <remarks>
	/// Intended to support single-threaded tests.
	/// Multi-threaded tests may benefit from an increase in functionality.
	/// </remarks>
	virtual void Sleep(DWORD dwMilliseconds);

	/// <summary>
	/// Retreives a (high-resolution) performance counter and converts the value to milliseconds (tick counts).
	/// The returned value is not expected to wrap.
	/// </summary>
	/// <returns>A number of milliseconds relative to previous and subsequent calls.</returns>
	virtual double GetPerfCount();

	/// <summary>
	/// Sleep until the current perf count has advanced to at least endPerfCount.
	/// </summary>
	/// <param name="endPerfCount">IN - The ending perf count.</param>
	virtual void SleepUntilPerfCount(double endPerfCount);

	/// <summary>
	/// Calls hHandle.try_wait_for(0).
	/// Returns the result immediately if the result is true.
	/// Otherwise, increments m_tickCount32 by dwMilliseconds and returns false.
	/// </summary>
	/// <param name="hHandle">IN - A handle to the object.</param>
	/// <param name="dwMilliseconds">The time-out interval, in milliseconds.</param>
	/// <returns>true indicates that hHandle was signaled, false indicates that the interval elapsed.</returns>
	virtual bool WaitForSingleObject(binsem &hHandle, DWORD dwMilliseconds);

	/// <summary>
	/// Gets a random value in the inclusive range.
	/// </summary>
	/// <param name="rangeMin">The range minimum.</param>
	/// <param name="rangeMax">The range maximum.</param>
	/// <returns>A random value in the inclusive range</returns>
	/// <remarks>
	/// Uses m_randRangeOffset.
	/// </remarks>
	virtual int GetRandRange(int rangeMin, int rangeMax);

#if 000//TODO-PORT
// #pragma region S5_UART // Simulate a SAS UART.
	/// <summary>
	/// The S5_UART_ClearBuffers function clears the transmit and/or receive buffers for specified carrier board UART.
	/// </summary>
	/// <param name="uart">[in] UART number (0-6).</param>
	/// <param name="mask">[in] bit-mask that specifies which buffer(s) to clear.  Valid values are CLR_RX_BUFFER, CLR_TX_BUFFER</ param>
	/// <returns>Fails if the Sentinel 5 UART Driver cannot be accessed.</returns>
	/// <remarks>
	/// Documentation was copied from the Braemac Sentinel 5 Carrier Rev 2 Software DD document.
	/// </remarks>
	virtual S5_Result S5_UART_ClearBuffers(UINT uart, USHORT mask);

	/// <summary>
	/// The S5_UART_SendBuffer function transmits a block of words on the specified carrier board UART.
	/// </summary>
	/// <param name="uart">[in] UART number (0-6).</param>
	/// <param name="buffer">[in] a pointer to a buffer of words to transmit.</param>
	/// <param name="length">[in] number of words to transmit.</param>
	/// <returns></returns>
	/// <remarks>
	/// Documentation was copied from the Braemac Sentinel 5 Carrier Rev 2 Software DD document.
	/// </remarks>
	virtual S5_Result S5_UART_SendBuffer(UINT uart, USHORT *buffer, UINT length);

	/// <summary>
	/// The S5_UART_GetBuffer function receives a block of words from the specified carrier board UART.
	/// </summary>
	/// <param name="uart">[in] UART number (0-6).</param>
	/// <param name="buffer">[in] a pointer to a buffer to store received words.</param>
	/// <param name="length">[inout] number of words to receive, and the number of words actually received.</param>
	/// <returns></returns>
	/// <remarks>
	/// Documentation was copied from the Braemac Sentinel 5 Carrier Rev 2 Software DD document.
	/// </remarks>
	virtual S5_Result S5_UART_GetBuffer(UINT uart, USHORT *buffer, UINT& length);

	/// <summary>
	/// Calculates the duration in tick counts to transmit or receive a SAS message of messageLength.
	/// </summary>
	/// <param name="transmitLength">IN - Length of the message.</param>
	/// <returns>The duration in tick counts to transmit or receive a message of transmitLength.</returns>
	static DWORD CalcSasMessageTickCountDuration(UINT messageLength);

	/// <summary>
	/// Calculates the duration in perf counts (milliseconds) to transmit or receive a SAS message of messageLength.
	/// </summary>
	/// <param name="transmitLength">IN - Length of the message.</param>
	/// <returns>The duration in perf counts (milliseconds) to transmit or receive a message of transmitLength.</returns>
	static double CalcSasMessagePerfCountDuration(UINT messageLength);
// #pragma endregion S5_UART
#endif//TODO-PORT

private:
	class CFileState
	{
	public:
		/// <summary>
		/// Default constructor. Initializes a new instance of the <see cref="CFileState"/> class.
		/// </summary>
		/// <remarks>
		/// Intended for unused instances.
		/// </remarks>
		CFileState();

		/// <summary>
		/// Standard constructor. Initializes a new instance of the <see cref="CFileState"/> class.
		/// </summary>
		/// <param name="fileName">IN - Name of the file.</param>
		/// <param name="nOpenFlagsExpected">IN - The primary open flags expected.</param>
		/// <param name="nOpenFlagsExpected">IN - The secondary open flags expected.</param>
		/// <param name="fileContents">IN - A pointer to the file contents.</param>
		/// <param name="fileLength">IN - The length of the file contents.</param>
		/// <remarks>
		/// Either the primary or secondary flags may be used for opening the file.
		/// </remarks>
		CFileState(const std::string &fileName,
			std::ios::openmode nOpenFlagsExpectedA,
			std::ios::openmode nOpenFlagsExpectedB,
			const void *fileContents,
			UINT fileLength);

		/// <summary>
		/// Destructor. Finalizes an instance of the <see cref="CFileState"/> class.
		/// </summary>
		~CFileState();

		/// <summary>
		/// Set the contents to be used for the next file open.
		/// </summary>
		/// <param name="fileContents">IN - A pointer to the file contents.</param>
		/// <param name="fileLength">IN - The length of the file contents.</param>
		void SetContents(const void *fileContents, UINT fileLength);

		const std::string m_fileName;
		const std::ios::openmode m_nOpenFlagsExpectedA;
		const std::ios::openmode m_nOpenFlagsExpectedB;
		std::ios::openmode m_nOpenFlagsUsed;
		bool m_isOpen;
		ULONGLONG m_finalFileGetLength;
		UINT m_finalFileLength;
		UINT m_finalFileLengthRead;
		char *m_finalFileContents;
		DWORD m_openCount; // Attempts.
		DWORD m_openSuccessCount;
		DWORD m_closeCount; // Attempts.
		DWORD m_finalizeCount; // Attempts.

	private:
		// Disable default copy constructor, and default assignment operator.
		CFileState(const CFileState &rhs);
		CFileState& operator=(const CFileState &rhs);
	};

	/// <summary>
	/// Opens a file.
	/// </summary>
	/// <param name="file">IN/OUT - The file.</param>
	/// <param name="lpszFileName">IN - The file name.</param>
	/// <param name="nOpenFlags">IN - The open flags.</param>
	/// <returns>true if file was opened successfully, false otherwise.</returns>
	virtual bool FileOpen(CTestFile &file, const std::string &lpszFileName, std::ios::openmode nOpenFlags);

private:
	/// <summary>
	/// Opens a file.
	/// </summary>
	/// <param name="file">IN/OUT - The file.</param>
	/// <param name="nOpenFlags">IN - The open flags.</param>
	/// <param name="fs">IN/OUT - The corresponding file state instance.</param>
	/// <returns></returns>
	virtual bool FileOpen(CTestFile &file, std::ios::openmode nOpenFlags, CFileState &fs);

public:
	/// <summary>
	/// Closes a file.
	/// </summary>
	/// <param name="file">IN/OUT - The file.</param>
	/// <param name="lpszFileName">IN - Name of the file.</param>
	virtual void FileClose(CTestFile &file, const std::string &lpszFileName);

	/// <summary>
	/// Finalizes a file.
	/// </summary>
	/// <param name="file">IN/OUT - The file.</param>
	/// <param name="lpszFileName">IN - Name of the file.</param>
	virtual void FileFinalize(CTestFile &file, const std::string &lpszFileName);

private:
	/// <summary>
	/// Performs a file close.
	/// </summary>
	/// <param name="file">IN/OUT - The file.</param>
	/// <param name="lpszFileName">IN - Name of the file.</param>
	/// <param name="isClose">true if only closing the file, false if called from a destructor.</param>
	void DoFileClose(CTestFile &file, const std::string &lpszFileName, bool isClose);

	/// <summary>
	/// Performs a file close.
	/// </summary>
	/// <param name="file">IN/OUT - The file.</param>
	/// <param name="isClose">true if only closing the file, false if called from a destructor.</param>
	/// <param name="fs">IN/OUT - The corresponding file state instance.</param>
	void DoFileClose(CTestFile &file, bool isClose, CFileState &fs);

public:
	static const time_t m_defaultCurrentTime;
	static const int m_timeZoneBiasSeconds;

	CFileState m_file1;
	CFileState m_file2;
	CFileState m_file3;
	CFileState m_file4;

	DWORD m_totalFileCreateCount;
	DWORD m_totalFileOpenCount;
	DWORD m_totalFileCloseCount;
	DWORD m_totalFileFinalizeCount;
	int m_totalGetCurrentTimeCount;

	DWORD m_totalFileDeleteCount;
	std::string m_lastFilenameDeleted;

	int m_randRangeOffset;
	DWORD m_tickCount32;
	double m_perfCount;
	time_t m_currentTime;

// #pragma region S5_UART // Simulate a SAS UART.
	static const DWORD m_sasBitsPerWord = 11; // Includes the start bit, 8 data bits, a parity bit, and one stop bit.
	static const DWORD m_sasBaudRate = 19200; // Transmisson rate in bits per second, for both transmit and receive.
	static const DWORD m_tickCountsPerSecond = 1000;
	static const UINT m_uartNumberMaximum = 6;
	static const DWORD m_rxTickCountNoDataTimeout = 30; // Also used for partial data timeouts.

	static const UINT m_uartNumberDefault = 1; // For SAS, use 1 or CSASSerialWrapper::SASUART.

	typedef std::vector<USHORT> UartData;
	typedef struct UartDataRecord {
		DWORD m_tickCount; // Absolute for writes, delta for reads.
		UartData m_data;
		UartDataRecord(DWORD tickCount, UartData data) { m_tickCount = tickCount; m_data = data; }
	} UartDataRecord;

	typedef std::vector<UartDataRecord> UartDataArray;

	// Variables used to control simulation.
	UINT m_uartNumber; // For SAS, use 1 or CSASSerialWrapper::SASUART.
	DWORD m_uartTxTickCountStart; // When Tx started.
	DWORD m_uartTxTickCountDuration; // How long Tx will last.

	UartDataArray m_uartRxData;
	DWORD m_uartReadIndex;

	// Counter(s) and flag(s) for testing results of operations performed.
	DWORD m_uartNumberOtherCount;
	DWORD m_uartClearTxCount;
	DWORD m_uartClearRxCount;
	DWORD m_uartClearBothCount;
	DWORD m_uartClearNoneCount;
	DWORD m_uartWriteCount;
	DWORD m_uartReadCount;
	bool m_uartRxUnexpected; // true if no m_uartRxData record for read attempt.

	UartDataArray m_uartTxData;
// #pragma endregion S5_UART
};
