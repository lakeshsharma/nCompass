#pragma once

#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveMulticastChannel.h"
#include "ProgressiveProtocol/ProgressiveMulticastFrame.h"

namespace SentinelNativeUnitTests { class ProgressiveMulticastHostTests; }
namespace SentinelNativeUnitTests { class ProgressiveLevelTests; }

class ProgressiveMulticastFrame;
///	<summary>
///	A class which manages the multicast sockets for OneLink level broadcasts.
///	</summary>
class ProgressiveMulticastHost
{
private:
	enum
	{
		MaxMulticastMessages =  64,
		///	<summary>The exit code to use if thread is forcibly terminated.</summary>
		TerminatedExitCode =  -1,
		///	<summary>Default maximum time to wait for internal thread to end before forcing it closed</summary>
        TerminateTimeout = 250
	};

private:
	binsem m_hEndEvent;

protected:
	typedef std::unique_ptr<ProgressiveMulticastChannelInterface> Channel;

private:
	std::unique_ptr<std::unordered_set<std::string>> m_urls;
	std::vector<Channel> m_channels;
	std::string			m_oneLinkSystemId;
	std::recursive_mutex	m_channelCS;

private:
	virtual std::unique_ptr<ProgressiveMulticastChannelInterface> CreateChannel(const std::string& url);
    inline virtual void OnCreateChannel(const std::string& url)
    {
         m_urls.get()->insert(url);
    }

    inline virtual void OnDeleteChannel(const std::string& url)
    {
        m_urls.get()->erase(url);
    }


protected:
	ProgressiveMulticastHost();
	inline virtual ~ProgressiveMulticastHost()
	{
		CloseChannels();
	}

	void CloseChannels();
	void ReconnectChannels();

	///	<summary>
	/// Updates the configuration.
	///	</summary>
	///	<param name="pOneLinkSystemId">The id from which multicasts will be accepted.</param>
	///	<param name="multicastChannels">An array of channels to receive multicast traffic.
	///		Each channel must be in the form "stomp:://ip:port". This MAY be modified.</param>
	///	<returns><b>true</b> on success, else <b>false</b>.</returns>
	bool UpdateMulticastConfiguration(const char* pOneLinkSystemId, std::vector<std::string>& updatedChannels);

	// This must be non-const since the receiver will change the body
	virtual void OnMulticastMessage(ProgressiveMulticastFrame& frame) = 0;

private:
	friend ProgressiveMulticastChannel;

	friend SentinelNativeUnitTests::ProgressiveMulticastHostTests;
	friend SentinelNativeUnitTests::ProgressiveLevelTests;
};

