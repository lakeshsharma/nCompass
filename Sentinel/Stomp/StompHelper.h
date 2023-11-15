#pragma once


#include "EnumMap.h"

namespace Stomp
{
	///	<summary>
	/// An enumeration describing the STOMP version.
	///	</summary>
	enum Version
	{
		/// <summary>STOMP version 1.0.</summary>
		V1_0,
		/// <summary>STOMP version 1.1.</summary>
		V1_1,
		/// <summary>STOMP version 1.2.</summary>
		V1_2,
		/// <summary>Version is invalid.</summary>
		Invalid_Version = -1
	};

	///	<summary>
	/// Returns a pointer to a string representing the version enumeration.
	///	</summary>
	///	<param name="version">A version enumeration.</param>
	///	<returns>
	/// A string representing the <paramref name="version"/>, or "VER_Invalid" if
	/// <paramref name="version"/> is invalid.
	/// </returns>
	/// <remarks>
	/// This is  primarily used for logging.
	/// </remarks>
	const char* GetVersionStr(Version version);
	const wchar_t* GetVersionStrW(Version version);

	///	<summary>
	/// Returns largest valid version in a string.
	///	</summary>
	///	<param name="version">A string with one or more comma separated versions.</param>
	///	<returns>
	/// If <paramref name="pVersion"/> is nullptr, empty or contains no valid versions,
	//  V1_0 will be returned, else the largest valid value found.
	/// version of <paramref name="pVersion"/>.
	/// </returns>
	/// <remarks>
	/// STOMP 1.1 requires that the CONNECT frame include the "accept-version" header,
	/// which is a comma separated list of protocol versions, such as "1.0,1.1".
	/// </remarks>
	Version GetVersion(const char* pVersion);

#define THE_SMALLEST_STOMP_FRAME "ERROR\n\n\n"

	///	<summary>The minimum STOMP frame length allowed.</summary>
	const size_t MinFrameLen = _countof(THE_SMALLEST_STOMP_FRAME); // Includes the terminating null.

	///	<summary>The minimum STOMP header length allowed.</summary>
	const size_t MinHeaderLen = _countof(THE_SMALLEST_STOMP_FRAME) - 1; // Removes the terminating null.

	///	<summary>The maximum STOMP frame length allowed.</summary>
	/// <remarks>
	/// Data larger than this should probably not be sent by STOMP.
	/// </remarks>
	const size_t MaxFrameLen = (1024 * 64);

	///	<summary>The maximum STOMP body length allowed.</summary>
	const size_t MaxBodyLen = (MaxFrameLen - MinFrameLen);

	///	<summary>
	/// An enumeration of the errors which may be produced by the STOMP classes.
	///	</summary>
	///	<remarks>
	/// All STOMP errors should be added to this enumeration. This is named 'Err' to
	/// prevent a clash with Command::Error.
	///	</remarks>
	enum Err : int
	{
		///	<summary>The operation was successful.</summary>
		Success = 0,

		///	<summary>The frame pointer was nullptr.</summary>
		NullFrame = 1,
		///	<summary>The frame was zero characters long.</summary>
		ZeroLength = 2,
		///	<summary>The frame is exceeds MaxFrameLen or the passed frame length is invalid.</summary>
		InvalidLength = 3,
		///	<summary>The frame is invalid.</summary>
		InvalidFrame = 4,
		///	<summary>The command is invalid.</summary>
		InvalidCommand = 5,
		///	<summary>A header is invalid.</summary>
		InvalidHeader = 6,
		///	<summary>A header key is invalid.</summary>
		InvalidHeaderKey = 7,
		///	<summary>A header key length is invalid.</summary>
		InvalidHeaderKeyLen = 8,
		///	<summary>A header value is invalid.</summary>
		InvalidHeaderValue = 9,
		///	<summary>A header value length is invalid.</summary>
		InvalidHeaderValueLen = 10,
		///	<summary>The body is invalid.</summary>
		InvalidBody = 11,
		///	<summary>The body length is invalid.</summary>
		InvalidBodyLen = 12,

		///	<summary>The header content-length does not match the body length.</summary>
		Invalid_content_length = 20,
		///	<summary>The OneLink header content-type is invalid.</summary>
		Invalid_content_type = 21,

		///	<summary>Generic return to halt processing from virtual "On" functions.</summary>
		StopParsing = 64,

		/// <summary>The number of the last enumeration; used internally.</summary>
		__ErrLast__
	};

	typedef SentinelExceptionT<Err> StompException;

	///	<summary>
	/// When pContentLen is used in Parser::Parse and there is no content-length header
	/// value, *pContentLen will be set to this value.
	///	</summary>
	const int ContentLengthMissing = -1;

	///	<summary>
	/// When pContentLen is used in Parser::Parse and there is a content-length header
	/// value, but it is invalid, *pContentLen will be set to this value.
	///	</summary>
	const int ContentLengthInvalid = -2;

	namespace RecvCommand
	{
		enum Type
		{
			Connected,
			Message,
			Receipt,
			Error,
			Invalid = -1
		};

		extern EnumMap<Type> emap;
	}

	///	<summary>
	///	STOMP header keys.
	///	</summary>
	///	<remarks>
	/// The following are STOMP header keys, with notes as to whether they are required
	///	and by what version.
	///	</remarks>
	namespace Header
	{
		// Connected
		extern const char* pVersion; // Required as of 1.1
		extern const char* pHeartBeat;
		extern const char* pSession;
		extern const char* pServer;

		// Message
		extern const char* pDestination; // Required
		extern const char* pSubscription; // Required as of 1.1
		extern const char* pContentLength;
		extern const char* pContentType;
		extern const char* pMessageId;

		// Receipt
		extern const char* pReceiptId; // Required

		// Error (pContentLength and pContentType are also optional in general, but NOT for OneLink)
		extern const char* pMessage;

		enum Type
		{
			ContentLength,
			ContentType,
			Destination, // Required
			HeartBeat,
			Message,
			MessageId,
			ReceiptId, // Required
			Server,
			Session,
			Subscription, // Required as of 1.1
			Version, // Required as of 1.1
			Invalid = -1
		};

		extern EnumMap<Type> emap;
	}

	namespace SendCommand
	{
		enum Type
		{
			Connect,
			Disconnect,
			Subscribe,
			Unsubscribe,
			Send,
			Ack,
			Invalid = -1
		};

		///	<summary>
		/// Returns the string associated with the command.
		///	</summary>
		///	<param name="command">The command.</param>
		///	<param name="throwOnError">If <b>true</b> and there is an error, throw an exception.</param>
		///	<returns>The string associated with the enumerated value. If the enumerated
		///		value is invalid and throwOnError is false, nullptr will be returned, else a
		///		StompException<Err> will be thrown.</returns>
		/// <exception cref="StompException">Thrown if command is invalid and throwOnError is true.</exception>
		const char* At(Type command, bool throwOnError);

		///	<summary>
		/// Returns the string associated with the command.
		///	</summary>
		///	<param name="command">The command.</param>
		///	<returns>The string associated with the enumerated value. If the enumerated
		///		value is invalid a StompException<Err> will be thrown.</returns>
		/// <exception cref="StompException">Thrown if command is invalid.</exception>
		const char* AtEx(Type command);
	}

    /*
	namespace SendHeader
	{
		//__declspec(selectany) 
        const char* pReceipt = "receipt";

		// SUBSCRIBE
		//__declspec(selectany) 
        const char* pAck = "ack";
		//__declspec(selectany) 
        const char* pId = "id";
	}
    */
}


