#pragma once
#include <stdafx.h>

#include "InternalMsgContent.h"

class CProgressiveWinnerPick : public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_WINNER_PICK>
{
public:
	int		m_levelId;
	std::string m_levelName;
	DWORD	m_receivedTickCount;

public:
	/// <summary>
	/// Constructor.
	/// </summary>
	///	<param name="receivedTickCount">The received tick count.</param>
	///	<param name="jsonDoc">The json document.</param>
	CProgressiveWinnerPick(DWORD receivedTickCount, const rapidjson::Document& jsonDoc);

private:
	CProgressiveWinnerPick(const CProgressiveWinnerPick& other);
	void operator=(const CProgressiveWinnerPick& other);
};
