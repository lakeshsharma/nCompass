#include "stdafx.h"
#include "Utilities.h"

// Sentinel includes.
#include "GlobalDefs.h"
#include "UI/ScreenDefs.h"
#include "Logging/LogString.h"
#include "Card.h"
#include "Config/CardConfig.h"
#include "Diagnostics.h"
#include "DirectoryManager.h"
//#include "Hardware/WatchDog.h"
#include "Platform.h"
#include "IPAddress.h"
#include "IPAddressArray.h"
#include "SentinelException.h"
#include "Util/Path.h"
#include "resource.h"
#include "time.h"
#include "PollerProtocol/PollerMessage.h"
#include <sstream>
#include <ctime>

#ifndef _UNIT_TEST
#include <curl/curl.h>
#endif

//TODO Put compiler, library, and third party includes in the pre-compiled header.
//TODO #include <WinDNS.h>// Only used in this file
//TODO #include "atlutil.h"// Only used in this file

using namespace std;

const std::string CUtilities::EmptyString;
std::string CUtilities::m_exeVersion;

time_t CUtilities::m_tzOffset = 0;

constexpr std::size_t MAX_LINE_SZ = 1024;
const DWORD TIMESTAMP_SIZE = 128;

LPCTSTR CUtilities::CASH_FORMAT = _T("%s.%02d");
const TCHAR CUtilities::ToHexStringFormatForFirstByte[] = _T("%2.2x");
const TCHAR CUtilities::ToHexStringFormatForOtherBytes[] = _T(" %2.2x");
const LPCTSTR SPACE = _T(" ");
const int CURRENCY_DIGITS = 2;
const long MICROSECS_PER_MILLISEC = 1000000;

const int TmStructYearOffset = 1900;
const int TmStructMonthOffset = 1;

static std::recursive_mutex m_systemCallMutex;

string CUtilities::CERTIFICATE_STORE_DATABASE_PATH;
string CUtilities::NSSDB_PATH;

const std::regex DECIMAL_PATTERN("^-?[0-9]*\\.[0-9]+$");

struct closedir_deleter {
	void operator()(DIR* d) const {
		closedir(d);
	}
};

void CUtilities::ConvertCharsToChars( LPCTSTR inString, char *outChars, int length )
{
	int strLen = _tcslen( inString );
	int i;

	memset(outChars, 0, length);
	for ( i = 0; (i < strLen) && (i < length - 1); i++)
	{
		outChars[i] = (char)(inString[i]);
	}
}

void CUtilities::ConvertCharsToChars(const std::string &inString, char *outChars, int length)
{
	ConvertCharsToChars(inString.c_str(), outChars, length);
}

__int64 CUtilities::ConvertStringToInt64(const string& inString)
{
	string temp(inString);
	size_t found = temp.find('.');
	if (found == std::string::npos)
	{
		temp.append("00");
	}
	else//there is only one zero after decimal point
	{
		if (temp.length() - found == (size_t)NUMBER_OF_PLACES_AFTER_DECIMAL_POINT)
		{
			temp.append("0");
		}		
		temp.erase(std::remove(temp.begin(), temp.end(), '.'));
	}
	return (__int64)strtol(temp.c_str(), nullptr, RADIX_DECIMAL);
}

std::string CUtilities::ConvertWideCharsToUtf8String(LPCWSTR inString)
{
	std::string retVal;

	if (NULL != inString)
	{
		std::wstring str(inString);
		std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
		retVal = myconv.to_bytes(str);
	}

	return retVal;
}

// Identical to stricmp(), more or less.
int CUtilities::CompareStringsNoCase(LPCTSTR str1, LPCTSTR str2)
{
	int diff(strcasecmp(str1, str2));
	return  diff;
}

int CUtilities::CompareStringsNoCase(const std::string &str1, const std::string &str2)
{
	return CompareStringsNoCase(str1.c_str(), str2.c_str());
}

std::string &CUtilities::MakeLower(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}


int CUtilities::CompareInts(int value1, int value2)
{
	int compareResult = 0;

	if (value1 != value2)
	{
		if (value1 < value2)
		{
			compareResult = -1;
		}
		else
		{
			compareResult = 1;
		}
	}

	return compareResult;
}

int CUtilities::CompareVersions(int major1, int minor1, int revision1, int build1, int major2, int minor2, int revision2, int build2)
{
	int compareResult = 0;

	compareResult = CompareInts(major1, major2);
	if (compareResult == 0)
	{
		compareResult = CompareInts(minor1, minor2);
		if (compareResult == 0)
		{
			compareResult = CompareInts(revision1, revision2);
			if (compareResult == 0)
			{
				compareResult = CompareInts(build1, build2);
			}
		}
	}

	return compareResult;
}


bool CUtilities::HasFileExtension(const std::string &filename, const std::string &extension)
{
	bool retVal(false);
	try {
		string filenam;
		string ext;
		string extIn;

		size_t lastOfPath = filename.find_last_of("/");
		if (lastOfPath == 0) 
		{
			filenam = filename;
		} else {
			filenam = filename.substr(lastOfPath+1);
		}
		
		if (filenam.size() >= extension.size())
		{
			int dot = filenam.find_last_of(".");
			if ((dot == 0) && (filenam[0] != '.'))
			{
				ext = filenam;
			} else {
				ext = filenam.substr(dot + 1);
			}

			dot = extension.find_last_of(".");
			if ((dot == 0) && (extension[0] != '.'))
			{
				extIn = extension;
			} else {
				extIn = extension.substr(dot + 1);
			}

			if (ext.size() >= extIn.size()) 
			{
				std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
				std::transform(extIn.begin(), extIn.end(), extIn.begin(), ::tolower);
				int esize = ext.size() - extIn.size();
				retVal = ((ext == extIn) || (extIn == ext.substr(esize)));
			}
		}
	}
	catch (...) 
	{
		retVal = false;
	}

	return retVal;
}

std::string CUtilities::MaskPin(const std::string &originalString)
{
	std::string maskedString = originalString;
	std::size_t found = maskedString.find("pin");
	std::string replacementString = "pin\":\"****\"";

	if (found != std::string::npos)
	{
		maskedString.replace(found, replacementString.size(), replacementString);
	}

	return maskedString;
}

time_t CUtilities::MakeTimeFromNvramBuffer( const byte *nvramBuffer, int bufferSize )
{
	size_t offset = 0;
	bool ok = true;

	int nYear, nMonth, nDay, nHour, nMin, nSec;

	CUtilities::MemCopyZero_FromOffset(&ok, &nYear, sizeof nYear, nvramBuffer, bufferSize, &offset);
	if ( nYear < 1970 )
		nYear = 1971;
	else if ( nYear > 2100 )
		nYear = 2000;

	CUtilities::MemCopyZero_FromOffset(&ok, &nMonth, sizeof nMonth, nvramBuffer, bufferSize, &offset);
	if ( nMonth < 1 || nMonth > 12 )
		nMonth = 1;

	CUtilities::MemCopyZero_FromOffset(&ok, &nDay, sizeof nDay, nvramBuffer, bufferSize, &offset);
	if ( nDay < 1 || nDay > 31 )
		nDay = 1;

	CUtilities::MemCopyZero_FromOffset(&ok, &nHour, sizeof nHour, nvramBuffer, bufferSize, &offset);
	if ( nHour < 0 || nHour > 23 )
		nHour = 0;

	CUtilities::MemCopyZero_FromOffset(&ok, &nMin, sizeof nMin, nvramBuffer, bufferSize, &offset);
	if ( nMin < 0 || nMin > 59 )
		nMin = 0;

	CUtilities::MemCopyZero_FromOffset(&ok, &nSec, sizeof nSec, nvramBuffer, bufferSize, &offset);
	if ( nSec < 0 || nSec > 59 )
		nSec = 0;

	time_t retTime(GetTimeFromFields( nYear, nMonth, nDay, nHour, nMin, nSec ));

	return retTime;
}

int CUtilities::GetNvramBufferFromTime( time_t time, byte *nvramBuffer, int bufferSize )
{
	size_t offset = 0;

	if ( bufferSize >= (int)NVRAM_TIME_SIZE )
	{
		int nYear;
		int nMonth;
		int nDay;
		int nHour;
		int nMin;
		int nSec;
		GetFieldsFromTime(time, nYear, nMonth, nDay, nHour, nMin, nSec);
		bool ok = true;
		CUtilities::MemCopySafe_ToOffset(&ok, nvramBuffer, bufferSize, &offset, &nYear, sizeof (int));
		CUtilities::MemCopySafe_ToOffset(&ok, nvramBuffer, bufferSize, &offset, &nMonth, sizeof (int));
		CUtilities::MemCopySafe_ToOffset(&ok, nvramBuffer, bufferSize, &offset, &nDay, sizeof (int));
		CUtilities::MemCopySafe_ToOffset(&ok, nvramBuffer, bufferSize, &offset, &nHour, sizeof (int));
		CUtilities::MemCopySafe_ToOffset(&ok, nvramBuffer, bufferSize, &offset, &nMin, sizeof (int));
		CUtilities::MemCopySafe_ToOffset(&ok, nvramBuffer, bufferSize, &offset, &nSec, sizeof (int));
	}

	return offset;
}

bool  CUtilities::IsEven(__int64 num)
{
	return ((num % 2) == 0);
}

SYSTEMTIME CUtilities::GetUTCSystemTime(time_t time)
{
	SYSTEMTIME systime;

	//This is causing issues with the time on transactions
	//GetSystemTime is always correct so there is no reason to do this math.
	//SPS Tracker #27611 and #27880
	/*time-= m_tzOffset;
	time.GetAsSystemTime(systime);*/

	struct tm *gmt = gmtime(&time);
	
	// Copy gmt to systime;
	systime.wYear = gmt->tm_year + TmStructYearOffset;
	systime.wDay = gmt->tm_mday;
	systime.wMonth = gmt->tm_mon + TmStructMonthOffset;
	systime.wHour = gmt->tm_hour;
	systime.wMinute = gmt->tm_min;
	systime.wSecond = gmt->tm_sec;
	
	//Tracker #30719
	systime.wMilliseconds = 0;

	return systime;
}


SYSTEMTIME CUtilities::GetLocalSystemTime(time_t time)
{
	SYSTEMTIME systime;

	//This is causing issues with the time on transactions
	//GetSystemTime is always correct so there is no reason to do this math.
	//SPS Tracker #27611 and #27880
	/*time-= m_tzOffset;
	time.GetAsSystemTime(systime);*/

	struct tm *gmt = localtime(&time);
	
	// Copy gmt to systime;
	systime.wYear = gmt->tm_year + TmStructYearOffset;
	systime.wDay = gmt->tm_mday;
	systime.wMonth = gmt->tm_mon + TmStructMonthOffset;
	systime.wHour = gmt->tm_hour;
	systime.wMinute = gmt->tm_min;
	systime.wSecond = gmt->tm_sec;
	
	//Tracker #30719
	systime.wMilliseconds = 0;

	return systime;
}


time_t CUtilities::MakeTimeFromMMDDYYYY_HHMMSS(byte *dateArray, byte *timeArray)
{
	int year, month, day, hour, minute, second;
	year = ((dateArray[2] & 0xf0) >> 4)*1000 + (dateArray[2] & 0x0f)*100 + ((dateArray[3] & 0xf0) >> 4)*10 + (dateArray[3] & 0x0f)*1;
	month = ((dateArray[0] & 0xf0) >>4)*10 + (dateArray[0] & 0x0f);
	day = ((dateArray[1] & 0xf0) >>4)*10 + (dateArray[1] & 0x0f);

	if (timeArray != NULL)
	{
		hour = ((timeArray[0] & 0xf0) >>4)*10 + (timeArray[0] & 0x0f);
		minute = ((timeArray[1] & 0xf0) >>4)*10 + (timeArray[1] & 0x0f);
		second = ((timeArray[2] & 0xf0) >>4)*10 + (timeArray[2] & 0x0f);
	}
	else
		hour = minute = second = 0;

	SYSTEMTIME sysTime;
	//Added By Spencer Fairbairn 10/29/2007
	//a certain game in Arizona is reporting doodee dates, causing a sentinel crash
	//this check ensures a good date
	if ( year >= 1970 &&
		month >= 1 && month <= 12 &&
		day >= 1 && day <= 31 &&
		hour >= 0 && hour <= 23 &&
		minute >= 0 && minute <= 59 &&
		second >= 0 && second <= 59
		)
	{
		sysTime.wYear = (WORD)year;
		sysTime.wMonth = (WORD)month;
		sysTime.wDay = (WORD)day;
		sysTime.wHour = (WORD)hour;
		sysTime.wMinute = (WORD)minute;
		sysTime.wSecond = (WORD)second;
		sysTime.wMilliseconds = 0;
	}
	else
	{
		//if it's poody, return now
		GetLocalSystemTime(&sysTime);
	}

	struct tm t;
	memset(&t, 0, sizeof(struct tm));
	
	t.tm_year = sysTime.wYear - TmStructYearOffset;
	t.tm_mon = sysTime.wMonth - TmStructMonthOffset;
	t.tm_mday = sysTime.wDay ;
	t.tm_hour = sysTime.wHour;
	t.tm_min = sysTime.wMinute;
	t.tm_sec = sysTime.wSecond;
	t.tm_isdst = 0;
	time_t time = std::mktime(&t);
	return time;
}

// "cardID" is pointer to a cardIDLen-byte array of arbitrary binary values.
std::string CUtilities::MakeStringFromCardID(const byte *cardID, int cardIDLen)
{
	std::string cardNumber;

	if (cardID != nullptr && cardIDLen > CCard::CARD_ID_TYPE_POS)
	{
		// A card prefix of CCardConfig::INVALID_CARD_PREFIX indicates that there is no (valid) cardID.
		if (CCardConfig::INVALID_CARD_PREFIX != cardID[CCard::CARD_ID_TYPE_POS])
		{
			cardNumber = MakeCStringFromBCD(cardID, cardIDLen);
		}
	}

	return cardNumber;
}

std::string CUtilities::MakeCStringFromBCD(const byte *bcdArray, byte bcdLength)
{
	TCHAR *tchArray = new TCHAR[bcdLength * 2 + 1];
	byte temp;

	for (int i = 0; i < bcdLength; i++)
	{
		temp = (bcdArray[i] & 0xf0) >> 4;
		if (temp > 9)
			tchArray[i*2] = temp + 55; //A
		else
			tchArray[i*2] = temp | 0x30; //zero - 9

		temp = bcdArray[i] & 0x0f;
		if (temp > 9)
			tchArray[i*2 + 1] = temp + 55;
		else
			tchArray[i*2 + 1] = temp | 0x30;
	}

	tchArray[bcdLength * 2] = 0;
	string retString(tchArray);
	delete [] tchArray;
	return retString;
}

std::string CUtilities::MakeStringFromBCD(const byte *bcdArray, int bcdLength)
{
	return CUtilities::MakeCStringFromBCD(bcdArray, (byte)bcdLength);
}

void CUtilities::MakeCardIDFromCString(LPCTSTR text, byte * buffer, int bufferLength)
{
	if (buffer != NULL && bufferLength > 0)
	{
		ZeroMemory(buffer, bufferLength);

		if (text != NULL)
		{
			int n = 0;
			unsigned int value(0); // Init to known value, in case swscanf_s() doesn't change it.
			BYTE converter;
			LPCTSTR bufPtr = text;
			int textLength = _tcslen(text);

			for (int i = 0; (i < bufferLength) && (n < textLength); i++)
			{
				sscanf(bufPtr + n, "%2x", &value);
				n+= 2;
				converter = (BYTE)value;
				memcpy(buffer + i, &converter, sizeof(BYTE));
			}
		}
	}
}

string CUtilities::ToHexString(const BYTE *data, DWORD dataLength, DWORD maximumDataLength)
{
	if (data == NULL)
	{
		dataLength = 0;
	}

	if (dataLength > maximumDataLength)
	{
		dataLength = maximumDataLength;
	}

	stringstream ss;
	if (dataLength > 0)
	{
		while (dataLength--)
		{
			ss << std::hex << std::setw(2) << std::setfill('0') << (unsigned)*data++;
			if (dataLength > 0)
			{
				ss << ' ';
			}
		}
	}

	return ss.str();
}

void CUtilities::FileTimeToComponents(const ULONGLONG &fileTime, DWORD &hours, DWORD &minutes, DWORD &seconds, DWORD &milliseconds)
{
	ULONGLONG tempTime = fileTime / FILETIME_UNITS_PER_MILLISECOND; // tempTime is in milliseconds.
	milliseconds = (DWORD)(tempTime % MILLISECONDS_PER_SECOND);

	tempTime /= MILLISECONDS_PER_SECOND; // tempTime is in seconds.
	seconds = (DWORD)(tempTime % SECONDS_PER_MINUTE);

	tempTime /= SECONDS_PER_MINUTE; // tempTime is in minutes.
	minutes = (DWORD)(tempTime % MINUTES_PER_HOUR);

	tempTime /= MINUTES_PER_HOUR; // tempTime is in hours.
	hours = (DWORD)tempTime;
}

bool CUtilities::MakeMMDDYYYYFromTime(time_t date, byte *bcdVal)
{
	tm dateTm = GetLocalTmFromTime(date);
	int year = dateTm.tm_year + TmStructYearOffset;
	int month = dateTm.tm_mon + TmStructMonthOffset;
	int day = dateTm.tm_mday;

	Dword2Bcd(month, &bcdVal[0],1);
	Dword2Bcd(day, &bcdVal[1],1);
	Dword2Bcd(year,&bcdVal[2],2);

	return true;
}

// TODO-PORT - Check performance of double-precision pow() function on ARM processors?
bool CUtilities::Dword2Bcd(DWORD dwVal, byte *bcdVal, int BcdSize)
{

	DWORD value;
	byte *sVal = new byte[BcdSize*2+4]; // 2 digits per BCD byte + 4 extra chars.
	bool retVal = true;
	byte tmpByte;
	double power;

	value = dwVal;
	for (int x = 0, i = BcdSize*2 - 1; i >= 0 && retVal == true; i -= 2, x++)
	{
		power = pow((double)10,i);
		tmpByte = (byte) ( power == 0 ? value : value / power); // get digit
		if (tmpByte > 9)
			retVal = false;
		sVal[x] = tmpByte << 4;
		value -= (DWORD)(tmpByte * power);

		power /= 10;
		tmpByte = (byte)(power == 0 ? value : value / power); // get digit
		if (tmpByte > 9)
			retVal = false;
		sVal[x] |= tmpByte;
		value -= (DWORD)(tmpByte * power);

	}

	if (retVal)
	{
		// all good here, copy Sval to bcdVal
		memcpy(bcdVal,sVal,BcdSize);
	}
	delete [] sVal;

	return retVal;
}

void CUtilities::SetLastError(int error)
{
	errno = error;
}

int CUtilities::GetLastError()
{
	return errno;
}

std::string CUtilities::GetLastErrorCodeString()
{
	int errorCode(GetLastError());

	char buf[512];
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE
	// The (POSIX) XSI-compliant version of strerror_r() is provided.
	if (0 != strerror_r(errorCode, buf, sizeof(buf)))
	{
		buf[0] = 0; // If the POSIX call failed then don't use the buffer contents.
	}
	char *errorDesc(buf);
#else
	// Otherwise, the GNU-specific version of strerror_r() is provided.
	char *errorDesc(strerror_r(errorCode, buf, sizeof(buf)));
#endif

	string errorString(FormatString("errorCode=%d errorDesc=%s", (int)errorCode, static_cast<LPCTSTR>(errorDesc)));
	return errorString;
}

// Note that chrono::milliseconds is signed integer type of at least 45 bits.
// So conversion from a DWORD should be okay.
void CUtilities::Sleep(DWORD dwMilliseconds)
{
    if (dwMilliseconds == 0) 
    {
		dwMilliseconds = SLEEP_YIELD_MILLISECONDS;
    }
    this_thread::sleep_for(chrono::milliseconds(dwMilliseconds));
}

// Also, clock() can be used directly if values in milliseconds are either guaranteed or not required.
// Also, clock() can be scaled to milliseconds.
// Also, clock() returns milliseconds since APPLICATION start on some systems.
DWORD CUtilities::GetTickCount()
{
	return (DWORD)chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count();
}

std::tm CUtilities::GetCurrentTm()
{
	//this version of GetCurrentTime uses ...
	//and so always returns a tm object with local time
	//based on the updated TZ offset.
	time_t utcTime(time(nullptr));
	tm localTm(GetLocalTmFromTime(utcTime));
	return localTm;
}

time_t CUtilities::GetCurrentTime()
{
	return time(nullptr);
}

time_t CUtilities::GetUTCTime()
{
	return GetUTCFromLocalTime(GetCurrentTime());
}

time_t CUtilities::GetUTCFromLocalTime(time_t localTime)
{
	localTime -= m_tzOffset;
	return localTime;
}

time_t CUtilities::GetLocalTimeFromUTC(time_t utcTime)
{
	if (IsDateValid(utcTime))
	{
		utcTime += m_tzOffset;
	}
	return utcTime;
}

time_t CUtilities::GetTimeFromUTCSystemTime(const SYSTEMTIME &utcTime)
{
	//then convert the utctime to a local ctime, and add the difference;
	time_t retVal;
	
	try
	{
		if (utcTime.wYear >= MINCTIMEYEAR && utcTime.wYear <= MAXCTIMEYEAR)
		{
			retVal = GetTimeFromFields(utcTime.wYear, utcTime.wMonth, utcTime.wDay, utcTime.wHour, utcTime.wMinute, utcTime.wSecond);
			retVal += m_tzOffset;
		}
		else
		{
			retVal = GetTimeFromUTCSystemTime(Get1970Time());
		}
	}
	catch (std::exception &)
	{
		retVal = GetTimeFromUTCSystemTime(Get1970Time());
	}

	return retVal;
}

void CUtilities::SetTzOffset(int offset)
{
	m_tzOffset = offset * 60; //convert offset in minutes to timespan in seconds
}

time_t CUtilities::GetTzOffset()
{
	return m_tzOffset;
}

bool CUtilities::IsTimezoneValid(int timezoneOffsetInMinutes)
{
	const int MAX_TIMEZONE_OFFSET = 24 * MINUTES_PER_HOUR;
	const int MIN_TIMEZONE_OFFSET = -24 * MINUTES_PER_HOUR;

	return (timezoneOffsetInMinutes > MIN_TIMEZONE_OFFSET && timezoneOffsetInMinutes < MAX_TIMEZONE_OFFSET);
}

// pErrorCode can be NULL.
bool CUtilities::FileExists( const std::string &filename)
{
	bool fileFound(true);
	struct stat fd;
	fileFound = 0 == stat(filename.c_str(), &fd);
	return fileFound;
}

static bool __sleep(DWORD timeout)
{
	CUtilities::Sleep(timeout);
	return true;
}

int CUtilities::GetFileSize(std::fstream& file)
{
    file.seekg(0,ios_base::end);
    int size = file.tellg();
	return size;
}

int CUtilities::GetFileAttributes(
	LPCTSTR pPathname,
	int* pErrorCode,
	bool waitForDisk,
	bool throwOnFailure)
{
	DWORD diskWaits = waitForDisk ? 0 : NUMBER_OF_WAIT_FOR_DISK_PAUSES;
	int errorCode;
	int attr(0);

	do
	{
		struct stat fileAttr;
		if (0 == stat(pPathname, &fileAttr))
		{
			attr = fileAttr.st_mode;
			errorCode = ERROR_SUCCESS;
		}
		else
		{
			errorCode = GetLastError();
		}

	} while (errorCode == ENOENT &&
		diskWaits++ < NUMBER_OF_WAIT_FOR_DISK_PAUSES &&	__sleep(WAIT_FOR_DISK_PAUSE));

	if (errorCode != ERROR_SUCCESS)
	{
		if (pErrorCode != nullptr)
			*pErrorCode = errorCode;

		if (throwOnFailure)
			throw SentinelFileException(pPathname, errorCode);
	}
	else
	{
		if (pErrorCode != nullptr)
			*pErrorCode = ERROR_SUCCESS;
	}

	return attr;
}

bool CUtilities::DoesFileExist(
	std::string sFile,
	int* pErrorCode,
	bool waitForDisk,
	bool throwOnFailure)
{
	return DoesFileExist(sFile.c_str(), pErrorCode, waitForDisk, throwOnFailure);
}

bool CUtilities::DoesFileExist(
	LPCTSTR pFile,
	int* pErrorCode,
	bool waitForDisk,
	bool throwOnFailure)
{
	int attr = GetFileAttributes(pFile, pErrorCode, waitForDisk, throwOnFailure);
	return S_ISREG(attr);
}

bool CUtilities::DoesDirExist(
	LPCTSTR pDir,
	int* pErrorCode,
	bool waitForDisk,
	bool throwOnFailure)
{
	DWORD attr = GetFileAttributes(pDir, pErrorCode, waitForDisk, throwOnFailure);
	return S_ISDIR(attr);
}

BOOL CUtilities::GetDiskFreeSpaceEx(
	LPCTSTR lpDirectoryName,
	PULARGE_INTEGER lpFreeBytesAvailable,
	PULARGE_INTEGER lpTotalNumberOfBytes,
	PULARGE_INTEGER lpTotalNumberOfFreeBytes
	)
{
	BOOL ok(FALSE);
	ULONGLONG freeBytesAvailable(0);
	ULONGLONG totalNumberOfBytes(0);
	ULONGLONG totalNumberOfFreeBytes(0);

	struct statvfs statInfo;
	if (0 == statvfs(lpDirectoryName, &statInfo)) {
		freeBytesAvailable = statInfo.f_bsize * (ULONGLONG)statInfo.f_bavail;
		totalNumberOfBytes = statInfo.f_frsize * (ULONGLONG)statInfo.f_blocks;
		totalNumberOfFreeBytes = statInfo.f_bsize * (ULONGLONG)statInfo.f_bfree;
		ok = TRUE;
	}

	if (lpFreeBytesAvailable != nullptr)
	{
		lpFreeBytesAvailable->QuadPart = freeBytesAvailable;
	}

	if (lpTotalNumberOfBytes != nullptr)
	{
		lpTotalNumberOfBytes->QuadPart = totalNumberOfBytes;
	}

	if (lpTotalNumberOfFreeBytes != nullptr)
	{
		lpTotalNumberOfFreeBytes->QuadPart = totalNumberOfFreeBytes;
	}

	return ok;
}

time_t CUtilities::SysTimeToUlarge(const SYSTEMTIME &inTime)
{
	time_t retVal;
	struct tm tTm;

	tTm.tm_year = inTime.wYear - TmStructYearOffset;
	tTm.tm_mon = inTime.wMonth - TmStructMonthOffset;
	tTm.tm_mday = inTime.wDay;
	tTm.tm_hour = inTime.wHour;
	tTm.tm_min = inTime.wMinute;
	tTm.tm_sec = inTime.wSecond;
	tTm.tm_isdst = 0;

    #ifdef LEAK_CHECK
    // The mktime function allocates memory internally that it does not release.
    // Decrementing track_heap_allocations-- hides these allocations from the leak report.
    //track_heap_allocations--; // disable heap tracking temporarily
    LockHeapAndDisableTracking();
    #endif

    retVal = mktime(&tTm);

    #ifdef LEAK_CHECK
    //track_heap_allocations++; // re-enable heap tracking
    UnlockHeapAndReenableTracking();
    #endif

	return retVal;
}


SYSTEMTIME CUtilities::UlargeToSysTime(time_t &inTime)
{
    #ifdef LEAK_CHECK
    // The localtime function allocates heap memory that can't be explicitly released.
    // We temporarily disable heap tracking to hide this leak from our report.
    LockHeapAndDisableTracking();
    #endif

	SYSTEMTIME systime;

	struct tm *gmt = localtime (&inTime);
	systime.wYear = gmt->tm_year + TmStructYearOffset;
	systime.wDay = gmt->tm_mday;
	systime.wMonth = gmt->tm_mon + TmStructMonthOffset;
	systime.wHour = gmt->tm_hour;
	systime.wMinute = gmt->tm_min;
	systime.wSecond = gmt->tm_sec;
	systime.wMilliseconds = 0;

    #ifdef LEAK_CHECK
    UnlockHeapAndReenableTracking();
    #endif

	return systime;
}

// Compares the provided system times.
// Returns -1 if s1 < s2
//			1 if s1 > s2
//			0 if s1 = s2
int CUtilities::CompareSysTimes( const SYSTEMTIME &st1, const SYSTEMTIME &st2 )
{
    if (st1.wYear < st2.wYear) {
        return -1;
    }
    
    if (st1.wYear > st2.wYear) {
        return 1;
    }

    if (st1.wMonth < st2.wMonth) {
        return -1;
    }

    if (st1.wMonth > st2.wMonth) {
        return 1;
    }

    if (st1.wDay < st2.wDay) {
        return -1;
    }

    if (st1.wDay > st2.wDay) {
        return 1;
    }

    if (st1.wHour < st2.wHour) {
        return -1;
    }

    if (st1.wHour > st2.wHour) {
        return 1;
    }

    if (st1.wMinute < st2.wMinute) {
        return -1;
    }

    if (st1.wMinute > st2.wMinute) {
        return 1;
    }

    if (st1.wSecond < st2.wSecond) {
        return -1;
    }

    if (st1.wSecond > st2.wSecond) {
        return 1;
    }

	return 0;
}

COLORREF CUtilities::ConvertRGBToBGR(unsigned int color)
{
	COLORREF retColor = 0;

	//extract the red and move it into place.
	retColor += (color & 0xFF0000) >> 16;

	//add the green
	retColor += (color & 0x00FF00);

	//and then the blue
	retColor += (color & 0x0000FF) << 16;


	return retColor;
}

std::string CUtilities::GetFormattedNumberString( DWORD val )
{
	string retVal;

	retVal = FormatString( _T("%d"), val );

	if( retVal.length() >= 4 )
	{
		retVal.insert(retVal.length() - 3, _T(","));
	}

	if(retVal.length() >= 8)
	{
		retVal.insert(retVal.length() - 7, _T(","));
	}

	if(retVal.length() >= 12)
	{
		retVal.insert(retVal.length() - 11, _T(","));
	}

	if(retVal.length() >= 16)
	{
		retVal.insert(retVal.length() - 15, _T(","));
	}

	return retVal;
}

std::string CUtilities::GetFormattedNumberString(DWORD val, const std::string &text)
{
	string retStr =CUtilities::GetFormattedNumberString(val);

	if (!text.empty())
	{
		retStr += _T(" ");
		retStr += text;
	}

	return retStr;
}

std::string CUtilities::GetFormattedDollarsPenniesString(DWORD dollars, DWORD pennies)
{
	string retStr;
	string number = GetFormattedNumberString(dollars);
	retStr = FormatString( CASH_FORMAT, number.c_str(), pennies);
	retStr.insert(0, CUtilities::LoadResourceString(IDS_CASH_SYMBOL));
	return retStr;
}

std::string CUtilities::GetFormattedDollarsPenniesString(uint64_t dollars, DWORD pennies)
{
	string retStr;
	string number = GetFormattedNumberString(dollars);
	retStr = FormatString(CASH_FORMAT, number.c_str(), pennies);
	retStr.insert(0, CUtilities::LoadResourceString(IDS_CASH_SYMBOL));
	return retStr;
}

std::string CUtilities::GetFormattedDollarsPenniesStringInt64(int64_t dollars, DWORD pennies)
{
	string retStr;
	string number = GetFormattedNumberString(dollars);
	retStr = FormatString( CASH_FORMAT, number.c_str(), pennies);
	retStr.insert(0, CUtilities::LoadResourceString(IDS_CASH_SYMBOL));
	return retStr;
}

std::string CUtilities::GetFormattedCashString(DWORD cashValue)
{
	string retStr(GetFormattedDollarsPenniesString(cashValue / ONE_DOLLAR_IN_CENTS, cashValue % ONE_DOLLAR_IN_CENTS));
	return retStr;
}

std::string CUtilities::GetFormattedCashString(int longCashValue)
{
	int cashValue = (int)abs(longCashValue);
	string retStr(GetFormattedDollarsPenniesString(cashValue / ONE_DOLLAR_IN_CENTS, cashValue % ONE_DOLLAR_IN_CENTS));
	if (longCashValue < 0)
	{
		retStr.insert(1, 1, _T('-'));
	}
	return retStr;
}

std::string CUtilities::GetFormattedCashString( long longlongCashValue)
{
	unsigned long cashValue = (unsigned long)abs(longlongCashValue);
	string retStr(GetFormattedDollarsPenniesStringInt64(cashValue / ONE_DOLLAR_IN_CENTS, cashValue % ONE_DOLLAR_IN_CENTS));
	if (longlongCashValue < 0)
	{
		retStr.insert(1, 1, _T('-'));
	}
	return retStr;
}

std::string CUtilities::GetFormattedCashString( long long longlongCashValue)
{
	uint64_t cashValue = (uint64_t)abs(longlongCashValue);
	string retStr(GetFormattedDollarsPenniesString(cashValue / (uint64_t)ONE_DOLLAR_IN_CENTS, (DWORD)(cashValue % ONE_DOLLAR_IN_CENTS)));
	if (longlongCashValue < 0)
	{
		retStr.insert(1, 1, _T('-'));
	}
	return retStr;
}


std::string CUtilities::GetFormattedCashString(DWORD cashValue, LPCTSTR text)
{
	string retStr = CUtilities::GetFormattedCashString(cashValue);

	if (text != NULL)
	{
		retStr += _T(" ");
		retStr += text;
	}

	return retStr;
}

std::string CUtilities::GetFormattedNumberString(__int64_t val)
{
	std::string retVal;
	char _buffer[128];
	sprintf(_buffer, _T("%lld"), (long long int)val);
	retVal = string(_buffer);

	static const int DIGITS_BETWEEN_COMMAS(3);
	int index(DIGITS_BETWEEN_COMMAS + 1);

	while (index <= (int)retVal.length())
	{
		retVal.insert(retVal.length() - index + 1, ",");
		index += DIGITS_BETWEEN_COMMAS + 1;
	}

	return retVal;
}

std::string CUtilities::GetFormattedNumberString(uint64_t val)
{
	std::string retVal;
	char _buffer[128];
	sprintf(_buffer, _T("%llu"), (long long unsigned int)val);
	retVal = string(_buffer);

	static const size_t DIGITS_BETWEEN_COMMAS(3);
	size_t index(DIGITS_BETWEEN_COMMAS + 1);

	while (index <= retVal.length())
	{
		retVal.insert(retVal.length() - index + 1, ",");
		index += DIGITS_BETWEEN_COMMAS + 1;
	}

	return retVal;
}


std::string CUtilities::GetFormattedNumberString(const std::string& value)
{
	std::string result = "";
	int integerPartEndIndex = value.find('.');
	if (integerPartEndIndex == std::string::npos) {
		integerPartEndIndex = value.length();
	}

	int i = integerPartEndIndex - 1;
	int count = 0;
	while (i >= 0) {
		result = value[i] + result;
		count++;
		if (count % 3 == 0 && i != 0) {
			result = ',' + result;
		}
		i--;
	}

	if (integerPartEndIndex != value.length()) {
		result += value.substr(integerPartEndIndex);
	}

	return result;
}

/// <summary>
/// If *pOk is false then the contents of destBuffer are zet to zero.
/// Otherwise if there is destBufferSize memory left in srcBuffer
///    then the memory is copied the contents of destBuffer
///    and srcBufferOffset is updated to next location to copy from.
/// Otherwise *pOk is set to false and the contents of destBuffer are zet to zero.
/// <para/>
/// If pSrcBuffer==NULL then a srcBufferSize of zero is used.
/// <para/>
/// The memory pointed to by destBuffer MUST be >= destBufferSize.
/// The memory pointed to by srcBuffer MUST be >= srcBufferSize.
/// </summary>
/// <param name="pOk">IN/OUT - true if operation and preceding operations succeeded.  False otherwise.</param>
/// <param name="pDestBuffer">IN - address of destination buffer.</param>
/// <param name="destBufferSize">IN - size of destination buffer.</param>
/// <param name="pSrcBuffer">IN - address of source buffer.</param>
/// <param name="srcBufferSize">IN - size of source buffer.</param>
/// <param name="pSrcBufferOffset">IN/OUT - offset of source buffer for data for this operation.</param>
void CUtilities::MemCopyZero_FromOffset(bool *pOk,
	void *pDestBuffer, size_t destBufferSize,
	const byte *pSrcBuffer, size_t srcBufferSize,  size_t *pSrcBufferOffset)
{
	if (pSrcBuffer == NULL)
	{
		srcBufferSize = 0;
	}

	if (*pOk)
	{
		if (destBufferSize <= srcBufferSize &&
			*pSrcBufferOffset <= srcBufferSize &&
			*pSrcBufferOffset + destBufferSize <= srcBufferSize)
		{
			memcpy_s(pDestBuffer, destBufferSize, pSrcBuffer + *pSrcBufferOffset, destBufferSize);
			*pSrcBufferOffset += destBufferSize;
		}
		else
		{
			*pOk = false;
		}
	}

	if (!*pOk)
	{
		ZeroMemory(pDestBuffer, destBufferSize);
	}
}

// The memory pointed to by destBuffer MUST be >= destBufferSize.
// The memory pointed to by srcBuffer MUST be >= copyLength.
// If *pOk is false then nothing is done.
// Otherwise if there is room in the destBuffer for copyLength of data at *pDestBufferOffset
//    then the memory is copied and *pDestBufferOffset is updated to next copy location.
// Otherwise *pOk is set to false.
void CUtilities::MemCopySafe_ToOffset(bool *pOk,
	byte *pDestBuffer, size_t destBufferSize,  size_t *pDestBufferOffset,
	const void *pSrcBuffer, size_t copyLength)
{
	if (*pOk)
	{
		if (copyLength <= destBufferSize &&
			*pDestBufferOffset <= destBufferSize &&
			*pDestBufferOffset + copyLength <= destBufferSize)
		{
			memcpy_s(pDestBuffer + *pDestBufferOffset, destBufferSize - *pDestBufferOffset, pSrcBuffer, copyLength);
			*pDestBufferOffset += copyLength;
		}
		else
		{
			*pOk = false;
		}
	}
}

void CUtilities::MemCopySafe_IncSourcePtr(bool &ok,
	void *target,
	size_t targetSize,
	const BYTE *&source,
	const BYTE *sourceEnd)
{
	if (ok && targetSize > 0)
	{
		ok = target != nullptr && source != nullptr && sourceEnd != nullptr;
		if (ok)
		{
			ok = source + targetSize <= sourceEnd;
			if (ok)
			{
				memcpy(target, source, targetSize);
				source += targetSize;
			}
		}
	}
}

/// <summary>
/// Copy as many bytes from the source to the destination as will fit.
/// Zero fill any remaining destination bytes.
/// Advance the pointer to the source buffer and decrement the source size by the number of bytes copied.
/// <para/>
/// If pSrcBuffer==NULL then srcBufferSize is set to 0.
/// </summary>
/// <param name="pDestBuffer">IN - address of destination buffer.</param>
/// <param name="destBufferSize">IN - size of destination buffer.</param>
/// <param name="pSrcBuffer">IN/OUT - address of source buffer.</param>
/// <param name="srcBufferSize">IN/OUT - size of source buffer.</param>
/// <remarks>
/// </remarks>
void CUtilities::MemCopyZeroIncSourcePtr(
		void *pDestBuffer, size_t destBufferSize,
		void const *&pSrcBuffer, size_t &srcBufferSize)
{
	if (pSrcBuffer == NULL)
	{
		srcBufferSize = 0;
	}

	size_t copySize = srcBufferSize;
	if (destBufferSize <= srcBufferSize)
	{
		copySize = destBufferSize;
		memcpy_s(pDestBuffer, destBufferSize, pSrcBuffer, destBufferSize);
	}
	else
	{
		memcpy_s(pDestBuffer, destBufferSize, pSrcBuffer, srcBufferSize);
		ZeroMemory((byte *)pDestBuffer + srcBufferSize, destBufferSize - srcBufferSize);
	}

	if (copySize > 0)
	{
		pSrcBuffer = (char const *)pSrcBuffer + copySize;
		srcBufferSize -= copySize;
	}
}

/// <summary>
/// Copy as many bytes from the source to the destination as will fit.
/// Zero fill any remaining destination bytes.
/// Advance the pointer to the source buffer and decrement the source size by the number of bytes copied.
/// <para/>
/// If pSrcBuffer==NULL or srcBufferLength &lt; 0 then srcBufferLength is set to 0.
/// </summary>
/// <param name="pDestBuffer">IN - address of destination buffer.</param>
/// <param name="destBufferSize">IN - size of destination buffer.</param>
/// <param name="pSrcBuffer">IN/OUT - address of source buffer.</param>
/// <param name="srcBufferLength">IN/OUT - size of source buffer.</param>
/// <remarks>
/// </remarks>
void CUtilities::MemCopyZeroIncSourcePtr(
		void *pDestBuffer, size_t destBufferSize,
		void const *&pSrcBuffer, int &srcBufferLength)
{
	srcBufferLength = max(0, srcBufferLength);
	size_t srcBufferSize = (size_t)srcBufferLength;
	MemCopyZeroIncSourcePtr(pDestBuffer, destBufferSize, pSrcBuffer, srcBufferSize);
	srcBufferLength = (int)srcBufferSize;
}

// CUtilities::MemCopyZero
//     IN pDestBuffer - address of destination buffer.
//     IN destBufferSize - size of destination buffer.
//     IN pSrcBuffer - address of source buffer.
//     IN srcBufferSize - size of source buffer.  If pSrcBuffer==NULL then 0 is used for the srcBufferSize.
// The memory pointed to by destBuffer MUST be >= destBufferSize.
// The memory pointed to by srcBuffer MUST be >= copyLength.
// If destBufferSize <= srcBufferSize
//     then destBufferSize bytes are copied from *pSrcBuffer to *pDestBuffer.
// If destBufferSize > srcBufferSize
//     then srcBufferSize bytes are copied from *pSrcBuffer to *pDestBuffer
//     and the remaining bytes of *pDestBuffer are set to zero.
void CUtilities::MemCopyZero(
		void *pDestBuffer, size_t destBufferSize,
		void const *pSrcBuffer, size_t srcBufferSize)
{
	MemCopyZeroIncSourcePtr(pDestBuffer, destBufferSize, pSrcBuffer, srcBufferSize);
}

// The memory pointed to by destBuffer MUST be >= destBufferSize.
// The memory pointed to by srcBuffer MUST be >= copyLength.
// If destBufferSize <= srcBufferSize
//     then destBufferSize bytes are copied from *pSrcBuffer to *pDestBuffer.
// If destBufferSize > srcBufferSize
//     then srcBufferSize bytes are copied from *pSrcBuffer to *pDestBuffer
//     and the remaining bytes of *pDestBuffer are set to zero.
void CUtilities::MemCopyZero(
		void *pDestBuffer, size_t destBufferSize,
		void const *pSrcBuffer, int srcBufferLength)
{
	//_ASSERT(0 <= srcBufferLength);
	srcBufferLength = max(0, srcBufferLength);
	CUtilities::MemCopyZero(pDestBuffer, destBufferSize, pSrcBuffer, (size_t)srcBufferLength);
}

// Get the version information from the executable
string CUtilities::GetEXEVersion(void)
{
	if (m_exeVersion.empty())
	{
		const string VersionFilename = "/version.txt";

		ifstream ifs(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_BASE) + VersionFilename, std::ifstream::in);

		m_exeVersion = "0.0.0.0";

		if (ifs.is_open())
		{
			string temp;
			ifs >> temp;
			m_exeVersion = temp;
			ifs >> temp;
			m_exeVersion += "." + temp;
			ifs >> temp;
			m_exeVersion += "." + temp;
			ifs >> temp;
			const DWORD MAX_NUMBER_OF_REVISION_DIGIT(3);
			if (temp.size() > MAX_NUMBER_OF_REVISION_DIGIT)
			{
				temp.erase(0, 1);
			}
			m_exeVersion += "." + temp;
			ifs.close();
			cout << "sentinel.img's version is" << " " << m_exeVersion << endl;
		}
	}

	return m_exeVersion;
}

bool CUtilities::RemoveReadOnlyAttribute(LPCTSTR pPathname, bool waitForDisk)
{
	DWORD errorCode;
	int retcode;
	unsigned int mode;
	DWORD diskWaits = waitForDisk ? 0 : NUMBER_OF_WAIT_FOR_DISK_PAUSES;
	struct stat fileAttributes;

	do
	{
		if ((retcode = stat(pPathname, &fileAttributes)) < 0) 
		{
			errorCode = retcode ? 0 : CUtilities::GetLastError();
		} else {
			mode = fileAttributes.st_mode;
		}

	} while (ENOENT == errorCode && diskWaits++ < NUMBER_OF_WAIT_FOR_DISK_PAUSES
			 && __sleep(WAIT_FOR_DISK_PAUSE));
	// Set the write flags
	mode = mode | S_IWUSR | S_IWOTH;
	retcode = chmod(pPathname, mode);
	return (retcode < 0)? FALSE : TRUE;
}

bool CUtilities::RenameFile(const std::string &existingFilename, const std::string &newFilename, DWORD *pErrorCode,
	bool waitForDisk, bool replaceExisting, bool throwOnFailure)
{
	int result(0);
	int errorCode;
	DWORD diskWaits = waitForDisk ? 0 : NUMBER_OF_WAIT_FOR_DISK_PAUSES;

	do
	{
		if (replaceExisting)
		{
			result = rename(existingFilename.c_str(), newFilename.c_str());
		}
		else
		{
			cout << "Test link " << existingFilename << " to " << newFilename << endl;
			
			// If newFilename exists then link() will fail.
			result = link(existingFilename.c_str(), newFilename.c_str());
			if (0 == result)
			{
				
				cout << "Test link successful, now unlink existing" << endl;
				
				// link() succeeded, so unlink() existingFilename.
				result = unlink(existingFilename.c_str());
				cout << "unlink result = " << result << endl;
			}
		}
		errorCode = (result == 0) ? 0 : CUtilities::GetLastError();

	} while (ENOENT == errorCode &&
		diskWaits++ < NUMBER_OF_WAIT_FOR_DISK_PAUSES && __sleep(WAIT_FOR_DISK_PAUSE));

	if (result && throwOnFailure)
	{
		string file(existingFilename);
		file += " -> ";
		file += newFilename;
		throw SentinelFileException(file.c_str(), errorCode);
	}

	if (pErrorCode != nullptr)
		*pErrorCode = (result == 0) ? ERROR_SUCCESS : errorCode;

    //Returning bool. So when result will be zero, wouldn't it be sending false for successful case and true for non-successful case.
    //Is it intentional.
	return result; 
}

bool CUtilities::DeleteFile(LPCTSTR filename, DWORD *pErrorCode, bool waitForDisk)
{
	bool result = true;
	DWORD errorCode;
	DWORD diskWaits = waitForDisk ? 0 : NUMBER_OF_WAIT_FOR_DISK_PAUSES;

	do
	{
		result = ::remove(filename);
		errorCode = result ? 0 : GetLastError();

	} while (!result &&
		diskWaits++ < NUMBER_OF_WAIT_FOR_DISK_PAUSES && __sleep(WAIT_FOR_DISK_PAUSE));

	if (pErrorCode != nullptr)
		*pErrorCode = result ? ERROR_SUCCESS : errorCode;

	return result;
}

bool CUtilities::CreateDirectory(const std::string &directoryToCreate)
{
	 const int ok = mkdir(directoryToCreate.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	 return ((-1 != ok) ? true: false);
}

bool CUtilities::DeleteDirectory(
	const std::string &directoryToDelete,
	bool leaveBaseDirectory,
	const std::map<std::string, bool>* pFilesToKeep)
 {
	 int result(0);

	 if (DoesDirExist(directoryToDelete.c_str()))
	 {
		 std::unique_ptr<DIR, closedir_deleter> dir(opendir(directoryToDelete.c_str()));
		 if (dir)
		 {
			 struct dirent *p;
			 while (!result && (p = readdir(dir.get())))
			 {
				 string fileName(p->d_name);
				 bool okayToDelete(pFilesToKeep == nullptr || pFilesToKeep->find(fileName) == pFilesToKeep->end());
				 string fullPath(directoryToDelete + "/" + fileName);

				 /* Skip . and .. */
				 if ("." == fileName || ".." == fileName) continue;

				 struct stat statbuf;
				 if (!stat(fullPath.c_str(), &statbuf))
				 {
					 if (S_ISDIR(statbuf.st_mode))
					 {
						 // recurse to lower directory
						 result = DeleteDirectory(fullPath, false, pFilesToKeep);
					 }
					 else if (okayToDelete)
					 {
						 result = unlink(fullPath.c_str());
					 }
				 }
			 }
		 }

		 if (!result && !leaveBaseDirectory)
		 {
			 result = rmdir(directoryToDelete.c_str());
		 }
	 }

	 return result;
 }

int CUtilities::FileCount(const std::string& directory, std::initializer_list<std::string> fileTypes)
{
	std::string lsCommand = "cd '" + directory + "' && ls ";

	for (auto fileType : fileTypes)
	{
		if (!fileType.empty())
		{
			lsCommand.append(fileType);
			lsCommand.append(" ");
		}
	}

	lsCommand.append(" | wc -l");
	return atoi(CUtilities::GetSystemResultString(lsCommand).c_str());
}

bool CUtilities::PathIsSlash(TCHAR ch)
{
	return (ch == '/') ? true : false;
}

bool CUtilities::PathIsDelimeter(TCHAR ch)
{
	return (ch == ':') ? true : false;
}

LPCTSTR CUtilities::PathAddSlashCString(std::string& path, bool useForwardSlash)
{
	UNREFERENCED_PARAMETER(useForwardSlash);
	if (!path.empty() && path.back() != '/')
		path += '/';
	return path.c_str();
}

LPCTSTR CUtilities::PathRemoveSlashCString(std::string& path)
{
	if (!path.empty() && path.back() == '/')
		path.pop_back();
	return path.c_str();
}

std::string CUtilities::PathAddSlash(LPCTSTR pPath, bool useForwardSlash)
{
	UNREFERENCED_PARAMETER(useForwardSlash);
	string path;
	
	if (pPath != nullptr && *pPath != 0)
	{
		path = string(pPath);
		if (!path.empty() && path.back() != '/')
			path += '/';
	}
	return path;
}

std::string CUtilities::PathRemoveSlash(LPCTSTR pPath)
{
	string path;

	if (pPath != nullptr && *pPath != 0)
	{
		path = string(pPath);
		if (!path.empty() && path.back() == '/')
			path.pop_back();
	}
	return path;
}

double CUtilities::WidthToHeightRatio(struct SIZE const &size)
{
	return size.cx / (double) size.cy;
}

double CUtilities::WidthToHeightRatio(struct SIZE const &size, DesktopOrientationType orientation)
{
	return CUtilities::Rotate(size.cx / (double) size.cy, orientation);
}

double CUtilities::WidthToHeightRatio(struct RECT const &rect)
{
	struct SIZE sz;
	sz.cx = rect.right - rect.left;
	sz.cy = rect.bottom - rect.top;
	return WidthToHeightRatio(sz);
}

// Rotate an aspect ratio.
double CUtilities::Rotate(double aspectRatio, DesktopOrientationType orientation)
{
	double newAspectRatio = aspectRatio;

	switch (orientation)
	{
	case DesktopOrientation90:
	case DesktopOrientation270:
		newAspectRatio = 1.0 / aspectRatio;
		break;
	case DesktopOrientationDefault:
	case DesktopOrientation180:
	default:
		break;
	}

	return newAspectRatio;
}

// Rotate a size.
struct SIZE CUtilities::Rotate(struct SIZE const &size, DesktopOrientationType orientation)
{
	struct SIZE newSize = size;

	switch ((int)orientation)
	{
	case DesktopOrientation90:
	case DesktopOrientation270:
		// Swap cx and cy.
		newSize.cx = size.cy;
		newSize.cy = size.cx;
		break;
	case DesktopOrientationDefault:
	case DesktopOrientation180:
	default:
		break;
	}

	return newSize;
}


// Rotate a rectangle in the opposite direction, for a given unrotated screen.
// For example, rectangle was calculated using a rotated screen; now unrotate the rectangle.
struct RECT CUtilities::RotateRectBack(struct RECT const &rect, struct SIZE const &screen, DesktopOrientationType orientation)
{
	struct RECT newRect = rect;

	switch (orientation)
	{
	case DesktopOrientation90:
		newRect.left = rect.top;
		newRect.top = screen.cy - rect.right;
		newRect.right = rect.bottom;
		newRect.bottom = screen.cy - rect.left;
		break;
	case DesktopOrientation270:
		newRect.left = screen.cx - rect.bottom;
		newRect.top = rect.left;
		newRect.right = screen.cx - rect.top;
		newRect.bottom = rect.right;
		break;
	case DesktopOrientation180:
		newRect.left = screen.cx - rect.right;
		newRect.top = screen.cy - rect.bottom;
		newRect.right = screen.cx - rect.left;
		newRect.bottom = screen.cy - rect.top;
		break;

	case DesktopOrientationDefault:
	default:
		break;
	}

	return newRect;
}

bool CUtilities::IsSmd(const std::string &filestr)
{
	string fileName = (filestr);

	string smd(SMD_EXTENSION);

	return CUtilities::HasFileExtension(fileName, smd);
}

//Returns true if filestr has an flv extension.
//Returns false otherwise.
bool CUtilities::IsFlv(const std::string &filestr)
{
	string fileName = (filestr);

	string flv(FLV_EXTENSION);

	return CUtilities::HasFileExtension(fileName, flv);
}

bool CUtilities::IsBmp(const std::string &filestr)
{
	string fileName = (filestr);

	string bmp(BMP_EXTENSION);

	return CUtilities::HasFileExtension(fileName, bmp);
}

short CUtilities::OrientationAngle(DesktopOrientationType orientation)
{
	short result = -1;

	switch ((int)orientation)
	{
		case DesktopOrientationDefault:
			result = 0;
			break;
		case DesktopOrientation90:
			result = 90;
			break;
		case DesktopOrientation180:
			result = 180;
			break;
		case DesktopOrientation270:
			result = 270;
			break;
	}

	return result;
}

DesktopOrientationType CUtilities::AngleOrientation(short angle)
{
	DesktopOrientationType orientation = DesktopOrientationNoChange;

	switch (angle)
	{
		case 0:
			orientation = DesktopOrientationDefault;
			break;
		case 90:
			orientation = DesktopOrientation90;
			break;
		case 180:
			orientation = DesktopOrientation180;
			break;
		case 270:
			orientation = DesktopOrientation270;
			break;
	}

	return orientation;
}

short CUtilities::RotateAngle(short start, short rotate)
{
	short result = start + rotate;
	if (360 <= result)
	{
		result -= 360;
	}
	else if (0 > result)
	{
		result += 360;
	}
	return result;
}

unsigned short CUtilities::CRC(unsigned char *s, int len, unsigned short crcval)
{
	// copied from CSASPollMessage class
	unsigned c,q;

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


void CUtilities::SetThreadName(std::thread::id dwThreadID, const std::string &threadName)
{
    UNREFERENCED_PARAMETER(&dwThreadID); 
    UNREFERENCED_PARAMETER(&threadName); 

    // There is currently not a way to name a thread without adding a new library.
    // We can keep this fucntion in case there becomes one.
}

std::thread::id CUtilities::GetCurrentThreadId()
{
	return this_thread::get_id();
}

void CUtilities::ShowThreadId(const std::string &threadName)
{
	ostringstream threadId;
	threadId << GetCurrentThreadId();

	// output threadid
	LogString(GENERAL, _T("Thread name = %s, id = %s"),
		static_cast<LPCTSTR>(threadName.c_str()),                                                                                          
		static_cast<LPCTSTR>(threadId.str().c_str()));
}


void CUtilities::FlushVolume()
{
    FlushFileSystemBuffers();
}

void CUtilities::FlushFileSystemBuffers()
{
	const string SystemSyncCommand = "sync";
	system(SystemSyncCommand.c_str());
}

/// <summary>
/// Builds a hex character string based on big endian byte array
/// </summary>
/// <param name="number">Data to be converted to string.</param>
/// <param name="numberLength">Length of data to be converted.</param>
/// <returns>Returns a pointer to a hexidecimal string representation.</returns>
char *CUtilities::BigEndianToHexCharArray( const byte *number, int numberLength )
{
	const byte SECONDCHAR = 0x0f;
	const byte FIRSTCHAR = 0xf0;
	const int NIBBLESIZE = 4;
	const int LASTDIGIT = 9;
	const byte NUMCHARS = 0x30;
	const byte ALPHACHARS = 0x37;

	char *str = NULL;
	if (number != NULL && numberLength > 0)
	{
		// Tracker 018406
		// convert 5 byte big-endian card number to hex string
		str = new char[numberLength*HEX_CHARS_PER_BYTE + 1];
		for (int i = 0; i < numberLength ;i++)
		{
			byte lNibble, hNibble;
			lNibble = number[i] & SECONDCHAR;
			hNibble = (number[i] & FIRSTCHAR) >> NIBBLESIZE;
			if (hNibble > LASTDIGIT)
				str[i * HEX_CHARS_PER_BYTE] = ALPHACHARS + hNibble;
			else
				str[i * HEX_CHARS_PER_BYTE] = NUMCHARS + hNibble;
			if (lNibble > LASTDIGIT)
				str[i * HEX_CHARS_PER_BYTE + 1] = ALPHACHARS + lNibble;
			else
				str[i * HEX_CHARS_PER_BYTE + 1] = NUMCHARS + lNibble;
		}
		str[numberLength * HEX_CHARS_PER_BYTE] = '\0';
	}

	return str;
}

std::string CUtilities::LoadResourceString(LPCTSTR id)
{
	return string(id);
}


std::string CUtilities::SetStringSliderValue( DWORD value )
{
	std::string retString = _T("");
	for (DWORD i = 0; i < value; i++)
	{
		retString += SPACE;
	}

	return retString;
}


string CUtilities::GetTimeStamp()
{
	char timeStamp[TIMESTAMP_SIZE];
    tm tmpTm(GetLocalTmFromTime(CUtilities::GetCurrentTime()));
	size_t time_size(strftime(timeStamp, TIMESTAMP_SIZE, "%m/%d/%Y %I:%M:%S %p", &tmpTm));
	return string(timeStamp, time_size);
}

bool CUtilities::IsDateValid(time_t date)
{
	bool retVal = true;
	if (date > GetTimeFromFields(2038, 1, 1, 0, 0, 0) || date < 0)
	{
		retVal = false;
	}
	return retVal;
}

std::string CUtilities::FormatDateTimeString(time_t date)
{
	return FormatDateTimeString(date, DATE_TIME_SECONDS_MASK);
}

std::string CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(time_t date)
{
	return FormatMilitaryDateTimeString(GetLocalTimeFromUTC(date));
}

std::string CUtilities::FormatMilitaryDateTimeString(time_t date)
{
	return FormatDateTimeString(date, MILITARY_DATE_TIME_MASK);
}

std::string CUtilities::FormatDateTimeString(time_t date, const std::string &mask)
{
	// Ensure date/time in range
	if (CUtilities::IsDateValid(date))
	{
		tm localTm(GetLocalTmFromTime(date));

        char mbstr[128];
        if (std::strftime(mbstr, sizeof(mbstr), mask.c_str(), &localTm)) {
            std::string timeStr(mbstr);
//printf("Date = [%s]\n", timeStr.c_str());
            return timeStr;
        }
	}

	return FormatString("%lld", (long long)date);
}

std::string CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(const SYSTEMTIME& utcTimestamp)
{
	std::string timestampString;

	if ((utcTimestamp.wYear == 0 && utcTimestamp.wMonth == 0) ||
		Is1970Time(utcTimestamp))
	{
		timestampString = LoadResourceString(IDS_LAST_TIMESTAMP_DEFAULT);
	}
	else
	{
		time_t date = CUtilities::GetTimeFromUTCSystemTime(utcTimestamp);
		timestampString = CUtilities::FormatMilitaryDateTimeString(date);
	}

	return timestampString;
}

std::string CUtilities::FormatTimeSpanString(time_t timeSpan)
{
	return FormatString(_T("%lld %02lld:%02lld:%lld"),
		(long long)(timeSpan / SECONDS_PER_DAY),
		(long long)((timeSpan / SECONDS_PER_HOUR) % HOURS_PER_DAY),
		(long long)((timeSpan / SECONDS_PER_MINUTE) % MINUTES_PER_HOUR),
		(long long)(timeSpan % SECONDS_PER_MINUTE));
}


long CUtilities::DNSLookup(LPCTSTR name, CIPAddressArray& ipArray)
{
	long retStatus = 0;
    struct addrinfo hints, *res, *p;
	long iPaddress;
	int status;

	ipArray.RemoveAll();

	memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    #ifdef LEAK_CHECK
    // The getaddrinfo function allocates memory internally that it does not release.
    // This is in addition to the documented "res" argument, which we do explicitly free below.
    // Decrementing track_heap_allocations-- hides these allocations from the leak report.
    //track_heap_allocations--; // disable heap tracking temporarily
    LockHeapAndDisableTracking();
    #endif

	if ((status = getaddrinfo(name, NULL, &hints, &res)) != 0) 
	{
        #ifdef LEAK_CHECK
        //track_heap_allocations++; // re-enable heap tracking
        UnlockHeapAndReenableTracking();
        #endif
		return errno;
    }

    #ifdef LEAK_CHECK
    //track_heap_allocations++; // re-enable heap tracking
    UnlockHeapAndReenableTracking();
    #endif

	for(p = res; p != NULL; p = p->ai_next) 
	{
		if (p == NULL) break;

		// ignore non-IPv4 addresses
		if (p->ai_family != AF_INET) continue;

		// get the pointer to the address itself,
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
		iPaddress = (long)ipv4->sin_addr.s_addr;

		BYTE address[IP_ADDRESS_SIZE];
		address[0] = (iPaddress & 0xff);
		address[1] = ((iPaddress >> 8) & 0xff);
		address[2] = ((iPaddress >> 16) & 0xff);
		address[3] = ((iPaddress >> 24) & 0xff);
		ipArray.Add(address);
    }

	if (res != NULL) freeaddrinfo(res);

	return retStatus;
}


SYSTEMTIME CUtilities::GetSystemTimeFromTm(const tm &sysTm)
{
	SYSTEMTIME sysTime = {};
	sysTime.wYear = sysTm.tm_year + EPOCH_YEAR;
	sysTime.wMonth = sysTm.tm_mon + 1;
	sysTime.wDayOfWeek = sysTm.tm_wday;
	sysTime.wDay = sysTm.tm_mday;
	sysTime.wHour = sysTm.tm_hour;
	sysTime.wMinute = sysTm.tm_min;
	sysTime.wSecond = min(59, sysTm.tm_sec);
	sysTime.wMilliseconds = 0;

	return sysTime;
}

void CUtilities::GetUTCSystemTime(LPSYSTEMTIME lpSystemTime)
{
	*lpSystemTime = GetSystemTimeFromTm(GetUTCSystemTm());
}

tm CUtilities::GetUTCSystemTm()
{
	time_t utcTime(time(nullptr));
	tm retVal;
	gmtime_r(&utcTime, &retVal);
	return retVal;
}

void CUtilities::GetLocalSystemTime(LPSYSTEMTIME lpSystemTime)
{
	*lpSystemTime = GetSystemTimeFromTm(GetLocalSystemTm());
}

tm CUtilities::GetLocalSystemTm()
{
	time_t localTime(GetCurrentTime());
	localTime += m_tzOffset;
	tm retVal;
	localtime_r(&localTime, &retVal);
	return retVal;
}

SYSTEMTIME CUtilities::Get1970Time()
{
	SYSTEMTIME JAN1970_SYSTIME = { 1970, 1, 4, 1, 0, 0, 0, 0 };

	return JAN1970_SYSTIME;
}

bool CUtilities::Is1970Time(const SYSTEMTIME& systemTime)
{
	return systemTime.wYear == JAN1970_SYSTIME.wYear &&
		systemTime.wMonth == JAN1970_SYSTIME.wMonth &&
		systemTime.wDay == JAN1970_SYSTIME.wDay &&
		systemTime.wHour == JAN1970_SYSTIME.wHour &&
		systemTime.wMinute == JAN1970_SYSTIME.wMinute &&
		systemTime.wSecond == JAN1970_SYSTIME.wSecond &&
		systemTime.wMilliseconds == JAN1970_SYSTIME.wMilliseconds;
}

__int64 CUtilities::GetMillisecondsSinceTimestamp(const SYSTEMTIME& timestamp)
{
	__int64 _milliseconds;

	SYSTEMTIME currentTimestamp;
	CUtilities::GetUTCSystemTime(&currentTimestamp);

    _milliseconds = GetSystemTimespanInMilliseconds(timestamp,currentTimestamp);
	return _milliseconds;
	}

__int64 CUtilities::GetSystemTimespanInMilliseconds(const SYSTEMTIME& systemTime1, const SYSTEMTIME& systemTime2)
	{
	time_t uLargeTime1 = SysTimeToUlarge(systemTime1);
	time_t uLargeTime2 = SysTimeToUlarge(systemTime2);

	__int64 i64long1 = ((__int64)uLargeTime1 * 1000) + systemTime1.wMilliseconds;
	__int64 i64long2 = ((__int64)uLargeTime2 * 1000) + systemTime2.wMilliseconds;
	__int64 i64diff = i64long2 - i64long1;
	
	return i64diff;
}

std::chrono::duration<int> CUtilities::GetTimeSpanSeconds(time_t begin, time_t end)
{
	chrono::system_clock::time_point btp = chrono::system_clock::from_time_t(begin);
	chrono::system_clock::time_point etp = chrono::system_clock::from_time_t(end);
	chrono::duration<int> span = chrono::duration_cast<chrono::seconds>(etp - btp);

	return span;
}


__int64 CUtilities::MillisecondsSince1970()
{
	time_t rawtime;
	SYSTEMTIME currentTime;
	time(&rawtime);
	currentTime = UlargeToSysTime(rawtime);

	return ConvertToMillisecondsSince1970(currentTime);
}

__int64 CUtilities::ConvertToMillisecondsSince1970(const SYSTEMTIME& systemTime)
{
	return (GetSystemTimespanInMilliseconds(JAN1970_SYSTIME, systemTime));
}



bool CUtilities::GetInt64FromString(LPCTSTR stringValue, __int64 &variable)
{
	bool ok = false;
	bool process = true;
	__int64 numericValue = 0;
	__int64 retvalue = variable;
	char _input[128];
	char _buffer[128];
	memset(_input, 0, sizeof(_input));
	memset(_buffer, 0, sizeof(_buffer));

	// make sure that the string is trimmed
	string _trim = string(stringValue);
	string _stemp = trimLeft(_trim);
	strcpy(_input, _stemp.c_str());

	for (int i = 0, j = 0; i < (int)strlen(_input); i++) 
	{
		if ((_input[i] == 0) || (_input[i] == TCHAR_COMMA)) continue;
		if ((isdigit(_input[i])) || (_input[i] == '-') ||
			(_input[i] == '+')) 	
		{
			if (_input[i] == '+') continue;
			_buffer[j++] = _input[i];
		} 
		else
		{
			process = false;
			break;
		}
	}
	string _sBuffer = string(_buffer);
	numericValue = 0;
	retvalue = variable;
	if (process) 
	{
		try
		{
			if (strlen(_buffer) > 0)
			{
				numericValue = std::stoll(_sBuffer);
				retvalue = numericValue;
				ok = true;
			}
		}
		catch (...) 
		{
			ok = false;
		}
	}
	variable = retvalue;
	return ok;
}

bool CUtilities::GetInt64FromString(const std::string &stringValue, __int64 &variable)
{
	bool ok = GetInt64FromString(stringValue.c_str(), variable);
	return ok;
}

bool CUtilities::GetInt64FromCurrencyString(LPCTSTR currency, __int64& value)
{
	bool ok(false);
	bool valid(false);
	__int64 dollars(0);
	__int64 initial(value);

	std::string currencyString = currency;
	currencyString = trimLeft(currencyString);

	// Verify that all digits are valid
	for (int index = 0; index < (int)currencyString.size(); index++) 
	{
		if (((isdigit(currencyString[index])) ||  
			(currencyString[index] == TCHAR_COMMA) || 
			(currencyString[index] == TCHAR_DECIMAL_POINT) || 
			(currencyString[index] == TCHAR_NEGATIVE_SIGN && index == 0) || 
			(currencyString[index] == TCHAR_PLUS_SIGN && index == 0)))
		{
			valid = true;
			ok = true;
		} else {
			valid = false;
			ok = false;
			break;
		}
	}

	int decimalPoint = currencyString.find(TCHAR_DECIMAL_POINT);

	if (valid) 
	{
		//
		// First convert the string to pennies format.
		if (decimalPoint == -1)
		{
			// First, check if the existing string is valid.
			ok = GetInt64FromString(currencyString, dollars);
			if (ok)
			{
				// Append '0' characters to the string to convert the string to pennies format.
				for (int index = 0; index < CURRENCY_DIGITS; ++index)
				{
					currencyString.append(1, TCHAR_ZERO);
				}
			}
		} else if ((int)currencyString.size() == (decimalPoint - 1)) {
			currencyString.erase(decimalPoint, 1);
			while ((int)currencyString.size() < (decimalPoint + 1 + CURRENCY_DIGITS))
			{
				currencyString.append(1, TCHAR_ZERO);
			}
		} else {
			std::string dollarsString = currencyString.substr(0, decimalPoint);
			ok = GetInt64FromString(dollarsString, dollars);
			if (ok)
			{
				// Truncate any excess digits.
				currencyString = currencyString.substr(0, decimalPoint + 1 + CURRENCY_DIGITS);

				// Append '0' characters to the string to get the correct number of currency digits.
				int stringLength = currencyString.size();
				for (int index = decimalPoint + 1; 
					 index < decimalPoint + 1 + CURRENCY_DIGITS; 
					 index++)
				{
					if (index < stringLength) 
					{
						continue;
					} else {
						currencyString.append(1, TCHAR_ZERO);
					}
				}
			}
		}
	}

	if (ok)
	{
		// Remove the decimal point.
		if (decimalPoint != -1) 
			currencyString.erase(decimalPoint, 1);
		ok = GetInt64FromString(currencyString, value);
	}
	if (!ok) 
	{
		value = initial;
	}

	return ok;
}

__int64 CUtilities::GetInt64FromCurrencyString(LPCTSTR currency)
{
	__int64 value(0);
	GetInt64FromCurrencyString(currency, value);
	return value;
}

__int64 CUtilities::GetInt64FromCurrencyString(const std::string& currency)
{
	__int64 value(GetInt64FromCurrencyString(currency.c_str()));
	return value;
}


std::string CUtilities::GetStdStringFromString(const std::string &value)
{
	return value;
}

std::string CUtilities::GetStdStringFromLPCTSTR(LPCTSTR value)
{
	// Convert a TCHAR string to a LPCSTR
	//CT2CA pszConvertedAnsiString(value);

	// construct a std::string using the LPCSTR input
	std::string strStd(value);

	return strStd;
}

std::string CUtilities::GetStdStringFromSystemTime(const SYSTEMTIME &mtime)
{
	char _buffer[256];
	string retstr;

	memset(_buffer, 0, sizeof(_buffer));
	sprintf(_buffer, "%02d/%02d/%02d %02d:%02d:%02d.%03d %s", 
			mtime.wMonth,
			mtime.wDay,
			mtime.wYear,
			(mtime.wHour > 12) ? (mtime.wHour - 12) : mtime.wHour,
			mtime.wMinute,
			mtime.wSecond,
			mtime.wMilliseconds,
			(mtime.wHour > 12) ? "PM" : "AM"
			);
	retstr = string(_buffer);
	return retstr;
}

std::string CUtilities::GetPrefixIdString(LPCTSTR prefix, LONG64 id, int maxLength)
{
	// Fix the maximum formatted length for large values to retain the sign and the least-significant digits,
	// as the least significant digits are expected to change between id changes.
	std::string transactionId(prefix);
	if (id < 0)
	{
		transactionId += TCHAR_NEGATIVE_SIGN;
	}

	unsigned long long unsignedId(abs(id));
	std::stringstream unsignedIdString;
	unsignedIdString << unsignedId;
	std::string unsignedIdstr = unsignedIdString.str();
	int combinedLength(transactionId.length() + unsignedIdstr.length());
	if (combinedLength > maxLength)
	{
		unsignedIdstr.erase(0, combinedLength - maxLength);
	}
	transactionId += unsignedIdstr;

	return transactionId;
}


int64_t CUtilities::ConvertStringToLongLong(std::string number)
{
	int64_t longLongNumber(-1);

	if (!number.empty())
	{
		try
		{
			longLongNumber = std::stoll(number, nullptr, 0);
		}
		catch (const std::out_of_range& oor)
		{
			LogString(ERROR_LOG, "ConvertStringToLongLong - Out of Range error, %s", oor.what());
		}
	}

	return longLongNumber;
}

string CUtilities::ConvertStringToDecimalIfNecessary(const string& str, int precision)
{
	string decimalStr(str);
	
	if (std::regex_match(str, DECIMAL_PATTERN))
	{
		std::stringstream ss(str);
		double decimal;
		ss >> decimal;
		if (!ss.fail()) 
		{
			ss.str("");
			ss.clear();
			ss << std::fixed << std::setprecision(precision) << decimal;
			decimalStr = ss.str();
		}
	}

	return decimalStr;
}

std::string &CUtilities::TrimLeft(std::string &str) {
	str.erase(str.begin(), std::find_if(str.begin(), str.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
	return str;
}

std::string &CUtilities::TrimRight(std::string &str)
{
	str.erase(std::find_if(str.rbegin(), str.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());
	return str;
}

std::string &CUtilities::Trim(std::string &str)
{
	return TrimLeft(TrimRight(str));
}

std::string CUtilities::trimLeft(std::string& str)
{
    size_t first = str.find_first_not_of(" \t\f\n\r\v");
	if (first == string::npos)
	{
		return "";
	}
	string retval = str.substr(first);
	return retval;
}
 
// Real-time and relative time:
// - Use DWORD and CUtilities::GetCurrentTicks() for most real-time and relative time needs.
// Wall time -- AKA date/time.
// Calendar time types are generally structures which are more difficult to perform calculations/comparisons on.
// Non-calendar time types are generally single arithmetic values which are easier to perform calculations/comparisons on.
//
// Note: The underlying implementation of time_t for this environment is a 64-bit signed integer value.
// This allows custom date/time routines to simplify operations.
//
// Types used:
// time_t -- Time in seconds.
// SYSTEMTIME -- Microsoft Windows-compatible calendar time. Used for storage and communications.
// tm -- Calendar time. Used primarily as temporary values during conversions.
// 
// Types not used:
// CTime -- Microsoft MFC/AFX time in seconds. Instead, use time_t.
// FILETIME -- Microsoft Windows time in tenths of microseconds -- Instead, use xxx.//TODO-PORT
//
// Standard date/time functions used.
// time() -- Get the current UTC time in seconds as time_t.
// gmtime() -- Convert a time in seconds (time to a calendar time -- the result has the same time zone as the source.
// timegm() -- Convert a calendar time (tm) to a time in seconds (time_t) -- the result has the same time zone as the source.
// - Note that timegm() is not part of Standard C/C++, but is readily available.
//
// Standard date/time conversion functions not used.
// difftime() -- Simply subtract the time_t values, giving a time_t result in seconds.
// mktime() -- mktime() uses the time zone of the operating environment. Instead, use xxx() which uses the time zone of our choice.
// localtime() -- localtime() uses the time zone of the operating environment. Instead, use xxx() which uses the time zone of our choice.
//
// Custom date/time functions used:
//
// Custom date/time functions not used.
// CTime CUtilities::GetCTimeFromUTCSystemTime(const SYSTEMTIME &) -- Use CUtilities::GetTimeFromUTCSystemTime().

time_t CUtilities::GetTimeFromLocalTm(const tm &localTm)
{
	time_t localTime = internal_timegm(&localTm);  
	time_t utcTime = GetUTCFromLocalTime(localTime);
	return utcTime;
}

time_t CUtilities::GetTimeFromFields(int years, int months, int days, int hours, int minutes, int seconds)
{
	tm localTm = GetTmFromFields(years, months, days, hours, minutes, seconds);

	return GetTimeFromLocalTm(localTm);
}

tm CUtilities::GetLocalTmFromTime(time_t timeValue)
{
	time_t localTime = GetLocalTimeFromUTC(timeValue);
	tm localTm;

    #ifdef LEAK_CHECK
    // The gmtime_r function allocates memory internally that it does not release.
    // Decrementing track_heap_allocations-- hides these allocations from the leak report.
    //track_heap_allocations--; // disable heap tracking temporarily
    LockHeapAndDisableTracking();
    #endif

	gmtime_r(&localTime, &localTm);

    #ifdef LEAK_CHECK
    //track_heap_allocations++; // re-enable heap tracking
    UnlockHeapAndReenableTracking();
    #endif

	return localTm;
}

tm CUtilities::GetTmFromSystemTime(const SYSTEMTIME &sysTime)
{
	tm theTime(GetTmFromFields(sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond));
	return theTime;
}

tm CUtilities::GetTmFromFields(int years, int months, int days, int hours, int minutes, int seconds)
{
	tm theTime = {};
	theTime.tm_year = years - EPOCH_YEAR;
	theTime.tm_mon = months - EPOCH_MONTH;
	theTime.tm_mday = days;
	theTime.tm_hour = hours;
	theTime.tm_min = minutes;
	theTime.tm_sec = seconds;
	theTime.tm_isdst = 0;

	return theTime;
}

void CUtilities::GetFieldsFromTime(time_t timeValue, int &years, int &months, int &days, int &hours, int &minutes, int &seconds)
{
	time_t localTime(GetLocalTimeFromUTC(timeValue));
	tm localTm;
	gmtime_r(&localTime, &localTm);
	GetFieldsFromTm(localTm, years, months, days, hours, minutes, seconds);
}

void CUtilities::GetFieldsFromTm(const tm &tmValue, int &years, int &months, int &days, int &hours, int &minutes, int &seconds)
{
	years = tmValue.tm_year + EPOCH_YEAR;
	months = tmValue.tm_mon + EPOCH_MONTH;
	days = tmValue.tm_mday;
	hours = tmValue.tm_hour;
	minutes = tmValue.tm_min;
	seconds = tmValue.tm_sec;
}

std::string FormatStringV(LPCSTR pFormat, va_list args)
{
	// Determine the number of characters needed to format the arguments.
	string result = "";
	va_list args_copy;
	va_copy(args_copy, args);
	int charsNeeded = vsnprintf(nullptr, 0, pFormat, args_copy);
	va_end(args_copy);

	if (charsNeeded > 0)
	{
		// Format the arguments.
		int charsAllocated = charsNeeded + 1; // Allocate an extra character for the trailing nul character.
		std::unique_ptr<char[]> formatted(new char[charsAllocated]);
		vsnprintf(&formatted[0], charsAllocated, pFormat, args);
		formatted[charsAllocated - 1] = 0; // Ensure that a terminating nul character is present.
		result = string(formatted.get());
	}

	return result;
}

std::string FormatString(LPCSTR pFormat, ...)
{
	va_list args;
	va_start(args, pFormat);
    std::string result = FormatStringV(pFormat, args);
	va_end(args);
    return result;
}

bool CUtilities::StringCompareIgnoreCase(std::string const& str1, std::string const& str2)
{
    if (str1.length()== str2.length()) 
    {
        return std::equal(str2.begin(), str2.end(), str1.begin(), StringComparePred);
    }
    else 
    {
        return false;
    }
}

bool CUtilities::StringComparePred(unsigned char char1, unsigned char char2)
{
    return std::tolower(char1) == std::tolower(char2);
}

void CUtilities::GetPathMemoryUsageInformation(const string& fileSytemPath, string& size, string& used, string& available)
{
	std::string memoryUsageInfo(GetSystemResultString("df -h " + fileSytemPath + " | awk 'FNR>1{print $2,$3,$4}'"));
	if (!memoryUsageInfo.empty())
	{
		istringstream iss(memoryUsageInfo);
		vector<string> values{ istream_iterator<string>{iss}, istream_iterator<string>{} };
		constexpr std::size_t NUMBER_OF_DRIVE_INFO_VALUES(3);
		if (NUMBER_OF_DRIVE_INFO_VALUES == values.size())
		{
			size = values[0];
			used = values[1];
			available = values[2];
		}		
	}
}


std::string CUtilities::GetSystemResultString(const std::string& systemCall)
{
	lock_guard<recursive_mutex> lock(m_systemCallMutex);

	std::string resultStr;

#ifndef _UNIT_TEST
	try
	{
		array<char, MAX_LINE_SZ> buffer;
		std::unique_ptr<FILE, decltype(&pclose)> pipeStream(popen(systemCall.c_str(), "r"), pclose);
		if (pipeStream)
		{
			while (fgets(buffer.data(), buffer.size(), pipeStream.get()) != nullptr)
			{
				if (buffer.size() > 0)
				{
					resultStr += buffer.data();
				}
			}
			if (string::npos != resultStr.find("\n") || string::npos != resultStr.find("\r"))
			{
				resultStr.pop_back();
			}
		}
		else
		{
			LogString(ERROR_LOG, "GetSystemResultString failed - %s", systemCall.c_str());
		}
	}
	catch (...)
	{
		LogString(ERROR_LOG, "GetSystemResultString exception - %s", systemCall.c_str());
	}
#endif

	return resultStr;
}

bool CUtilities::CopyFile(const std::string &source, const std::string &target)
{
    bool copyResult = true;

#ifndef _UNIT_TEST
    string fileCopySystemCommand = FormatString("cp %s %s", source.c_str(), target.c_str());

    copyResult = (system(fileCopySystemCommand.c_str()) == 0);
#endif

    return copyResult;
}

std::string CUtilities::UrlEncode(const std::string& s)
{
	string retVal=s;

#ifndef _UNIT_TEST
	CURL *curl_handle = curl_easy_init();
	if (curl_handle) 
	{
		char *output = curl_easy_escape(curl_handle, s.c_str(), s.length());
		if (output) 
		{
			retVal = string(output);
			curl_free(output);
		}
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);
#endif

	return retVal;
}

void CUtilities::FindAndReplace(string& subject, const string& search, const string& replace)
{
	size_t pos(0);
	while ((pos = subject.find(search, pos)) != string::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

void CUtilities::EraseAllSubStr(string & subject, const string & toErase)
{
	size_t pos = string::npos;

	// Search for the substring in string in a loop untill nothing is found
	while ((pos = subject.find(toErase)) != string::npos)
	{
		// If found then erase it from string
		subject.erase(pos, toErase.length());
	}
}

void CUtilities::EraseSubStrings(string& subject, const std::vector<string>& strList)
{
	// Iterate over the given list of substrings. For each substring call EraseAllSubStr() to remove its all occurrences from subject string.
	for (auto str : strList)
	{
		EraseAllSubStr(subject, str);
	}
}

DWORD CUtilities::EncryptPBTPin(DWORD unencryptedPIN)
{
	DWORD encryptedPIN = 0;

	//positions are read from left	(1234)	
	encryptedPIN += (unencryptedPIN / 1000) % 10;	//move pos 1 to pos 4
	encryptedPIN += (unencryptedPIN % 10) * 10;		//move pos 4 to pos 3
	encryptedPIN += ((unencryptedPIN / 100) % 10) * 100; //move pos 2 to pos 2
	encryptedPIN += ((unencryptedPIN / 10) % 10) * 1000; //move pos 3 to pos 1

	encryptedPIN += MAGIC_ENCRYPTION_NUMBER;

	return encryptedPIN;

}

DWORD CUtilities::UnencryptPBTPin(DWORD encryptedPIN)
{
	DWORD unencryptedPIN = 0;

	encryptedPIN -= MAGIC_ENCRYPTION_NUMBER;

	//positions are read from left	(1234)	
	unencryptedPIN += (encryptedPIN % 10) * 1000;	//move pos 4 to pos 1
	unencryptedPIN += (encryptedPIN / 10) % 10;		//move pos 3 to pos 4
	unencryptedPIN += ((encryptedPIN / 100) % 10) * 100; //move pos 2 to pos 2
	unencryptedPIN += ((encryptedPIN / 1000) % 10) * 10; //move pos 1 to pos 3


	return unencryptedPIN;

}

void CUtilities::FixDirDelimiter(std::string &path)
{
	for_each(path.begin(), path.end(), [](char &c)
				{
		if (c == '\\')
		{
			c = '/';
		}
	});
}

void PrintTm(struct tm *one, struct tm *two)
{
	if (two == NULL) {
		printf("%d\n", one->tm_year);
		printf("%d\n", one->tm_mon);
		printf("%d\n", one->tm_mday);
		printf("%d\n", one->tm_hour);
		printf("%d\n", one->tm_min);
		printf("%d\n", one->tm_sec);
	} else {
		printf("ONE\t\tTWO\n");
		printf("%d\t=\t%d\n", one->tm_year, two->tm_year);
		printf("%d\t=\t%d\n", one->tm_mon,  two->tm_mon);
		printf("%d\t=\t%d\n", one->tm_mday, two->tm_mday);
		printf("%d\t=\t%d\n", one->tm_hour, two->tm_hour);
		printf("%d\t=\t%d\n", one->tm_min,  two->tm_min);
		printf("%d\t=\t%d\n", one->tm_sec,  two->tm_sec);
	}
}

bool CUtilities::ConvertHexStringToBytes(std::string hexString, byte *bytes, int byteLength)
{
	bool retVal = true;

	if (bytes != nullptr)
	{
		for (int byteLoc = 0; byteLoc < byteLength && byteLoc < hexString.length(); byteLoc++)
		{
			bytes[byteLoc] = ConvertCharToByte(hexString[byteLoc * 2]) * 16 + ConvertCharToByte(hexString[byteLoc * 2 + 1]);
		}

	}

	return retVal;
}

uint8_t CUtilities::ConvertCharToByte(char input)
{
	uint8_t val = 0;
	if (input >= '0' && input <= '9')
	{
		val = input - '0';
	}
	else if (input >= 'A' && input <= 'F')
	{
		val = input - 'A' + 10;
	}
	else if (input >= 'a' && input <= 'f')
	{
		val = input - 'a' + 10;
	}

	return val;
}

std::string CUtilities::GetGUID()
{
	// example format: "a530e37c-aaa0-408d-9c48-3b08c41d58da"
	char strUuid[128] = { 0 };
	sprintf(strUuid, "%08x-%04x-%04x-%04x-%06x%06x",
		rand() & 0xffffffff,
		rand() & 0xffff,
		rand() & 0xffff,
		rand() & 0xffff,
		rand() & 0xffffff,
		rand() & 0xffffff
	);

	return string(strUuid);
}

void CUtilities::SetupDatabaseCertsForChrome()
{
	string homeFolder("/home/user");
	struct stat statHomerUser;
	bool isHomeUser(!stat(homeFolder.c_str(), &statHomerUser));
	if (isHomeUser)
	{
		CERTIFICATE_STORE_DATABASE_PATH = homeFolder;
		CERTIFICATE_STORE_DATABASE_PATH.append("/.pki");
		CUtilities::GetSystemResultString(FormatString("rm %s/%s -r", homeFolder.c_str(), ".cache"));
		CUtilities::GetSystemResultString(FormatString("rm %s/%s -r", homeFolder.c_str(), ".config/chromium"));
		CUtilities::GetSystemResultString(FormatString("rm %s/%s -r", homeFolder.c_str(), ".config/pulse"));
		CUtilities::GetSystemResultString(FormatString("rm %s/%s -r", homeFolder.c_str(), ".pki"));
	}
	else
	{
		homeFolder = "home/root";
		CERTIFICATE_STORE_DATABASE_PATH = homeFolder;
		CERTIFICATE_STORE_DATABASE_PATH.append("/.pki");
		CUtilities::GetSystemResultString(FormatString("rm %s/%s -r", homeFolder.c_str(), ".chromium"));
		CUtilities::GetSystemResultString(FormatString("rm %s/%s -r", homeFolder.c_str(), ".pki"));
	}

	//Make the certificate store database path 
	NSSDB_PATH = CERTIFICATE_STORE_DATABASE_PATH + "/nssdb";
	string makeCertStoreDatabasePath(FormatString("mkdir -p %s", NSSDB_PATH.c_str()));
	cout << "makeCertStoreDatabasePath=" << makeCertStoreDatabasePath << endl;
	CUtilities::GetSystemResultString(makeCertStoreDatabasePath);

	//Creates an empty database store
	string createEmptyDatabaseStore(FormatString("certutil -d sql:%s -N --empty-password", NSSDB_PATH.c_str()));
	cout << "createEmptyDatabaseStore=" << createEmptyDatabaseStore << endl;
	CUtilities::GetSystemResultString(createEmptyDatabaseStore);

	if (isHomeUser)
	{
		//Gives access to chrome
		CUtilities::GetSystemResultString(FormatString("chown -R user:user %s", homeFolder.c_str()));
		CUtilities::GetSystemResultString(FormatString("chown -R user:user %s", CERTIFICATE_STORE_DATABASE_PATH.c_str()));
	}
}

void CUtilities::AddCertsToCertificateDatabaseStore(const string& certsDirectory, vector<string>& listOfCerts)
{
	const string ACTIVE_TAG("Not Before: ");
	const string EXPIRATION_TAG("Not After : ");

	std::unique_ptr<DIR, closedir_deleter> dir(opendir(certsDirectory.c_str()));
	if (dir)
	{
		struct dirent *p;
		while ((p = readdir(dir.get())))
		{
			string fileName(p->d_name);
			/* Skip . and .. */
			if ("." != fileName && ".." != fileName)
			{
				string certPath(certsDirectory + "/" + fileName);
				struct stat statbuf;
				if (!stat(certPath.c_str(), &statbuf) && !S_ISDIR(statbuf.st_mode))
				{
					string addCertToDatabaseStore(FormatString("certutil -d sql:%s -A -n '%s' -i %s -t TCP,TCP,TCP", NSSDB_PATH.c_str(), fileName.c_str(), certPath.c_str()));
					string resultStr(CUtilities::GetSystemResultString(addCertToDatabaseStore));
					if (resultStr.empty())
					{
						string getCertInfo(FormatString("certutil -L -d sql:%s -n %s", NSSDB_PATH.c_str(), fileName.c_str()));
						std::string line;
						string certInfo(CUtilities::GetSystemResultString(getCertInfo));
						istringstream stream(certInfo);
						string certInfoStr(fileName);
						std::size_t found;
						while (std::getline(stream, line))
						{
							found = line.find(ACTIVE_TAG);
							if (found != std::string::npos)
							{
								line.erase(0, found + EXPIRATION_TAG.length());
								certInfoStr.append(" Active Date=" + line);
								continue;
							}

							found = line.find(EXPIRATION_TAG);
							if (found != std::string::npos)
							{
								line.erase(0, found + EXPIRATION_TAG.length());
								certInfoStr.append(" Expiration Date=" + line);
								listOfCerts.push_back(certInfoStr);
								break;
							}
						}
					}
				}
			}
		}
	}
	string copyDatabaseCertsToRoot(FormatString("cp %s / -r", CERTIFICATE_STORE_DATABASE_PATH.c_str()));
	CUtilities::GetSystemResultString(copyDatabaseCertsToRoot);
}

bool CUtilities::CheckIfNvramNeedsToBeCleared()
{
	return FileExists(CDirectoryManager::CLEAR_MEMORY);
}

bool CUtilities::IsBannedPin(const string& pin)
{
	const string AUTO_UPLOAD_LOAD_PIN("7295");
	return AUTO_UPLOAD_LOAD_PIN == pin;
}

bool CUtilities::IsAutomationMode()
{
	return FileExists(CDirectoryManager::AUTOMATION_MODE);
}

void CUtilities::GetResolution(DWORD &width, DWORD &height)
{
	// Need to handle MW resolution later.
	width = LCD_RESOLUTION_WIDTH;
	height = LCD_RESOLUTION_HEIGHT;
}