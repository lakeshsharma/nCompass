#pragma once
#include <stdafx.h>
#include "UdpSocket2.h"
#include "binsem.h"

namespace SentinelNativeUnitTests { class ProgressiveMulticastChannelTests; }

class ProgressiveMulticastHost;

///	<summary>
/// Enumeration of ProgressiveChannel constructor error codes
///	</summary>
enum ProgressiveChannelError
{
	///	<summary>A parameter is an empty string or invalid in a way not described by other values.</summary>
	InvalidParameter		= -100,
	///	<summary>The protocol is not stomp:// or udpstomp://.</summary>
	InvalidProtocol			= -101,
	///	<summary>The port is invalid.</summary>
	InvalidPort				= -102,
	///	<summary>For stomp://, the address is not in the range 224.0.0.0 to 239.255.255.255 (224.0.0.0/4).</summary>
	InvalidMulticastAddress	= -103
};

///	<summary>
///	This class manages the multicast socket for the OneLink broadcasts.
///	</summary>
class ProgressiveMulticastChannelSocket : public UdpSocket2
{
private:
	static bool ParseUrl(const char* pUrl, in_addr& addr, WORD& port);
#ifdef _DEBUG
	std::string	m_protocol;
#endif

protected:
	///	<summary>
	///	The URL to which to listen. This is used by other classes to uniquely
	///	identify a ProgressiveMulticastChannel instance.
	///	</summary>
	std::string	m_url;
	///	<summary>
	///	The multicast address to which to listen.
	///	</summary>
	in_addr	m_addr;
	///	<summary>
	///	The multicast port to which to listen.
	///	</summary>
	WORD m_port;

public:
	///	<summary>
	///	Constructs a ProgressiveMulticastChannelSocket object.
	///	</summary>
	///	<param name="oneLinkSystemId">The OneLink system id to listen to.</param>
	///	<param name="url">The URL to listen to.</param>
	///	<remarks>
	/// <paramref name="url"> must be in the form stomp://<ip address>:<port> or 
	///	udpstomp://<ip address>:<port>. All other forms will cause an exception to be thrown.
	///	</remarks>
	/// <exception cref="ProgressiveChannelException">Thrown when anything goes wrong while creating the connection.</exception>
	ProgressiveMulticastChannelSocket(const std::string& url);

	///	<summary>
	///	Returns the url used to create this socket.
	///	</summary>
	///	<returns>The url used to create this socket.</returns>
	const std::string& GetUrl() const;

private:
	ProgressiveMulticastChannelSocket(const ProgressiveMulticastChannelSocket&);
	void operator=(const ProgressiveMulticastChannelSocket&);

	friend SentinelNativeUnitTests::ProgressiveMulticastChannelTests;
};

///////////////////////////////////////////////////////////////////////////

///	<summary>
///	A pure virtual base class that facilitates some of the tests.
/// </summary>

class  ProgressiveMulticastChannelInterface
{
public :
	///	<summary>
	/// When this is called, the derived class should notify all threads and objects
	/// that this instance will soon be deleted.
	///	</summary>
	virtual void Close() = 0;

	///	<summary>
	///	Returns the URL of the multicast channel. This is used to uniquely identify
	/// a ProgressiveMulticastChannel instance.
	///	</summary>
	///	<returns>The URL of the multicast channel.</returns>
	virtual const std::string& GetUrl() const = 0;

	///	<summary>
	///	ProgressiveMulticastChannelInterface destructor.
	///	</summary>
	inline virtual ~ProgressiveMulticastChannelInterface(){};
};

class ProgressiveMulticastChannel : public ProgressiveMulticastChannelInterface
{
public:
	typedef std::unique_ptr<ProgressiveMulticastChannelSocket> Socket;

private:
	std::unique_ptr<std::thread> m_thread;
	binsem m_hEndEvent;
	Socket m_socket;
	ProgressiveMulticastHost* m_pHost;

	void ThreadMain();
	static unsigned  _thread(ProgressiveMulticastChannel *pClass);
	static const DWORD ThreadEndTimeout = 1000 * 2;

public:
	///	<summary>
	/// ProgressiveMulticastChannel constructor.
	///	</summary>
	///	<param name="socket">The socket to which to listen.</param>
	///	<param name="terminateTimeout">Maximum time to wait for internal thread to
	///		end before forcing it closed.</param>
	///	<remarks>
	/// The socket must already be open when this is called.
	///	</remarks>
	ProgressiveMulticastChannel(Socket& socket, ProgressiveMulticastHost* m_pHost);

	///	<summary>
	/// Destructs this ProgressiveMulticastChannel instance.
	/// </summary>
	/// ProgressiveMulticastChannel::~ProgressiveMulticastChannel() 

	inline virtual ~ProgressiveMulticastChannel()
	{
		Close();
		m_thread.get()->join();//Timed join is still to be implemented

	}

	///	<summary>
	/// Closes the channel.
	///	</summary>
	void Close();

	///	<summary>
	///	Returns the URL to which this channel is listening.
	///	</summary>
	///	<returns>The URL to which this channel is listening</returns>
	const std::string& GetUrl() const;

private:
	ProgressiveMulticastChannel(const ProgressiveMulticastChannel&);
	void operator=(const ProgressiveMulticastChannel&);
};

