#include "stdafx.h"
#include "UnitTestingUtils.h"

// Project includes.
#include "Config.h"
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

// Third-party includes. As of 2016-11-08, not included via Sentinel/stdafx.h.
#include <float.h>

using namespace std;

const LPCTSTR CUnitTestingUtils::TEXT_CASH_FORMAT = _T("%s.%02d");
const LPCTSTR CUnitTestingUtils::TEXT_CASH_SYMBOL(_T("$"));
const LPCTSTR CUnitTestingUtils::TEXT_THOUSANDS_SEPARATOR(_T(","));

const LPCSTR CUnitTestingUtils::TEXT_CURRENCY_FORMAT("%llu.%02llu");
const CHAR CUnitTestingUtils::TEXT_CURRENCY_NEGATIVE_SIGN('-');

const LPCTSTR CUnitTestingUtils::TEXT_FALSE(_T("false"));
const LPCTSTR CUnitTestingUtils::TEXT_TRUE(_T("true"));

const LPCTSTR CUnitTestingUtils::LPCTSTR_DISABLED(_T("Disabled"));
const LPCTSTR CUnitTestingUtils::LPCTSTR_ENABLED(_T("Enabled"));

const LPCTSTR CUnitTestingUtils::LPCTSTR_NA(_T("N/A"));

const SYSTEMTIME CUnitTestingUtils::JAN1970_SYSTEMTIME = { 1970, 1, 4, 1, 0, 0, 0, 0 };

const double DBL_EPSILON_ALLOWED(100 * DBL_EPSILON); // Allow a factor of 100 from ideal error tolerance.

void CUnitTestingUtils::GetTestDirAndDrive(string subDir, string& dir)
{
	//Better to make test directory in /tmp as that will help to keep soource location clean
	dir = string("/tmp/") + subDir;
}

#if 000//TODO-PORT
void CUnitTestingUtils::GetTestDir(LPCWSTR subDir, CString& dir)
{
	wchar_t path[MAX_PATH + 1];
	::GetCurrentDirectory(_countof(path), path);

	dir = path;
	dir += L"\\";
	dir += subDir;
}


void CUnitTestingUtils::GetErrorMessage(DWORD error, CString& errorMsg)
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0,
		NULL );

	errorMsg = (LPCWSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);
}
#endif//TODO-PORT

void CUnitTestingUtils::SetNvramInvalid()
{
	CMemoryStorage::SetInactiveLocation(SPRINKLE1_OFFSET, ~SPRINKLE_VALUE);
}

void CUnitTestingUtils::SetNvramValid()
{
	// Set the sprinkles (to the newsprinkle  value), so CMemoryStorage::IntegrityCheck() returns true.
	CMemoryStorage::SetSprinkles();
	CMemoryStorage::SetInactiveLocation(NVRAM_CONFIG_CHECKBYTE_OFFSET, NVRAM_CONFIG_CHECKBYTE_VALUE);

	// Load CMemoryStorage::PreviousNVRAMVersion with whatever is in NVRAM and upgrade as needed.
	CMemoryStorage::UpgradeNVRAM();

	CConfig config(false, "");
	config.BuildYourself(); // Call CConfig::CheckForNVRAMUpgrade() indirectly.

	// Load CMemoryStorage::PreviousNVRAMVersion with the current NVRAM version,
	// to indicate that the memory storage is already upgraded.
	CMemoryStorage::UpgradeNVRAM();
}

void CUnitTestingUtils::SetNvramUpgraded()
{
	NvramWrite(NVRAM_MAP_VERSION_OFFSET, &NVRAM_MAP_VERSION_VALUE, sizeof(NVRAM_MAP_VERSION_VALUE));
	CMemoryStorage::PreviousNVRAMVersion = NVRAM_MAP_VERSION_VALUE;
}

void CUnitTestingUtils::SetNvramValidAndUpgraded()
{
	SetNvramValid();
	SetNvramUpgraded();
}

void CUnitTestingUtils::NvramWrite(DWORD nvramOffset, const void *data, UINT dataLength)
{
	const byte *dataBuffer = reinterpret_cast <const byte *>(data);
	for (DWORD index = 0; index < dataLength; ++index)
	{
		CMemoryStorage::SetInactiveLocation(nvramOffset + index, *dataBuffer);
		++dataBuffer;
	}
}

void CUnitTestingUtils::NvramWrite(DWORD nvramOffset, BYTE data)
{
	NvramWrite(nvramOffset, &data, sizeof(data));
}

void CUnitTestingUtils::SetOldSprinkles()
{
	//This sets the known values throughout NVRAM for later checking.
	BYTE value = OLD_SPRINKLE_VALUE;

	NvramWrite(SPRINKLE1_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE2_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE3_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE4_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE5_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE6_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE7_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE8_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE9_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE10_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE11_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE12_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE13_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE14_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE15_OFFSET, &value, sizeof value);
	NvramWrite(SPRINKLE16_OFFSET, &value, sizeof value);
}

void CUnitTestingUtils::SetNvramVersion(DWORD nvramVersion)
{
	NvramWrite(NVRAM_MAP_VERSION_OFFSET, &nvramVersion, sizeof(nvramVersion));
}

void CUnitTestingUtils::SetNvramVersionAndSprinkles(DWORD nvramVersion)
{
	if (nvramVersion <= NVRAM_ORIGINAL_VERSION)
	{
		CUnitTestingUtils::SetOldSprinkles();
	}
	else
	{
		CMemoryStorage::SetSprinkles();
	}
	CUnitTestingUtils::SetNvramVersion(nvramVersion);
	CMemoryStorage::UpgradeNVRAM(); // Set CMemoryStorage::PreviousNVRAMVersion.
}

void CUnitTestingUtils::SetNvramNeedsUpgrade(DWORD toNvramVersion)
{
	CMemoryStorage::SetSprinkles();
	SetNvramVersion(toNvramVersion - 1);
	CMemoryStorage::UpgradeNVRAM(); // Set CMemoryStorage::PreviousNVRAMVersion.
}

/// <summary>
/// Get current log index information.
/// </summary>
/// <param name="logs">IN - The logs.</param>
/// <param name="maxLogs">OUT - The max logs.</param>
/// <param name="currentLogIndex">OUT - Index of the current log.</param>
/// <remarks>
/// 2013-06-13 Indexes are currently zero-based.  I.e, minLogIndex=0, maxLogIndex=maxLogsExpected-1.
/// </remarks>
void CUnitTestingUtils::GetLogIndexInfo(const CLogs *logs,
	int &maxLogs,
	int &currentLogIndex)
{
	// Set defaults. Set info to zero in case log information is not available.
	maxLogs = 0;
	currentLogIndex = 0;

	// Get info as available.
	if (logs != NULL)
	{
		maxLogs = logs->GetMaxLogs();
		currentLogIndex = logs->GetIndex();
	}

	ASSERT_TRUE(maxLogs >= 0) << "maxLogs >= 0 in GetLogIndexInfo";
	ASSERT_TRUE(currentLogIndex >= 0) << "currentLogIndex >= 0 in GetLogIndexInfo";
}

void CUnitTestingUtils::GetLogIndexInfoExpected(const CLogs *logs,
	LogIndexInfoExpected &logIndexInfoExpected,
	int logsExpected)
{
	ASSERT_TRUE(logsExpected >= 0) << "logsExpected >= 0 in GetLogIndexInfoExpected";

	GetLogIndexInfo(logs,
		logIndexInfoExpected.m_maxLogsExpected,
		logIndexInfoExpected.m_logIndexStart);

	logIndexInfoExpected.m_logIndexExpected = logIndexInfoExpected.m_logIndexStart + logsExpected;
	if (logIndexInfoExpected.m_maxLogsExpected > 0)
	{
		logIndexInfoExpected.m_logIndexExpected %= logIndexInfoExpected.m_maxLogsExpected;
	}
}

void CUnitTestingUtils::AssertLogInfoExpected(const CLogs *logs,
	const LogIndexInfoExpected &logIndexInfoExpected,
	int testIndex,
	LPCTSTR caller)
{
	// Test #1.  Verify that "logs" is NOT null.
	{
		std::stringstream failedMsg;
		failedMsg << "logs=" << (const void *)logs
            << " expected=NOT NULL" 
            << " testIndex=" << (int)testIndex
            << " from " << static_cast<LPCTSTR>(__TFUNCTION__);

        ASSERT_TRUE(logs != NULL) << failedMsg.str();
	}

	// Test #2.  Verify that the number of records per log is still the same.
	const int maxLogsResult = logs->GetMaxLogs();

	{
		bool areEqual = maxLogsResult == logIndexInfoExpected.m_maxLogsExpected;
        std::stringstream failedMsg;
        failedMsg << "maxLogsResult=" << (int)maxLogsResult
            << " expected=" << (int)logIndexInfoExpected.m_maxLogsExpected
            << " testIndex=" << (int)testIndex
            << " from " << static_cast<LPCTSTR>(__TFUNCTION__);

        ASSERT_TRUE(areEqual) << failedMsg.str();
	}

	// Test #3.  Verify that the correct number of log records were added.
	{
		const int lastLogIndexResult = logs->GetIndex();

		// CLogs allows the index to be out of range when index==maxLogs.
		bool areEqual = lastLogIndexResult == logIndexInfoExpected.m_logIndexExpected ||
			(lastLogIndexResult == maxLogsResult && logIndexInfoExpected.m_logIndexExpected == 0);

        std::stringstream failedMsg;
        failedMsg << "lastLogIndexResult=" << (int)lastLogIndexResult
            << " expected=" << (int)logIndexInfoExpected.m_logIndexExpected
            << " start=" << (int)logIndexInfoExpected.m_logIndexStart
            << " max=" << (int)logIndexInfoExpected.m_maxLogsExpected
            << " testIndex=" << (int)testIndex
            << " from " << static_cast<LPCTSTR>(__TFUNCTION__);

        ASSERT_TRUE(areEqual) << failedMsg.str();
	}
}

#if 000//TODO-PORT


bool CUnitTestingUtils::RemoveFile(LPCTSTR filename)
{
	bool ok(true);
	if (!::DeleteFile(filename))
	{
		ok = ERROR_FILE_NOT_FOUND == GetLastError();
	}

	return ok;
}

bool CUnitTestingUtils::RemoveFileAndDirectory(LPCTSTR directoryName, LPCTSTR filename)
{
	RemoveDirectory(directoryName);
	CString path(directoryName);
	path += '\\';
	path += filename;
	return RemoveFile(path);
}

ULONGLONG CUnitTestingUtils::GetFileSize(LPCTSTR filename)
{
	ULONGLONG fileSize((ULONGLONG)-1);

	CFile file;
	if (file.Open(filename, CFile::shareDenyNone))
	{
		fileSize = file.GetLength();
		file.Close();
	}

	return fileSize;
}

#endif//TODO-PORT

// Read the file in text mode so that extra text mode '\r' carriage returns are stripped.
void CUnitTestingUtils::GetCStringFromTextFile(LPCTSTR filename, std::string &written)
{
	written.clear();
	std::ifstream file;
	file.open(filename, std::ifstream::in);
	
	if (!file.eof() && !file.fail())
	{
		string line;
		while (std::getline(file, line))
		{
			written += line;
		}

		file.close();

	}
}

void CUnitTestingUtils::GetByteArrayFromFile(LPCTSTR filename, std::vector<char> &contents)
{
	std::ifstream file;
	file.open(filename, std::ifstream::in);
	
	if (!file.eof() && !file.fail())
	{
		file.seekg(0, std::ios_base::end);
		std::size_t fileSize = static_cast<std::size_t>(file.tellg());
		contents.resize(fileSize);
	
		file.seekg(0, std::ios_base::beg);
		file.read(&contents[0], fileSize);
	}
	file.close();

}

bool CUnitTestingUtils::IsEqual(const void *buffer1, UINT bufferLength1, const void *buffer2, UINT bufferLength2)
{
	bool ok(bufferLength1 == bufferLength2);
	if (ok)
	{
		ok = 0 == memcmp(buffer1, buffer2, bufferLength2);
	}

	return ok;
}

bool CUnitTestingUtils::IsEqual(const BYTE *buffer1, UINT bufferLength1, const CHAR *buffer2, int bufferLength2)
{
	bool ok(bufferLength1 == (UINT)bufferLength2);
	if (ok)
	{
		ok = 0 == memcmp(buffer1, buffer2, bufferLength2);
	}

	return ok;
}

bool CUnitTestingUtils::IsEqual(const CHAR *buffer1, int bufferLength1, const BYTE *buffer2, UINT bufferLength2)
{
	bool ok((UINT)bufferLength1 == bufferLength2);
	if (ok)
	{
		ok = 0 == memcmp(buffer1, buffer2, bufferLength2);
	}

	return ok;
}

bool CUnitTestingUtils::IsEqual(const CHAR *buffer1, UINT bufferLength1, const CHAR *buffer2, UINT bufferLength2)
{
	bool ok(bufferLength1 == bufferLength2);
	if (ok)
	{
		ok = 0 == memcmp(buffer1, buffer2, bufferLength2);
	}

	return ok;
}

std::ios::pos_type CUnitTestingUtils::GetLength(const std::shared_ptr<std::istream> &file)
{
	ios::pos_type filePosition(file->tellg());
	file->seekg(0, ios::end);
	ios::pos_type fileLength(file->tellg());
	file->seekg(filePosition, ios::beg);

	if (ios::pos_type(-1) == fileLength)
	{
		fileLength = 0;
	}
	return fileLength;
}

std::ios::pos_type CUnitTestingUtils::GetLength(const std::shared_ptr<std::iostream> &file)
{
	return GetLength(static_cast<const std::shared_ptr<std::istream>>(file));
}

std::ios::pos_type CUnitTestingUtils::GetLength(const std::shared_ptr<std::ostream> &file)
{
	ios::pos_type filePosition(file->tellp());
	file->seekp(0, ios::end);
	ios::pos_type fileLength(file->tellp());
	file->seekp(filePosition, ios::beg);

	if (ios::pos_type(-1) == fileLength)
	{
		fileLength = 0;
	}
	return fileLength;
}

bool CUnitTestingUtils::IsEqual(const std::shared_ptr<std::istream> &file, const BYTE *buffer, UINT bufferLength)
{
	file->seekg(0, ios::end);
	ios::pos_type fileLength(GetLength(file));
	bool ok(fileLength == (ios::pos_type)bufferLength);

	if (ok)
	{
		// Try to read bufferLength + 1 bytes, but test for only bufferLength bytes read.
		UINT readBufferLength((UINT)fileLength + 1);
		BYTE *readBuffer = new BYTE[readBufferLength];
		file->seekg(0);
		UINT numBytesRead = file->read((char *)readBuffer, readBufferLength).gcount();
		ok = IsEqual(readBuffer, numBytesRead, buffer, bufferLength);

		delete[] readBuffer;
	}

	return ok;
}

bool CUnitTestingUtils::IsEqual(const std::shared_ptr<std::istream> &file, const CHAR *buffer, UINT bufferLength)
{
	file->seekg(0, ios::end);
	ios::pos_type fileLength(GetLength(file));
	bool ok(fileLength == (ios::pos_type)bufferLength);
	if (ok)
	{
		// Try to read bufferLength + 1 bytes, but test for only bufferLength bytes read.
		UINT readBufferLength((UINT)fileLength + 1);
		CHAR *readBuffer = new CHAR[readBufferLength];
		file->seekg(0);
		UINT numBytesRead = file->read((char *)readBuffer, readBufferLength).gcount();
		ok = IsEqual(readBuffer, numBytesRead, buffer, bufferLength);

		delete[] readBuffer;
	}

	return ok;
}

bool CUnitTestingUtils::IsEqual(const std::shared_ptr<std::istream> &file, const std::string &expected)
{
	return IsEqual(file, expected.c_str(), expected.length());
}

std::string CUnitTestingUtils::FormatIndexes(const int *testIndex, const int *extraIndex)
{
	string indexStr;
	if (testIndex != NULL || extraIndex != NULL)
	{
		indexStr = _T(" testIndex=");

		if (testIndex != NULL)
		{
			indexStr += FormatString(_T("%d"), (int)*testIndex);
		}

		if (extraIndex != NULL)
		{
			indexStr += FormatString(_T("/%d"), (int)*extraIndex);
		}
	}

	return indexStr;
}


void CUnitTestingUtils::AssertEqual(bool result,
	bool expected,
	LPCTSTR resultName,
	const int *testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	const bool success(result == expected);
	if (!success)
	{
		string extraIndexStr(FormatIndexes(testIndex, extraIndex));

		string failedMsg;
		failedMsg = FormatString(_T("%s=%d expected=%d%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(int)result,
			(int)expected,
            static_cast<LPCTSTR>(extraIndexStr.c_str()),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
        ASSERT_TRUE(false) << failedMsg;
	}
}

void CUnitTestingUtils::AssertEqual(bool result,
	bool expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	AssertEqual(result, expected, resultName, &testIndex, functionName, lineNumber, extraIndex);
}

void CUnitTestingUtils::AssertEqual(int result,
	int expected,
	LPCTSTR resultName,
	const int *testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	const bool success(result == expected);
	if (!success)
	{
		string extraIndexStr(FormatIndexes(testIndex, extraIndex));

		string failedMsg;
		failedMsg = FormatString(_T("%s=%d expected=%d%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(int)result,
			(int)expected,
			static_cast<LPCTSTR>(extraIndexStr.c_str()),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		ASSERT_TRUE(false)<< failedMsg;
	}
}

void CUnitTestingUtils::AssertEqual(int result,
	int expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	AssertEqual(result, expected, resultName, &testIndex, functionName, lineNumber, extraIndex);
}

void CUnitTestingUtils::AssertEqual(byte result,
	byte expected,
	LPCTSTR resultName,
	const int *testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	const bool success(result == expected);
	if (!success)
	{
		string extraIndexStr(FormatIndexes(testIndex, extraIndex));

		string failedMsg;
		failedMsg = FormatString(_T("%s=%u expected=%u%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(unsigned)result,
			(unsigned)expected,
			static_cast<LPCTSTR>(extraIndexStr.c_str()),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		ASSERT_TRUE(false)<< failedMsg;
	}
}

void CUnitTestingUtils::AssertEqual(byte result,
	byte expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	AssertEqual(result, expected, resultName, &testIndex, functionName, lineNumber, extraIndex);
}

void CUnitTestingUtils::AssertEqual(WORD result,
	WORD expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	const bool success(result == expected);
	if (!success)
	{
		string extraIndexStr;
		if (extraIndex != NULL)
		{
			extraIndexStr = FormatString(_T("/%d"), (int)*extraIndex);
		}

		string failedMsg;
		failedMsg = FormatString(_T("%s=%u expected=%u testIndex=%d%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(unsigned)result,
			(unsigned)expected,
			(int)testIndex,
			static_cast<LPCTSTR>(extraIndexStr.c_str()),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		ASSERT_TRUE(false) << failedMsg;
	}
}

#if 000//TODO-PORT
void CUnitTestingUtils::AssertEqual(UINT result,
	UINT expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	const bool success(result == expected);
	if (!success)
	{
		CString extraIndexStr;
		if (extraIndex != NULL)
		{
			extraIndexStr.Format(_T("/%d"), (int)*extraIndex);
		}

		CString failedMsg;
		failedMsg.Format(_T("%s=%u expected=%u testIndex=%d%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(unsigned)result,
			(unsigned)expected,
			(int)testIndex,
			static_cast<LPCTSTR>(extraIndexStr),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		Assert::IsTrue(false, failedMsg);
	}
}
#endif
void CUnitTestingUtils::AssertEqual(DWORD result,
	DWORD expected,
	LPCTSTR resultName,
	const int *testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	const bool success(result == expected);
	if (!success)
	{
		string extraIndexStr(FormatIndexes(testIndex, extraIndex));

		string failedMsg;
		failedMsg = FormatString(_T("%s=%u expected=%u%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(unsigned)result,
			(unsigned)expected,
			static_cast<LPCTSTR>(extraIndexStr.c_str()),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
        ASSERT_TRUE(false)<< failedMsg;
	}
}

void CUnitTestingUtils::AssertEqual(DWORD result,
	DWORD expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	AssertEqual(result, expected, resultName, &testIndex, functionName, lineNumber, extraIndex);
}

void CUnitTestingUtils::AssertEqual(int64_t result,
	int64_t expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	const bool success(result == expected);
	if (!success)
	{
		string failedMsg;
		failedMsg = FormatString(_T("%s=%" PRId64 " expected=%" PRId64 " testIndex=%d in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			result,
			expected,
			testIndex,
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		ASSERT_TRUE(false)<< failedMsg;
	}
}

#if 000//TODO-PORT
void CUnitTestingUtils::AssertEqual(ULONGLONG result,
	ULONGLONG expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	const bool success(result == expected);
	if (!success)
	{
		CString extraIndexStr;
		if (extraIndex != NULL)
		{
			extraIndexStr.Format(_T("/%d"), (int)*extraIndex);
		}

		CString failedMsg;
		failedMsg.Format(_T("%s=%llu expected=%llu testIndex=%d%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(unsigned long long)result,
			(unsigned long long)expected,
			(int)testIndex,
			static_cast<LPCTSTR>(extraIndexStr),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		Assert::IsTrue(false, failedMsg);
	}
}

void CUnitTestingUtils::AssertEqual(double result,
	double expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{

	double delta = abs(expected - result);
	double epsilon = delta;
	if (abs(expected) > 0)
	{
		epsilon = abs(delta / expected);
	}
	bool success(epsilon <= DBL_EPSILON_ALLOWED);

	if (!success)
	{
		CString extraIndexStr;
		if (extraIndex != NULL)
		{
			extraIndexStr.Format(_T("/%d"), (int)*extraIndex);
		}

		CString failedMsg;
		failedMsg.Format(_T("%s=%#.16g expected=%#.16g testIndex=%d%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(double)result,
			(double)expected,
			(int)testIndex,
			static_cast<LPCTSTR>(extraIndexStr),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		Assert::IsTrue(false, failedMsg);
	}
}

void CUnitTestingUtils::AssertEqual(LPCTSTR result,
	LPCTSTR expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	const bool success1(result != NULL);
	if (!success1)
	{
		CString extraIndexStr;
		if (extraIndex != NULL)
		{
			extraIndexStr.Format(_T("/%d"), (int)*extraIndex);
		}

		CString failedMsg;
		failedMsg.Format(_T("%s: result=NULL testIndex=%d%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(int)testIndex,
			static_cast<LPCTSTR>(extraIndexStr),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		Assert::IsTrue(false, failedMsg);
	}

	const bool success2(0 == _tcscmp(result, expected));
	if (!success2)
	{
		CString extraIndexStr;
		if (extraIndex != NULL)
		{
			extraIndexStr.Format(_T("/%d"), (int)*extraIndex);
		}

		CString failedMsg;
		failedMsg.Format(_T("%s=\"%s\" expected=\"%s\" testIndex=%d%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			static_cast<LPCTSTR>(result),
			static_cast<LPCTSTR>(expected),
			(int)testIndex,
			static_cast<LPCTSTR>(extraIndexStr),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		Assert::IsTrue(false, failedMsg);
	}
}

void CUnitTestingUtils::AssertEqual(const CString &result,
	const CString &expected,
	LPCTSTR resultName, 
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	const bool success(result == expected);
	if (!success)
	{
		CString extraIndexStr;
		if (extraIndex != NULL)
		{
			extraIndexStr.Format(_T("/%d"), (int)*extraIndex);
		}

		CString failedMsg;
		failedMsg.Format(_T("%s=\"%s\" expected=\"%s\" testIndex=%d%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			static_cast<LPCTSTR>(result),
			static_cast<LPCTSTR>(expected),
			(int)testIndex,
			static_cast<LPCTSTR>(extraIndexStr),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		Assert::IsTrue(false, failedMsg);
	}
}

void CUnitTestingUtils::AssertEqual(const CStringA &result,
	const CStringA &expected,
	LPCTSTR resultName, 
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	const bool success(result == expected);
	if (!success)
	{
		CString extraIndexStr;
		if (extraIndex != NULL)
		{
			extraIndexStr.Format(_T("/%d"), (int)*extraIndex);
		}

		CString failedMsg;
		failedMsg.Format(_T("%s=\"%S\" expected=\"%S\" testIndex=%d%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			static_cast<LPCSTR>(result),
			static_cast<LPCSTR>(expected),
			(int)testIndex,
			static_cast<LPCTSTR>(extraIndexStr),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		Assert::IsTrue(false, failedMsg);
	}
}
#endif
void CUnitTestingUtils::AssertEqual(const std::string& result,
	const std::string& expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	const bool success(result == expected);
	if (!success)
	{
		std::string extraIndexStr;
		if (extraIndex != NULL)
		{
			extraIndexStr = FormatString(_T("/%d"), (int)*extraIndex);
		}

		std::string failedMsg;
		failedMsg = FormatString(_T("%s=\"%s\" expected=\"%s\" testIndex=%d%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			static_cast<LPCTSTR>(result.c_str()),
			static_cast<LPCTSTR>(expected.c_str()),
			(int)testIndex,
			static_cast<LPCTSTR>(extraIndexStr.c_str()),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		ASSERT_TRUE(false) <<  failedMsg;
	}
}

void CUnitTestingUtils::AssertNotNull(const void *result,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	bool success(result != NULL);
	if (!success)
	{
		std::string extraIndexStr;
		if (extraIndex != NULL)
		{
			extraIndexStr = FormatString(_T("/%d"), (int)*extraIndex);
		}

		std::string failedMsg;
		failedMsg = FormatString(_T("%s=%p expected=NotNull testIndex=%d%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(void*)result,
			(int)testIndex,
			static_cast<LPCTSTR>(extraIndexStr.c_str()),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		ASSERT_TRUE(false) <<  failedMsg;
	}
}

void CUnitTestingUtils::AssertEqual(const void *result,
	const void *expected,
	LPCTSTR resultName,
	const int *testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	bool success(result == expected);
	if (!success)
	{
		string extraIndexStr(FormatIndexes(testIndex, extraIndex));

		string failedMsg;
		failedMsg = FormatString(_T("%s=%p expected=%p%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(void*)result,
			(void*)expected,
			static_cast<LPCTSTR>(extraIndexStr.c_str()),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		ASSERT_TRUE(false)<< failedMsg;
	}
}

void CUnitTestingUtils::AssertEqual(const void *result,
	const void *expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	AssertEqual(result, expected, resultName, &testIndex, functionName, lineNumber, extraIndex);
}

void CUnitTestingUtils::AssertEqual(const void *result,
	size_t resultSize,
	const void *expected,
	size_t expectedSize,
	LPCTSTR resultName,
	const int *testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	const bool success1(result != NULL || resultSize == 0);
	if (!success1)
	{
		string extraIndexStr(FormatIndexes(testIndex, extraIndex));

		string failedMsg;
		failedMsg = FormatString(_T("%s: result=NULL resultSize=%u%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(unsigned)resultSize,
			static_cast<LPCTSTR>(extraIndexStr.c_str()),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		ASSERT_TRUE(false) << failedMsg;
	}

	const bool success2(expected != NULL || expectedSize == 0);
	if (!success2)
	{
		string extraIndexStr(FormatIndexes(testIndex, extraIndex));

		string failedMsg;
		failedMsg = FormatString(_T("%s: expected=NULL expectedSize=%u%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(unsigned)expectedSize,
			static_cast<LPCTSTR>(extraIndexStr.c_str()),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		ASSERT_TRUE(false) << failedMsg;
	}

	const bool success3(resultSize == expectedSize);
	if (!success3)
	{
		string extraIndexStr(FormatIndexes(testIndex, extraIndex));

		string failedMsg;
		failedMsg = FormatString(_T("%s: resultSize=%u expected=%u%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(unsigned)resultSize,
			(unsigned)expectedSize,
			static_cast<LPCTSTR>(extraIndexStr.c_str()),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		ASSERT_TRUE(false) << failedMsg;
	}

	const bool success(0 == memcmp(result, expected, expectedSize));
	if (!success)
	{
		string extraIndexStr(FormatIndexes(testIndex, extraIndex));

		string resultStr = CUtilities::ToHexString(reinterpret_cast<const BYTE*>(result), resultSize);
		string expectedStr = CUtilities::ToHexString(reinterpret_cast<const BYTE*>(expected), expectedSize);

		string failedMsg;
		failedMsg = FormatString(_T("%s=\"%s\" expected=\"%s\"%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			static_cast<LPCTSTR>(resultStr.c_str()),
			static_cast<LPCTSTR>(expectedStr.c_str()),
			static_cast<LPCTSTR>(extraIndexStr.c_str()),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		ASSERT_TRUE(false) << failedMsg;
	}
}

void CUnitTestingUtils::AssertEqual(const void *result,
	size_t resultSize,
	const void *expected,
	size_t expectedSize,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	AssertEqual(result, resultSize, expected, expectedSize, resultName, &testIndex, functionName, lineNumber, extraIndex);
}

void CUnitTestingUtils::AssertEqual(const SYSTEMTIME& result,
	const SYSTEMTIME& expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	AssertEqual(result.wYear, expected.wYear, (resultName + string(_T(".wYear"))).c_str(),
		testIndex, functionName, lineNumber, extraIndex);
	AssertEqual(result.wMonth, expected.wMonth, (resultName + string(_T(".wMonth"))).c_str(),
		testIndex, functionName, lineNumber, extraIndex);
	AssertEqual(result.wDayOfWeek, expected.wDayOfWeek, (resultName + string(_T(".wDayOfWeek"))).c_str(),
		testIndex, functionName, lineNumber, extraIndex);
	AssertEqual(result.wDay, expected.wDay, (resultName + string(_T(".wDay"))).c_str(),
		testIndex, functionName, lineNumber, extraIndex);
	AssertEqual(result.wHour, expected.wHour, (resultName + string(_T(".wHour"))).c_str(),
		testIndex, functionName, lineNumber, extraIndex);
	AssertEqual(result.wMinute, expected.wMinute, (resultName + string(_T(".wMinute"))).c_str(),
		testIndex, functionName, lineNumber, extraIndex);
	AssertEqual(result.wSecond, expected.wSecond, (resultName + string(_T(".wSecond"))).c_str(),
		testIndex, functionName, lineNumber, extraIndex);
	AssertEqual(result.wMilliseconds, expected.wMilliseconds, (resultName + string(_T(".wMilliseconds"))).c_str(),
		testIndex, functionName, lineNumber, extraIndex);
}

void CUnitTestingUtils::AssertNotEqual(const SYSTEMTIME& result,
	const SYSTEMTIME& notExpected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	ASSERT_TRUE((result.wYear != notExpected.wYear)||(result.wMonth != notExpected.wMonth)||(result.wDayOfWeek != notExpected.wDayOfWeek)
				||(result.wDay != notExpected.wDay)||(result.wHour != notExpected.wHour)||(result.wMinute != notExpected.wMinute)
				||(result.wSecond != notExpected.wSecond)) << "AssertNotEqual: SYSTEMTIME";
}

void CUnitTestingUtils::AssertNotEqual(WORD result,
	WORD expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	const bool success(result != expected);
	if (!success)
	{
		string extraIndexStr;
		if (extraIndex != NULL)
		{
			extraIndexStr = FormatString(_T("/%d"), (int)*extraIndex);
		}

		string failedMsg;
		failedMsg = FormatString(_T("%s=%u expected=%u testIndex=%d%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(unsigned)result,
			(unsigned)expected,
			(int)testIndex,
			static_cast<LPCTSTR>(extraIndexStr.c_str()),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		ASSERT_TRUE(false) << failedMsg;
	}
}

void CUnitTestingUtils::AssertEqual(const CIPAddress& result,
	const CIPAddress& expected,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int *extraIndex)
{
	if (result != expected)
	{
		string resultIPAddressString;
		result.GetIP(resultIPAddressString);
		string expectedIPAddressString;
		expected.GetIP(expectedIPAddressString);

		string extraIndexStr;
		if (extraIndex != NULL)
		{
			extraIndexStr = FormatString(_T("/%d"), (int)*extraIndex);
		}

		string failedMsg;
		failedMsg = FormatString(_T("%s: result={%s:%ld} expected={%s:%ld} testIndex=%d%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			static_cast<LPCTSTR>(resultIPAddressString.c_str()),
			(long)result.GetPort(),
			static_cast<LPCTSTR>(expectedIPAddressString.c_str()),
			(long)expected.GetPort(),
			(int)testIndex,
			static_cast<LPCTSTR>(extraIndexStr.c_str()),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		ASSERT_TRUE(false)<< failedMsg;
	}
}
#if 000//TODO-PORT
void CUnitTestingUtils::AssertEqual(CFile &file, const BYTE *buffer, UINT bufferLength,
	LPCTSTR resultName,
	int testIndex,
	LPCTSTR functionName,
	int lineNumber,
	const int* extraIndex)
{
	ULONGLONG fileLength(file.GetLength());

	if (fileLength != bufferLength)
	{
		CString extraIndexStr(FormatIndexes(&testIndex, extraIndex));

		CString failedMsg;
		failedMsg.Format(_T("%s: fileLength=%llu expected=%u%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(unsigned long long)fileLength,
			(unsigned)bufferLength,
			static_cast<LPCTSTR>(extraIndexStr),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		Assert::IsTrue(false, failedMsg);
	}

	// Try to read bufferLength + 1 bytes, but test for only bufferLength bytes read.
	UINT readBufferLength((UINT)fileLength + 1);
	BYTE *readBuffer = new BYTE[readBufferLength];
	file.SeekToBegin();
	UINT numBytesRead = file.Read(readBuffer, readBufferLength);

	if (fileLength != numBytesRead)
	{
		CString extraIndexStr(FormatIndexes(&testIndex, extraIndex));

		CString failedMsg;
		failedMsg.Format(_T("%s: fileLength=%llu numBytesRead=%u%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			(unsigned long long)fileLength,
			(unsigned)numBytesRead,
			static_cast<LPCTSTR>(extraIndexStr),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		Assert::IsTrue(false, failedMsg);
	}

	if (!IsEqual(readBuffer, numBytesRead, buffer, bufferLength))
	{
		CString extraIndexStr(FormatIndexes(&testIndex, extraIndex));

		CString resultStr = CUtilities::ToHexString(reinterpret_cast<const BYTE*>(readBuffer), numBytesRead);
		CString expectedStr = CUtilities::ToHexString(reinterpret_cast<const BYTE*>(buffer), bufferLength);

		CString failedMsg;
		failedMsg.Format(_T("%s=\"%s\" expected=\"%s\"%s in %s(%d)"),
			static_cast<LPCTSTR>(resultName),
			static_cast<LPCTSTR>(resultStr),
			static_cast<LPCTSTR>(expectedStr),
			static_cast<LPCTSTR>(extraIndexStr),
			static_cast<LPCTSTR>(functionName),
			(int)lineNumber);
		Assert::IsTrue(false, failedMsg);
	}
}
#endif//TODO-PORT

BYTE *CUnitTestingUtils::NewMemDup(const BYTE *source, size_t sourceSize)
{
	BYTE *duplicate = source != NULL ? new BYTE[sourceSize] : NULL;
	if (source != NULL)
	{
		memcpy(duplicate, source, sourceSize);
	}
	return duplicate;
}

BYTE *CUnitTestingUtils::NewMemDup(LPCTSTR source, size_t &returnSize)
{
	returnSize = StringSize(source);
	BYTE *duplicate = NewMemDup(reinterpret_cast<const BYTE*>(source), StringSize(source));
	return duplicate;
}

LPTSTR CUnitTestingUtils::NewStrDup(LPCTSTR source)
{
	LPTSTR duplicate = (LPTSTR)NewMemDup(reinterpret_cast<const BYTE*>(source), StringSize(source));
	return duplicate;
}

size_t CUnitTestingUtils::StringSize(LPCTSTR source)
{
	size_t bufferSize(0);
	
	if (source != NULL)
	{
		bufferSize = (_tcslen(source) + 1) * sizeof(*source);
	}

	return bufferSize;
}

std::string CUnitTestingUtils::GetFormattedNumberString(uint64_t val)
{
	string retVal;

	retVal = FormatString(_T("%llu"), (unsigned long long)val);

	for (int index(DIGITS_BETWEEN_THOUSANDS_SEPARATOR);
		retVal.length() > index;
		index += DIGITS_BETWEEN_THOUSANDS_SEPARATOR + 1)
	{
		retVal.insert(retVal.length() - index, TEXT_THOUSANDS_SEPARATOR);
	}

	return retVal;
}


std::string CUnitTestingUtils::GetFormattedDollarsPenniesString(uint64_t dollars, uint64_t pennies)
{
	string retStr;
	// Note that this format only uses the lower 32-bits of "pennies" and that it is used as a signed integer.
	retStr = FormatString(TEXT_CASH_FORMAT, GetFormattedNumberString(dollars).c_str(), (int)pennies);
	retStr.insert(0, TEXT_CASH_SYMBOL);
	return retStr;
}

std::string CUnitTestingUtils::GetFormattedCashString(DWORD cashValue)
{
	string retStr(GetFormattedDollarsPenniesString(cashValue / ONE_DOLLAR_IN_CENTS, cashValue % ONE_DOLLAR_IN_CENTS));
	return retStr;
}

std::string CUnitTestingUtils::GetFormattedCashString(long longCashValue)
{
	DWORD cashValue = (DWORD)abs(longCashValue);
	string retStr(GetFormattedDollarsPenniesString(cashValue / ONE_DOLLAR_IN_CENTS, cashValue % ONE_DOLLAR_IN_CENTS));
	if (longCashValue < 0)
	{
		retStr.insert(1, 1, _T('-'));
	}
	return retStr;
}

std::string CUnitTestingUtils::GetCurrencyString(__int64 val)
{
	string retVal;

	uint64_t uval = (uint64_t)(val < 0 ? -val : val);
	retVal = FormatString(TEXT_CURRENCY_FORMAT,
		(unsigned long long)(uval / ONE_DOLLAR_IN_CENTS),
		(unsigned long long)(uval % ONE_DOLLAR_IN_CENTS));

	if (val < 0)
	{
		retVal.insert(0, 1, TEXT_CURRENCY_NEGATIVE_SIGN);
	}

	return retVal;
}

LPCTSTR CUnitTestingUtils::GetBoolString(bool value)
{
	return value ? TEXT_TRUE : TEXT_FALSE;
}

LPCTSTR CUnitTestingUtils::GetEnabledDisabledString(bool value)
{
	return value ? LPCTSTR_ENABLED : LPCTSTR_DISABLED;
}


std::string  CUnitTestingUtils::FormatLocalMilitaryDateTimeStringFromUTC(time_t dateTime)
{
	if (time_t(0) <= dateTime && dateTime <= time_t(CUtilities::GetTimeFromFields(3000, 12, 31, 0, 0, 0)))
	{
		dateTime += CUtilities::GetTzOffset();
	}

	return FormatMilitaryDateTimeString(dateTime);
}

std::string CUnitTestingUtils::FormatMilitaryDateTimeString(time_t dateTime)
{
	string dateTimeString(CUtilities::FormatDateTimeString(dateTime, _T("%m/%d/%Y %H:%M:%S")));
	return dateTimeString;
}

bool CUnitTestingUtils::AreEqual(const SYSTEMTIME &a, const SYSTEMTIME &b)
{
	// Ignore the wDayOfWeek member.
	bool same =
		a.wYear == b.wYear &&
		a.wMonth == b.wMonth &&
		a.wDay == b.wDay &&
		a.wHour == b.wHour &&
		a.wMinute == b.wMinute &&
		a.wSecond == b.wSecond &&
		a.wMilliseconds == b.wMilliseconds;

	return same;
}

std::string CUnitTestingUtils::FormatLocalMilitaryDateTimeString(const SYSTEMTIME& systemTime)
{
	string dateTimeString;

	if ((systemTime.wYear == 0 && systemTime.wMonth == 0) || AreEqual(systemTime, JAN1970_SYSTEMTIME))
	{
		dateTimeString = LPCTSTR_NA;
	}
	else
	{
		time_t dateTime(CUtilities::GetTimeFromUTCSystemTime(systemTime));
		dateTimeString = FormatMilitaryDateTimeString(dateTime);
	}

	return dateTimeString;
}

void CUnitTestingUtils::SetSasLengthAndCrc(byte *buffer, size_t length)
{
	const size_t minimumLength(SAS_OPTIONAL_LENGTH_OFFSET + SAS_OPTIONAL_LENGTH_SIZE + SAS_CRC_SIZE);

	if (buffer != NULL && length >= minimumLength)
	{
		// Set the length and CRC fields.
		buffer[SAS_OPTIONAL_LENGTH_OFFSET] = (BYTE)(length - minimumLength); // Length of data (that follows the length byte).
		SetSasCrc(buffer, length);
	}
}

void CUnitTestingUtils::SetSasCrc(byte *buffer, size_t length)
{
	if (buffer != NULL && length >= sizeof(WORD))
	{
		WORD sasCrc(CalcSasCrc(buffer, length - sizeof(WORD)));
		buffer[length - sizeof(WORD)] = (byte)sasCrc; // Lower byte of CRC.
		buffer[length - sizeof(WORD) + 1] = (byte)(sasCrc >> 8); // Upper byte of CRC.
	}
}

WORD CUnitTestingUtils::CalcSasCrc(const byte *s, size_t len)
{
	WORD crcval(0);
	unsigned c, q;

	for (; len > 0; len--)
	{
		c = *s++;
		q = (crcval ^ c) & 017;
		crcval = (crcval >> 4) ^ (q * 010201);
		q = (crcval ^ (c >> 4)) & 017;
		crcval = (crcval >> 4) ^ (q * 010201);
	}
	return (crcval);
}

CNotificationTextConfig CUnitTestingUtils::GetDefaultNotificationTextConfig()
{
	CNotificationTextConfig notificationTextConfig;
	return notificationTextConfig;
}

void CUnitTestingUtils::SleepElapsedTickCount(DWORD startTickCount, DWORD delayTickCount)
{
	if (delayTickCount > 0)
	{
		DWORD tickCount(CUtilities::GetTickCount());
		DWORD sleptTickCount(tickCount - startTickCount);
		while (delayTickCount > sleptTickCount)
		{
			CUtilities::Sleep(delayTickCount - sleptTickCount);

			tickCount = CUtilities::GetTickCount();
			sleptTickCount = tickCount - startTickCount;
		}
	}
}

void CUnitTestingUtils::SleepElapsedTickCount(DWORD delayTickCount)
{
	if (delayTickCount > 0)
	{
		SleepElapsedTickCount(CUtilities::GetTickCount(), delayTickCount);
	}
}
