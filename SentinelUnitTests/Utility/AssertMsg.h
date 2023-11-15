#pragma once

namespace AssertMsg
{
	///	<summary>
	/// Formats an assert message.
	///	</summary>
	///	<param name="pBuffer">Storage location for the output.</param>
	///	<param name="bufferLen">The size in characters of the storage location. Must be greater than 7.</param>
	///	<param name="lineNumber">The lineNum. Formatted only if not negative.</param>
	///	<param name="pFieldName">The field name. Formatted if not nullptr.</param>
	///	<param name="pFormat">Format specification.</param>
	///	<param name="args">A va_list of arguments.</param>
	///	<returns>A pointer to the string within the formatted data.</returns>
	///	<remarks>
	/// An assert message is formatted roughly according to the following pattern:
	///
	///		[Line lineNumber][: pFieldName][: pFormat]
	///
	/// If all the preceding parameters are not used, the leading colon and space will not be used.
	///
	/// If a non-thrown error occurs, the buffer is set to zero length and a
	/// pointer to it returned.
	///
	/// This will never return a nullptr; if pFormat is nullptr, an empty
	/// string will be returned.
	///	</remarks>
	const wchar_t* FormatV(wchar_t* pBuffer, size_t bufferLen, int lineNum, const wchar_t* pFieldName, const wchar_t* pFormat, va_list args);

	///	<summary>
	/// Formats an assert message.
	///	</summary>
	///	<param name="pBuffer">Storage location for the output.</param>
	///	<param name="bufferLen">The size in characters of the storage location. Must
	///		be greater than 7.</param>
	///	<param name="lineNumber">The lineNum. Formatted only if not negative.</param>
	///	<param name="pFieldName">The field name. Formatted if not nullptr.</param>
	///	<param name="pFormat">Format specification.</param>
	///	<param name="...">A variable of arguments.</param>
	///	<returns>A pointer to the formatted data.</returns>
	///	<remarks>
	/// An assert message is formatted roughly according to the following pattern:
	///
	///		[Line lineNumber][: pFieldName][: pFormat]
	///
	/// If all the preceding parameters are not used, the leading colon and space
	/// will not be used.
	///
	/// If a non-thrown error occurs, the buffer is set to zero length and a
	/// pointer to it returned.
	///
	/// This will never return a nullptr; if pFormat is nullptr, an empty
	/// string will be returned.
	///
	/// The default argument of nullptr for pFormat followed by the ellipses is legal
	/// C++ and supported at least of Visual Studio 2010.
	///	</remarks>
	const wchar_t* Format(wchar_t* pBuffer, size_t bufferLen, int lineNum, const wchar_t* pFieldName = nullptr, const wchar_t* pFormat = nullptr, ...);

	///	<summary>
	/// Formats an assert message.
	///	<param name="lineNumber">The line number. Formatted only if not negative.</param>
	///	<param name="pFieldName">The field name. Formatted if not nullptr.</param>
	///	<param name="pFormat">Format specification.</param>
	///	<param name="args">A va_list of arguments.</param>
	///	<returns>A pointer to the formatted data.</returns>
	///	<remarks>
	/// Same as previous Format functions, but uses a global 512 byte buffer, which is
	///	overwritten with each call. This makes it non-thread safe.
	///
	/// The internal buffer is 512 characters. vswprintf_is used to format
	/// the string. Exceptions are intentionally not
	/// caught.
	/// </remarks>
	const wchar_t* FormatV(int lineNum, const wchar_t* pFieldName, const wchar_t* pFormat, va_list args);

	///	<summary>
	/// Formats an assert message.
	///	<param name="lineNumber">The line number. Formatted only if not negative.</param>
	///	<param name="pFieldName">The field name. Formatted if not nullptr.</param>
	///	<param name="pFormat">Format specification.</param>
	///	<param name="...">A variable of arguments.</param>
	///	<returns>A pointer to the formatted data.</returns>
	///	<remarks>
	/// Same as previous Format functions, but uses a global 512 byte buffer, which is
	///	overwritten with each call. This makes it non-thread safe.
	///
	/// The internal buffer is 512 characters. vswprintf is used to format
	/// the string. Exceptions are intentionally not
	/// caught.
	/// </remarks>
	const wchar_t* Format(int lineNum, const wchar_t* pFieldName = nullptr, const wchar_t* pFormat = nullptr, ...);

	///	<summary>
	/// Write formatted data to a string.
	///	</summary>
	///	<param name="pFormat">Format specification.</param>
	///	<returns>A pointer to the formatted data</returns>
	///	<remarks>
	/// This uses a global 512 byte buffer which is shared with the above
	/// and is not thread safe. Every call will overwrite the data in the buffer.
	/// This will never return a nullptr; if pFormat is nullptr, an empty string
	/// will be returned.
	///
	/// The internal buffer is 512 characters. vswprintf is used to format
	/// the string. Exceptions are intentionally not
	/// caught.
	///
	/// If a non-thrown error occurs, the buffer is set to zero length and a
	/// pointer to it returned.
	///
	/// For the curious, this is called FormatOnly to prevent it being called
	/// if Format is called with some incorrect parameters.
	///	</remarks>
	const wchar_t* FormatOnly(const wchar_t* pFormat, ...);

	///	<summary>
	/// Write formatted data to a string.
	///	</summary>
	///	<param name="pFormat">Format specification.</param>
	///	<returns>A pointer to the formatted data</returns>
	///	<remarks>
	/// This uses a global 512 byte buffer which is shared with the above
	/// and is not thread safe. Every call will overwrite the data in the buffer.
	/// This will never return a nullptr; if pFormat is nullptr, an empty string
	/// will be returned.
	///
	/// The internal buffer is 512 characters. vsnprintf is used to format
	/// the string. Exceptions are intentionally not
	/// caught.
	///
	/// If a non-thrown error occurs, the buffer is set to zero length and a
	/// pointer to it returned.
	///
	/// For the curious, this is called FormatOnly to prevent it being called
	/// if Format is called with some incorrect parameters.
	///	</remarks>
	const char* FormatOnly(const char* pFormat, ...);

	static const size_t AssertMsgBufferLen = 512;
}
