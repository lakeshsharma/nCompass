#pragma once

/// <summary>
/// Class which acts as interface for PollerProtocol. Currently it is used by PollerGameListInfo.
/// </summary>
class CPollerProtocolInterface
{
public:
	CPollerProtocolInterface() {};
	virtual ~CPollerProtocolInterface() {};

	// <summary>
	///Send Game List to Poller.
	/// </summary>
	virtual void SendGameListToPoller() = 0;
};
