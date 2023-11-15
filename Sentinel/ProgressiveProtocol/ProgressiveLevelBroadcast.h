#pragma once

#include "InternalMsgContent.h"
#include <stdafx.h>

namespace SentinelNativeUnitTests
{ 
	class ProgressiveLevelTests;
	class ProgressiveLevelsTests;
	class ProgressiveLevelsTests2;
	class ProgressiveLevelManagerTests;
	class ProgressiveLevelManagerTests2;
	class ProgressiveLevelForwardTests;
	class ProgressiveLevelBroadcastTests;
}

///	<summary>
/// A class representing the level data received in a single level broadcast.
///	</summary>
///	<remarks>
/// Level information may combined into one instance or sent in multiple instances
///	depending on how the information was received from OneLink.
///	</remarks>
class CProgressiveLevelBroadcast : public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_LEVEL_BROADCAST>
{
public:
	///	<summary>
	/// A class representing a level's broadcast data.
	///	</summary>
	class Level
	{
	public:
		int64_t id;
		int64_t value;
        int64_t m_countdownStartTime;
		int64_t m_countdownEndTime;
        std::string name;

		/// <summary>
		/// Level constructor.
		/// </summary>
		///	<param name="level">The json level iterator.</param>
		Level(rapidjson::Value::ConstValueIterator level);

		///	<summary>
		/// Copy constructor.
		///	</summary>
		///	<param name="other">The Level object to copy from.</param>
		Level(const Level& other);

	private:
		Level(int64_t levelId, int64_t levelValue, int64_t countdownStartTime, int64_t countdownEndTime, LPCTSTR pName);
		Level(int64_t levelId, int64_t levelValue, int64_t countdownStartTime, int64_t countdownEndTime, LPCWSTR pName);
		Level& operator=(const Level& other) = delete;

		friend CProgressiveLevelBroadcast;
		friend SentinelNativeUnitTests::ProgressiveLevelTests;
		friend SentinelNativeUnitTests::ProgressiveLevelBroadcastTests;
	};

public:
	std::vector<Level> m_levels;
	DWORD m_receivedTickCount;

public:
	/// <summary>
	/// Constructs a CProgressiveLevelBroadcast object.
	/// </summary>
	///	<param name="receivedTickCount">The received tick count.</param>
	///	<param name="jsonDoc">The json document.</param>
	CProgressiveLevelBroadcast(DWORD receivedTickCount, const rapidjson::Document& jsonDoc);

private:
	// used only for testing
	CProgressiveLevelBroadcast(DWORD receivedTickCount);
	CProgressiveLevelBroadcast(const CProgressiveLevelBroadcast& other) = delete;
	void operator=(const CProgressiveLevelBroadcast& other) = delete;

	///	<summary>
	/// Adds a level to the internal vector.
	///	</summary>
	///	<param name="levelId">The level ID.</param>
	///	<param name="levelValue">The level value.</param>
	void AddLevel(int64_t levelId, int64_t levelValue, int64_t countdownStartTime, int64_t countdownEndTime, LPCTSTR pName = _T(""));

	friend SentinelNativeUnitTests::ProgressiveLevelTests;
	friend SentinelNativeUnitTests::ProgressiveLevelsTests;
	friend SentinelNativeUnitTests::ProgressiveLevelsTests2;
	friend SentinelNativeUnitTests::ProgressiveLevelManagerTests;
	friend SentinelNativeUnitTests::ProgressiveLevelManagerTests2;
	friend SentinelNativeUnitTests::ProgressiveLevelForwardTests;
	friend SentinelNativeUnitTests::ProgressiveLevelBroadcastTests;
};
