#pragma once
#include "ProgressiveProtocol/ProgressiveLevelForward.h"

class CSASLimits
{
public:
	static const int MIN_SAS_PROGRESSIVE_GROUP = 1; // 0 is reserved for non-SAS progressives.
	static const int MAX_SAS_PROGRESSIVE_GROUP = 0xff;

	static const int MIN_SAS_PROGRESSIVE_LEVEL = 1;
	static const int MAX_SAS_PROGRESSIVE_LEVEL = 32;

	/// <summary>
	/// Returns true if progressiveGroup is a valid SAS progressive group, false otherwise.
	/// </summary>
	/// <param name="progressiveGroup">IN - The progressive group.</param>
	/// <returns>true if progressiveGroup is a valid SAS progressive group, false otherwise.</returns>
	static bool IsValidSasProgressiveGroup(int progressiveGroup);

	/// <summary>
	/// Returns true if progressiveLevel is a valid SAS progressive level, false otherwise.
	/// </summary>
	/// <param name="progressiveLevel">IN - The progressive level.</param>
	/// <returns>true if progressiveLevel is a valid SAS progressive level, false otherwise.</returns>
	static bool IsValidSasProgressiveLevel(BYTE progressiveLevel);

	/// <summary>
	/// Returns true if progressiveType is a valid SAS progressive type, false otherwise.
	/// </summary>
	/// <param name="progressiveType">IN - The progressive type.</param>
	/// <returns>true if progressiveType is a valid SAS progressive type, false otherwise.</returns>
	static bool IsValidSasProgressiveType(ProgressiveLevelType::Type progressiveType);

	/// <summary>
	/// Returns true if receivedTickCount is not stale, false otherwise.
	/// </summary>
	/// <param name="receivedTickCount">IN - The tick count when the associated amount was received from the external progressive system.</param>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <returns>true if receivedTickCount is not stale, false otherwise.</returns>
	static bool IsValidSasProgressiveLevelReceivedTickCount(DWORD receivedTickCount, DWORD tickCount);

	/// <summary>
	/// Returns true if levelRecord contains valid SAS progressive information and is not stale, false otherwise.
	/// </summary>
	/// <param name="levelRecord">IN - A progressive level record for a single level.</param>
	/// <returns>true if levelRecord is valid SAS progressive information and is not stale, false otherwise.</returns>
	/// <remarks>
	/// All amounts are treated as valid.
	/// It is expected that an amount value of 0 will be sent to the EGM for messages that do not support the amount value.
	/// </remarks>
	static bool IsValidSasProgressiveLevelRecord(const CProgressiveLevelForward::Level &levelRecord, DWORD tickCount);

	/// <summary>
	/// Validates the progressive level information for SAS purposes. Removes all stale and out of range data.
	/// In other words, after validation, only valid SAS data remains.
	/// </summary>
	/// <param name="levelInfo">IN/OUT - The progressive level information.</param>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <remarks>
	/// All amounts are treated as valid.
	/// It is expected that an amount value of 0 will be sent to the EGM for messages that do not support the amount value.
	/// </remarks>
	static void ValidateSasProgressiveLevels(CProgressiveLevelForward &levelInfo, DWORD tickCount);
};
