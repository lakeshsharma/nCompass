#pragma once
#include "IPAddress.h"
#include "UI/Logs.h"
#include "UI/NotificationTextConfig.h"

class CUnitTestingUtils
{
public:
	static const LPCTSTR TEXT_CASH_FORMAT;
	static const LPCTSTR TEXT_CASH_SYMBOL;
	static const LPCTSTR TEXT_THOUSANDS_SEPARATOR;
	static const int DIGITS_BETWEEN_THOUSANDS_SEPARATOR = 3;

	static const LPCSTR TEXT_CURRENCY_FORMAT;
	static const CHAR TEXT_CURRENCY_NEGATIVE_SIGN;

	static const LPCTSTR TEXT_FALSE;
	static const LPCTSTR TEXT_TRUE;

	static const LPCTSTR LPCTSTR_DISABLED;
	static const LPCTSTR LPCTSTR_ENABLED;

	static const LPCTSTR LPCTSTR_NA;

	static const SYSTEMTIME JAN1970_SYSTEMTIME;

	static const size_t SAS_OPTIONAL_LENGTH_OFFSET = 2; // Offset of Type S long poll optional length field, when present.
	static const size_t SAS_OPTIONAL_LENGTH_SIZE = sizeof(BYTE); // Size of Type S long poll optional length field, when present.
	static const size_t SAS_CRC_SIZE = sizeof(WORD); // Size of SAS CRC trailer field, when present.

	static void GetTestDirAndDrive(std::string subDir, std::string& dir);
	
#if 000//TODO-PORT
	static void GetTestDir(LPCWSTR subDir, CString& dir);
	static void GetErrorMessage(DWORD error, CString& errorMsg);
#endif//TODO-PORT

	/// <summary>
	/// Sets the nvram invalid, so CMemoryStorage::IntegrityCheck() returns false.
	/// </summary>
	static void SetNvramInvalid();

	/// <summary>
	/// Sets the NVRAM valid, so CMemoryStorage::IntegrityCheck() returns true,
	/// all NVRAM upgrades have been performed,
	/// and CMemoryStorage::PreviousNVRAMVersion is set to the current version.
	/// </summary>
	static void SetNvramValid();

	/// <summary>
	/// Sets the nvram as upgraded, such that:
	///     CConfig::CheckForNVRAMUpgrade() will change nothing, and
	///     CMemoryStorage::UpgradeNVRAM() will change nothing.
	/// </summary>
	static void SetNvramUpgraded();

	/// <summary>
	/// Sets the nvram valid and upgraded, such that:
	///     CMemoryStorage::IntegrityCheck() returns true,
	///     CConfig::ConfigIntegrityCheck() returns true,
	///     CConfig::CheckForNVRAMUpgrade() will change nothing, and
	///     CMemoryStorage::UpgradeNVRAM() will change nothing.
	/// </summary>
	static void SetNvramValidAndUpgraded();

	/// <summary>
	/// Write data to NVRAM.
	/// </summary>
	/// <param name="nvramOffset">IN - The NVRAM offset to write to.</param>
	/// <param name="data">IN - a pointer to the data to be written.</param>
	/// <param name="dataLength">IN - The length of data to be written.</param>
	static void NvramWrite(DWORD nvramOffset, const void *data, UINT dataLength);

	/// <summary>
	/// Write a BYTE to NVRAM.
	/// </summary>
	/// <param name="nvramOffset">IN - The NVRAM offset to write to.</param>
	/// <param name="data">IN - a BYTE of data.</param>
	static void NvramWrite(DWORD nvramOffset, BYTE data);

	/// <summary>
	/// Sets the NVRAM sprinkles to the old sprinkle values..
	/// </summary>
	static void SetOldSprinkles();

	/// <summary>
	/// Sets the NVRAM map version.
	/// </summary>
	/// <param name="nvramVersion">IN - The NVRAM version value.</param>
	static void SetNvramVersion(DWORD nvramVersion);

	/// <summary>
	/// Sets the NVRAM map version.
	/// Sets the sprinkles to match the NVRAM map version.
	/// Sets CMemoryStorage::PreviousNVRAMVersion to the NVRAM map version -- by calling CMemoryStorage::UpgradeNVRAM().
	/// </summary>
	/// <param name="nvramVersion">IN - The NVRAM version value.</param>
	static void SetNvramVersionAndSprinkles(DWORD nvramVersion);

	/// <summary>
	/// Sets NVRAM and CMemoryStorage so it is being upgraded from prior to toNvramVersion.
	/// </summary>
	/// <param name="toNvramVersion">IN - The NVRAM version that needs to be upgraded to.</param>
	static void SetNvramNeedsUpgrade(DWORD toNvramVersion);

	typedef struct
	{
		int m_maxLogsExpected;
		int m_logIndexStart; // Actually, historical information that can be used if reporting an error.
		int m_logIndexExpected;
	} LogIndexInfoExpected;

	/// <summary>
	/// Calculate expected log index information.
	/// </summary>
	/// <param name="logs">IN - The logs.</param>
	/// <param name="logIndexInfoExpected">OUT - The log index info expected.</param>
	/// <param name="logsExpected">IN - The number of additional log records expected.</param>
	/// <remarks>
	/// 2013-06-13 Indexes are currently zero-based.  I.e, minLogIndex=0, maxLogIndex=maxLogsExpected-1.
	/// </remarks>
	static void GetLogIndexInfoExpected(const CLogs *logs,
		LogIndexInfoExpected &logIndexInfoExpected,
		int logsExpected);

	/// <summary>
	/// Assert expected log index information.
	/// </summary>
	/// <param name="logs">IN - The logs.</param>
	/// <param name="logIndexInfoExpected">IN - The log index info expected.</param>
	/// <param name="testIndex">IN - Index of the test.</param>
	/// <param name="caller">IN - The caller.</param>
	/// <remarks>
	/// 2013-06-13 Indexes are currently zero-based.  I.e, minLogIndex=0, maxLogIndex=maxLogsExpected-1.
	/// </remarks>
	static void AssertLogInfoExpected(const CLogs *logs,
		const LogIndexInfoExpected &logIndexInfoExpected,
		int testIndex,
		LPCTSTR caller);

	static bool RemoveFile(LPCTSTR filename);

	static bool RemoveFileAndDirectory(LPCTSTR directoryName, LPCTSTR filename);

	static ULONGLONG GetFileSize(LPCTSTR filename);

	/// <summary>
	/// Put the contents of a text file into the CString provided.
	/// </summary>
	/// <param name="filename">IN - The name of the file to read.</param>
	/// <param name="contents">OUT - The contents of the file.</param>
	/// <remarks>
	/// Read the file in text mode so that extra text mode '\r' carriage returns are stripped.
	/// </remarks>
	static void GetCStringFromTextFile(LPCTSTR filename, string &contents);

	/// <summary>
	/// Put the contents of a text file into the byte array.
	/// </summary>
	/// <param name="filename">IN - The name of the file to read.</param>
	/// <param name="contents">OUT - The contents of the file.</param>
	/// <remarks>
	/// Read the file in text mode so that extra text mode '\r' carriage returns are stripped.
	/// </remarks>
	static void GetByteArrayFromFile(LPCTSTR filename, std::vector<char> &contents);

	/// <summary>
	/// Returns true if bufferLength1 == bufferLength2 and buffer1 contents == buffer2 contents, false otherwise.
	/// </summary>
	/// <param name="buffer1">IN - The buffer.</param>
	/// <param name="bufferLength1">IN - Length of the buffer.</param>
	/// <param name="buffer2">IN - The buffer.</param>
	/// <param name="bufferLength2">IN - Length of the buffer.</param>
	/// <returns>true if the buffers have the same contents, false otherwise.</returns>
	static bool IsEqual(const void *buffer1, UINT bufferLength1, const void *buffer2, UINT bufferLength2);

	/// <summary>
	/// Returns true if bufferLength1 == bufferLength2 and buffer1 contents == buffer2 contents, false otherwise.
	/// </summary>
	/// <param name="buffer1">IN - The buffer.</param>
	/// <param name="bufferLength1">IN - Length of the buffer.</param>
	/// <param name="buffer2">IN - The buffer.</param>
	/// <param name="bufferLength2">IN - Length of the buffer.</param>
	/// <returns>true if the buffers have the same contents, false otherwise.</returns>
	static bool IsEqual(const BYTE *buffer1, UINT bufferLength1, const CHAR *buffer2, int bufferLength2);

	/// <summary>
	/// Returns true if bufferLength1 == bufferLength2 and buffer1 contents == buffer2 contents, false otherwise.
	/// </summary>
	/// <param name="buffer1">IN - The buffer.</param>
	/// <param name="bufferLength1">IN - Length of the buffer.</param>
	/// <param name="buffer2">IN - The buffer.</param>
	/// <param name="bufferLength2">IN - Length of the buffer.</param>
	/// <returns>true if the buffers have the same contents, false otherwise.</returns>
	static bool IsEqual(const CHAR *buffer1, int bufferLength1, const BYTE *buffer2, UINT bufferLength2);

	/// <summary>
	/// Returns true if bufferLength1 == bufferLength2 and buffer1 contents == buffer2 contents, false otherwise.
	/// </summary>
	/// <param name="buffer1">IN - The buffer.</param>
	/// <param name="bufferLength1">IN - Length of the buffer.</param>
	/// <param name="buffer2">IN - The buffer.</param>
	/// <param name="bufferLength2">IN - Length of the buffer.</param>
	/// <returns>true if the buffers have the same contents, false otherwise.</returns>
	static bool IsEqual(const CHAR *buffer1, UINT bufferLength1, const CHAR *buffer2, UINT bufferLength2);

	/// <summary>
	/// Gets the length of the file.
	/// </summary>
	/// <param name="file">IN - The file.</param>
	/// <returns>The length of the file.</returns>
	/// <remarks>
	/// Can return 0 if the file failbit is set.
	/// </remarks>
	static std::ios::pos_type GetLength(const std::shared_ptr<std::istream> &file);

	/// <summary>
	/// Gets the length of the file.
	/// </summary>
	/// <param name="file">IN - The file.</param>
	/// <returns>The length of the file.</returns>
	/// <remarks>
	/// Can return 0 if the file failbit is set.
	/// </remarks>
	static std::ios::pos_type GetLength(const std::shared_ptr<std::iostream> &file);

	/// <summary>
	/// Gets the length of the file.
	/// </summary>
	/// <param name="file">IN - The file.</param>
	/// <returns>The length of the file.</returns>
	/// <remarks>
	/// Can return 0 if the file failbit is set.
	/// </remarks>
	static std::ios::pos_type GetLength(const std::shared_ptr<std::ostream> &file);

	/// <summary>
	/// Returns true if the file is equal to the buffer, false otherwise.
	/// </summary>
	/// <param name="file">IN - The file, which should be ready to read from.</param>
	/// <param name="buffer">IN - The buffer.</param>
	/// <param name="bufferLength">IN - Length of the buffer.</param>
	/// <returns>true if the file is equal to the buffer, false otherwise.</returns>
	static bool IsEqual(const std::shared_ptr<std::istream> &file, const BYTE *buffer, UINT bufferLength);

	/// <summary>
	/// Returns true if the file is equal to the buffer, false otherwise.
	/// </summary>
	/// <param name="file">IN - The file, which should be ready to read from.</param>
	/// <param name="buffer">IN - The buffer.</param>
	/// <param name="bufferLength">IN - Length of the buffer.</param>
	/// <returns>true if the file is equal to the buffer, false otherwise.</returns>
	static bool IsEqual(const std::shared_ptr<std::istream> &file, const CHAR *buffer, UINT bufferLength);

	/// <summary>
	/// Returns true if the file contents are equal to the contents expected, false otherwise.
	/// </summary>
	/// <param name="file">IN - The file, which should be ready to read from.</param>
	/// <param name="expected">IN - The expected file contents.</param>
	/// <returns>true if the file contents are equal to the expected contents, false otherwise.</returns>
	static bool IsEqual(const std::shared_ptr<std::istream> &file, const std::string &expected);

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test. May be NULL.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(bool result,
		bool expected,
		LPCTSTR resultName,
		const int *testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int *extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(bool result,
		bool expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test. May be NULL.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(int result,
		int expected,
		LPCTSTR resultName,
		const int *testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int *extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(int result,
		int expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test. May be NULL.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(byte result,
		byte expected,
		LPCTSTR resultName,
		const int *testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int *extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(byte result,
		byte expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int *extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(WORD result,
		WORD expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);


	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(UINT result,
		UINT expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);


	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test. May be NULL.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(DWORD result,
		DWORD expected,
		LPCTSTR resultName,
		const int *testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int *extraIndex = NULL);

#if 0 //Duplicated?
	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(DWORD result,
		DWORD expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);
#endif

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(int64_t result,
		int64_t expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(ULONGLONG result,
		ULONGLONG expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is approximately equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(double result,
		double expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value. MAY be NULL.</param>
	/// <param name="expected">IN - The expected value. MUST NOT be NULL.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(LPCTSTR result,
		LPCTSTR expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);


	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(const std::string &result,
		const std::string &expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);

#if 0 // Duplicate
	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(const std::string &result,
		const std::string &expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(const std::string &result,
		const std::string &expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);
#endif

	/// <summary>
	/// Asserts that the result pointer is not NULL.
	/// </summary>
	/// <param name="result">IN - The result pointer.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertNotNull(const void* result,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);

	/// <summary>
	/// Asserts that the result pointer is exactly equal to the expected pointer.
	/// </summary>
	/// <param name="result">IN - The result pointer.</param>
	/// <param name="expectedSize">IN - The expected pointer.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(const void *result,
		const void *expected,
		LPCTSTR resultName,
		const int *testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int *extraIndex = NULL);

	/// <summary>
	/// Asserts that the result pointer is exactly equal to the expected pointer.
	/// </summary>
	/// <param name="result">IN - The result pointer.</param>
	/// <param name="expectedSize">IN - The expected pointer.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(const void* result,
		const void* expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - A pointer to the result data.</param>
	/// <param name="resultSize">IN - The size of the result data.</param>
	/// <param name="expected">IN - A pointer to the expected data.</param>
	/// <param name="expectedSize">IN - The size of the expected data.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(const void* result,
		size_t resultSize,
		const void *expected,
		size_t expectedSize,
		LPCTSTR resultName,
		const int *testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int *extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - A pointer to the result data.</param>
	/// <param name="resultSize">IN - The size of the result data.</param>
	/// <param name="expected">IN - A pointer to the expected data.</param>
	/// <param name="expectedSize">IN - The size of the expected data.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(const void* result,
		size_t resultSize,
		const void* expected,
		size_t expectedSize,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(const SYSTEMTIME& result,
		const SYSTEMTIME& expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is not equal to the notExpected value.
	/// </summary>
	/// <param name="result">IN - The result value</param>
	/// <param name="notExpected">IN - The not expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertNotEqual(const SYSTEMTIME &result,
		const SYSTEMTIME &notExpected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int *extraIndex = NULL);

    /// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value.</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertNotEqual(WORD result,
		WORD expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);

	/// <summary>
	/// Asserts that the result value is equal to the expected value.
	/// </summary>
	/// <param name="result">IN - The result value</param>
	/// <param name="expected">IN - The expected value.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	/// <remarks>
	/// Uses the CIPAddress inequality operator.
	/// </remarks>
	static void AssertEqual(const CIPAddress& result,
		const CIPAddress& expected,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);

#if 000//TODO-PORT
	/// <summary>
	/// Returns true if the file is equal to the buffer, false otherwise.
	/// </summary>
	/// <param name="file">IN - The file, which should be ready to read from.</param>
	/// <param name="buffer">IN - The buffer.</param>
	/// <param name="bufferLength">IN - Length of the buffer.</param>
	/// <param name="resultName">IN - Descriptive string for result value.</param>
	/// <param name="testIndex">IN - Index number for the test.</param>
	/// <param name="functionName">IN - Name of the source code function that is performing the test.</param>
	/// <param name="lineNumber">IN - The source code line number.</param>
	/// <param name="extraIndex">IN - Extra index number for the test. May be NULL.</param>
	static void AssertEqual(CFile &file,
		const BYTE *buffer,
		UINT bufferLength,
		LPCTSTR resultName,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL);
#endif//TODO-PORT

	/// <summary>
	/// Allocates a duplicate memory block using "new" and copies the source memory block to the duplicate memory block.
	/// </summary>
	/// <param name="source">IN - The source memory block.</param>
	/// <param name="sourceSize">IN - The size of the source memory block, in bytes.</param>
	/// <returns>A pointer to the duplicate memory block, returns NULL if source == NULL.</returns>
	/// <remarks>
	/// The caller is responsible for deleting the duplicate string.
	/// </remarks>
	static BYTE* NewMemDup(const BYTE* source, size_t sourceSize);

	/// <summary>
	/// Allocates a duplicate memory block using "new" and copies the source string and trailing NUL character to the duplicate memory block.
	/// </summary>
	/// <param name="source">IN - The source memory block.</param>
	/// <param name="returnSize">OUT - The size of the returned memory block, in bytes.</param>
	/// <returns>A pointer to the duplicate memory block, returns NULL if source == NULL.</returns>
	/// <remarks>
	/// The caller is responsible for deleting the duplicate string.
	/// The returnSize includes the trailing NUL character.
	/// </remarks>
	static BYTE* NewMemDup(LPCTSTR source, size_t &returnSize);

	/// <summary>
	/// Allocates a duplicate string and copies the source string to the duplicate string.
	/// Like strdup(), except it takes a memory block (and size) as input, and uses "new" instead of malloc() to allocate memory.
	/// </summary>
	/// <param name="source">IN - The source string.</param>
	/// <returns>A pointer to the duplicate string, returns NULL if source == NULL.</returns>
	/// <remarks>
	/// The caller is responsible for deleting the duplicate memory block.
	/// </remarks>
	static LPTSTR NewStrDup(LPCTSTR source);

	/// <summary>
	/// Returns the size of the source string (in bytes), including the size of the termininating NUL character.
	/// </summary>
	/// <param name="source">IN - The source string.</param>
	/// <returns>The size of the string.</returns>
	static size_t StringSize(LPCTSTR source);

	/// <summary>
	/// Gets a formatted number string, with commas as appropriate.
	/// The formatted number string can be used in a CString::Format() function for cash strings.
	/// </summary>
	/// <param name="val">IN - The value to format.</param>
	/// <returns>The value formatted as a string.</returns>
	/// <remarks>
	/// Use to verify CUtilities functionality.
	/// </remarks>
	static std::string GetFormattedNumberString(uint64_t val);

	/// <summary>
	/// Return the provided dollars and pennies values formatted as a cash string.
	/// </summary>
	/// <param name="dollars">IN - The dollar value.</param>
	/// <param name="pennies">IN - The pennies value.  SHOULD be less than 100.</param>
	/// <returns>The provided cash value formatted as a string.</returns>
	/// <remarks>
	/// If pennies > INT_MAX then pennies is formatted as a signed 32-bit integer, possibly with a negative value.
	/// Use to verify CUtilities functionality.
	/// </remarks>
	static std::string GetFormattedDollarsPenniesString(uint64_t dollars, uint64_t pennies);

	/// <summary>
	/// Return the provided cash value formatted as a cash string.
	/// </summary>
	/// <param name="cashValue">IN - The cash value.</param>
	/// <returns>The provided cash value formatted as a string.</returns>
	/// <remarks>
	/// Use to verify CUtilities functionality.
	/// </remarks>
	static std::string GetFormattedCashString(DWORD cashValue);

	/// <summary>
	/// Return the provided cash value formatted as a cash string
	/// </summary>
	/// <param name="cashValue">IN - The cash value.</param>
	/// <returns>The provided cash value formatted as a string.</returns>
	static std::string GetFormattedCashString(long cashValue);

	/// <summary>
	/// Gets a currency string, with two digits to the right of the decimal point.
	/// </summary>
	/// <param name="val">IN - The value to format.</param>
	/// <returns>The value formatted as a string.</returns>
	/// <remarks>
	/// Use to verify JsonWriter::Currency() and JsonGetDoubleEx() functionality.
	/// </remarks>
	static std::string GetCurrencyString(__int64 val);

	/// <summary>
	/// Gets a pointer to a string corresponding to the bool value.
	/// </summary>
	/// <returns>A pointer to a string corresponding to the bool value.
	/// <param name="value">IN value to get a string pointer for.</param></returns>
	/// <remarks>
	/// Use to verify CWebDiagnostics functionality.
	/// </remarks>
	static LPCTSTR GetBoolString(bool value);

	/// <summary>
	/// Gets a pointer to an enabled/disabled string corresponding to the bool value.
	/// </summary>
	/// <param name="value">IN value to get a string pointer for.</param>
	/// <returns>A pointer to a enabled/disabled string corresponding to the bool value.</returns>
	static LPCTSTR GetEnabledDisabledString(bool value);

	/// <summary>
	/// Formats a UTC date/time as a local date time string (using a military format) if date is valid,
	/// format as an integer value otherwise.
	/// </summary>
	/// <param name="dateTime">IN - The UTC date/time.</param>
	/// <returns>Formatted date/time value, or an integer string value if date is invalid.</returns>
	static std::string FormatLocalMilitaryDateTimeStringFromUTC(time_t dateTime);

	/// <summary>
	/// Formats a date time string (using a military format) if date is valid, int value otherwise.
	/// </summary>
	/// <param name="date">IN - The date.</param>
	/// <returns>Formatted date/time value, integer string value if date is invalid.</returns>
	static std::string FormatMilitaryDateTimeString(time_t date);

	/// <summary>
	/// Gets whether two system time values are equal.
	/// </summary>
	/// <param name="a">IN - The first system time.</param>
	/// <param name="b">IN - The second system time.</param>
	/// <returns>true if the system time values are equal, false otherwise.</returns>
	/// <remarks>
	/// The wDayOfWeek member is ignored.
	/// </remarks>
	static bool AreEqual(const SYSTEMTIME &a, const SYSTEMTIME &b);

	/// <summary>
	/// Formats a local date time string (using a military format) if date is valid, int value otherwise.
	/// </summary>
	/// <param name="date">IN - The UTC system time.</param>
	/// <returns>Formatted local date/time value, integer string value if date is invalid.</returns>
	static std::string FormatLocalMilitaryDateTimeString(const SYSTEMTIME& systemTime);

	/// <summary>
	/// Sets the SAS "optional" length byte and the SAS CRC for the specified buffer, provided the buffer is long enough.
	/// </summary>
	/// <param name="buffer">IN - The buffer.</param>
	/// <param name="length">IN - The length of the buffer.</param>
	static void SetSasLengthAndCrc(byte *buffer, size_t length);

	/// <summary>
	/// Sets the SAS CRC for the specified buffer, provided the buffer is long enough.
	/// </summary>
	/// <param name="buffer">IN - The buffer.</param>
	/// <param name="length">IN - The length of the buffer.</param>
	static void SetSasCrc(byte *buffer, size_t length);

	/// <summary>
	/// Calculates the SAS CRC for the specified buffer.
	/// </summary>
	/// <param name="s">IN - The buffer.  MUST NOT be NULL.</param>
	/// <param name="len">The buffer length.</param>
	/// <returns>The SAS CRC for the specified buffer.</returns>
	static WORD CalcSasCrc(const byte *s, size_t len);

	/// <summary>
	/// Gets default notification text configuration.
	/// </summary>
	/// <returns>Default notification text configuration.</returns>
	static CNotificationTextConfig GetDefaultNotificationTextConfig();

	/// <summary>
	/// Sleep until the tick count has advanced by at least delayTickCount, since startTickCount.
	/// </summary>
	/// <param name="startTickCount">IN - The start tick count.</param>
	/// <param name="delayTickCount">IN - The delay tick count.</param>
	static void SleepElapsedTickCount(DWORD startTickCount, DWORD delayTickCount);

	/// <summary>
	/// Sleep until the tick count has advanced by at least delayTickCount.
	/// </summary>
	/// <param name="delayTickCount">IN - The delay tick count.</param>
	static void SleepElapsedTickCount(DWORD delayTickCount);

private:
	static std::string FormatIndexes(const int *testIndex, const int *extraIndex);

	static void GetLogIndexInfo(const CLogs *logs,
		int &maxLogs,
		int &currentLogIndex);
};
