#pragma once

///	<summary>
/// The default read buffer size.
///	</summary>
///	<remarks>
/// 8k was selected after testing.
///	</remarks>
const UINT DefaultReadBufferSize = 8 * 1024; // 8k was selected after testing

///	<summary>
/// The time-out value, in milliseconds, to use for Internet connection requests.
///	</summary>
///	<remarks>
/// See INTERNET_OPTION_CONNECT_TIMEOUT in
///	https://msdn.microsoft.com/en-us/library/aa385328.aspx for more information.
///	</remarks>
const DWORD DefaultConnectTimeout = 10 * 1000;

///	<summary>
/// The number of times WinINet attempts to resolve and connect to a host.
///	</summary>
///	<remarks>
/// See INTERNET_OPTION_CONNECT_RETRIES in
///	https://msdn.microsoft.com/en-us/library/aa385328.aspx for more information.
///	</remarks>
const DWORD DefaultConnectRetries = 3;

///	<summary>
/// The time-out value, in milliseconds, to receive a response to a request.
///	</summary>
///	<remarks>
/// See INTERNET_OPTION_RECEIVE_TIMEOUT in
///	https://msdn.microsoft.com/en-us/library/aa385328.aspx for more information.
///	</remarks>
const DWORD DefaultReceiveTimeout = 10 * 1000;

typedef int(*progress_callback)(void*, int64_t, int64_t, int64_t, int64_t);

///	<summary>
/// Performs an HTTP Get.
///	</summary>
///	<param name="pSrcFile">The source filename.</param>
///	<param name="pStream">The output stream. If nullptr, nothing will be saved.</param>
///	<param name="statusCode">Status code.</param>
///	<param name="useCert">User cert flag.</param>
///	<param name="timeout">Timeout in seconds.</param>
///	<param name="callback">Callback function for progress.</param>
///	<returns><b>true</b> on success, else <b>false</b>.</returns>
/// <remarks>
/// This will perform periodic WatchDog check ins.
/// </remarks>
/// <exception cref="CInternetException">Thrown if there is a WinInet failure.</exception>
/// <exception cref="CFileException">Thrown if the URL is a file and there is an error opening it.</exception>
/// <exception cref="SentinelCancelException">Thrown if cancel was called. The data members will be empty.</exception>
/// <exception cref="ios_base::failure">Thrown if there is an ostream failure.</exception>
bool HttpGet(
	std::string srcFile,
	std::ostream* pStream,
	long&   statusCode,
	bool useCert = false,
	long timeout = 0,
	progress_callback callback = nullptr);


