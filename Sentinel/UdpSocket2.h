#pragma once
#include <stdafx.h>

namespace SentinelNativeUnitTests { class UdpSocket2Tests; };

///	<summary>
/// A wrapper around a Windows UDP SOCKET
///	</summary>
///	<remarks>
/// This is called UdpSocket2 since it required several modifications to the original
/// UdpSocket class and I didn't want to put at risk the few places that use the original
/// until this class is fully proven.
///	</remarks>
class UdpSocket2
{
friend SentinelNativeUnitTests::UdpSocket2Tests;
public:
	/// <summary>
	/// Error codes returned by various methods.
	/// </summary>
	/// <remarks>
	/// For those methods which return error codes, a positive integer is a code
	/// returned by WSAGetLastError() while a negative code will be one from this
	/// enumeration.
	/// </remarks>
	enum
	{
		///	<summary>
		/// A non-WSA error occurred during a read.
		///	</summary>
		ErrReadCanceled = -6,
		///	<summary>
		/// A non-WSA error occurred during a read.
		///	</summary>
		ErrReadInvalid = -5,
		///	<summary>
		/// A non-WSA error occurred during a read.
		///	</summary>
		ErrReadError = -4,
		///	<summary>
		/// A timeout occurred during a read.
		///	</summary>
		ErrReadTimeout = -3,
		///	<summary>
		/// Socket has already been created.
		///	</summary>
		ErrConnected = -2,
		///	<summary>
		/// Socket has not been created.
		///	</summary>
		ErrInvalidSocket = -1,
		///	<summary>
		/// Operation completed successfully.
		///	</summary>
		Success =  0
	};

protected:
	SOCKET m_socket;
	std::recursive_mutex m_readCS;
	bool m_isCanceled;

private:
	UdpSocket2(const UdpSocket2&);
	void operator=(const UdpSocket2&);

public:
	///	<summary>
	/// Constructs a UdpSocket2 object.
	///	</summary>
	///	<param name="create">If <b>true</b>, creates a UDP socket.</param>
	UdpSocket2(bool create = false);

	///	<summary>
	/// Destructs this UdpSocket2 instance.
	///	</summary>
	virtual ~UdpSocket2();

	///	<summary>
	/// Creates and binds a UDP socket.
	///	</summary>
	///	<param name="addr">The address and port to which to bind.</param>
	///	<param name="reuse">If <b>true</b>, sets the SO_REUSEADDR option.</param>
	///	<param name="allowBroadcast">If <b>true</b>, sets the SO_BROADCAST option.</param>
	///	<param name="timeoutMillseconds">If not 0 or INFINITE, sets SO_RCVTIMEO option.</param>
	///	<returns>Zero on success., else an error code.</returns>
	virtual int Create(const sockaddr_in& addr, bool reuse = true, bool allowBroadcast = true, DWORD timeoutMillseconds = 0);

	///	<summary>
	/// Creates and binds a UDP socket.
	///	</summary>
	///	<param name="pAddr">The address to which to bind.</param>
	///	<param name="port">The port to which to bind.</param>
	///	<param name="reuse">If <b>true</b>, sets the SO_REUSEADDR option.</param>
	///	<param name="allowBroadcast">If <b>true</b>, sets the SO_BROADCAST option.</param>
	///	<param name="timeoutMillseconds">If not 0 or INFINITE, sets SO_RCVTIMEO option.</param>
	///	<returns>Zero on success, else an error code.</returns>
	virtual int Create(const char* pAddr, unsigned short port, bool reuse = true, bool allowBroadcast = true, DWORD timeoutMillseconds = 0);

	///	<summary>
	/// Creates and binds a UDP socket.
	///	</summary>
	///	<param name="addr">The address to which to bind.</param>
	///	<param name="port">The port to which to bind.</param>
	///	<param name="reuse">If <b>true</b>, sets the SO_REUSEADDR option.</param>
	///	<param name="allowBroadcast">If <b>true</b>, sets the SO_BROADCAST option.</param>
	///	<param name="timeoutMillseconds">If not 0 or INFINITE, sets SO_RCVTIMEO option.</param>
	///	<returns>Zero on success, else an error code.</returns>
	virtual int Create(in_addr addr, unsigned short port, bool reuse = true, bool allowBroadcast = true, DWORD timeoutMillseconds = 0);

	///	<summary>
	/// Creates a UDP socket.
	///	</summary>
	///	<param name="addr">The address to which to bind.</param>
	///	<param name="reuse">If <b>true</b>, sets the SO_REUSEADDR option.</param>
	///	<param name="allowBroadcast">If <b>true</b>, sets the SO_BROADCAST option.</param>
	///	<param name="timeoutMillseconds">If not 0 or INFINITE, sets SO_RCVTIMEO option.</param>
	///	<returns>Zero on success, else an error code.</returns>
	virtual int Create(bool reuse = true, bool allowBroadcast = true, DWORD timeoutMillseconds = 0);

	///	<summary>
	/// Binds the socket.
	///	</summary>
	///	<param name="pAddr">The address to which to bind.</param>
	///	<param name="port">The port to which to bind.</param>
	///	<returns>Zero on success, else an error code.</returns>
	virtual int	Bind(const char* pAddr, unsigned short port);

	///	<summary>
	/// Binds the socket.
	///	</summary>
	///	<param name="addr">The address to which to bind.</param>
	///	<param name="port">The port to which to bind.</param>
	///	<returns>Zero on success, else an error code.</returns>
	virtual int Bind(in_addr addr, unsigned short port);

	///	<summary>
	/// Binds the socket.
	///	</summary>
	///	<param name="addr">The address and port to which to bind.</param>
	///	<returns>Zero on success, else an error code.</returns>
	virtual int Bind(const sockaddr_in& addr);

	///	<summary>
	/// Sets the IP_MULTICAST_IF option on the socket.
	///	</summary>
	///	<param name="interfaceAddr">The interface for sending IPv4 multicast traffic.</param>
	///	<returns>Zero on success, else an error code.</returns>
	///	<remarks>
	/// See the documentation on IP_MULTICAST_IF before using it. In general, it isn't
	/// needed.
	///	</remarks>
	int SetMulticastInterface(ULONG interfaceAddr);

	///	<summary>
	/// Joins a multicast group.
	///	</summary>
	///	<param name="groupAddr">The address of the group to join.</param>
	///	<param name="interfaceAddr">The interface address of the group to join.</param>
	///	<returns>Zero on success, else an error code.</returns>
	///	<remarks>
	/// See the documentation on multicast sockets for a more in depth discussion of
	/// what this method and these parameters do.
	///	</remarks>
	int JoinMulticastGroup(ULONG groupAddr, ULONG interfaceAddr = INADDR_ANY);

	///	<summary>
	/// Leaves a multicast group.
	///	</summary>
	///	<param name="groupAddr">The address of the group to leave.</param>
	///	<param name="interfaceAddr">The interface address of the group to leave.</param>
	///	<returns>Zero on success, else an error code.</returns>
	///	<remarks>
	/// See the documentation on multicast sockets for a more in depth discussion of
	/// what this method and these parameters do.
	///	</remarks>
	int LeaveMulticastGroup(ULONG groupAddr, ULONG interfaceAddr = INADDR_ANY);

	///	<summary>
	/// Closes the socket.
	///	</summary>
	///	<returns>Zero on success, else an error code.</returns>
	///	<remarks>
	/// <para>
	/// Any blocking reads will return an error.
	/// </para>
	/// <para>
	/// This calls Cancel and then sleeps for 0 milliseconds, giving a chance for other
	/// threads to do work before closing the socket.
	/// </para>
	///	</remarks>
	virtual int Close();

	/// <summary>
	/// Set the cancel flag, stopping future reads.
	///	</summary>
	///	<returns>Zero on success, else an error code. For this class, it currently only sets
	///		a flag and returns zero.</returns>
	///	<remarks>
	/// This will interrupt any reads that aren't yet in a blocking state. Once they are
	/// doing a blocking read, the cancel has no effect.
	///	</remarks>
	virtual int Cancel();

	/// <summary>
	/// Clears the cancel flag.
	///	</summary>
	///	<returns>Zero on success, else an error code. For this class, it currently only
	///		clears a flag and returns zero.</returns>
	virtual int ResetCancel();

	/// <summary>
	/// Indicates whether the cancel flag is set.
	///	</summary>
	///	<returns><b>true</b> if the cancel flag is set, else <b>false</b>.</returns>
	bool IsCanceled() const;

	/// <summary>
	/// Indicates whether the contained socket handle is not INVALID_SOCKET.
	///	</summary>
	///	<returns><b>true</b> if the contains socket handle is not INVALID_SOCKET, else
	///		<b>false</b>.</returns>
	///	<remarks>
	/// If something else closes the socket and nothing sets the handle in this class
	/// to INVALID_SOCKET, this will return true.
	///	</remarks>
	bool IsConnected() const;

	///	<summary>
	/// Calls select on the UDP socket.
	///	</summary>
	///	<param name="timeoutMilliseconds">The time to wait in milliseconds. If less
	///		than zero, this method does nothing and returns zero.</param>
	///	<returns>Zero on success, else an error code.</returns>
	///	<remarks>
	/// Select is a relatively time intensive task. There are better ways to do timed
	///	reads, such as setting a timeout on the socket, using WSAEventSelect or IOCP.
	///	</remarks>
	int Select(long timeout);

	///	<summary>
	/// Received a datagram.
	///	</summary>
	///	<param name="pBuffer">A buffer for the incoming data.</param>
	///	<param name="bufferLen">The length, in bytes, of the buffer pointed to by
	///		<paramref name="pBuffer"/>.</param>
	///	<param name="bytesRead">On success, the number of bytes read, else zero.</param>
	///	<param name="timeoutMilliseconds">If greater or equal to zero, <see cref="Select"/>
	///		is called with this value. If less than zero, <see cref="Select"/> is not
	///		called.</param>
	///	<param name="pFrom">An optional pointer that will hold the source address upon
	///		successful return (on failure the contents of this value are undefined.)</param>
	///	<returns>Zero on success, else an error code.</returns>
	///	<remarks>
	/// <para>
	/// Internally this calls recvfrom; see MSDN documentation for a additional
	///	information and error codes.
	/// </para>
	/// <para>
	///	If the socket is closed during the recvfrom call, this method will return
	///	ErrReadCanceled.
	/// </para>
	///	</remarks>
	virtual int Read(char* pBuffer, int bufferLen, int &bytesRead, long timeoutMilliseconds = -1, sockaddr* pFrom = nullptr);

	///	<summary>
	/// Sends data to a specific destination.
	///	</summary>
	///	<param name="addr">The address of the target socket.</param>
	///	<param name="pData">A pointer to a buffer containing the data to be
	///		transmitted.</param>
	///	<param name="dataLen">The length, in bytes, of the data pointed to by
	///		<paramref name="pData"/>.</param>
	///	<param name="flags">A set of flags that specify the way in which the call
	///		is made.</param>
	///	<returns>Zero on success, else an error code.</returns>
	///	<remarks>
	/// Internally this calls sendto; see MSDN documentation for a additional
	///	information and error codes.
	///	</remarks>
	int SendTo(sockaddr_in& addr, const void* pData, int dataLen, int flags = 0);

	///	<summary>
	/// Sends data to a specific destination.
	///	</summary>
	///	<param name="pAddr">The address of the target socket.</param>
	///	<param name="port">The port of the target socket.</param>
	///	<param name="pData">A pointer to a buffer containing the data to be
	///		transmitted.</param>
	///	<param name="dataLen">The length, in bytes, of the data pointed to by
	///		<paramref name="pData"/>.</param>
	///	<param name="flags">A set of flags that specify the way in which the call
	///		is made.</param>
	///	<returns>Zero on success, else an error code.</returns>
	///	<remarks>
	/// Internally this calls sendto; see MSDN documentation for a additional
	///	information and error codes.
	///	</remarks>
	int SendTo(const char* pAddr, unsigned short port, const void* pData, int dataLen, int flags = 0);

	///	<summary>
	/// Sends data to a specific destination.
	///	</summary>
	///	<param name="addr">The address of the target socket.</param>
	///	<param name="port">The port of the target socket.</param>
	///	<param name="pData">A pointer to a buffer containing the data to be
	///		transmitted.</param>
	///	<param name="dataLen">The length, in bytes, of the data pointed to by
	///		<paramref name="pData"/>.</param>
	///	<param name="flags">A set of flags that specify the way in which the call
	///		is made.</param>
	///	<returns>Zero on success, else an error code.</returns>
	///	<remarks>
	/// Internally this calls sendto; see MSDN documentation for a additional
	///	information and error codes.
	///	</remarks>
	int SendTo(in_addr addr, unsigned short port, const void* pData, int dataLen, int flags = 0);

	///	<summary>
	///	Indicates whether the passed address is within the multicast address range.
	///	</summary>
	///	<param name=""></param>
	///	<returns>Whether the passed address is within the multicast address range.</returns>
	///	<remarks>
	/// The IP range 224.0.0.0 through 239.255.255.255 (224.0.0.0/4) is assigned by
	/// RFC 5771 to be multicast addresses.
	///	</remarks>
	static bool IsAddrMulticast(in_addr addr);
};


