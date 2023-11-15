#include "stdafx.h"
#include "SASLimits.h"

bool CSASLimits::IsValidSasProgressiveGroup(int progressiveGroup)
{
	bool isValid(MIN_SAS_PROGRESSIVE_GROUP <= progressiveGroup && progressiveGroup <= MAX_SAS_PROGRESSIVE_GROUP);
	return isValid;
}

bool CSASLimits::IsValidSasProgressiveLevel(BYTE progressiveLevel)
{
	bool isValid(MIN_SAS_PROGRESSIVE_LEVEL <= progressiveLevel && progressiveLevel <= MAX_SAS_PROGRESSIVE_LEVEL);
	return isValid;
}

bool CSASLimits::IsValidSasProgressiveType(ProgressiveLevelType::Type progressiveType)
{
	bool isValid(ProgressiveLevelType::Standard == progressiveType);
	return isValid;
}

bool CSASLimits::IsValidSasProgressiveLevelReceivedTickCount(DWORD receivedTickCount, DWORD tickCount)
{
	DWORD elapsedTickCount = tickCount - receivedTickCount;
	bool isValid(elapsedTickCount <= CProgressiveLevelForward::MaxInterval);
	return isValid;
}

bool CSASLimits::IsValidSasProgressiveLevelRecord(const CProgressiveLevelForward::Level &levelRecord, DWORD tickCount)
{
	bool isValid = IsValidSasProgressiveLevel(levelRecord.sasLevel) &&
		IsValidSasProgressiveType(levelRecord.type) &&
		IsValidSasProgressiveLevelReceivedTickCount(levelRecord.updatedTickCount, tickCount);
	return isValid;
}

void CSASLimits::ValidateSasProgressiveLevels(CProgressiveLevelForward &levelInfo, DWORD tickCount)
{
	int group(levelInfo.GetProgressiveGroupId());
	if (!IsValidSasProgressiveGroup(group))
	{
		// Not a valid SAS progressive group. Remove all level data.
		levelInfo.Clear();
	}
	else
	{
		for (auto it = levelInfo.cbegin(); it != levelInfo.cend();)
		{
			// Check if the level is (still) good enough for us.
			if (!IsValidSasProgressiveLevelRecord(*it, tickCount))
			{
				// The level is not good enough. Remove it.
				it = levelInfo.EraseLevel(it);
			}
			else
			{
				++it;
			}
		}
	}
}
