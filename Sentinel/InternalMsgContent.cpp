#include "stdafx.h"
#include "Utilities.h"
#include "InternalMsgContent.h"

void CInternalMsgContentStore::SetContent(IInternalMsgContent* content, InternalMsgContentType contentType)
{
	// Assumes content will be cleaned up by invoker
	if (NULL == content)
	{
		m_contentMap.erase(contentType);
	}
	else
	{
		if (content->IsWrapper())
		{
			RemoveContent(contentType, true);
		}
		m_contentMap[contentType] = content;
	}
}

void CInternalMsgContentStore::RemoveContent(InternalMsgContentType contentType, bool releaseData)
{
	ContentMap::iterator contentItr = m_contentMap.find(contentType);
	if (m_contentMap.end() != contentItr)
	{
		if (releaseData)
		{
			contentItr->second->ReleaseData();
		}
		delete contentItr->second;
		m_contentMap.erase(contentItr);
	}
}

IInternalMsgContent* CInternalMsgContentStore::GetContent(InternalMsgContentType contentType)
{
	ContentMap::iterator contentItr = m_contentMap.find(contentType);
	if (m_contentMap.end() != contentItr)
	{
		return contentItr->second;
	}
	return NULL;
}

const IInternalMsgContent* CInternalMsgContentStore::GetContent(InternalMsgContentType contentType) const
{
	ContentMap::const_iterator contentItr = m_contentMap.find(contentType);
	if (m_contentMap.end() != contentItr)
	{
		return contentItr->second;
	}
	return NULL;
}

void CInternalMsgContentStore::ClearAllContent()
{
	for (ContentMap::iterator contentItr = m_contentMap.begin(); contentItr != m_contentMap.end(); contentItr++)
	{
		if (NULL != contentItr->second)
		{
			delete contentItr->second;
		}
		CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
	}
	m_contentMap.clear();
}
