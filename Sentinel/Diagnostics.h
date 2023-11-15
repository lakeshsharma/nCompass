#pragma once
#include <sys/file.h>
#include "DiagnosticsPersistance.h"
#include "Hardware/WatchDog.h"
#include "TicketPBTMaxAmount.h"
#include "TiltsDef.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class DiagnosticsUnitTests; }

const DWORD SLEEP_PER_RETRYLOOP = 100;
const DWORD RETRY_LOOPS = 50;

class CDiagnostics
{
public:	
	/// <summary>
    /// Gets a thread callstack and writes to string buffer passed.
    /// </summary>
    /// <param name="name">Name of the thread.</param>
    /// <param name="buffer">OUT - buffer to receive callstack.</param>
    /// <param name="bufferSize">Size of buffer passed.</param>
    /// <returns>Remaining free characters of buffer.</returns>
    /// <remarks>
    /// The 'thread' handle needs STANDARD_RIGHTS_READ privilages.
    /// </remarks>
    static size_t ReportThreadCallstackToBuffer(const std::string& name, char* buffer, size_t bufferSize);

    /// <summary>
    /// Write a callstack buffer to a file in as safe a way as possilbe.
    /// </summary>
    /// <param name="path">File path.</param>
    /// <param name="name">Name of the thread.</param>
	/// <param name="exceptionType">exception type.</param>
    /// <param name="callstack">Buffer with callstack.</param>
    /// <returns>Number of bytes written to file.</returns>
    static int ReportThreadCallstackToFile(const std::string &path, const std::string &name, const std::string& exceptionType, const std::string& callstack);

	/// <summary>
	/// Write exception information to a file.
	/// </summary>
	/// <param name="path">File path.</param>
	/// <param name="name">Name of the thread.</param>
	/// <param name="exceptionType">exception type.</param>
	/// <param name="callstack">Buffer with callstack.</param>
	/// <param name="threadTimeOut">thread timeOut flag </param>
	static void WriteExceptionToFile(const std::string &path, const std::string &name, const std::string& exceptionType, const std::string& callstack, bool threadTimeOut);

	/// <summary>
	/// Append thread callstack information to a string.
	/// </summary>
	/// <param name="exceptionString">exception string</param>
	/// <param name="name">Name of the thread.</param>
	/// <param name="exceptionType">exception type.</param>
	/// <param name="callstack">Buffer with callstack.</param>
	static void AppendThreadCallstackToString(std::string &exceptionString, const std::string &name, const std::string& exceptionType, const std::string& callstack);

	/// <summary>
	/// Append secondary threads header to string
	/// </summary>
	/// <param name="exceptionString">exception string</param>
	static void AppendSecondaryThreadsHeaderToString(std::string &exceptionString);

	/// <summary>
	/// Append secondary threads footer to string
	/// </summary>
	/// <param name="exceptionString">exception string</param>
	static void AppendSecondaryThreadsFooterToString(std::string &exceptionString);

private:
	static void AppendExceptionHeaderToString(std::string &exceptionString);
	static void AppendExceptionFooterToString(std::string &exceptionString);
	static void AppendThreadCallstackToStringInner(std::string &exceptionString, bool crashed, const std::string &thread, const std::string& exceptionType, const std::string& callstack);
	static void AppendThreadsHeaderToString(std::string &exceptionString);
	static void AppendThreadsFooterToString(std::string &exceptionString);
	static int ReportRebootToFileInner(const std::string& path, TiltCode code);
	static int ReportErrorToFileInner(const std::string &path, const std::string& message);
    static void WriteStringToFile(const std::string &path, const std::string &stringToWrite);

public:
	/// <summary>
	/// Write a Reboot Tilt code to a file in as safe a way as possilbe.
	/// </summary>
	/// <param name="path">File path.</param>
	/// <param name="code">Reboot Tilt code.</param>
	/// <param name="identifier">Identifier used for logging.</param>
	/// <returns>Number of bytes written to file.</returns>
	static int ReportRebootToFile(const std::string& path, TiltCode code, const std::string &identifier);

	/// <summary>
	/// Writean error to a file in as safe a way as possible.
	/// </summary>
	/// <param name="path">The path.</param>
	/// <param name="errorMessage">The error message.</param>
	/// <param name="identifier">The identifier for logging.</param>
	/// <returns>Number of bytes written to file.</returns>
	static int ReportErrorToFile(const std::string& path, const std::string &errorMessage, const std::string &identifier);

	/// <summary>
	/// Reports the PBT/ticket diagnostic info to file.
	/// </summary>
	/// <param name="path">File path.</param>
	/// <param name="errorMessage">The error message.</param>
	/// <param name="objectString">The string representation of PBT or Ticket object.</param>
	/// <param name="tilt">IN - The diagnostic tilt code.</param>
	/// <returns>Number of bytes written to file.</returns>
	static int ReportPBTTicketDiagnosticToFile(const std::string& path, const std::string& errorMessage, 
		const std::string& objectString, TiltCode tilt);

	/// <summary>
	/// Reports diagnostic info for a bad Poller Protocol packet (message/transaction) created for sending, or already sent.
	/// The diagnostic info is written to text file(s).
	/// </summary>
	/// <param name="path">IN - The file path to the text file.</param>
	/// <param name="timeStamp">IN - Timestamp for the diagnostic info.</param>
	/// <param name="tiltCode">IN - The diagnostic tilt code.</param>
	/// <param name="reason">IN - The reason the transaction is not okay</param>
	/// <param name="message">IN - The raw message buffer for the transation.</param>
	/// <param name="messageLength">IN - The length of the message buffer data to report.</param>
	/// <returns>Number of bytes written to the text file.</returns>
	static int ReportBadPollerProtocolSendPacketToTextFile(
        const std::string& path,
		const std::string& timeStamp,
		TiltCode tiltCode,
		const std::string& reason,
		const BYTE *message,
		WORD messageLength);

	/// <summary>
	/// Write bad NVRAM sprinkle values to error file.
	/// </summary>
	/// <param name="path">File path.</param>
    /// <param name="badSprinkleValues">NVRAM sprinkle 
    /// values read</param> 
	/// <returns>Number of bytes written to file.</returns>
	static int ReportNVRAMSprinklesToFile(const std::string& path, std::vector<BYTE> &badSprinkleValues);

	/// <summary>
	/// Reports diagnostic info for a bad Poller Protocol packet (message/transaction) created for sending, or already sent.
	/// The diagnostic info is written to .SDF binary format Sentinel debug file(s).
	/// </summary>
	/// <param name="path">IN - The file path to the log file.</param>
	/// <param name="message">IN - The raw message buffer for the transation.</param>
	/// <param name="messageLength">IN - The length of the message buffer data to report.</param>
	/// <returns>Number of bytes written to the log file.</returns>
	static int ReportBadPollerProtocolSendPacketToLogFile(
        const std::string& path,
		const BYTE *message,
		WORD messageLength);

	/// <summary>
	/// Reports diagnostic info for a bad Poller Protocol packet (message/transaction) created for sending, or already sent.
	/// The diagnostic info is written to the default file(s).
	/// </summary>
	/// <param name="tiltCode">IN - The diagnostic tilt code.</param>
	/// <param name="reason">IN - The reason the transaction is not okay</param>
	/// <param name="message">IN - The raw message buffer for the transation.</param>
	/// <param name="messageLength">IN - The length of the message buffer data to report.</param>
	/// <returns>Number of bytes written to the default text file.</returns>
	static int ReportBadPollerProtocolSendPacket(TiltCode tiltCode,
		const std::string& reason,
		const BYTE *message,
		WORD messageLength);

	/// <summary>
	/// Constructor
	/// </summary>
	static CDiagnostics& Instance();

	/// <summary>
	/// Delete singleton and make sure 'simulation' has ended.
	/// </summary>
	static void CleanupInstance();

	/// <summary>
	/// Access to the Reboot Tilt Code
	/// </summary>
	/// <returns>Returns the Reboot Tilt Code stored.</returns>
	TiltCode GetRebootTiltCode() const;

	/// <summary>
	/// Stores the Reboot Tilt Code for use on startup.
	/// </summary>
	/// <param name="code">Reboot Tilt Code to be stored.</param>
	/// <param name="identifier">Logging identifer.</param>
	/// <remarks>
	/// Handles what diagnostics used for Reboot Tilt Codes before shutdown: logging, error.txt, etc.
	/// </remarks>
	void PersistReboot(TiltCode code, const std::string &identifier, DiagnosticsPersistFlag flag = DIAGNOSTICS_PERSIST_BOTH);

	/// <summary>
	/// Clear the Reboot Tilt Code.
	/// </summary>
	void ClearReboot();

	/// <summary>
	/// Gets the counter that increments when a diagnostic file is updated.
	/// </summary>
	/// <returns>The counter that increments when a diagnostic file is updated.</returns>
	static DWORD GetFileUpdateCount();

	/// <summary>
	/// Attempts to read bufferSize bytes from end of the file.
	/// </summary>
	/// <param name="buffer">OUT - The buffer.</param>
	/// <param name="bufferSize">IN - Size of the buffer.</param>
	/// <param name="bytesRead">OUT - The bytes read.</param>
	/// <param name="fileSize">OUT - Size of the file.</param>
	/// <returns>true if the file was read from, false otherwise.</returns>
	/// <remarks>
	/// Uses a istream-compatible instance to improve testability.
	/// Minimizing use of stack (function calls) and memory management is not critical here, unlike routines that log crash information.
	/// </remarks>
	static BOOL ReadEndOfFile(const std::shared_ptr<std::istream> &file, char *buffer, UINT bufferSize, UINT &bytesRead, ULONGLONG &fileSize);

	/// <summary>
	/// Gets the maximum ticket amount.
	/// </summary>
	/// <returns>Maximum ticket amount.</returns>
	long GetMaxTicketAmount();

	/// <summary>
	/// Gets the maximum PBT amount.
	/// </summary>
	/// <returns>Maximum PBT amount.</returns>
	long GetMaxPBTAmount();

private:
    static const DWORD MAXIMUM_BAD_MESSAGE_LENGTH = 2048; // Truncate to this length when reporting, if needed.
    static const long int MAX_LOG_FILE_SIZE = (16 * 1024); // Truncate on next open if this length is exceeded.

	static CDiagnostics* m_instance;
	static std::recursive_mutex m_cs;

	CDiagnostics();
	virtual ~CDiagnostics();

	CDiagnosticsPersistance m_config;
	CTicketPBTMaxAmount *m_ticketPbtMaxAmount;


	// A critical section is a simple user-memory structure.
	// Using a critical section to prevent file contention is appropriate even when reporting call stacks.
	static std::recursive_mutex  m_csFileAccess;

	// Status indicating if one or more diagnostic files have been updated.  Increments for each change.
	// Use inline interlocked increment "functions" when updating to ensure correct results.
	static volatile std::atomic_long m_fileUpdateCount;

	static const DWORD FILE_FLUSH_WAIT = 30000;

	static void OpenLogFile(FILE *&stream, const std::string& path);
	static void CloseLogFile(FILE *stream);

	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::DiagnosticsUnitTests;
};

#ifdef LEAK_CHECK
extern int live_pointers;
extern int malloc_seq;
void LockHeapAndDisableTracking();
void UnlockHeapAndReenableTracking();
void InitMemoryLeakChecking();
void ShowMemoryLeakInfo();
void emit_backtrace_here();
#endif
