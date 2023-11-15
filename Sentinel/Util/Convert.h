#pragma once
// Do not put any additional headers in this file.

///	<summary>
///	Conversion functions.
///	</summary>
namespace Convert
{
	const unsigned BaseTen = 10U;

	///	<summary>
	/// Convert a UTF-16 string to a UTF-8 string.
	///	</summary>
	///	<param name="pStr">A pointer to the UTF-16 string.</param>
	///	<returns>The converted string.</returns>
	std::string WStrToUtf8(const wchar_t* pStr);

	///	<summary>
	/// Convert a UTF-16 string to a UTF-8 string.
	///	</summary>
	///	<param name="str">A reference to a wstring.</param>
	///	<returns>The converted string.</returns>
	std::string WStrToUtf8(const std::wstring& str);

	///	<summary>
	/// Convert a UTF-16 string to a UTF-8 string.
	///	</summary>
	///	<param name="pStr">A pointer to the UTF-16 string.</param>
	///	<param name="length">The length of the string.</param>
	///	<returns>The converted string.</returns>
	std::string WStrToUtf8(const wchar_t* pStr, size_t length);

	///	<summary>
	/// Convert a UTF-8 string to a wstring (UTF-16 on Windows).
	///	</summary>
	///	<param name="pStr">A pointer to the UTF-8 string.</param>
	///	<returns>The converted string.</returns>
	std::wstring Utf8ToWStr(const char* pStr);

	///	<summary>
	/// Convert a UTF-8 string to a wstring (UTF-16 on Windows).
	///	</summary>
	///	<param name="str">A reference to a string.</param>
	///	<returns>The converted string.</returns>
	std::wstring Utf8ToWStr(const std::string& str);

	///	<summary>
	/// Convert a UTF-8 string to a wstring (UTF-16 on Windows).
	///	</summary>
	///	<param name="pStr">A pointer to the UTF-8 string.</param>
	///	<param name="length">The length of the string.</param>
	///	<returns>The converted string.</returns>
	std::wstring Utf8ToWStr(const char* pStr, size_t length);

	///	<summary>
	/// Convert a UTF-8 string to a string .
	///	</summary>
	///	<param name="pStr">A pointer to the UTF-8 string.</param>
	///	<returns>The converted string.</returns>
	std::string Utf8ToString(const char* pStr);

#ifdef _MFC_VER
	///	<summary>
	/// Convert a UTF-8 string to a CStringW (UTF-16).
	///	</summary>
	///	<param name="pStr">A pointer to the UTF-8 string.</param>
	///	<returns>The converted string.</returns>
	CStringW Utf8ToCString(const char* pStr);

	///	<summary>
	/// Convert a UTF-8 string to a CStringW (UTF-16).
	///	</summary>
	///	<param name="str">A reference to a string.</param>
	///	<returns>The converted string.</returns>
	CStringW Utf8ToCString(const std::string& str);

	///	<summary>
	/// Convert a UTF-8 string to a CStringW (UTF-16).
	///	</summary>
	///	<param name="pStr">A pointer to the UTF-8 string.</param>
	///	<param name="length">The length of the string.</param>
	///	<returns>The converted string.</returns>
	CStringW Utf8ToCString(const char* pStr, size_t length);
#endif

	///	<summary>
	///	Converts a string to a uint64_t.
	///	</summary>
	///	<param name="pStr">The string to convert</param>
	///	<param name="val">The value calculated until the error.</param>
	///	<param name="pEnd">Optional pointer to character which ends scan.</param>
	///	<returns><b>true</b> if conversion was successful, else <b>false</b>.</returns>
	bool ToUint64(const char* pStr, uint64_t& val, const char** pEnd = nullptr);

	///	<summary>
	///	Converts a string to an int64_t.
	///	</summary>
	///	<param name="pStr">The string to convert</param>
	///	<param name="val">The value calculated until the error.</param>
	///	<param name="pEnd">Optional pointer to character which ends scan.</param>
	///	<returns><b>true</b> if conversion was successful, else <b>false</b>.</returns>
	bool ToInt64(const char* pStr, int64_t& val, const char** pEnd = nullptr);

	///	<summary>
	///	Converts a string to a uint32_t.
	///	</summary>
	///	<param name="pStr">The string to convert</param>
	///	<param name="val">The value calculated until the error.</param>
	///	<param name="pEnd">Optional pointer to character which ends scan.</param>
	///	<returns><b>true</b> if conversion was successful, else <b>false</b>.</returns>
	bool ToUint32(const char* pStr, uint32_t& val, const char** pEnd = nullptr);

	///	<summary>
	///	Converts a string to an int32_t.
	///	</summary>
	///	<param name="pStr">The string to convert</param>
	///	<param name="val">The value calculated until the error.</param>
	///	<param name="pEnd">Optional pointer to character which ends scan.</param>
	///	<returns><b>true</b> if conversion was successful, else <b>false</b>.</returns>
	bool ToInt32(const char* pStr, int32_t& val, const char** pEnd = nullptr);
}

