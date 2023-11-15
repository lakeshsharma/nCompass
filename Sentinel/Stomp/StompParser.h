#pragma once


#include "StompHelper.h"

namespace Stomp { class StompParserTests; }

namespace Stomp
{
	///	<summary>
	/// The base class used for parsing STOMP frames.
	///	</summary>
	///	<remarks>
	/// This is a base class. It SHALL NEVER contain any data. It is not pure
	/// virtual since it can be used to validate a STOMP packet.
	///	</remarks>
	class Parser
	{
	public:
		///	<summary>
		/// Constructs a Parser object.
		///	</summary>
		Parser();

		///	<summary>
		/// Destructs this Parser instance.
		///	</summary>
		virtual ~Parser();

		/// <summary>
		/// Parses a STOMP <paramref name="pFrame"/> coming from a STOMP server.
		/// </summary>
		/// <param name="pFrame">A pointer to a STOMP frame. This data <b>WILL</b> be
		///		modified.</param>
		/// <param name="frameLen">Length of the frame received, including the
		///		terminating	zero.</param>
		/// <param name="version">The STOMP version to use for parsing.</param>
		/// <param name="headerOnly">Stop parsing after the header.</param>
		/// <param name="pContentLen">An optional pointer to receive the value of the 
		///		content-length header. If a valid pointer is passed, it will be
		///		initially set to ContentLengthMissing. If a content-length header is
		///		encountered and it is a valid number, *<paramref name="pContentLen"/>
		///		will be set to that	value, else it will be set to ContentLengthInvalid.
		///		In accordance with the STOMP standard, the last content-length
		///		encountered will take precedence.</param>
		/// <returns>
		///	Stomp::Success upon successful parsing, else an error code. 
		/// </returns>
		/// <remarks>
		/// <paramref name="pFrame"/> <b>WILL</b> be modified during parsing.
		///	Header keys and values and the body will point to data inside this
		/// memory block.
		///
		/// if <paramref name="headerOnly"/> is <b>false</b>, the <paramref name="pFrame"/>
		/// must be terminated with a NULL character and <paramref name="frameLen"/>
		///	must be the exact length of the frame, including the terminating NULL.
		///	In other words, if <paramref name="pFrame"/>[<paramref name="frameLen"/> - 1]
		///	is not zero, Stomp::InvalidHeader will be returned.
		///
		/// if <paramref name="headerOnly"/> is <b>true</b>, the <paramref name="pFrame"/>
		/// must be terminated with a pair of valid end-of-line characters and 
		/// <paramref name="frameLen"/>	must be the exact length of the header. In other
		///	words, if <paramref name="pFrame"/>[<paramref name="frameLen"/> - 1] and
		/// <paramref name="pFrame"/>[<paramref name="frameLen"/> - 2] are not valid
		/// end-of-lines for the version, Stomp::InvalidFrame will be returned.
		///
		/// As the frame is parsed, if it is valid, <see cref="OnCommand"/> will be called,
		/// followed by <see cref="OnHeader"/> for every header present and finally by
		///	<see cref="OnBody"/> if present. The called method may terminate parsing from
		///	any of these by returning a value other than Frame::Success.
		///
		/// Note that STOMP 1.0 and 1.1 frames are parsed identically. STOMP 1.2
		/// allows lines to be terminated with carriage-return/line-feed (\r\n)
		/// (versus just line-feed (\n), requires that carriage-returns be
		/// escaped in non-CONNECTED headers and <paramref name="version"/>
		/// prohibits trimming whitespace in headers.
		///
		/// The parser deviates from the STOMP standard in four minor ways:
		///
		/// STOMP 1.0 and 1.1 are ambiguous about how spaces are treated after
		/// header keys and before values. This is exacerbated by the examples
		/// which use spaces between the colon and the value. STOMP 1.2 and later
		/// stipulate that keys and values are to be treated literally. For STOMP
		/// 1.0 and 1.1 frames, this parser will trim all tabs and spaces after
		/// header keys and before header values.
		///
		/// Colons are required to be escaped in headers with \c. That would
		/// suggest that any literal colon encountered after the colon delimiter
		/// should result in a fatal error. This parse ignores this and processes
		/// subsequent literal colons as though they are part of the value.
		///
		/// CONNECTED and RECEIPT frames are not supposed to have bodies. This parser
		/// will process any body present for these frames.
		///
		/// The STOMP standard specifies that the content-length header can specify
		/// the length of the body. This is to allow binary data to be safely
		/// transmitted and to help validate the frame. Since the size of the frame
		/// is known, this parser ignored any content-length header; it is up to the
		/// caller to validate this header value if present.
		/// </remarks>
		Err Parse(char* pFrame, size_t frameLen, Version version, bool headerOnly = false, int* pContentLen = nullptr);

		///	<summary>
		/// A virtual method called by the parser when it encounters a valid command.
		///	</summary>
		///	<param name="Command">A valid command from the set: Connected, Message,
		///		Receipt and Error.</param>
		///	<returns>The caller must return Success to continue parsing, else a
		///		non-zero value.</returns>
		///	<remarks>
		/// This will <b>not</b> pass an invalid command.
		///
		/// A non-<i>Stomp::Success</i> return value will cause the Parsing to
		/// cease, and that value will be returned by the Parse method. The
		/// Stomp::Err messages are positive integers, so if custom errors are
		/// desired, either return a negative value or a value greater than
		/// Stomp::__ErrLast__.
		///	</remarks>
		virtual Err OnCommand(RecvCommand::Type command);

		///	<summary>
		/// A virtual method called by the parser when it parses a valid header.
		///	</summary>
		///	<param name="pKey">A NULL terminated string representing a header
		///		key.</param>
		///	<param name="keyLen">The length of <paramref name="pKey"/>, not
		///		including the terminating NULL.</param>
		///	<param name="pValue">A NULL terminated string representing a header
		///		value.</param>
		///	<param name="valueLen">The length of <paramref name="pValue"/>, not
		///		including the terminating NULL.</param>
		///	<returns>The caller must return Success to continue parsing, else a
		///		non-zero value.</returns>
		///	<remarks>
		/// <paramref name="pKey"/> will never be nullptr or point to a zero length
		///	string. Therefore, <paramref name="keyLen"/> will never be zero.
		/// <paramref name="pValue"/> will never be nullptr, but may point to a zero
		///	length string, in which case <paramref name="valueLen"/> will be zero.
		///
		/// A non-<i>Stomp::Success</i> return value will cause the Parsing to
		/// cease, and that value will be returned by the Parse method. The
		/// Stomp::Err messages are positive integers, so if custom errors are
		/// desired, either return a negative value or a value greater than
		/// Stomp::__ErrLast__.
		///
		/// For the curious, <paramref name="keylen"/> and
		/// <paramref name="valuelen"/> are determined during parsing, so there is
		/// no performance impact by including them.
		///	</remarks>
		virtual Err OnHeader(char* pKey, size_t keyLen, char* pValue, size_t valueLen);

		///	<summary>
		/// The virtual method called by the parser when it parses a valid body.
		///	</summary>
		///	<param name="pBody">The body of the frame, if present. This will never
		/// 	be nullptr.</param>
		///	<param name="bodyLen">The number of bytes from the start of body to,
		///		but not including, the frame terminating zero.</param>
		/// <remarks>
		/// The STOMP standard specifies that the content-length header can specify
		/// the length of the body. This is to allow binary data to be safely
		/// transmitted and to help validate the frame. Since the size of the frame
		/// is known, this parser ignored any content-length header; it is up to the
		/// caller to validate this header value if present.
		///
		/// CONNECTED and RECEIPT frames are not supposed to have bodies. However, this
		/// method will be called if these frames have a body.
		///
		/// Any value returned here will, in turn, be returned by the Parse method.
		/// The Stomp::Err messages are positive integers, so if custom errors are
		/// desired, either return a negative value or a value greater than
		/// Stomp::__ErrLast__.
		/// </remarks>
		virtual Err OnBody(char* pBody, size_t bodyLen);

	private:
		friend class Stomp::StompParserTests;

		/// <summary>
		/// Determines and fixes a header, possibly resolving valid escape sequences.
		/// </summary>
		/// <remarks>
		///	The commands and headers are encoded in UTF-8. All frames except the CONNECT and CONNECTED 
		///	are required to escape any carriage return (for STOMP 1.2), line feed or colon characters found in the resulting UTF-8 strings.
		///	
		///	C-style string literal escapes are used to encode any carriage return, line feed or colon that are
		///	found within the UTF-8 encoded headers.When decoding frame headers, the following transformations
		///	MUST be applied :
		///	
        /// \r translates to carriage return (0x0d) \ n translates to
        /// line feed (0x09) \ c translates to : \ \ Undefined escape
        /// sequences such as \t MUST be treated as a fatal protocol error.
		///	Conversely when encoding frame headers, the reverse transformation MUST be applied.
		///	
		/// STOMP 1.0 and 1.1 are ambiguous about how spaces are treated after
		/// header keys and before values. This is exacerbated by the examples
		/// which use spaces between the colon and the value. STOMP 1.2 and later
		/// stipulate that keys and values are to be treated literally. For STOMP
		/// 1.0 and 1.1 frames, this parser will trim all tabs and spaces after
		/// header keys and before header values.
		///
		/// Colons are required to be escaped in headers with \c. That would
		/// suggest that any literal colon encountered after the colon delimiter
		/// should result in a fatal error. This parse ignores this and processes
		/// subsequent literal colons as though they are part of the value.
		///	</remarks>
		struct ParseHeader
		{
			char*   pKey;
			size_t  keyLen;

			char*   pVal;
			size_t  valLen;

			Version version;

			ParseHeader(Version version);
			virtual ~ParseHeader();

			virtual Err Parse(char*& pFrame);

			void Reset(char* pFrame);
		};

		// CONNECTED headers do not escape characters
		struct ParseHeaderConnected : public ParseHeader
		{
			ParseHeaderConnected(Version version);

			virtual Err Parse(char*& pFrame) override;
		};

		static int GetEolLen(const char* pStr, Version version);
		static RecvCommand::Type DetermineCommand(char*& pStr, Version version);
		static int GetContentLengthVal(const char* pStr);
		static bool VerifyEndOfHeader(const char* pEnd, Version version);

		Parser(const Parser&);
		void operator=(const Parser&);
	};
} // Stomp namespace



