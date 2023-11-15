#pragma once

#include "GlobalDefs.h"
#include "IPAddress.h"
//#include "IPAddressArray.h"

static const DWORD NVRAM_TIME_SIZE = sizeof(int) * 6;
static const DWORD NVRAM_PLAYER_TIME_SIZE = sizeof(DWORD);

const WORD SYSTEMTIME_DELTA_TO_MILLISEC = 10000;
const DWORD SYSTEMTIME_DELTA_TO_SEC = SYSTEMTIME_DELTA_TO_MILLISEC * 1000;
const DWORD SYSTEMTIME_DELTA_TO_MIN = SYSTEMTIME_DELTA_TO_SEC * 60;
const ULONGLONG SYSTEMTIME_DELTA_TO_HOUR = (ULONGLONG)SYSTEMTIME_DELTA_TO_MIN * 60;
const SYSTEMTIME JAN1970_SYSTIME = { 1970, 1, 4, 1, 0, 0, 0, 0 };
const WORD MINCTIMEYEAR = 1970;
const WORD MAXCTIMEYEAR = 3000;

const BYTE CR = 0x0d;
const BYTE NUL = 0x00;
const __int64 INVALID_VALUE = -7766279631452241719;

const DWORD BAD_THREAD_COUNT = (DWORD)-1;

class CIPAddressArray;

class CUtilities
{

public:
//// #pragma region public constants
	static const int HEX_CHARS_PER_BYTE = 2; // For BigEndianToHexCharArray().
	static const time_t EpochTime = 0;
	static const int EPOCH_YEAR = 1900; // For struct tm.
	static const int EPOCH_MONTH = 1; // For struct tm.

	static const std::string EmptyString;
	static std::string m_exeVersion;

	static std::string CERTIFICATE_STORE_DATABASE_PATH;
	static std::string NSSDB_PATH;
//// #pragma endregion public constants

	/// <summary>
	/// Converts inString to a NUL-terminated char buffer, truncating each inString character to 1 byte.
	/// The remainder of outChar is zero filled.
	/// </summary>
	/// <param name="inString">IN - The in string.</param>
	/// <param name="outChar">OUT - The output buffer.</param>
	/// <param name="length">IN - The length of the outChar buffer.</param>
	/// <remarks>
	/// If length >  0 then outChar will have a NUL termination character, even if there is not room for all the characters from inString.
	/// If length i== 0 then nothing is done.
	/// </remarks>
	static void ConvertCharsToChars(LPCTSTR inString, char *outChar, int length);

	/// <summary>
	/// Converts a number string to int64.
	/// </summary>
	/// <param name="inString">the string number to convert.</param>
	/// <returns>__int64 representation of the string</returns>
	static __int64 ConvertStringToInt64(const std::string& inString);

	/// <summary>
	/// Converts inString to a NUL-terminated char buffer, truncating each inString character to 1 byte.
	/// The remainder of outChar is zero filled.
	/// </summary>
	/// <param name="inString">IN - The in string.</param>
	/// <param name="outChar">OUT - The output buffer.</param>
	/// <param name="length">IN - The length of the outChar buffer.</param>
	/// <remarks>
	/// If length >  0 then outChar will have a NUL termination character, even if there is not room for all the characters from inString.
	/// If length i== 0 then nothing is done.
	/// </remarks>
	static void ConvertCharsToChars(const std::string &inString, char *outChar, int length);

	/// <summary>
	/// Converts the wide chars to UTF8 string.
	/// </summary>
	/// <param name="inString">The in string.</param>
	/// <returns></returns>
	static std::string ConvertWideCharsToUtf8String(LPCWSTR inString);

	/// <summary>
	/// Compare two integer values.
	/// <remarks> Handles negative input values correctly -- i.e., wraparound on subtraction is avoided/handled correctly.</remarks>
	/// </summary>
	/// <param name="value1">First value to be compared with</param>
	/// <param name="value2">Second value to be compared with</param>
	/// <returns>
	/// Returns < 0 if first value is less than second value.
	/// Returns > 0 if first value is greater than second value.
	/// Otherwise, returns 0.
	/// </returns>
	static int CompareInts(int value1, int value2);

	/// <summary>
	/// Compare two versions.
	/// <remarks> Handles negative input values correctly -- i.e., wraparound on subtraction is avoided/handled correctly.</remarks>
	/// </summary>
	/// <param name="major1">major part of the first version</param>
	/// <param name="minor1">minor part of the first version</param>
	/// <param name="revision1">revision number of the first version</param>
	/// <param name="build1">build number of the first version</param>
	/// <param name="major2">major part of the second version</param>
	/// <param name="minor2">minor part of the second version</param>
	/// <param name="revision2">revision number of the second version</param>
	/// <param name="build2">build number of the second version</param>
	/// <returns>
	/// Returns < 0 if first version is less than second version.
	/// Returns > 0 if first version is greater than second version.
	/// Otherwise, returns 0.
	/// </returns>
	static int CompareVersions(int major1, int minor1, int revision1, int build1, int major2, int minor2, int revision2, int build2);

	static time_t MakeTimeFromNvramBuffer( const byte *nvramBuffer, int bufferSize );
	static int GetNvramBufferFromTime( time_t time, byte *nvramBuffer, int bufferSize );
	static bool  IsEven(__int64 num);
	static time_t MakeTimeFromMMDDYYYY_HHMMSS(byte *dateArray, byte *timeArray = 0);

	/// <summary>
	/// Gets a std::string for a card identifier.
	/// </summary>
	/// <param name="cardID">IN - The card identifier. If not NULL, MUST point to a buffer at least cardIDLen bytes long.</param>
	/// <param name="cardIDLen">INT - The length of the card identifier in bytes.</param>
	/// <returns>A std::string for a card identifier</returns>
	/// <remarks>
	/// If cardID is NULL or the card prefix is CCardConfig::INVALID_CARD_PREFIX then an empty string is returned,
	/// otherwise a string with a length of 2 * cardIDLen (defaults to a string length of 10) is returned.
	/// </remarks>
	static std::string MakeStringFromCardID(const byte *cardID, int cardIDLen = CARD_ID_LEN);
	static std::string MakeCStringFromBCD(const byte *bcdArray, byte bcdLength);
	static std::string MakeStringFromBCD(const byte *bcdArray, int bcdLength);

	/// <summary>
	/// Sets "buffer" to a card ID (2 hexadecimal digits per byte) using the provided "text".
	/// </summary>
	/// <param name="text">IN - The card number to compare to; typically 1 hexadecimal digit per character.</param>
	/// <param name="buffer">OUT - A pointer to a buffer to receive the card ID information.</param>
	/// <param name="bufferLength">IN - Length of buffer.</param>
	/// <remarks>
	/// "text" MUST be (at least) 2*"bufferLength" hexadecimal characters.
	/// Otherwise, the value of "buffer" not defined.
	/// </remarks>
	static void MakeCardIDFromCString(LPCTSTR text, byte * buffer, int bufferLength);

	/// <summary>
	/// Return a CString with a hexadecimal formatted representation of dataLength bytes of data.
	/// </summary>
	/// <param name="data">IN - byte array with data to be formmated.  MUST be at least dataLength bytes long.</param>
	/// <param name="dataLength">IN - data length.</param>
	/// <param name="maximumDataLength">IN - the maximum data length to use.</param>
	/// <returns>A string with data formatted as 
	/// hexadecimal.</returns> 
	/// <remarks>
	/// If data is NULL then a dataLength of 0 is isued.
	/// If dataLength > maximumDataLength then only the first maximumDataLength bytes of data are used.
	/// </remarks>
	static std::string ToHexString(const BYTE *data, DWORD dataLength, DWORD maximumDataLength = ToHexStringMaximumDataLength);

	/// <summary>
	/// Convert a value in FILETIME units into component values.
	/// </summary>
	/// <param name="fileTime">IN - The input value in FILETIME units.</param>
	/// <param name="hours">OUT - The component hours -- range 0..512409557 or 0..0x0x1e8abfd5.</param>
	/// <param name="minutes">OUT - The component minutes -- range 0..59.</param>
	/// <param name="seconds">OUT - The component seconds -- range 0..59.</param>
	/// <param name="milliseconds">OUT - The component milliseconds -- range 0..999.</param>
	/// <remarks>
	/// Note that the maximum input value for "filetime" results in an "hours" value that fits in a DWORD.
	/// I.e., 0xffffffffffffffff / 60 / 60 / 1000 / 10000 == 0x0x1e8abfd5.
	/// </remarks>
	static void FileTimeToComponents(const ULONGLONG &fileTime, DWORD &hours, DWORD &minutes, DWORD &seconds, DWORD &milliseconds);

	static bool MakeMMDDYYYYFromTime(time_t date, byte *bcd);
	static bool Dword2Bcd(DWORD dwVal, byte *bcdVal, int BcdSize);

	/// <summary>
	/// Compare two strings, ignoring case.
	/// </summary>
	/// <param name="str1">IN - the first string.</param>
	/// <param name="str2">IN - the second string.</param>
	/// <returns>Zero if the strings are identical (ignoring case), less than 0 if str1 is less than str2 (ignoring case), 
	/// and greater than 0 if str1 is greater than str2 (ignoring case).</returns>
	static int CompareStringsNoCase(LPCTSTR str1, LPCTSTR str2);

	/// <summary>
	/// Compare two strings, ignoring case.
	/// </summary>
	/// <param name="str1">IN - the first string.</param>
	/// <param name="str2">IN - the second string.</param>
	/// <returns>Zero if the strings are identical (ignoring case), less than 0 if str1 is less than str2 (ignoring case), 
	/// and greater than 0 if str1 is greater than str2 (ignoring case).</returns>
	static int CompareStringsNoCase(const std::string &str1, const std::string &str2);

	static std::string &MakeLower(std::string &str);

	static bool HasFileExtension(const std::string &filename, const std::string &extension);

	static std::string MaskPin(const std::string &originalString);

	/// <summary>
	/// Sets the value of errno.
	/// </summary>
	/// <param name="error">IN - The new value for errno.</param>
	static void SetLastError(int error);

	/// <summary>
	/// Return the value of errno.
	/// </summary>
	/// <returns>The value of errno.</returns>
	static int GetLastError();

	/// <summary>
	/// Return the formatted value of errno and the corresponding string value.
	/// </summary>
	/// <returns>The formatted value of errno and the corresponding string value.</returns>
	static std::string GetLastErrorCodeString();

	/// <summary>
	/// Sleeps the current thread for specified number of milliseconds.
	/// </summary>
	/// <param name="dwMilliseconds">IN - The number of milliseconds to sleep.</param>
	static void Sleep(DWORD dwMilliseconds);

	static DWORD GetTickCount();
	static std::tm GetCurrentTm();
	static time_t GetCurrentTime();
	static time_t GetUTCTime();
	static time_t GetUTCFromLocalTime(time_t localTime);

	/// <summary>
	/// Gets the local time from a UTC time.
	/// </summary>
	/// <param name="utcTime">IN - The UTC time.</param>
	/// <returns>The local time from the UTC time.</returns>
	/// <remarks>
	/// if utcTime is invalid then its value is returned unchangd.
	/// </remarks>
	static time_t GetLocalTimeFromUTC(time_t utcTime);

	static SYSTEMTIME GetUTCSystemTime (time_t time);
	static time_t GetTimeFromUTCSystemTime(const SYSTEMTIME &utcTime);
	static void SetTzOffset(int offset);
	static time_t GetTzOffset();

	/// <summary>
	/// Determine if the specified timezone is valid.
	/// </summary>
	///	<param name="timezoneOffsetInMinutes">The timezone offset in minutes</param>
	///	<returns><b>true</b> if the specified timezone is valid</b>.</returns>
	static bool IsTimezoneValid(int timezoneOffsetInMinutes);

	/// <summary>
	/// Determine if the file exists.
	/// </summary>
	///	<param name="filename">The new name of the file.</param>
	static bool FileExists(const std::string &filename);

	/// <summary>
	/// Gets the attributes of a file or a directory.
	/// </summary>
	/// <param name="pPathname">A pointer to the file or folder.</param>
	/// <param name="pErrorCode">An optional pointer to an error code.</param>
	///	<param name="waitForDisk">If <b>true</b>, wait for the disk to spin up.</param>
	///	<param name="throwOnFailure">If <b>true</b> and an error occurs, throw an exception.</param>
	///	<returns><b>true</b> if the file was found, else <b>false</b>.</returns>
	/// <exception cref="SentinelFileException">Thrown if useExceptions is true and
	///		thre is an error.</exception>
	static int GetFileAttributes(
		LPCTSTR pPathname,
		int* pErrorCode = nullptr,
		bool waitForDisk = false,
		bool throwOnFailure = false);


	/// <summary>
	/// Gets the size of a file.
	/// </summary>
	/// <param name="file">A reference to the file.</param>
	///	<returns><b>size of the file</b>.</returns>
	static int GetFileSize(std::fstream& file);

	/// <summary>
	/// Checks whether a file exists.
	/// </summary>
	/// <param name="pPathname">A pointer to the possible file.</param>
	/// <param name="pErrorCode">An optional pointer to an error code.</param>
	///	<param name="waitForDisk">If <b>true</b>, wait for the disk to spin up.</param>
	///	<param name="throwOnFailure">If <b>true</b> and an error occurs, throw an exception.</param>
	///	<returns><b>true</b> if the file exists, else <b>false</b>.</returns>
	/// <exception cref="SentinelFileException">Thrown if useExceptions is true and
	///		thre is an error.</exception>
	static bool DoesFileExist(
		LPCTSTR pFile,
		int* pErrorCode = nullptr,
		bool waitForDisk = false,
		bool throwOnFailure = false);

	static bool DoesFileExist(
		std::string sFile,
		int* pErrorCode = nullptr,
		bool waitForDisk = false,
		bool throwOnFailure = false);

	/// <summary>
	/// Checks whether a directory exists.
	/// </summary>
	/// <param name="pPathname">A pointer to the possible directory.</param>
	/// <param name="pErrorCode">An optional pointer to an error code.</param>
	///	<param name="waitForDisk">If <b>true</b>, wait for the disk to spin up.</param>
	///	<param name="throwOnFailure">If <b>true</b> and an error occurs, throw an exception.</param>
	///	<returns><b>true</b> if the directory exists, else <b>false</b>.</returns>
	/// <exception cref="SentinelFileException">Thrown if useExceptions is true and
	///		thre is an error.</exception>
	static bool DoesDirExist(
		LPCTSTR pDir,
		int* pErrorCode = nullptr,
		bool waitForDisk = false,
		bool throwOnFailure = false);

	/// <summary>
	/// Get disk space information.
	/// </summary>
	/// <param name="lpDirectoryName">A pointer to the possible directory.</param>
	/// <param name="lpFreeBytesAvailable">An optional pointer to a variable that receives the total number of free bytes on the disk that are available to the caller.</param>
	/// <param name="lpTotalNumberOfBytes">An optional pointer to a variable that receives the total number of bytes on the disk that are available to the caller.</param>
	/// <param name="lpTotalNumberOfFreeBytes">An optional pointer to a variable that receives the total number of free bytes on the disk.</param>
	///	<returns>If the function succeeds, the return value is nonzero. If the function fails, the return value is zero(0) and errno is set appropriately.</returns>
	static BOOL GetDiskFreeSpaceEx(
		LPCTSTR lpDirectoryName,
		PULARGE_INTEGER lpFreeBytesAvailable,
		PULARGE_INTEGER lpTotalNumberOfBytes,
		PULARGE_INTEGER lpTotalNumberOfFreeBytes
		);

 	static time_t SysTimeToUlarge(const SYSTEMTIME &inTime);
	static SYSTEMTIME UlargeToSysTime(time_t &inTime);

	/// <summary>
	/// Compares two SYSTEMTIME values.
	/// </summary>
	/// <param name="st1">IN - The first SYSTIME value.</param>
	/// <param name="st2">IN - The second SYSTIME value.</param>
	/// <returns>Returns -1 if st1 is less than st2. Returns 1 if st1 is greater st2. Returns 0 if st1 is equal st2.</returns>
	static int CompareSysTimes( const SYSTEMTIME &st1, const SYSTEMTIME &st2 );
	
 	static COLORREF ConvertRGBToBGR(unsigned int color);

	/// <summary>
	/// Gets a formatted number string, with commas as appropriate.
	/// The formatted number string can be used for cash strings.
	/// </summary>
	/// <param name="val">IN - The value to format.</param>
	/// <returns>The value formatted as a string.</returns>
	static std::string GetFormattedNumberString( DWORD val );

	/// <summary>
	/// Gets a formatted number string, with commas as appropriate, plus optional text.
	/// </summary>
	/// <param name="val">IN - The value to format.</param>
	/// <param name="text">IN - A pointer to text.  If not NULL then a space and the text is appended to the formatted number string.</param>
	/// <returns>The value formatted as a string, plus optional text.</returns>
	static std::string GetFormattedNumberString(DWORD val, const std::string &text);

	/// <summary>
	/// Return the provided dollars and pennies values formatted as a cash string.
	/// </summary>
	/// <param name="dollars">IN - The dollar value.</param>
	/// <param name="pennies">IN - The pennies value.  SHOULD be less than 100.</param>
	/// <returns>the provided cash value formatted as a string.</returns>
	/// <remarks>
	/// If dollars > INT_MAX then dollars is formatted as a signed integer with a negative value.
	/// If pennies > INT_MAX then pennies is formatted as a signed integer with a negative value.
	/// </remarks>
	static std::string GetFormattedDollarsPenniesString(DWORD dollars, DWORD pennies = 0);

	/// <summary>
	/// Return the provided dollars and pennies values formatted as a cash string.
	/// </summary>
	/// <param name="dollars">IN - The dollar value.</param>
	/// <param name="pennies">IN - The pennies value.  SHOULD be less than 100.</param>
	/// <returns>the provided cash value formatted as a string.</returns>
	/// <remarks>
	/// If dollars > INT_MAX then dollars is formatted as a signed integer with a negative value.
	/// If pennies > INT_MAX then pennies is formatted as a signed integer with a negative value.
	/// </remarks>
	static std::string GetFormattedDollarsPenniesString(uint64_t dollars, DWORD pennies = 0);

	/// <summary>
	/// Return the provided dollars and pennies values formatted as a cash string.
	/// </summary>
	/// <param name="dollars">IN - The dollar value.</param>
	/// <param name="pennies">IN - The pennies value.  SHOULD be less than 100.</param>
	/// <returns>the provided cash value formatted as a string.</returns>
	/// <remarks>
	/// If dollars > INT_LONG then dollars is formatted as a signed long with a negative value.
	/// If pennies > INT_MAX then pennies is formatted as a signed integer with a negative value.
	/// </remarks>
	static std::string GetFormattedDollarsPenniesStringInt64(int64_t dollars, DWORD pennies);

	/// <summary>
	/// Return the provided cash value formatted as a cash string.
	/// </summary>
	/// <param name="cashValue">IN - The cash value.</param>
	/// <returns>The provided cash value formatted as a string.</returns>
	static std::string GetFormattedCashString(DWORD cashValue);

	/// <summary>
	/// Return the provided cash value formatted as a cash string
	/// </summary>
	/// <param name="cashValue">IN - The cash value.</param>
	/// <returns>The provided cash value formatted as a string.</returns>
	static std::string GetFormattedCashString(int cashValue);

	/// <summary>
	/// Return the provided cash value formatted as a cash string, plus optional text.
	/// </summary>
	/// <param name="cashValue">IN - The cash value.</param>
	/// <param name="text">IN - A pointer to text.  If not NULL then a space and the text is appended to the formatted cash string.</param>
	/// <returns>The provided cash value formatted as a string, plus optional text..</returns>
	static std::string GetFormattedCashString(DWORD cashValue, LPCTSTR text);

	/// <summary>
	/// Return the provided cash value formatted as a cash string
	/// </summary>
	/// <param name="cashValue">IN - The cash value.</param>
	/// <returns>The provided cash value formatted as a string.</returns>
	static std::string GetFormattedCashString( long cashValue);

	/// <summary>
	/// Return the provided cash value formatted as a cash string
	/// </summary>
	/// <param name="cashValue">IN - The cash value.</param>
	/// <returns>The provided cash value formatted as a string.</returns>
	static std::string GetFormattedCashString( long long cashValue);

	/// <summary>
	/// Gets a formatted number string, with commas as appropriate.
	/// The formatted number string can be used for cash strings.
	/// </summary>
	/// <param name="val">IN - The value to format.</param>
	/// <returns>The value formatted as a string.</returns>
	static std::string GetFormattedNumberString(uint64_t val);

	/// <summary>
	/// Gets a formatted number string, with commas as appropriate.
	/// The formatted number string can be used for cash strings.
	/// </summary>
	/// <param name="val">IN - The value to format.</param>
	/// <returns>The value formatted as a string.</returns>
	static std::string GetFormattedNumberString(__int64_t val);

	/// <summary>
	/// Gets a formatted number string, with commas as appropriate.
	/// The formatted number string can be used for cash strings.
	/// </summary>
	/// <param name="val">IN - The value to format.</param>
	/// <returns>The value formatted as a string.</returns>
	static std::string GetFormattedNumberString(const std::string& value);
		
	static void MemCopySafe_ToOffset(bool *pOk,
		byte *pDestBuffer, size_t destBufferSize,  size_t *pDestBufferOffset,
		const void *pSrcBuffer, size_t copyLength);
	static void MemCopyZero_FromOffset(bool *pOk,
		void *pDestBuffer, size_t destBufferSize,
		const byte *pSrcBuffer, size_t srcBufferSize,  size_t *pSrcBufferOffset);
	static void MemCopyZeroIncSourcePtr(
		void *pDestBuffer, size_t destBufferSize,
		void const *&pSrcBuffer, size_t &srcBufferSize);
	static void MemCopyZeroIncSourcePtr(
		void *pDestBuffer, size_t destBufferSize,
		void const *&pSrcBuffer, int &srcBufferLength);
	static void MemCopyZero(
		void *pDestBuffer, size_t destBufferSize,
		void const *pSrcBuffer, size_t srcBufferSize);
	static void MemCopyZero(
		void *pDestBuffer, size_t destBufferSize,
		void const *pSrcBuffer, int srcBufferLength);

	/// <summary>
	/// Copies memory safely from the source buffer to the target buffer and increments the source pointer.
	/// </summary>
	/// <param name="ok">IN/OUT - true if okay, false if NOT okay.</param>
	/// <param name="target">IN/OUT - The target buffer.</param>
	/// <param name="targetSize">IN - A pointer to the target buffer size.</param>
	/// <param name="source">IN/OUT - A pointer to the source buffer.</param>
	/// <param name="sourceEnd">IN - A pointer to the byte past the end of the source buffer.</param>
	/// <remarks>
	/// Nothing is done if ok is false or targetSize is 0.
	/// Else
	///    ok is set false if target == nullptr || source == nullptr || sourceEnd == nullptr
	///    ok is set false if source + targetSize > sourceEnd.
	///    target is not changed if ok is false, otherwise target is set from the source buffer and the source pointer is incremented.
	/// </remarks>
	static void MemCopySafe_IncSourcePtr(bool &ok,
		void *target,
		size_t targetSize,
		const byte *&source,
		const byte *sourceEnd);

	static std::string GetEXEVersion(void);
	
	///	<summary>
	///	Removes the read only attribute from a file object.
	///	</summary>
	///	<param name="filename">Full path name of file whose read only attribute is to be removed..</param>
	///	<param name="waitForDisk">If <b>true</b>, wait for the disk to spin up.</param>
	static bool RemoveReadOnlyAttribute(LPCTSTR pPathname, bool waitForDisk);
	///	</summary>
	///	<param name="filename">The current name of the file.</param>
	///	<param name="filename">The new name of the file.</param>
	///	<param name="pErrorCode">Optional pointer to receive error code.</param>
	///	<param name="waitForDisk">If <b>true</b>, wait for the disk to spin up.</param>
	///	<param name="replaceExisting">If <b>true</b> and newFilename exists, it will be
	///		replaced.</param>
	///	<param name="throwOnFailure">If <b>true</b> and an error occurs, throw an exception.</param>
	///	<returns><b>true</b> if the file was moved and/or renamed, else <b>false</b>.</returns>
	/// <exception cref="SentinelCancelException">Thrown if useExceptions is true and
	///		hCancel is valid and signaled.</exception>
	/// <exception cref="SentinelFileException">Thrown if useExceptions is true and
	///		thre is an error.</exception>
	static bool RenameFile(const std::string &existingFilename, const std::string &newFilename, DWORD *pErrorCode = NULL,
		bool waitForDisk = false, bool replaceExisting = false, bool throwOnFailure = false);

	///	<summary>
	///	Deletes a file.
	///	</summary>
	///	<param name="filename">Full path name of file to delete.</param>
	///	<param name="pErrorCode">Optional pointer to receive error code.</param>
	///	<param name="waitForDisk">If <b>true</b>, wait for the disk to spin up.</param>
	///	<returns><b>true</b> if the file was deleted, else <b>false</b>.</returns>
	static bool DeleteFile(LPCTSTR filename, DWORD *pErrorCode = NULL, bool waitForDisk = false);

	///	<summary>
	///	Create a new directory
	///	</summary>
	///	<param name="directoryToCreate">Path of directory to Create.</param>
	///	<returns><b>true</b> on success, else <b>false</b>.</returns>
	static bool CreateDirectory(const std::string &directoryToCreate);

	///	<summary>
	///	Gets the count of files in a directory. Will also filter down to the filenames/suffix passed in in files.
	///	Can be called like: FileCount("/tmp", {"*.crt", "*.pem"}); to find all files in dir with crt and pem suffix.
	///	Will count every file if no "files" are passed in.
	///	</summary>
	///	<param name="directory">Path of directory to count files in.</param>
	/// <param name="files">Files or search items passed to the ls command.</param>
	///	<returns>Number of files in the directory.</returns>
	static int FileCount(const std::string& directory, std::initializer_list<std::string> files);

	///	<summary>
	///	Deletes a directory tree.
	///	</summary>
	///	<param name="directoryToDelete">Path of directory to delete.</param>
	///	<param name="leaveBaseDirectory">If <b>true</b>, do not remove the base directory.</param>
	///	<param name="pFilesToKeep">Optional pointer to a set of files to keep.</param>
	///	<returns><b>true</b> on success, else <b>false</b>.</returns>
	///	<remarks>
	///	This function is recursive.
	///
	///	This will not throw an exception on DeleteFile failure, but continue attempting to
	/// delete the directory tree.
	///	</remarks>
	static bool DeleteDirectory(
		const std::string &directoryToDelete,
		bool leaveBaseDirectory = false,
		const std::map<std::string, bool>* pFilesToKeep = nullptr);

	///	<summary>
	///	Compares two strings and ignores case
	///	</summary>
	///	<param name="str1">First string to be compared.</param>
	/// <param name="str2">Second string to be compared.</param>
	/// <returns><b>true</b> if the strings are equal, else<b>false</b>.</returns>
	static bool StringCompareIgnoreCase(std::string const& str1, std::string const& str2);


	///	<summary>
	///	Indicates whether a character is a forward (/) or backward (\) slash.
	///	</summary>
	///	<param name="ch">The character to check.</param>
	///	<returns><b>true</b> if the passed character is a forward (/) or backward (\) slash.</returns>
	static bool PathIsSlash(TCHAR ch);

	///	<summary>
	///	Indicates whether a character is a colon (:) or slash (/\).
	///	</summary>
	///	<param name="ch">The character to check.</param>
	///	<returns><b>true</b> if the passed character is a colon (:) or slash (/\).</returns>
	static bool PathIsDelimeter(TCHAR ch);

	///	<summary>
	/// Appends a slash to the passed path if one is not already present.
	///	</summary>
	///	<param name="path">The path to check and possibly change.</param>
	///	<param name="useForwardSlash">Whether to us a forward (/) or backward (\) slash.</param>
	///	<returns>A pointer to the resulting string.</returns>
	static LPCTSTR PathAddSlashCString(std::string& path, bool useForwardSlash = false);

	///	<summary>
	///	Removes a slash from the passed path if one is present.
	///	</summary>
	///	<param name="path">The path to check and possibly change.</param>
	///	<returns>A pointer to the resulting string.</returns>
	static LPCTSTR PathRemoveSlashCString(std::string& path);

	///	<summary>
	/// Appends a slash to a path if one is not already present.
	///	</summary>
	///	<param name="pPath">The path to check.</param>
	///	<param name="useForwardSlash">Whether to us a forward (/) or backward (\) slash.</param>
	///	<returns>A CString instance containing the path.</returns>
	static std::string PathAddSlash(LPCTSTR pPath, bool useForwardSlash = false);

	///	<summary>
	///	Removes a slash from a path if one is present.
	///	</summary>
	///	<param name="pPath">The path to check.</param>
	///	<returns>A CString instance containing the path.</returns>
	static std::string PathRemoveSlash(LPCTSTR pPath);

	static void CRectSetSize(struct RECT &rect, const struct SIZE &size);
	static double WidthToHeightRatio(struct SIZE const &size);
	static double WidthToHeightRatio(struct SIZE const &size, DesktopOrientationType orientation);
	static double WidthToHeightRatio(struct RECT const &rect);
	static struct SIZE Rotate(struct SIZE const &size, DesktopOrientationType orientation);
	static double Rotate(double aspectRatio, DesktopOrientationType orientation);
	static struct RECT RotateRectBack(struct RECT const &rect, struct SIZE const &screenSize, DesktopOrientationType orientation);

    /// <summary> 
    /// Checks if the supplied filename is for a FLV. 
    /// </summary> 
    /// <param name="filestr">IN - The filename to check</param> 
    /// <returns>True if FLV filenme</returns> 
	static bool IsFlv(const std::string &filestr);

    /// <summary> 
    /// Checks if the supplied filename is for a BMP. 
    /// </summary> 
    /// <param name="filestr">IN - The filename to check</param> 
    /// <returns>True if BMP filenme</returns> 
	static bool IsBmp(const std::string &filestr);

    /// <summary> 
    /// Checks if the supplied filename is for a SMD. 
    /// </summary>
    /// <param name="filestr">IN - The filename to check</param> 
    /// <returns>True if SMD filenme</returns> 
	static bool IsSmd(const std::string &filestr);

	static short OrientationAngle(DesktopOrientationType orientation);
	static DesktopOrientationType AngleOrientation(short angle);
	static short RotateAngle(short start, short rotate);
	static unsigned short CRC(unsigned char *s, int len, unsigned short crcval);


	/// <summary>
	/// Sets the name of the thread (for debugging purposes).
	/// </summary>
	/// <param name="dwThreadID">IN - The thread identifier.</param>
	/// <param name="threadName">IN - The name of the thread.</param>
	/// <remarks>
//TODO	/// Per MSDN: Note that the thread name is copied to the thread so that the memory for the threadName parameter can be released.
	/// </remarks>
	static void SetThreadName(std::thread::id dwThreadID, const std::string &threadName);

	/// <summary>
	/// Gets the current thread identifier.
	/// </summary>
	/// <returns>The current thread identifier.</returns>
	static std::thread::id GetCurrentThreadId();

	/// <summary>
	/// Logs the thread name and current thread identifier.
	/// </summary>
	/// <param name="threadName">IN - The name of the thread.</param>
	static void ShowThreadId(const std::string &threadName);

	static void FlushVolume();

    static void FlushFileSystemBuffers();

	static char *BigEndianToHexCharArray( const byte *number, int numberLength );

	static std::string LoadResourceString(LPCTSTR id);

	static std::string SetStringSliderValue(DWORD value);
     
    /// <summary>
	/// Helper function to get local time to a string to use as a 'timestamp' for logging and error reporting.
	/// </summary>
	/// <returns>Returns the timestamp.</returns>
	static std::string GetTimeStamp();

	/// <summary>
	/// Determines whether date is a valid time_t date.
	/// </summary>
	/// <param name="date">The date.</param>
	/// <returns>True when date is valid.</returns>
	static bool IsDateValid(time_t date);

	/// <summary>
	/// Formats a date time string if date is valid, int value otherwise.
	/// </summary>
	/// <param name="date">IN - The date.</param>
	/// <returns>Formatted date/time value, integer string value if date is invalid.</returns>
	static std::string/*CString*/ FormatDateTimeString(time_t/*CTime*/ date);

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
	/// Formats a UTC timestamp as a local date time string (using a military format) if date is valid, "N/A" or int value otherwise.
	/// </summary>
	/// <param name="utcTimestamp">IN - The UTC timestamp.</param>
	/// <returns>Formatted date/time value, "N/A" if year and month are zero, or an integer string value if date is invalid.</returns>
	static std::string FormatLocalMilitaryDateTimeStringFromUTC(const SYSTEMTIME& utcTimestamp);

	/// <summary>
	/// Formats a date time string if date is valid, int value otherwise.
	/// </summary>
	/// <param name="date">IN - The date.</param>
	/// <param name="mask">IN - The format mask.</param>
	/// <returns>Formatted date/time value, integer string value if date is invalid.</returns>
	static std::string FormatDateTimeString(time_t date, const std::string &mask);

	/// <summary>
	/// Gets a time span formatted as a string with days, hours, minutes, and seconds.
	/// </summary>
	/// <param name="date">IN - The time span.</param>
	/// <returns>A time span formatted as a string with days, hours, minutes, and seconds.</returns>
	static std::string FormatTimeSpanString(time_t timeSpan);

	static std::string GetStdStringFromSystemTime(const SYSTEMTIME &mtime);


	/// <summary>
	/// Do a DNS lookup for a given domain name.
	/// </summary>
	/// <param name="name">domain to lookup.</param>
	/// <param name="ipArray">OUT array to hold the ip addresses found as an array of CIPAddress objects.</param>
	/// <returns>'0' if ip address are found. Otherwise, return the DNS Status error.</returns>
	static long DNSLookup(LPCTSTR name, CIPAddressArray& ipArray);


	/// <summary>
	/// Get the current time in milliseconds since UTC January 1, 1970.
	/// </summary>
	/// <returns>milliseconds since Jan 1, 1970 UTC</returns>
	static __int64 MillisecondsSince1970();

	/// <summary>
	/// Converts to provided SystemTime to milliseconds since UTC January 1, 1970
	/// </summary>
	/// <param name="systemTime">The system time to convert.</param>
	/// <returns>milliseconds since Jan 1, 1970 UTC</returns>
	static __int64 ConvertToMillisecondsSince1970(const SYSTEMTIME& systemTime);

	/// <summary>
	/// Gets the difference between two timestamps in milliseconds.
	/// If the first timespan is larger than the second, the result will be negative.
	/// If the first timespan is smaller than the second, the result will be positive.
	/// </summary>
	/// <param name="systemTime1">the first timestamp</param>
	/// <param name="systemTime2">the second timestamp</param>
	/// <returns>the difference between the two timestamps in milliseconds</returns>
	static __int64 GetSystemTimespanInMilliseconds(const SYSTEMTIME& systemTime1, const SYSTEMTIME& systemTime2);

	/// <summary>
	/// Gets the seconds between 2 time_t values.
	/// </summary>
	/// <param name="begin">the first time_t</param>
	/// <param name="end">the second time_t</param>
	/// <returns>the duration between the two time_t values in 
	/// seconds</returns> 
	static std::chrono::duration<int> GetTimeSpanSeconds(time_t begin, time_t end);

	/// <summary>
	/// Gets the system time from a tm struct.
	/// </summary>
	/// <param name="sysTm">IN - The tm struct.</param>
	/// <returns>A SYSTEMTIME struct corresponding to the tm struct.</returns>
	static SYSTEMTIME GetSystemTimeFromTm(const tm &sysTm);

	/// <summary>
	/// Get the current UTC system time.
	/// </summary>
	/// <returns>A SYSTEMTIME struct with the UTC current date/time.</returns>
	static void GetUTCSystemTime(LPSYSTEMTIME lpSystemTime);

	/// <summary>
	/// Get the current UTC tm.
	/// </summary>
	/// <returns>A tm struct with the current UTC date/time.</returns>
	static tm GetUTCSystemTm();

	/// <summary>
	/// Get the current UTC system time.
	/// </summary>
	/// <returns>A SYSTEMTIME struct with the UTC current date/time.</returns>
	static void GetLocalSystemTime(LPSYSTEMTIME lpSystemTime);

	static SYSTEMTIME GetLocalSystemTime(time_t time);

	/// <summary>
	/// Get the current local tm.
	/// </summary>
	/// <returns>A tm struct with the current UTC date/time.</returns>
	static tm GetLocalSystemTm();

	/// <summary>
	/// Get a Jan 1, 1970 system time.
	/// </summary>
	/// <returns>A SYSTEMTIME struct with the date/time of Jan 1, 1970.</returns>
	static SYSTEMTIME Get1970Time();

	/// <summary>
	/// Get a Jan 1, 1970 tm structure.
	/// </summary>
	/// <returns>A tm struct with the date/time of Jan 1, 1970.</returns>
	static tm Get1970Tm();

	/// <summary>
	/// Get a Jan 1, 1970 time_t value in local time.
	/// </summary>
	/// <returns>A local time_t value with the date/time of Jan 1, 1970.</returns>
	static time_t GetLocal1970Timet();

	/// <summary>
	/// Is the specified SYSTEMTIME Jan 1, 1970.
	/// </summary>
	/// <param name="systemTime">The system time to check</param>
	/// <returns>true if the provided time is Jan 1 1970</returns>
	static bool Is1970Time(const SYSTEMTIME& systemTime);

	/// <summary>
	/// Gets the milliseconds since the specified timestamp.
	/// This will return a negative number if the specified time is in the future.
	/// </summary>
	/// <param name="systemTime">The timestamp.</param>
	/// <returns>the number of milliseconds since the specified time</returns>
	static __int64 GetMillisecondsSinceTimestamp(const SYSTEMTIME& systemTime);

	/// <summary>
	/// Gets an __int64 value from a string.
	/// </summary>
	/// <param name="stringValue">IN - The string value.</param>
	/// <param name="value">IN/OUT - The value from the conversion if true is returned, otherwise unchanged.</param>
	/// <returns>true if the conversion was successful, false otherwise.</returns>
	static bool GetInt64FromString(LPCTSTR stringValue, __int64 &variable);

	/// <summary>
	/// Gets an __int64 value from a string.
	/// </summary>
	/// <param name="stringValue">IN - The string value.</param>
	/// <param name="value">IN/OUT - The value from the conversion if true is returned, otherwise unchanged.</param>
	/// <returns>true if the conversion was successful, false otherwise.</returns>
	static bool GetInt64FromString(const std::string &stringValue, __int64 &variable);

	/// <summary>
	/// Gets an __int64 value in pennies from a currency string.
	/// </summary>
	/// <param name="currency">IN - The currency string.</param>
	/// <param name="value">IN/OUT - The value in pennies if true is returned, otherwise unchanged.</param>
	/// <returns>true if the conversion was successful, false otherwise.</returns>
	/// <remarks>
	/// Excess digits to the right of a decimal point in the currency string are truncated.
	/// </remarks>
	static bool GetInt64FromCurrencyString(LPCTSTR currency, __int64& value);

	/// <summary>
	/// Gets an __int64 value in pennies from a currency string.
	/// </summary>
	/// <param name="currency">IN - The currency string.</param>
	/// <returns>The value in cents. Returns 0 if the conversion was unsuccessful.</returns>
	/// <remarks>
	/// Excess digits to the right of a decimal point in the currency string are truncated.
	/// </remarks>
	static __int64 GetInt64FromCurrencyString(LPCTSTR currency);

	/// <summary>
	/// Gets an __int64 value in pennies from a currency string.
	/// </summary>
	/// <param name="currency">IN - The currency string.</param>
	/// <returns>The value in cents.</returns>
	static __int64 GetInt64FromCurrencyString(const std::string& currency);

	/// <summary>
	/// Gets a standard string for a CString value.
	/// </summary>
	/// <param name="value">IN - The value.</param>
	/// <returns>A standard string for the CString value.</returns>
	static std::string GetStdStringFromString(const std::string &value);

	/// <summary>
	/// Gets a standard string for a LPCTSTR value.
	/// </summary>
	/// <param name="value">IN - The value.</param>
	/// <returns>A standard string for the LPCTSTR value.</returns>
	static std::string GetStdStringFromLPCTSTR(LPCTSTR value);

	/// <summary>
	/// Gets a string identifier, composed of the specified prefix and id.
	/// </summary>
	/// <param name="prefix">IN - The prefix.</param>
	/// <param name="id">IN - The identifier number.</param>
	/// <param name="maxLength">IN - The maximum length of the returned string.</param>
	/// <returns>A string identifier, composed of the specified prefix and id.</returns>
	/// <remarks>
	/// If the combined length (of the prefix and the formatted id) would be greater than maxLength,
	/// then the sign (if any) of the id is retained and as many of the least significant digits are retained as possible
	/// -- as the least significant digits are expected to change when the id changes.
	/// This allows the (least significant) digits of the input to be easily matched with the output when needed.
	/// Note that leading zeros are retained when preceding digits are removed from the formatted id.
	/// </remarks>
	static std::string GetPrefixIdString(LPCTSTR prefix, LONG64 id, int maxLength);

	/// <summary>
	/// URL encode a string.
	/// </summary>
	/// <param name="stringToEncode">string to encode</param>
	/// <returns>returns UrlEncoded string</returns>
	static std::string UrlEncode(const std::string& stringToEncode);

	/// <summary>
	/// Find and replace all string value with a new specified value
	/// </summary>
	/// <param name="subject">string to alter</param>
	/// <param name="search">string to search</param>
	/// <param name="replace">new value to replace a specifed value</param>
	static void FindAndReplace(std::string& subject, const std::string& search, const std::string& replace);

	/// <summary>
	/// Erases all the substring occurence from a string.
	/// </summary>
	/// <param name="subject">string to alter</param>
	/// <param name="toErase">string to erase</param>
	static void EraseAllSubStr(std::string & subject, const std::string & toErase);

	/// <summary>
	/// Erases all the vector of substrings occurence from a string.
	/// </summary>
	/// <param name="subject">string to alter</param>
	/// <param name="strList">vector of substring to erase</param>
	static void EraseSubStrings(std::string& subject, const std::vector<std::string>& strList);

	/// <summary>
	/// Converts the string to long long.
	/// </summary>
	/// <param name="number">The number.</param>
	/// <returns>int64_t equivalent of the number string.</returns>
	static int64_t ConvertStringToLongLong(std::string number);

	/// <summary>
	/// Converts the decimal string to decimal string with X amount of decimal points if necessary.
	/// </summary>
	/// <param name="number">str to convert.</param>
	/// <param name="precision">precision of the decimal number</param>
	/// <returns>decimal string with precision decimal equivalent of the number string or the same str value.</returns>
	static std::string ConvertStringToDecimalIfNecessary(const std::string& str, int precision);

	static time_t m_tzOffset;
	static const DWORD NUMBER_OF_WAIT_FOR_DISK_PAUSES = 20;
	static const DWORD WAIT_FOR_DISK_PAUSE = 1000;
	static const DWORD WAIT_FOR_DISK_MILLISECONDS = NUMBER_OF_WAIT_FOR_DISK_PAUSES * WAIT_FOR_DISK_PAUSE;
	static const DWORD MS_VC_Exception = 0x406D1388;
	static const DWORD ToHexStringMaximumDataLength = 256;
	static const TCHAR ToHexStringFormatForFirstByte[];
	static const TCHAR ToHexStringFormatForOtherBytes[];
	static LPCTSTR CASH_FORMAT;
	static const TCHAR TCHAR_DECIMAL_POINT = _T('.');
	static const TCHAR TCHAR_NEGATIVE_SIGN = _T('-');
	static const TCHAR TCHAR_PLUS_SIGN = _T('+');
	static const TCHAR TCHAR_ZERO = _T('0');
	static const TCHAR TCHAR_NINE = _T('9');
	static const int STRING_CONVERSION_RADIX = 10; // Only support base 10 for numeric values.
	static const TCHAR TCHAR_COMMA = _T(',');

	// Need ASCII chars for std::string.
	static const char CHAR_DECIMAL_POINT = '.';
	static const char CHAR_ZERO = '0';

	/// <summary>
	/// Trims all whitespace from the beginning of the string.
	/// </summary>
	/// <param name="str">IN/OUT - The string.</param>
	/// <returns>A reference to the string.</returns>
	static std::string &TrimLeft(std::string &str);

	/// <summary>
	/// Trims all whitespace from the end of the string.
	/// </summary>
	/// <param name="str">IN/OUT - The string.</param>
	/// <returns>A reference to the string.</returns>
	static std::string &TrimRight(std::string &str);

	/// <summary>
	/// Trims all whitespace from the beginning and the end of the string.
	/// </summary>
	/// <param name="str">IN/OUT - The string.</param>
	/// <returns>A reference to the string.</returns>
	static std::string &Trim(std::string &str);

	static std::string trimLeft(std::string& str);
	// Time conversion methods that get/return time_t values.

	/// <summary>
	/// Gets a time_t value from a struct tm value -- without time zone adjustment.
	/// </summary>
	/// <param name="sysTm">IN - The tm struct value.</param>
	/// <returns>A time_t value corresponding to the tm struct value -- without time zone adjustment.</returns>
	static time_t internal_timegm(std::tm const *t);

	/// <summary>
	/// Gets a time_t value from a local time struct tm value.
	/// </summary>
	/// <param name="localTm">IN - The tm struct value.</param>
	/// <returns>A time_t value corresponding to the tm struct value.</returns>
	/// <remarks>
	/// Same conversion as maketm(), except that CUtilities:m_tzOffset is used for the time zone offset.
	/// </remarks>
	static time_t GetTimeFromLocalTm(const tm &localTm);

	/// <summary>
	/// Gets the UTC time from local time fields.
	/// </summary>
	/// <param name="years">The local time year (approx. 1900...3000)</param>
	/// <param name="months">The local time month (1..12).</param>
	/// <param name="days">The local time day (1..31).</param>
	/// <param name="hours">The local time hour (0..23).</param>
	/// <param name="minutes">The local time minute (0..59).</param>
	/// <param name="seconds">The local time second (0..59).</param>
	/// <returns>The UTC time from local time fields</returns>
	static time_t GetTimeFromFields(int years, int months, int days, int hours, int minutes, int seconds);

	// Time conversion methods that get/return struct tm values.

	/// <summary>
	/// Gets a local time struct tm value from a time value.
	/// </summary>
	/// <param name="timeValue">IN - The time value.</param>
	/// <returns>A local time struct tm value from the time value.</returns>
	/// <remarks>
	/// Same conversion as localtime_s(), except that CUtilities:m_tzOffset is used for the time zone offset.
	/// </remarks>
	static tm GetLocalTmFromTime(time_t timeValue);

	/// <summary>
	/// Gets a struct tm value from a system time value.
	/// </summary>
	/// <param name="sysTime">IN - The system time value.</param>
	/// <returns>A struct tm value from the system time value.</returns>
	static tm GetTmFromSystemTime(const SYSTEMTIME &);

	/// <summary>
	/// Gets a tm struct value from time fields.
	/// </summary>
	/// <param name="years">The year (1900 ...?)</param>
	/// <param name="months">The month (1..12).</param>
	/// <param name="days">The day (1..31).</param>
	/// <param name="hours">The hour (0..23).</param>
	/// <param name="minutes">The minute (0..59).</param>
	/// <param name="seconds">The second (0..61).</param>
	/// <returns>The UTC time from fields</returns>
	/// <remarks>
	/// tm_wday is set to 0.
	/// tm_yday is set to 0.
	/// tm_isdts is set to 0.
	/// </remarks>
	static tm GetTmFromFields(int years, int months, int days, int hours, int minutes, int seconds);

	// Time conversion methods that get/return field values.

	/// <summary>
	/// Gets local time fields from a time_t value.
	/// </summary>
	/// <param name="timeValue">IN - The time_t value.</param>
	/// <param name="years">OUT - The local time year (approx. 1900...3000)</param>
	/// <param name="months">OUT - The local time month (1..12).</param>
	/// <param name="days">OUT - The local time day (1..31).</param>
	/// <param name="hours">OUT - The local time  (0..23).</param>
	/// <param name="minutes">OUT - The local time minute (0..59).</param>
	/// <param name="seconds">OUT - The local time second (0..59).</param>
	static void GetFieldsFromTime(time_t timeValue, int &years, int &months, int &days, int &hours, int &minutes, int &seconds);

	/// <summary>
	/// <summary>
	/// Gets time fields from a struct tm value.
	/// </summary>
	/// <param name="tmValue">IN - The struct tm value.</param>
	/// <param name="years">OUT - The year (approx. 1900...3000)</param>
	/// <param name="months">OUT - The month (1..12).</param>
	/// <param name="days">OUT - The day (1..31).</param>
	/// <param name="hours">OUT - The hour (0..23).</param>
	/// <param name="minutes">OUT - The minute (0..59).</param>
	/// <param name="seconds">OUT - The second (0..61).</param>
	static void GetFieldsFromTm(const tm &tmValue, int &years, int &months, int &days, int &hours, int &minutes, int &seconds);

	/// <summary> This is the predicate for our string compre no 
	/// case method. 
	/// </summary>
	/// <param name="char1">First character to compre.</param>
	/// <param name="char2">Second character to compre.</param>
	/// <returns><b>true</b> if the characters are equal, else<b>false</b>.</returns> 
	static bool StringComparePred(unsigned char char1, unsigned char char2);

	/// <summary>
	/// Gets path memory usage information.
	/// </summary>
	/// <param name="fileSytemPath">OUT- file system path.</param>
	/// <param name="size">OUT- size of the drive.</param>
	/// <param name="used">OUT- used memory size.</param>
	/// <param name="available">OUT- available memory size.</param>
	static void GetPathMemoryUsageInformation(const std::string& fileSytemPath, std::string& size, std::string& used, std::string& available);

	/// <summary>
	/// Gets the system result string.
	/// </summary>
	/// <param name="systemCall">The system call.</param>
	/// <returns>system result string</returns>
	static std::string GetSystemResultString(const std::string& systemCall);

	/// <summary>
	/// Makes system call to copy a file.
	/// </summary>
	/// <param name="source">Source file path</param>
	/// <param name="target">Target file path</param>
	/// <returns>true if system call succeeded</returns>
    static bool CopyFile(const std::string &source, const std::string &target);
	
	/// <summary> Replace \ with / in string
	/// </summary>
	/// <param name="path">Path to convert to linux format</param>
	static void FixDirDelimiter(std::string &path);
	
	/// <summary>
	/// Convert hex string to byte array
	/// </summary>
	/// <param name="hexString">Hex string to convert</param>
	/// <param name="bytes">Byte array to fill</param>
	/// <param name="byteLength">Length of byte array</param>
	/// <returns>true if system call succeeded</returns>
	static bool ConvertHexStringToBytes(std::string hexString, byte *bytes, int byteLength);

	/// <summary>
	/// Convert char to byte
	/// </summary>
	/// <param name="input">Char value</param>
	/// <returns>byte value of input character</returns>
	static uint8_t ConvertCharToByte(char input);
	
	/// <summary> get GUIDs using rand()
	/// </summary>
	/// <returns>GUID as string</returns>
	static std::string GetGUID();

	/// <summary>
	/// Encrypt the PBT Pin before sending over network to Poller.
	/// </summary>
	/// <param name="unencryptedPIN">The time.</param>
	/// <returns>Encrypted PBT Pin</returns>
	static DWORD EncryptPBTPin(DWORD unencryptedPIN);

	/// <summary>
	/// Unencrypt the PBT PIN.
	/// </summary>
	/// <param name="EncryptedPIN">The encypted PIN.</param>
	/// <returns>Unencrypted PBT Pin</returns>
	static DWORD UnencryptPBTPin(DWORD encryptedPIN);

	/// <summary>
	/// Setups the database certificates for chrome
	/// </summary>
	static void SetupDatabaseCertsForChrome();

	/// <summary>
	/// Adds all the certs int he given directory
	/// </summary>
	/// <param name="certsDirectory">certs directory path to be added to certificate database store</param>
	/// <param name="listOfCerts">OUT - List of certs added to the certificate database store</param>
	static void AddCertsToCertificateDatabaseStore(const std::string& certsDirectory, std::vector<std::string>& listOfCerts);

	/// <summary>
	/// Check if NVRAM needs to be cleared.
	/// </summary>
	/// <returns>true if NVRAM needs to be cleared, false otherwise.</returns>
	static bool CheckIfNvramNeedsToBeCleared();

	/// <summary>
	/// Check if PIN is banned
	/// </summary>
	/// <returns>true if PIN is banned, false otherwise.</returns>
	static bool IsBannedPin(const std::string& pin);

	/// <summary>
	/// Check if running in automation mode
	/// </summary>
	/// <returns>true if running in automation mode.</returns>
	static bool IsAutomationMode();

	/// <summary>
	/// Gets the resolution of current device.
	/// </summary>
	/// <param name="width">OUT - resolution width</param>
	/// <param name="height">OUT - resolution height</param>
	static void GetResolution(DWORD &width, DWORD &height);
};

std::string FormatString(LPCSTR pFormat, ...)
	__attribute__((format(printf, 1, 2)));
std::string FormatStringV(LPCSTR pFormat, va_list args);

void PrintTm(struct tm *one, struct tm *two);

template< class T > void DeleteVariable( T*& pVal )
{
    delete pVal;
    pVal = nullptr;
}

template< class T > void DeleteArray( T*& pVal )
{
    delete[] pVal;
    pVal = nullptr;
}

