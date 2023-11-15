#pragma once
#include "InternalMsgContent.h"
#include "IPAddress.h"
#include "ProgressiveConfig.h"

namespace SentinelNativeUnitTests { class ProgressiveConnectionInfo; }
namespace SentinelNativeUnitTests { class ProgressiveConnectionUnitTests; }
namespace SentinelNativeUnitTests { class ProgressiveConnectionUnitTests2; }

/// <summary>
/// Progressive connection state types
/// </summary>
enum ProgressiveConnectionState
{
	NotStarted,
	Disabled,
	WaitingForInfo,
	InDiscovery,
	Connected,
	Disconnected,
	InitiateDiscovery,
	Error
};

/// <summary>
/// Progressive connection Error types
/// </summary>
enum ProgressiveConnectionError
{
	ProgressiveConnectionErrorNone,
	CouldNotFindServer,
	DiscoveryConnectionError, // Not used.  Need to update user documentation if used.
	MessageTransmissionError, // Not used.  Need to update user documentation if used.
	ConnectFailed,
	DisconnectFailed,
};

/// <summary>
/// Progressive ConnectionInfo Class
/// </summary>
class CProgressiveConnectionInfo : public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_CONNECTION_INFO>
{
public:
	/// <summary>
	/// Constructor
	/// </summary>
	CProgressiveConnectionInfo();

	/// <summary>
	/// Copy Constructor. Initializes a new instance of the <see cref="CProgressiveConnectionInfo"/> class.
	/// </summary>
	/// <param name="rhs">The RHS.</param>
	CProgressiveConnectionInfo(const CProgressiveConnectionInfo &rhs);

	/// <summary>
	/// Deconstructor
	/// </summary>
	virtual ~CProgressiveConnectionInfo();

	/// <summary>
	/// Assigment operator.
	/// </summary>
	/// <param name="rhs">The CProgressiveConnectionInfo to be copied from.</param>
	/// <returns>A reference to the new instance.</returns>
	CProgressiveConnectionInfo& operator=(const CProgressiveConnectionInfo &rhs);
	
	/// <summary>
	/// Loads the progressive configuration.
	/// </summary>
	/// <param name="progConfig">IN - The ping interval.</param>
	void SetPingInterval(DWORD pingInterval);

	/// <summary>
	/// Get last message ip
	/// </summary>
	/// <returns>message ip</returns>
	const CIPAddress& GetLastMessageIP() const;

	/// <summary>
	/// Set last message ip
	/// </summary>
	/// <param name="messageIP">new message</param>
	void SetLastMessageIP(const CIPAddress &messageIP);

	/// <summary>
	/// Get last connection time stamp
	/// </summary>
	/// <returns>time stamp</returns>
	const SYSTEMTIME & GetLastConnectTimestamp() const;

	/// <summary>
	/// Gets the last connect timestamp string.
	/// </summary>
	/// <returns>timestamp string</returns>
	std::string GetLastConnectTimestampString() const;

	/// <summary>
	/// Set last connect time stamp
	/// </summary>
	/// <param name="timestamp">new timestamp</param>
	void SetLastConnectTimestamp(const SYSTEMTIME &timestamp);


	/// <summary>
	/// Get last message time stamp
	/// </summary>
	/// <returns>time stamp</returns>
	const SYSTEMTIME & GetLastMessageTimestamp() const;

	/// <summary>
	/// Gets the last message timestamp string.
	/// </summary>
	/// <returns>timestamp string</returns>
	std::string GetLastMessageTimestampString() const;

	/// <summary>
	/// Set last message time stamp to the specified time
	/// </summary>
	/// <param name="timestamp">new timestamp</param>
	void SetLastMessageTimestamp(const SYSTEMTIME &timestamp);

	/// <summary>
	/// Set last message time stamp to the current time.
	/// </summary>
	void SetLastMessageTimestamp();

	/// <summary>
	/// Returns whether it is time to send a ping message.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <returns>true if it is time to send a ping message, false otherwise.</returns>
	bool IsTimeForPing(DWORD tickCount) const;

	/// <summary>
	/// Sets the last heatbeat message tick count.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	void SetLastHeartbeatMessageTickCount(DWORD tickCount);

	/// <summary>
	/// Get last patron info time stamp
	/// </summary>
	/// <returns>time stamp</returns>
	const SYSTEMTIME & GetLastPatronInfoTimestamp() const;

	/// <summary>
	/// Set last patron info time stamp to the specified time
	/// </summary>
	/// <param name="timestamp">new timestamp</param>
	void SetLastPatronInfoTimestamp(const SYSTEMTIME &timestamp);

	/// <summary>
	/// Set last patron info time stamp to the current time.
	/// </summary>
	void SetLastPatronInfoTimestamp();

	/// <summary>
	/// Get last card-in time stamp
	/// </summary>
	/// <returns>time stamp</returns>
	const SYSTEMTIME & GetLastCardInTimestamp() const;

	/// <summary>
	/// Set last card-in time stamp to the specified time
	/// </summary>
	/// <param name="timestamp">new timestamp</param>
	void SetLastCardInTimestamp(const SYSTEMTIME &timestamp);

	/// <summary>
	/// Set last card-in time stamp to the current time.
	/// </summary>
	void SetLastCardInTimestamp();

	/// <summary>
	/// Get connection state
	/// </summary>
	/// <returns>connection state</returns>
	ProgressiveConnectionState GetState() const;

	/// <summary>
	/// Set connection state
	/// </summary>
	/// <param name="state">new state</param>
	void SetState(ProgressiveConnectionState state);

	/// <summary>
	/// Get last connection error
	/// </summary>
	/// <returns>connection error</returns>
	ProgressiveConnectionError GetLastError() const;

	/// <summary>
	/// Set last connection error
	/// </summary>
	/// <param name="error">new connection error</param>
	void SetLastError(ProgressiveConnectionError error);

	/// <summary>
	/// Get the progressive connection startup timestamp
	/// </summary>
	/// <returns>time stamp</returns>
	const SYSTEMTIME & GetStartupTimestamp() const;

	/// <summary>
	/// Gets progressive connection startup timestamp string.
	/// </summary>
	/// <returns>timestamp string</returns>
	const std::string GetStartupTimestampString() const;


	/// <summary>
	/// Set progressive connection startup time stamp
	/// </summary>
	/// <param name="timestamp">new timestamp</param>
	void SetStartupTimestamp(const SYSTEMTIME &timestamp);

	/// <summary>
	/// Set progressive connection startup time stamp to the current time.
	/// </summary>
	/// <param name="timestamp">new timestamp</param>
	void SetStartupTimestamp();

	/// <summary>
	/// Sets the connect attempt count for the current address.
	/// </summary>
	/// <param name="connectAttemptCount">the count of connection attempts to teh current address</param>
	void SetConnectAttemptCount(const int connectAttemptCount);

	/// <summary>
	/// Gets the connect attempt count for the current address
	/// </summary>
	/// <returns>the count of connection attempts to the current address</returns>
	int GetConnectAttemptCount() const;

	/// <summary>
	/// Get last connection attempt time stamp
	/// </summary>
	/// <returns>time stamp</returns>
	const SYSTEMTIME & GetLastConnectAttemptTimestamp() const;

	/// <summary>
	/// Set last connection attempt time stamp
	/// </summary>
	/// <param name="timestamp">new timestamp</param>
	void SetLastConnectAttemptTimestamp(const SYSTEMTIME &timestamp);

	/// <summary>
	/// Get last address update time stamp
	/// </summary>
	/// <returns>time stamp</returns>
	const SYSTEMTIME & GetLastAddressUpdateTimestamp() const;

	/// <summary>
	/// Set last address update time stamp
	/// </summary>
	/// <param name="timestamp">new timestamp</param>
	void SetLastAddressUpdateTimestamp(const SYSTEMTIME &timestamp);

	/// <summary>
	/// Sets the last address update timestamp to the current time
	/// </summary>
	void SetLastAddressUpdateTimestamp();

	/// <summary>
	/// A connection is being attempted.
	/// </summary>
	void ConnectAttempt();

	/// <summary>
	/// A successful connection has been made.
	/// </summary>
	void SetConnected();

	/// <summary>
	/// Gets a string that corresponds to the current connection state and last connection error.
	/// </summary>
	/// <param name="shortString">IN - true to return shorter strings for mechanic card displays, false otherwise.</param>
	/// <returns>A string that corresponds to the current connection state and last connection error.</returns>
	/// <remarks>
	/// 2014-06-10. The "Mechanic Card: Progressive Connection Info" screen display fields can handle about 20 characters.
	/// </remarks>
	std::string GetConnectionStateString(bool shortString = false) const;


	/// <summary>
	/// Cleans up the dynamically allocated memory
	/// </summary>
	/// <param </param>
	/// <returns></returns>
	/// <remarks>
	/// This is helper function
	/// </remarks>
	void CleanUp();

	/// <summary>
	/// Copies the content of rhs object to current object
	/// </summary>
	/// <param </param>
	/// <returns></returns>
	/// <remarks>
	/// This is helper function and usually used in assignment 
	/// operator and copy constructor 
	/// </remarks>
	void CopyObj(const CProgressiveConnectionInfo& rhs);

    
	/// <summary>
	/// Gets the disconnect count from one link concentrator since bootup
	/// </summary>
	/// <returns>The disconnection count for one link failure to ncompass </returns>
	int GetDisconnectCount(void) const;

	/// <summary>
    /// Sets the disconnect count from one link concentrator since 
    /// bootup 
	/// </summary>
	/// <param> </param>
	void  SetDisconnectTimeStampSinceBoot(void) ;

	/// <summary>
	/// Sets the disconnect count from one link concentrator since bootup
	/// </summary>
	/// <param> </param>
	void IncrementDisconnectCount(void) ;

	/// <summary>
	/// Sets the disconnect count from one link concentrator since bootup
	/// </summary>
	/// <param name="counter">Counter of disconnection error</param>
    void SetDisconnectCount(int counter);

    	/// <summary>
	/// Gets the disconnect count from one link concentrator since bootup
	/// </summary>
	/// <param name="timestamp">New timestamp</param>
    void SetDisconnectTimeStampSinceBoot(SYSTEMTIME timestamp);

	/// <summary>
	///Gets the disconnect count from one link concentrator since bootup
	/// </summary>
	/// <returns>The disconnection timestamp for one link failure to ncompass </returns>
	SYSTEMTIME GetDisconnectTimeStampSinceBoot(void) const;

	/// <summary>
	/// Gets the timestamp of the last time multicast socket failed.
	/// </summary>
	/// <returns>The timestamp of the last time multicast socket failed</returns>
	const SYSTEMTIME& GetLastMulticastSocketFailedTimeStamp(int& failedMulticastCount) const;

	/// <summary>
	/// Sets the timestamp of the last time multicast socket failed.
	/// </summary>
	void SetLastMulticastSocketFailedTimeStamp();

private:
	/// <summary>
	/// The IP address of the last successful mesage
	/// </summary>
	CIPAddress m_lastMessageIP;

	/// <summary>
	/// The timestamp of the progressive startup
	/// </summary>
	SYSTEMTIME m_startupTimestamp;

	/// <summary>
	/// The timestamp of the last successful message
	/// </summary>
	SYSTEMTIME m_lastMessageTimestamp;

	/// <summary>
	/// The timestamp of the last patron information sent to the progressive
	/// </summary>
	SYSTEMTIME m_lastPatronInfoTimestamp;

	/// <summary>
	/// The timestamp of the last card-in message sent to the progressive
	/// </summary>
	SYSTEMTIME m_lastCardInTimestamp;

	/// <summary>
	/// The timestamp that a connection was last attempted
	/// </summary>
	SYSTEMTIME m_lastConnectAttemptTimestamp;

	/// <summary>
	/// The timestamp of the last successful connection
	/// </summary>
	SYSTEMTIME m_lastConnectionTimestamp;
	
	/// <summary>
	/// The current state of the progressive connection
	/// </summary>
	ProgressiveConnectionState m_connectionState;
	
	/// <summary>
	/// The last connection error
	/// </summary>
	ProgressiveConnectionError m_lastConnectionError;

	/// <summary>
	/// The count of connection attempts
	/// </summary>
	int m_connectAttemptCount;

	/// <summary>
	/// The tick count of the last successful heartbeat message
	/// </summary>
	DWORD m_lastHeartbeatMessageTickCount;

	/// <summary>
	/// The timestamp that an address update (DNS lookup) was performed
	/// </summary>
	SYSTEMTIME m_lastAddressUpdateTimestamp;

	/// <summary>
	/// The minimum tick count to elapse before sending a heartbeat message.
	/// </summary>
	DWORD m_pingInterval;

	/// <summary>
	/// The disconnect count from one link concentrator since boot up of ncompass.
	/// </summary>
	int m_DisconnectCount;

	/// <summary>
	/// The last disconnect time from one link concentrator since boot up of ncompass
	/// </summary>
	SYSTEMTIME	m_DisconnectTimeStampSinceBoot;

	/// <summary>
	/// The timestamp of the last time multicast socket failed.
	/// </summary>
	SYSTEMTIME m_lastMulticastSocketFailedTimeStamp;

	int m_multicastFailCount;

	friend SentinelNativeUnitTests::ProgressiveConnectionInfo;
	friend SentinelNativeUnitTests::ProgressiveConnectionUnitTests;
	friend SentinelNativeUnitTests::ProgressiveConnectionUnitTests2;
};

