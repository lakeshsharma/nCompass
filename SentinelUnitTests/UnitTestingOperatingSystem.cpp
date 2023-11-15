#include "stdafx.h"
#include "UnitTestingOperatingSystem.h"

// Sentinel includes.
#include "Utilities.h"

using namespace std;

#if 000//TODO-PORT - Probably do not need stdio (non-binary text) file support on Linux.
LPTSTR CStdioMemFile::ReadString(LPTSTR lpsz, UINT nMax)
{
	UNREFERENCED_PARAMETER(lpsz);
	UNREFERENCED_PARAMETER(nMax);
	return NULL;
}

BOOL CStdioMemFile::ReadString(CString &rString)
{
	BOOL retVal(FALSE); // Return TRUE if anything was read from the file (except carriage return characters).
	bool done(false);

	rString.Empty();
	do
	{
		char ch(0);
		UINT bytesRead = CMemFile::Read(&ch, sizeof(ch));
		if (0 == bytesRead)
		{
			done = true;
		}
		else
		{
			retVal = TRUE;
			if ('\r' == ch)
			{
				// Skip carriage returns.
			}
			else if ('\n' == ch)
			{
				done = true;
			}
			else
			{
				rString += ch;
			}
		}
	} while (!done);

	return retVal;
}

void CStdioMemFile::WriteString(LPCTSTR lpsz)
{
	static const char cr('\r');

	if (lpsz != NULL)
	{
		char ch = (char)*lpsz++;
		while (ch != (char)0)
		{
			if (ch == '\n')
			{
				CMemFile::Write(&cr, sizeof(cr));
			}
			CMemFile::Write(&ch, sizeof(ch));

			ch = (char)*lpsz++;
		}
	}
}

CStdioTestFile::CStdioTestFile(IOperatingSystemTest &operatingSystemTest) :
	m_operatingSystemTest(operatingSystemTest)
{
}

CStdioTestFile::~CStdioTestFile()
{
	m_operatingSystemTest.FileFinalize(*this, m_fileName);
}

BOOL CStdioTestFile::Open(
	LPCTSTR lpszFileName,
	UINT nOpenFlags,
	CFileException* pError)
{
	UNREFERENCED_PARAMETER(pError);

	m_fileName = lpszFileName;
	bool ok = m_operatingSystemTest.FileOpen(*this, lpszFileName, nOpenFlags);
	return ok;
}

void CStdioTestFile::Close()
{
	m_operatingSystemTest.FileClose(*this, m_fileName);
}
#endif//TODO-PORT - Probably do not need stdio (non-binary text) file support on Linux.

const time_t COperatingSystemTest::m_defaultCurrentTime(CUtilities::GetTimeFromFields(2014, 05, 15, 12, 34, 56));
const int COperatingSystemTest::m_timeZoneBiasSeconds(10 * 3600); // 36000 seconds == 10 hours.

COperatingSystemTest::COperatingSystemTest() :
	m_file1(_T(""), std::ios::in, std::ios::in, NULL, 0),
	m_file2(_T(""), std::ios::in, std::ios::in, NULL, 0),
	m_totalFileCreateCount(0),
	m_totalFileOpenCount(0),
	m_totalFileCloseCount(0),
	m_totalFileFinalizeCount(0),
	m_totalGetCurrentTimeCount(0),
	m_totalFileDeleteCount(0),
	m_randRangeOffset(0),
	m_tickCount32(0),
	m_perfCount(0),
	m_currentTime(m_defaultCurrentTime),
// #pragma region S5_UART
	m_uartNumber(m_uartNumberDefault),
	m_uartTxTickCountStart(0),
	m_uartTxTickCountDuration(0),
	m_uartReadIndex(0),
	m_uartNumberOtherCount(0),
	m_uartClearTxCount(0),
	m_uartClearRxCount(0),
	m_uartClearBothCount(0),
	m_uartClearNoneCount(0),
	m_uartWriteCount(0),
	m_uartReadCount(0),
	m_uartRxUnexpected(false)
// #pragma endregion S5_UART
{
}

COperatingSystemTest::COperatingSystemTest(
	const std::string &fileName1,
	std::ios::openmode nOpenFlagsExpected1,
	const void* fileContents1,
	UINT fileLength1,
	const std::string &fileName2,
	std::ios::openmode nOpenFlagsExpected2,
	const void* fileContents2,
	UINT fileLength2) :
	m_file1(fileName1, nOpenFlagsExpected1, nOpenFlagsExpected1, fileContents1, fileLength1),
	m_file2(fileName2, nOpenFlagsExpected2, nOpenFlagsExpected2, fileContents2, fileLength2),
	m_totalFileCreateCount(0),
	m_totalFileOpenCount(0),
	m_totalFileCloseCount(0),
	m_totalFileFinalizeCount(0),
	m_totalGetCurrentTimeCount(0),
	m_totalFileDeleteCount(0),
	m_randRangeOffset(0),
	m_tickCount32(0),
	m_perfCount(0),
	m_currentTime(m_defaultCurrentTime),
// #pragma region S5_UART
	m_uartNumber(m_uartNumberDefault),
	m_uartTxTickCountStart(0),
	m_uartTxTickCountDuration(0),
	m_uartReadIndex(0),
	m_uartNumberOtherCount(0),
	m_uartClearTxCount(0),
	m_uartClearRxCount(0),
	m_uartClearBothCount(0),
	m_uartClearNoneCount(0),
	m_uartWriteCount(0),
	m_uartReadCount(0),
	m_uartRxUnexpected(false)
// #pragma endregion S5_UART
{
}

COperatingSystemTest::COperatingSystemTest(
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
	const std::string &fileName4,
	std::ios::openmode nOpenFlagsExpected4,
	const void* fileContents4,
	UINT fileLength4) :
	m_file1(fileName1, nOpenFlagsExpected1, nOpenFlagsExpected1, fileContents1, fileLength1),
	m_file2(fileName2, nOpenFlagsExpected2, nOpenFlagsExpected2, fileContents2, fileLength2),
	m_file3(fileName3, nOpenFlagsExpected3, nOpenFlagsExpected3, fileContents3, fileLength3),
	m_file4(fileName4, nOpenFlagsExpected4, nOpenFlagsExpected4, fileContents4, fileLength4),
	m_totalFileCreateCount(0),
	m_totalFileOpenCount(0),
	m_totalFileCloseCount(0),
	m_totalFileFinalizeCount(0),
	m_totalGetCurrentTimeCount(0),
	m_totalFileDeleteCount(0),
	m_randRangeOffset(0),
	m_tickCount32(0),
	m_currentTime(m_defaultCurrentTime),
// #pragma region S5_UART
	m_uartNumber(m_uartNumberDefault),
	m_uartTxTickCountStart(0),
	m_uartTxTickCountDuration(0),
	m_uartReadIndex(0),
	m_uartNumberOtherCount(0),
	m_uartClearTxCount(0),
	m_uartClearRxCount(0),
	m_uartClearBothCount(0),
	m_uartClearNoneCount(0),
	m_uartWriteCount(0),
	m_uartReadCount(0),
	m_uartRxUnexpected(false)
// #pragma endregion S5_UART
{
}

/// <summary>
/// Standard constructor. Initializes a new instance of the <see cref="COperatingSystemTest" /> class.
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
COperatingSystemTest::COperatingSystemTest(
	const std::string &fileName1,
	std::ios::openmode nOpenFlagsExpected1A,
	std::ios::openmode nOpenFlagsExpected1B,
	const void* fileContents1,
	UINT fileLength1,
	const std::string &fileName2,
	std::ios::openmode nOpenFlagsExpected2A,
	std::ios::openmode nOpenFlagsExpected2B,
	const void* fileContents2,
	UINT fileLength2) :
	m_file1(fileName1, nOpenFlagsExpected1A, nOpenFlagsExpected1B, fileContents1, fileLength1),
	m_file2(fileName2, nOpenFlagsExpected2A, nOpenFlagsExpected2B, fileContents2, fileLength2),
	m_totalFileCreateCount(0),
	m_totalFileOpenCount(0),
	m_totalFileCloseCount(0),
	m_totalFileFinalizeCount(0),
	m_totalGetCurrentTimeCount(0),
	m_totalFileDeleteCount(0),
	m_randRangeOffset(0),
	m_tickCount32(0),
	m_perfCount(0),
	m_currentTime(m_defaultCurrentTime),
// #pragma region S5_UART
	m_uartNumber(m_uartNumberDefault),
	m_uartTxTickCountStart(0),
	m_uartTxTickCountDuration(0),
	m_uartReadIndex(0),
	m_uartNumberOtherCount(0),
	m_uartClearTxCount(0),
	m_uartClearRxCount(0),
	m_uartClearBothCount(0),
	m_uartClearNoneCount(0),
	m_uartWriteCount(0),
	m_uartReadCount(0),
	m_uartRxUnexpected(false)
// #pragma endregion S5_UART
{
}

COperatingSystemTest::COperatingSystemTest(UINT sasUartNumber) :
	m_file1(_T(""), std::ios::in, std::ios::in, NULL, 0),
	m_file2(_T(""), std::ios::in, std::ios::in, NULL, 0),
	m_totalFileCreateCount(0),
	m_totalFileOpenCount(0),
	m_totalFileCloseCount(0),
	m_totalFileFinalizeCount(0),
	m_totalGetCurrentTimeCount(0),
	m_totalFileDeleteCount(0),
	m_randRangeOffset(0),
	m_tickCount32(0),
	m_perfCount(0),
	m_currentTime(m_defaultCurrentTime),
// #pragma region S5_UART
	m_uartNumber(sasUartNumber),
	m_uartTxTickCountStart(0),
	m_uartTxTickCountDuration(0),
	m_uartReadIndex(0),
	m_uartNumberOtherCount(0),
	m_uartClearTxCount(0),
	m_uartClearRxCount(0),
	m_uartClearBothCount(0),
	m_uartClearNoneCount(0),
	m_uartWriteCount(0),
	m_uartReadCount(0),
	m_uartRxUnexpected(false)
// #pragma endregion S5_UART
{
}

COperatingSystemTest::~COperatingSystemTest()
{
}

// Increment the returned time by one second for each additional call.
time_t COperatingSystemTest::GetCurrentTime()
{
	time_t currentTime(m_currentTime);
	m_currentTime += 1;
	++m_totalGetCurrentTimeCount;
	return currentTime;
}

// Increment the returned time by one second for each additional call.
time_t COperatingSystemTest::GetUTCTime()
{
	// VS 2013: Trying to subtract the time zone bias produces a compiler error.
	time_t currentTime(m_currentTime + -m_timeZoneBiasSeconds);
	m_currentTime += 1;
	++m_totalGetCurrentTimeCount;
	return currentTime;
}

DWORD COperatingSystemTest::GetTickCount32()
{
	return m_tickCount32;
}

void COperatingSystemTest::Sleep(DWORD dwMilliseconds)
{
	m_tickCount32 += dwMilliseconds;
	m_perfCount += dwMilliseconds;
}

double COperatingSystemTest::GetPerfCount()
{
	return m_perfCount;
}

bool COperatingSystemTest::WaitForSingleObject(binsem &hHandle, DWORD dwMilliseconds)
{
	bool waitResult(hHandle.try_wait_for(0));
	if (!waitResult)
	{
		m_tickCount32 += dwMilliseconds;
	}
	return waitResult;
}

void COperatingSystemTest::SleepUntilPerfCount(double endPerfCount)
{
	double currentPerfCount = GetPerfCount();
	double remainingPerfCount = endPerfCount - currentPerfCount;
	if (remainingPerfCount > 0)
	{
		Sleep((DWORD)ceil(remainingPerfCount)); // Round up.
	}
}

int COperatingSystemTest::GetRandRange(int rangeMin, int rangeMax)
{
	int returnValue(rangeMin);
	int range(rangeMax - rangeMin);

	if (rangeMin < rangeMax && range > 0)
	{
		returnValue = rangeMin + m_randRangeOffset % range;
	}

	return returnValue;
}

#if 000//TODO-PORT
// #pragma region S5_UART
S5_Result COperatingSystemTest::S5_UART_ClearBuffers(UINT uart, USHORT mask)
{
	if (uart != m_uartNumber) ++m_uartNumberOtherCount;
	S5_Result result(S5DLL_STATUS_OK);
	if (uart > m_uartNumberMaximum) result = S5DLL_STATUS_WARNING;

	switch (mask & (CLR_RX_BUFFER | CLR_TX_BUFFER))
	{
	case CLR_RX_BUFFER:
		++m_uartClearRxCount;
		break;
	case CLR_TX_BUFFER:
		++m_uartClearTxCount;
		m_uartTxTickCountDuration = 0;
		break;
	case (CLR_RX_BUFFER | CLR_TX_BUFFER):
		++m_uartClearBothCount;
		break;
	default:
		++m_uartClearNoneCount;
		result = S5DLL_STATUS_WARNING;
		break;
	}

	return result;
}

S5_Result COperatingSystemTest::S5_UART_SendBuffer(
	UINT uart,
	USHORT *buffer,
	UINT length
	)
{
	++m_uartWriteCount;
	if (uart != m_uartNumber) ++m_uartNumberOtherCount;
	S5_Result result(S5DLL_STATUS_OK);
	if (uart > m_uartNumberMaximum) result = S5DLL_STATUS_WARNING;

	m_uartTxData.push_back(UartDataRecord(m_tickCount32, UartData(buffer, buffer + length)));

	if (0 == m_uartTxTickCountDuration)
	{
		m_uartTxTickCountStart = m_tickCount32;
	}
	m_uartTxTickCountDuration += CalcSasMessageTickCountDuration(length);

	return result;
}

S5_Result COperatingSystemTest::S5_UART_GetBuffer(
	UINT uart,
	USHORT *buffer,
	UINT& length
	)
{
	++m_uartReadCount;
	if (uart != m_uartNumber) ++m_uartNumberOtherCount;
	S5_Result result(S5DLL_STATUS_OK);
	if (uart > m_uartNumberMaximum) result = S5DLL_STATUS_WARNING;

	if (NULL == buffer && length > 0)
	{
		length = 0;
		result = S5DLL_STATUS_WARNING;
	}

		// Any data requested and do we have any data to return;
	if (length > 0 && m_uartRxData.size() > m_uartReadIndex)
	{
		DWORD remainingRxTickCount(m_rxTickCountNoDataTimeout);

		// Wait for transmission(s) to finish, if any.
		if (m_uartTxTickCountDuration > 0)
		{
			DWORD deltaTickCount = min(remainingRxTickCount, m_uartTxTickCountDuration);
			m_tickCount32 += deltaTickCount;
			m_uartTxTickCountDuration -= deltaTickCount;
			remainingRxTickCount -= deltaTickCount;
		}

		if (0 == m_uartTxTickCountDuration && remainingRxTickCount > 0)
		{
			// Transmission(s) finished before maximum Rx wait, wait for data.
			if (m_uartRxData[m_uartReadIndex].m_tickCount > 0)
			{
				DWORD deltaTickCount = min(remainingRxTickCount, m_uartRxData[m_uartReadIndex].m_tickCount);
				m_tickCount32 += deltaTickCount;
				m_uartRxData[m_uartReadIndex].m_tickCount -= deltaTickCount;
				remainingRxTickCount -= deltaTickCount;
			}
		}

		if (0 == m_uartTxTickCountDuration && 0 == m_uartRxData[m_uartReadIndex].m_tickCount)
		{
			// Transmission(s) finished before maximum Rx wait and data is ready.
			UINT originalLength(length);
			if (length > m_uartRxData[m_uartReadIndex].m_data.size())
			{
				result = S5DLL_STATUS_WARNING;
			}

			length = min(length, m_uartRxData[m_uartReadIndex].m_data.size());
			if (length > 0)
			{
				memcpy(buffer, &m_uartRxData[m_uartReadIndex].m_data.front(), length * sizeof(*buffer));
				// Remove the data read from the buffer.
				m_uartRxData[m_uartReadIndex].m_data.erase(m_uartRxData[m_uartReadIndex].m_data.begin(),
					m_uartRxData[m_uartReadIndex].m_data.begin() + length);
			}
			m_tickCount32 += CalcSasMessageTickCountDuration(length);

			if (0 == length)
			{
				// For no data read, use up the remaining rxTickCount.
				m_tickCount32 += remainingRxTickCount;
			}
			else if (length < originalLength)
			{
				// For partial reads, wait for the no data timeout after reading the available data.
				m_tickCount32 += m_rxTickCountNoDataTimeout;
			}

			// If all the data has been read, point the index to the next element.
			if (0 == m_uartRxData[m_uartReadIndex].m_data.size())
			{
				++m_uartReadIndex;
			}
		}
		else
		{
			length = 0;
			result = S5DLL_STATUS_WARNING;
		}
	}
	else
	{
		// Have time pass even if length requested was zero.
		m_tickCount32 += m_rxTickCountNoDataTimeout;
		length = 0;
		result = S5DLL_STATUS_WARNING;
	}

	return result;
}

DWORD COperatingSystemTest::CalcSasMessageTickCountDuration(UINT messageLength)
{
	// For RS-232, baud rate and bits/second are the same.
	// For SAS, the transmit and receive baud rates are the same.
	// Round up, so multiple message lengths of 1 accumulate time.
	return (DWORD)((m_tickCountsPerSecond * messageLength * m_sasBitsPerWord + m_sasBaudRate - 1) / m_sasBaudRate);
}

double COperatingSystemTest::CalcSasMessagePerfCountDuration(UINT messageLength)
{
	// For RS-232, baud rate and bits/second are the same.
	// For SAS, the transmit and receive baud rates are the same.
	return (m_tickCountsPerSecond * (double)messageLength * m_sasBitsPerWord) / m_sasBaudRate;
}
// #pragma endregion S5_UART
#endif//TODO-PORT

std::shared_ptr<std::iostream> COperatingSystemTest::CreateFile(const std::string &filename, std::ios_base::openmode mode)
{
	++m_totalFileCreateCount;
	std::shared_ptr<CTestFile> file(new CTestFile(*this));
	file->Open(filename, mode);
	return file;
}

std::ios::pos_type COperatingSystemTest::GetLength(const std::shared_ptr<std::istream> &file)
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


std::ios::pos_type COperatingSystemTest::GetLength(CTestFile &file)
{
	ios::pos_type filePosition(file.tellg());
	file.seekg(0, ios::end);
	ios::pos_type fileLength(file.tellg());
	file.seekg(filePosition, ios::beg);

	if (ios::pos_type(-1) == fileLength)
	{
		fileLength = 0;
	}
	return fileLength;
}


#if 000//TODO-PORT - Probably do not need stdio (non-binary text) file support on Linux.
CStdioFile* COperatingSystemTest::CreateStdioFile()
{
	++m_totalFileCreateCount;
	return new CStdioTestFile(*this);
}
#endif//TODO-PORT - Probably do not need stdio (non-binary text) file support on Linux.

bool COperatingSystemTest::DeleteFile(LPCTSTR filename, DWORD *pErrorCode, bool bWaitForDisk)
{
	++m_totalFileDeleteCount;
	m_lastFilenameDeleted = filename;

	if (pErrorCode != NULL)
	{
		*pErrorCode = ERROR_SUCCESS;
	}
	return true;
}

void COperatingSystemTest::ChangeFileContents(const void* fileContents1, UINT fileLength1)
{
	m_file1.SetContents(fileContents1, fileLength1);
}


COperatingSystemTest::CFileState::CFileState() :
	m_nOpenFlagsExpectedA((ios::openmode)0),
	m_nOpenFlagsExpectedB((ios::openmode)0),
	m_nOpenFlagsUsed((ios::openmode)0),
	m_isOpen(false),
	m_finalFileGetLength(0),
	m_finalFileLength(0),
	m_finalFileLengthRead(0),
	m_finalFileContents(NULL),
	m_openCount(0),
	m_openSuccessCount(0),
	m_closeCount(0),
	m_finalizeCount(0)
{
	SetContents(nullptr, 0);
}

COperatingSystemTest::CFileState::CFileState(const std::string &fileName,
	std::ios::openmode nOpenFlagsExpectedA,
	std::ios::openmode nOpenFlagsExpectedB,
	const void *fileContents,
	UINT fileLength) :

	m_fileName(fileName),
	m_nOpenFlagsExpectedA(nOpenFlagsExpectedA),
	m_nOpenFlagsExpectedB(nOpenFlagsExpectedB),
	m_nOpenFlagsUsed((ios::openmode)0),
	m_isOpen(false),
	m_finalFileGetLength(0),
	m_finalFileLength(0),
	m_finalFileLengthRead(0),
	m_finalFileContents(NULL),
	m_openCount(0),
	m_openSuccessCount(0),
	m_closeCount(0),
	m_finalizeCount(0)
{
	SetContents(fileContents, fileLength);
}

COperatingSystemTest::CFileState::~CFileState()
{
	delete[] m_finalFileContents;
}

void COperatingSystemTest::CFileState::SetContents(const void *fileContents, UINT fileLength)
{
	m_finalFileLength = 0;
	delete[] m_finalFileContents;
	m_finalFileLength = fileLength;
	m_finalFileContents = new char[m_finalFileLength];
	memcpy_s(m_finalFileContents, m_finalFileLength, fileContents, fileLength);
}

bool COperatingSystemTest::FileOpen(CTestFile &file, const std::string &lpszFileName, std::ios::openmode nOpenFlags)
{
	++m_totalFileOpenCount;
	bool ok = false;

	if (lpszFileName.length() > 0)
	{
		if (m_file1.m_fileName == lpszFileName)
		{
			ok = FileOpen(file, nOpenFlags, m_file1);
		}
		else if (m_file2.m_fileName == lpszFileName)
		{
			ok = FileOpen(file, nOpenFlags, m_file2);
		}
		else if (m_file3.m_fileName == lpszFileName)
		{
			ok = FileOpen(file, nOpenFlags, m_file3);
		}
		else if (m_file4.m_fileName == lpszFileName)
		{
			ok = FileOpen(file, nOpenFlags, m_file4);
		}
	}

	return ok;
}

bool COperatingSystemTest::FileOpen(CTestFile &file, std::ios::openmode nOpenFlags, CFileState &fs)
{
	bool ok(false);

	++fs.m_openCount;
	fs.m_nOpenFlagsUsed = nOpenFlags;

	// Appending to files is not expected.
	if (!fs.m_isOpen &&
		(fs.m_nOpenFlagsExpectedA == nOpenFlags || fs.m_nOpenFlagsExpectedB == nOpenFlags))
	{
		if (0 != (nOpenFlags & ios::trunc))
		{
			// Truncate the file.
			file.str().clear();
		}
		else
		{
			// Initialize file contents on each open, because it could be a new file instance for each open.
			file.str().clear();
			file.write(fs.m_finalFileContents, fs.m_finalFileLength);
		}
		file.seekg(0);
		file.seekp(0, ios::end);
		fs.m_isOpen = true;
		ok = true;
		++fs.m_openSuccessCount;
	}

	return ok;
}

void COperatingSystemTest::FileClose(CTestFile &file, const std::string &lpszFileName)
{
	DoFileClose(file, lpszFileName, true);
}

void COperatingSystemTest::FileFinalize(CTestFile &file, const std::string &lpszFileName)
{
	DoFileClose(file, lpszFileName, false);
}

void COperatingSystemTest::DoFileClose(CTestFile &file, const std::string &lpszFileName, bool isClose)
{
	if (isClose)
	{
		++m_totalFileCloseCount;
	}
	else
	{
		++m_totalFileFinalizeCount;
	}

	if (lpszFileName.length() > 0)
	{
		if (m_file1.m_fileName == lpszFileName)
		{
			DoFileClose(file, isClose, m_file1);
		}
		else if (m_file2.m_fileName == lpszFileName)
		{
			DoFileClose(file, isClose, m_file2);
		}
		else if (m_file3.m_fileName == lpszFileName)
		{
			DoFileClose(file, isClose, m_file3);
		}
		else if (m_file4.m_fileName == lpszFileName)
		{
			DoFileClose(file, isClose, m_file4);
		}
	}
}

void COperatingSystemTest::DoFileClose(CTestFile &file, bool isClose, CFileState &fs)
{
	if (isClose)
	{
		++fs.m_closeCount;
	}
	else
	{
		++fs.m_finalizeCount;
	}

	if (fs.m_isOpen)
	{
		file.clear(); // Clear any errors/eof, so that seeks and reads will work again.
		fs.m_finalFileGetLength = GetLength(file);
		fs.m_finalFileLength = (UINT)fs.m_finalFileGetLength;
		delete[] fs.m_finalFileContents;
		fs.m_finalFileContents = new char[fs.m_finalFileLength];
		file.seekg(0);
		fs.m_finalFileLengthRead = file.read(fs.m_finalFileContents, fs.m_finalFileLength).gcount();
		file.seekg(0);
		fs.m_isOpen = false;
	}
}
