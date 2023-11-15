#include "stdafx.h"
#include "ProgressiveMulticastHost.h"
#include "Logging/LogString.h"
#include "Utilities.h"

using namespace std;

ProgressiveMulticastHost::ProgressiveMulticastHost():
m_urls(new unordered_set<string>)
{
}

void ProgressiveMulticastHost::CloseChannels()
{
	lock_guard<std::recursive_mutex> cs(m_channelCS);
	if (!m_channels.empty())
	{
		for (auto it = m_channels.begin(); it != m_channels.end(); ++it)
		{
			(*it)->Close();
		}

		CUtilities::Sleep(10);
		m_channels.clear();
		LogString(PROGRESSIVEUDP, "ProgressiveMulticastHost::CloseChannels: Cleared");
		// Do NOT clear the m_urls set here
	}
}

void ProgressiveMulticastHost::ReconnectChannels()
{
	lock_guard<std::recursive_mutex> cs(m_channelCS);
	if (m_channels.empty() && m_urls && !m_urls->empty())
	{
		LogString(PROGRESSIVEUDP, "ProgressiveMulticastHost::ReconnectChannels");

		unique_ptr<unordered_set<string>> urls(new unordered_set<string>);
		urls.swap(m_urls);
		for_each(urls->cbegin(), urls->cend(), [this](const string& url)
		{
			try
			{
				m_channels.push_back(CreateChannel(url));
			}
			catch (SentinelExceptionT<ProgressiveChannelError>& e)
			{
				LogString(PROGRESSIVEUDP, "Error %d: %s", e.code(), e.what());
			}
		});
	}
}

std::unique_ptr<ProgressiveMulticastChannelInterface> ProgressiveMulticastHost::CreateChannel(const std::string& url)
{
	ProgressiveMulticastChannel::Socket socket(new ProgressiveMulticastChannelSocket(url));
	LogString(PROGRESSIVEUDP, "ProgressiveMulticastChannel::Socket created: %s", socket->GetUrl().c_str());
	Channel channel(new ProgressiveMulticastChannel(socket, this));
	OnCreateChannel(channel->GetUrl());
	return channel;
}

bool ProgressiveMulticastHost::UpdateMulticastConfiguration(const char* pOneLinkSystemId, vector<string>& updatedChannels)
{
	lock_guard<std::recursive_mutex> cs(m_channelCS);
	if (m_oneLinkSystemId.compare(pOneLinkSystemId))
	{
		CloseChannels();
		m_urls.reset(new unordered_set<string>);
		m_oneLinkSystemId = pOneLinkSystemId;
	}

	if (!m_channels.empty())
	{
		for (auto channelsIterator = m_channels.begin(); channelsIterator != m_channels.end(); )
		{
			const string& url = (*channelsIterator)->GetUrl();

			bool foundOne = false;
			auto updatedIterator = updatedChannels.begin();
			while (updatedIterator != updatedChannels.end())
			{
				if (url.compare(*updatedIterator) == 0)
				{
					updatedIterator = updatedChannels.erase(updatedIterator);
					foundOne = true;
					break;
				}
				else
				{
					++updatedIterator;
				}
			}

			if (!foundOne)
			{
				OnDeleteChannel((*channelsIterator)->GetUrl());
				(*channelsIterator)->Close();
				CUtilities::Sleep(0);
				channelsIterator = m_channels.erase(channelsIterator);
			}
			else
			{
				++channelsIterator;
			}
		}
	}

	bool rval = true;

	for (auto updatedIterator = updatedChannels.cbegin();
		updatedIterator != updatedChannels.cend(); ++updatedIterator)
	{
		try
		{
			m_channels.push_back(CreateChannel(*updatedIterator));
		}
		catch (SentinelExceptionT<ProgressiveChannelError>& e)
		{
			LogString(PROGRESSIVEUDP , "Error %d: %s", e.code(), e.what());
			rval = false;
		}
	}

	return rval;
}

