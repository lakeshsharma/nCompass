#pragma once

#include "InternalMsgContent.h"
#include "ProgressiveKeys.h"

namespace SentinelNativeUnitTests
{
	class ProgressiveLevelForwardTests;
	class ProgressiveLevelBroadcastTests;
};

/// <summary>
/// Class for forwarding progressive level SAS information.
/// </summary>
class CProgressiveLevelForward : public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_FORWARD>
{
public:
	struct Level
	{
		__int64 amount;
		DWORD updatedTickCount;
		ProgressiveLevelType::Type type;
		BYTE sasLevel;
		__int64 levelId; // The One Link "unique" identifier for the progressive/bonus.
		std::string levelName;
		bool eligible;
		bool showOdometer;
		bool isPte;
		bool isPrize;
		bool isPlayXGetY;
		bool isScatter;
		std::string prizeDescription;
		int64_t m_countdownStartTime;
		int64_t m_countdownEndTime;

		/// <summary>
		/// Standard SAS constructor. Initializes a new instance of the <see cref="Level"/> struct.
		/// </summary>
		/// <param name="sasLevel">IN - The SAS level.</param>
		/// <param name="amount">IN - The amount.</param>
		/// <param name="type">IN - The type.</param>
		/// <param name="updatedTickCount">IN - The updated tick count.</param>
		Level(BYTE sasLevel, __int64 amount, ProgressiveLevelType::Type type, DWORD updatedTickCount);

		/// <summary>
		/// Bonusing constructor. Initializes a new instance of the <see cref="Level"/> struct.
		/// </summary>
		/// <param name="levelId">IN - The level identifier.</param>
		/// <param name="levelName">IN - Name of the level.</param>
		/// <param name="eligible">IN - true if player is eligible for the level, false otherwise.</param>
		/// <param name="showOdometer">IN - true if the odometer should be displayed to the player, false otherwise.</param>
		/// <param name="isPte">IN - true if PTE, false otherwise.</param>
		/// <param name="isPrize">IN - true if award type is Prize, false otherwise.</param>
		/// <param name="isPlayXGetYTrigger">IN - true if TriggerType is PlayXGetYTrigger, false otherwise.</param>,
		/// <param name="isScatter">IN - true if award type is Scatter, false otherwise.</param>
		/// <param name="prizeDescription">IN - Prize description.</param>		
		/// <param name="countdownStartTime">IN - scatter countdown end time.</param>
		/// <param name="countdownEndTime">IN - scatter countdown end time.</param>
		/// <param name="amount">IN - The amount.</param>
		/// <param name="type">IN - The type.</param>
		/// <param name="updatedTickCount">IN - The updated tick count.</param>
		Level(__int64 levelId,
			LPCSTR levelName,
			bool eligible,
			bool showOdometer,
			bool isPte,
			bool isPrize,
			bool isPlayXGetYTrigger,
			bool isScatter,
			LPCSTR prizeDescription,
			int64_t countdownStartTime,
			int64_t countdownEndTime,
			__int64 amount,
			ProgressiveLevelType::Type type,
			DWORD updatedTickCount);
	};

private:
	std::vector<Level> m_levels;
	int m_progressiveGroupId;
	bool m_useAftLockOnTransfers;

public:
	///	<summary>
	/// The minimum elapsed tick counts to forward level values since the last levels were forwarded IF an update has occurred.
	///	</summary>
	static const DWORD MinInterval = 1 * 1000; // Not so often as to seriously interfere with SAS polling.

	///	<summary>
	/// The minimum elapsed tick counts to forward level values, since the last time values were forwarded IF an update has not occurred.
	///	</summary>
	static const DWORD StrobeInterval = 2 * 1000; // Often enough that an EGM can miss one and not go progressive down.

	///	<summary>
	/// The maximum elapsed tick counts after a level has been updated and is still not considered stale.
	/// 6 seconds should be long enough to send one time after an update and strobe two more times before the value becomes stale.
	///	</summary>
	static const DWORD MaxInterval = 6 * 1000;

	///	<summary>
	/// An hour in tick counts.
	///	</summary>
	static const DWORD OneHour = 60 * 60 * 1000;

	///	<summary>
	/// The maximum elapsed time where a level value should be retained.
	///	</summary>
	static const DWORD DeleteInterval = 24 * OneHour;

public:
	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CProgressiveLevelForward"/> class.
	/// </summary>
	/// <param name="progressiveGroupId">IN - The (SAS) group identifier.</param>
	/// <param name="useAftLockOnTransfers">IN - <b>true</b> if the AFT lock is to be used on transfers, else <b>false</b>.</param>
	CProgressiveLevelForward(int progressiveGroupId, bool useAftLockOnTransfers);

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CProgressiveLevelForward"/> class.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	CProgressiveLevelForward(const CProgressiveLevelForward &rhs);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CProgressiveLevelForward"/> class.
	/// </summary>
	virtual ~CProgressiveLevelForward();

	/// <summary>
	/// Assignment operator.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	/// <returns>A reference to the target of the assignment.</returns>
	CProgressiveLevelForward& operator=(const CProgressiveLevelForward &rhs);

	/// <summary>
	/// Gets the (SAS) group identifier, normally in 1..255.
	/// </summary>
	/// <returns>The (SAS) group identifier.</returns>
	int GetProgressiveGroupId() const;

	/// <summary>
	/// Indicates whether to use the AFT lock on transfers.
	/// </summary>
	/// <returns><b>true</b> if the AFT lock is to be used on transfers, else <b>false</b>.</returns>
	bool UseAftLockOnTransfers() const;

	/// <summary>
	/// Removes all the levels.
	/// </summary>
	/// <remarks>
	/// m_progressiveGroupId and m_useAftLockOnTransfers are not affected.
	/// </remarks>
	void Clear();

	/// <summary>
	/// Add a level item.
	/// </summary>
	/// <param name="sasLevel">IN - A (SAS) progressive level, normally 1-32.</param>
	/// <param name="amount">IN - An amount in pennies, normally greater than 0 and
	///		(much) less than 10,000,000,000.</param>
	/// <param name="type">IN - The level type.</param>
	/// <param name="updatedTickCount">IN - The tick count when the this information
	///		was last received from a level broadcast.</param>
	/// <remarks>
	/// A specific level should only be added once.
	/// </remarks>
	void AddLevel(BYTE sasLevel, __int64 amount, ProgressiveLevelType::Type type, DWORD updatedTickCount);

	/// <summary>
	/// Add a level item.
	/// </summary>
	/// <param name="levelId">IN - The unique One Link LeveId.</param>
	/// <param name="levelName">IN - The name of the level for use with player displays.</param>
	/// <param name="eligible">IN - true if player is eligible for the level, false otherwise.</param>
	/// <param name="showOdometer">IN - true if the odometer should be displayed to the player, false otherwise.</param>
	/// <param name="isPte">IN - true if the level is PTE, false otherwise.</param>
	/// <param name="isPrize">IN - true if award type is Prize, false otherwise.</param>
	/// <param name="isPlayXGetY">IN - true if trigger type is PlayXGetYTrigger, false otherwise.</param>
	/// <param name="isScatter">IN - true if award type is Scatter, false otherwise.</param>
	/// <param name="prizeDescription">IN - Prize description.</param>
	/// <param name="countdownStartTime">IN - Scatter start time.</param>
	/// <param name="countdownEndTime">IN - Scatter end time.</param>
	/// <param name="amount">IN - An amount in pennies, normally greater than 0 and
	///		(much) less than 10,000,000,000.</param>
	/// <param name="type">IN - The level type.</param>
	/// <param name="updatedTickCount">IN - The tick count when the this information
	///		was last received from a level broadcast.</param>
	/// <remarks>
	/// A specific level should only be added once.
	/// </remarks>
	void AddLevel(__int64 levelId,
		LPCSTR levelName,
		bool eligible,
		bool showOdometer,
		bool isPte,
		bool isPrize,
		bool isPlayXGetY,
		bool isScatter,
		LPCSTR prizeDescription,
		int64_t countdownStartTime,
		int64_t countdownEndTime,
		__int64 amount,
		ProgressiveLevelType::Type type,
		DWORD updatedTickCount);

	///	<summary>
	/// Returns the number of levels.
	///	</summary>
	///	<returns>The number of levels.</returns>
	size_t GetNumLevels() const;

	///	<summary>
	/// Returns a const_iterator pointing to the first level.
	///	</summary>
	///	<returns>A const_iterator pointing to the first level.</returns>
	std::vector<Level>::const_iterator cbegin() const;

	///	<summary>
	/// Returns a const_iterator pointing to the past-the-end level.
	///	</summary>
	///	<returns>A const_iterator pointing to the past-the-end level.</returns>
	std::vector<Level>::const_iterator cend() const;

	/// <summary>
	/// Removes the level.
	/// </summary>
	/// <param name="it">IN - The iterator for the level to remove.</param>
	/// <returns>The iterator following the removed element. If the removed element was last element, the cend() iterator is returned.</returns>
	std::vector<CProgressiveLevelForward::Level>::const_iterator EraseLevel(
		std::vector<CProgressiveLevelForward::Level>::const_iterator it);

private:
	friend SentinelNativeUnitTests::ProgressiveLevelForwardTests;
	friend SentinelNativeUnitTests::ProgressiveLevelBroadcastTests;
};
