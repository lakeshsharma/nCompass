#pragma once


#include "StompParser.h"
#include "OptionalVal.h"
#include "Hash.h"

namespace Stomp{class StompFrameTests;}

namespace Stomp
{

	/// <summary>
	/// Parses STOMP frames received from a server.
	/// </summary>
	/// <remarks>
	/// This class only parses frames, it does not act on them and enforces only the
	///	minimum rules.
	/// 
	/// See http://stomp.github.io/index.html for the full standard documentation.
	///
	/// See Parse method for documentation on those areas where we depart from the
	///	standard.
	/// </remarks>
	class Frame : protected Parser
	{
    public:
        //friend class SentinelNativeUnitTests::StompFrameTests;
		friend class StompFrameTests;
	protected:
		typedef std::map<const char*, const char*, str_less_than> HeaderMap;

    protected:
		std::vector<char>	m_frame;
		RecvCommand::Type	m_command;
		HeaderMap			m_headers;
		char*				m_pBody;
		size_t				m_bodyLen;
		OptionalVal<size_t> m_contentLen;

	public:
		/// <summary>
		/// <see cref=Frame"/> constructor.
		/// </summary>
		/// <param name="reserve">The number of bytes to reserve with the internal
		///		buffer.</param>
		Frame(size_t reserve = 0);

		/// <summary>
		/// <see cref=Frame"/> destructor.
		/// </summary>
		virtual ~Frame();

		/// <summary>
		/// Resets this instance except for the frame buffer.
		/// </summary>
		/// <param name="resizeToZero">If <b>true</b>, the size of the internal buffer
		///		will be set to zero, else it will left as is.</param>
		/// <remarks>
		/// Removes all elements from header map and optionally resizes the internal buffer
		///	to zero while retaining it's capacity.
		/// 
		/// The Parse method calls this with <b>false</b> as its first operation.
		/// </remarks>
		virtual void Reset(bool resizeToZero = true);

		/// <summary>
		/// Empties this instance.
		/// </summary>
		/// <remarks>
		/// Removes all elements from header map and frees all memory of the internal buffer,
		///	leaving memory footprint at a minimum.
		/// </remarks>
		virtual void Empty();

		/// <summary>
		/// Returns the current size of the internal buffer.
		/// </summary>
		/// <returns>The current size of the internal buffer.</returns>
		size_t GetBufferLen() const;

		/// <summary>
		/// Returns the current capacity of the internal buffer.
		/// </summary>
		/// <returns>The current capacity of the internal buffer.</returns>
		/// <remarks>
		///	The capacity will always be equal to or greater than the size of the
		///	internal buffer.
		///	</remarks>
		size_t GetBufferCapacity() const;

		///	<summary>
		///	Allocates the internal buffer and returns a pointer to it.
		///	</summary>
		/// <param name="len">The length to allocate.</param>
		///	<returns>A pointer to the allocated buffer.</returns>
		/// <remarks>
		/// If len == 0 or len > MaxFrameLen, nullptr will be returned.
		///
		/// If <paramref name="len"/> is greater than the current capacity of the
		///	internal buffer, memory will be reallocated.
		///
		/// This method calls <see cref=Reset"/>(false) as its first operation.
		/// </returns>
		char* GetBuffer(size_t len);

		///	<summary>
		///	Sets the size of the internal buffer.
		///	</summary>
		/// <param name="len">The length to set.</returns>
		/// <returns>Stomp::Success upon successful release, else an error code.</returns>
		/// <remarks>
		/// This method will not change the capacity of the internal buffer and thus will not
		///	cause memory to be reallocated.
		/// </remarks>
		Err ReleaseBuffer(size_t len);

		/// <summary>
		/// Parses a STOMP frame coming from a server according to the <paramref name="version"/>.
		/// </summary>
		/// <param name="version">The STOMP version to use for parsing.</param>
		/// <returns>
		///	Stomp::Success upon successful parsing, else an error code.
		/// </returns>
		/// <remarks>
		/// The internal buffer <b>WILL</b> be modified. Header keys and values and the
		///	body will point to data inside this buffer.
		///
		/// The data must be NULL terminated and be the exact length of the current
		///	frame, including the terminating NULL. In other words, if Buffer[len - 1]
		///	is not zero, InvalidFrame will be returned.
		///
		/// On error, the data parsed to this point is left intact.
		///
		/// Note that STOMP 1.0 and 1.1 frames are parsed identically. STOMP 1.2 allows
		/// lines to be terminated with carriage-return/line-feed pair (\r\n) and allows
		/// the carriage-return to be escaped in non-CONNECTED headers.
		///
		/// The parser deviates from the STOMP standard in four minor ways:
		///
		/// STOMP 1.0 and 1.1 are ambiguous about how spaces are treated after header
		///	keys and before values. This is exacerbated by the examples in both which
		/// use spaces between the colon and the value. STOMP 1.2 and later specify that
		///	keys and values are to be treated literally, including any trailing and/or
		///	leading spaces. For STOMP 1.0 and 1.1 frames, this parser will trim all tabs
		/// and spaces after header keys and before header values.
		///
		/// Colons are required to be escaped in headers with \c. That would suggest
		///	that any literal colon encountered after the first colon delimiter, should
		///	result in a fatal error. We ignore this and process subsequent literal
		///	colons as though they are part of the value.
		///
		/// CONNECTED and RECEIPT frames are not supposed to have bodies. This parser
		/// will process any body present for these frames.
		///
		/// The STOMP standard specifies that the content-length header can specify the
		///	length of the body. This is to allow binary data to be safely transmitted
		///	and to help validate the frame. Since we know the size of the frame to be
		///	processed, the parser ignores any content-length header; it is up to the
		///	caller to validate this value, if present.
		/// </remarks>
		Err Parse(Version version = Stomp::V1_1);

		/// <summary>
		/// Returns the command.
		/// </summary>
		/// <returns>The command.</returns>
		RecvCommand::Type GetCommand() const;

		///	<summary>
		///	Returns the number of header key/value pairs.
		///	</summary>
		///	<returns>The number of header key/value pairs.</returns>
		size_t GetNumHeaders() const;

		/// <summary>
		/// Returns the header value associated with a key.
		/// </summary>
		/// <param name="pKey">A pointer to the key to find.</param>
		/// <param name="pDefaultVal">The default value to use if key is not found.</param>
		/// <returns>The value associated with the key or <paramref name="pDefaultVal"/>
		///		if key is not found.</returns>
		/// <remarks>A valid key will never return a nullptr.</remarks>
		const char* GetHeaderValue(const char* pKey, const char* pDefaultVal = nullptr) const;

		/// <summary>
		/// Sets or processes the header value associated with a key.
		/// </summary>
		/// <param name="pKey">A pointer to the key.</param>
		/// <param name="pVal">The value to use. This value <b>MUST</b> remain valid
		///		for the life of the frame.</param>
		/// <param name="save">Whether to save the key/value in the header map.</param>
		///	Stomp::Success upon success, else an error code.
		/// <remarks>If the key already exists, the value will be replaced.</remarks>
		Err SetHeaderValue(const char* pKey, const char* pVal, bool save = true);

		///	<summary>
		/// Returns the value of the content-length key.
		///	</summary>
		///	<returns>The value of the content-length key, else zero (0) if there was
		///		no content-length header.</returns>
		size_t GetContentLen() const;

		///	<summary>
		/// Returns a pointer to the body of the frame.
		///	</summary>
		///	<returns>A pointer to the body of the frame or <b>nullptr</b> if there
		///		is no body.</returns>
		///	<remarks>
		/// Although this returns a char pointer, the contents may be binary. The
		///	'content-type' header should be used, and checked, to determine the
		///	format of the body data.
		///
		/// This is not const since the parser may use in place parsing.
		///	</remarks>
		char* GetBody();

		///	<summary>
		/// Returns the length of the body of the frame.
		///	</summary>
		///	<returns>The length of the body of the frame.</returns>
		size_t GetBodyLen() const;

		///	<summary>
		/// Returns the body in a std::string.
		///	</summary>
		///	<returns>A std::string containing the body of the message.</returns>
		std::string GetBodyAsString() const;

		///	<summary>
		/// Adds a character to the internal buffer.
		///	</summary>
		/// <param name="ch">The character to add.</param>
		///	<returns><b>true</b> if ch is zero, else <b>false</b>.</returns>
		/// <remarks>
		///	This is useful when reading frames from a stream. Since this may cause
		///	the internal buffer to reallocate it should not be called if any parsing
		///	has been since it will likely invalidate their pointers. No checking is
		///	done for this condition for performance reasons.
		///
		/// For a new frame, Reset() must be called before calling this method.
		/// </remarks>
		bool Add(char ch);

	protected:
		virtual Err OnCommand(RecvCommand::Type command);
		virtual Err OnHeader(char* pKey, size_t keylen, char* pValue, size_t valueLen);
		virtual Err OnBody(char* pBody, size_t bodyLen);

	private:
		Frame(const Frame& frame);
		Frame& operator=(const Frame& otherFrame);

	};
} // Stomp namespace



