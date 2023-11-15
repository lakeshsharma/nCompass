#pragma once
// Do not put any additional headers in this file.

///	<summary>
///	String functions.
///	</summary>
namespace String
{
	///	<summary>
	/// Concatenates two strings.
	///	</summary>
	///	<param name="pStr1">The first string. May be nullptr.</param>
	///	<param name="pStr2">The second string. May be nullptr.</param>
	///	<returns>A string made of pStr2 concatenated to pStr1. If both are nullptr,
	///		an empty string is returned.</returns>
	std::string Concat(const char* pStr1, const char* pStr2);

	///	<summary>
	/// Concatenates two strings.
	///	</summary>
	///	<param name="pStr1">The first string. May be nullptr.</param>
	///	<param name="pStr2">The second string. May be nullptr.</param>
	///	<returns>A string made of pStr2 concatenated to pStr1. If both are nullptr,
	///		an empty string is returned.</returns>
	std::string Concat(const char* pStr1, const wchar_t* pStr2);

	///	<summary>
	/// Concatenates two strings.
	///	</summary>
	///	<param name="pStr1">The first string. May be nullptr.</param>
	///	<param name="pStr2">The second string. May be nullptr.</param>
	///	<returns>A string made of pStr2 concatenated to pStr1. If both are nullptr,
	///		an empty string is returned.</returns>
	std::string Concat(const wchar_t* pStr1, const char* pStr2);

	///	<summary>
	/// Concatenates two strings.
	///	</summary>
	///	<param name="pStr1">The first string. May be nullptr.</param>
	///	<param name="pStr2">The second string. May be nullptr.</param>
	///	<returns>A string made of pStr2 concatenated to pStr1. If both are nullptr,
	///		an empty string is returned.</returns>
	std::string Concat(const wchar_t* pStr1, const wchar_t* pStr2);

	///	<summary>
	/// Returns the length of the end-of-line (EOL) characters pointed to by pStr.
	///	</summary>
	///	<param name="pStr">The pointer to the EOL characters.</param>
	///	<returns>The EOL length.</returns>
	///	<remarks>
	/// "\n" will ALWAYS return 1. (i.e. "\n\r" will return one.)
	/// "\r\n" will return 2.
	/// "\r" by itself will return 1.
	/// ("\r\n\n\r" will return 2. Upon incrementing the pointer, it will point to "\n",
	///	which will return 1. Increment the pointer again and it will return 1.)
	///	</remarks>
	int GetEolLen(const char* pStr);

	///	<summary>
	/// Removes all instances of pFind in str.
	///	</summary>
	///	<param name="str">The string to search.</param>
	///	<param name="pFind">The string to find.</param>
	///	<returns>A reference to str.</returns>
	std::string& RemoveAll(std::string& str, const char* pFind);

	///	<summary>
	/// Replaces all instances of pFind with pReplace in str.
	///	</summary>
	///	<param name="str">The string to search.</param>
	///	<param name="pFind">The string to find.</param>
	///	<param name="pReplace">The string to replace. If this is nullptr or zero length,
	///		RemoveAll will be called.</param>
	///	<returns>A reference to str.</returns>
	std::string& ReplaceAll(std::string& str, const char* pFind, const char* pReplace);

	///	<summary>
	/// The maximum length of a uint64_t as a hexadecimal string.
	///	</summary>
	const int MaxLen_Uint64HexStr = 16;

	///	<summary>
	/// Converts a value to hexadecimal characters, appending the result to a string. 
	///	</summary>
	///	<param name="val">The value to convert.</param>
	///	<param name="str">The string to append.</param>
	///	<param name="useUpperCase">If <b>true</b> use uppercase characters.</param>
	///	<param name="padLen">If greater than the length of the resulting string, ASCII
	///		'0's will be pre-pended to this length.</param>
	///	<returns>A reference to str.</returns>
	///	<remarks>
	/// This exists to minimize the use of the CRT and/or the OS.
	///	</remarks>
	std::string& AppendHex(uint64_t val, std::string& str, bool useUpperCase = false, int padLen = 0);

	///	<summary>
	///	Converts a value to hexadecimal characters.
	///	</summary>
	///	<param name="val">The value to convert.</param>
	///	<param name="useUpperCase">If <b>true</b> use uppercase characters, else lower case.</param>
	///	<param name="padLen">The number of hex characters to pad string to left with zeros.</param>
	///	<returns>A hexadecimal string.</returns>
	std::string ToHex(uint64_t val, bool useUpperCase = false, int padLen = 0);

	///	<summary>
	/// Converts data to a hexadecimal string.
	///	</summary>
	///	<param name="_pData">The data which to convert.</param>
	///	<param name="dataLen">The length of the data to convert.</param>
	///	<param name="str">The string to receive the converted data.</param>
	///	<param name="useUpperCase">If <b>true</b> use uppercase characters.</param>
	///	<param name="separator">If non-zero, the character to inserted between each
	///		byte of data.</param>
	///	<returns>A hexadecimal string representing the data.</returns>
	std::string& BinToHex(const void* _pData, size_t dataLen, std::string& str, bool useUpperCase = false, char separator = 0);

	///	<summary>
	/// Converts data to a hexadecimal string.
	///	</summary>
	///	<param name="_pData">The data which to convert.</param>
	///	<param name="dataLen">The length of the data to convert.</param>
	///	<param name="useUpperCase">If <b>true</b> use uppercase characters.</param>
	///	<param name="separator">If non-zero, the character to inserted between each
	///		byte of data.</param>
	///	<returns>A hexadecimal string representing the data.</returns>
	std::string BinToHex(const void* pData, size_t dataLen, bool useUpperCase = false, char separator = 0);

	///	<summary>
	/// Converts data to a hexadecimal string.
	///	</summary>
	///	<param name="_pData">The data which to convert.</param>
	///	<param name="dataLen">The length of the data to convert.</param>
	///	<param name="pBuffer">The buffer to receive the resulting string.</param>
	///	<param name="bufferLen">The length of pBuffer.</param>
	///	<param name="useUpperCase">If <b>true</b> use uppercase characters.</param>
	///	<param name="separator">If non-zero, the character to inserted between each
	///		byte of data.</param>
	///	<returns>A hexadecimal string representing the data.</returns>
	char* BinToHex(const void* pData, size_t dataLen, char* pBuffer, size_t bufferLen, bool useUpperCase = false, char separator = 0);

	///	<summary>
	/// Safely copies a string into a buffer.
	///	</summary>
	///	<param name="pDst">Pointer to the destination buffer.</param>
	///	<param name="pSrc">Pointer to the source string.</param>
	///	<param name="len">Maximum number of characters to copy, including the
	///		terminating null.</param>
	///	<param name="fill">If <b>true</b> and len exceeds length of pSrc, buffer will be
	///		filled with zeros up to len.</param>
	///	<param name="pErr">Optional pointer to a bool that will be set to <b>true</b> if
	///		the buffer would overrun.</param>
	///	<returns>Length of destination string.</returns>
	///	<remarks>
	///	If pDst is nullptr, function will return 0. If pSrc is nullptr and len greater
	///	than zero, *pDst will be set to 0. If len is less than or equal to zero, nothing
	///	will happen.
	///	</remarks>
	size_t SafeCopy(char* pDst, const char* pSrc, size_t len, bool fill = false, bool* pErr = nullptr);

	///	<summary>
	/// Safely copies a string into a buffer.
	///	</summary>
	///	<param name="pDst">Pointer to the destination buffer.</param>
	///	<param name="pSrc">Pointer to the source string.</param>
	///	<param name="len">Maximum number of characters to copy, including the
	///		terminating null.</param>
	///	<param name="fill">If <b>true</b> and len exceeds length of pSrc, buffer will be
	///		filled with zeros up to len.</param>
	///	<param name="pErr">Optional pointer to a bool that will be set to <b>true</b> if
	///		the buffer would overrun.</param>
	///	<returns>Length of destination string.</returns>
	///	<remarks>
	///	If pDst is nullptr, function will return 0. If pSrc is nullptr and len greater
	///	than zero, *pDst will be set to 0. If len is less than or equal to zero, nothing
	///	will happen.
	///	</remarks>
	size_t SafeCopy(wchar_t* pDst, const wchar_t* pSrc, size_t len, bool fill = false, bool* pErr = nullptr);
}
