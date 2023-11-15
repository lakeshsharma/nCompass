#pragma once

namespace SentinelNativeUnitTests 
{ 
	class CountHistoryTests;
	class ProgressiveLevelTests;
	class WebDiagnostics_Progressive;
}

class CountHistory
{
protected:
	enum 
	{
		SetTime,
		ResetTime,
		RolledTime,
		IntervalTime,
		__lastTimeType__
	};

public:
	class Data
	{
	protected:
		unsigned m_count;
		unsigned m_milliseconds;
		bool	 m_isSet;

	public:
		///	<summary>
		/// Constructor.
		///	</summary>
		Data(unsigned count = 0, unsigned milliseconds = 0, bool isSet = false);

		///	<summary>
		/// Indicates whether this is in a set state.
		///	</summary>
		///	<returns><b>true</b> if this is in a set state, else <b>false</b>.</returns>
		bool IsSet() const;

		///	<summary>
		/// Returns the count.
		///	</summary>
		///	<returns>The count.</returns>
		unsigned GetCount() const;

		///	<summary>
		/// Returns the total elapsed milliseconds.
		///	</summary>
		///	<returns>The total elapsed milliseconds.</returns>
		unsigned GetMilliseconds() const;

		///	<summary>
		/// Indicates whether this instance is equal to another.
		///	</summary>
		///	<returns><b>true</b> this instance is equal to other, else <b>false</b>.</returns>
		bool operator==(const Data& other) const;

	private:
		friend CountHistory;
		friend SentinelNativeUnitTests::CountHistoryTests;
		friend SentinelNativeUnitTests::ProgressiveLevelTests;
		friend SentinelNativeUnitTests::WebDiagnostics_Progressive;
	};

protected:
	Data	   m_today;
	Data	   m_yesterday;
	SYSTEMTIME m_lastSetTime;
	SYSTEMTIME m_lastResetTime;
	SYSTEMTIME m_lastRolledTime; 
	SYSTEMTIME m_lastIntervalTime;

public:
	///	<summary>
	/// Constructor.
	///	</summary>
	CountHistory();

	///	<summary>
	/// Copy constructor.
	///	</summary>
	///	<param name="other">The CountHistory to copy.</param>
	CountHistory(const CountHistory& other);

	///	<summary>
	/// Destructor.
	///	</summary>
	~CountHistory();

	///	<summary>
	/// Assignment operator.
	///	</summary>
	///	<param name="other">The CountHistory to copy.</param>
	/// <returns>A reference to this instance.</returns>
	CountHistory& operator=(const CountHistory& other);

	///	<summary>
	/// Indicates whether the last set time is valid.
	///	</summary>
	///	<returns><b>true</b> if the last set time is valid, else <b>false</b>.</returns>
	bool IsLastSetTimeValid() const;

	///	<summary>
	/// Returns the last LOCAL set time.
	///	</summary>
	///	<returns>The last LOCAL set time.</returns>
	SYSTEMTIME GetLastSetTime() const;

	///	<summary>
	/// Indicates whether the last reset time is valid.
	///	</summary>
	///	<returns><b>true</b> if the last reset time is valid, else <b>false</b>.</returns>
	bool IsLastResetTimeValid() const;

	///	<summary>
	/// Returns the last LOCAL reset time.
	///	</summary>
	///	<returns>The last LOCAL reset time.</returns>
	SYSTEMTIME GetLastResetTime() const;

	///	<summary>
	/// Returns the data for the specified day.
	///	</summary>
	///	<param name="pToday">Optional pointer to receive data for today.</param>
	///	<param name="pYesterday">Optional pointer to receive data for pYesterday.</param>
	///	<param name="pLocalTime">Optional pointer to the LOCAL system time.</param>
	///	<remarks>
	///	This may trigger a roll.
	///	</remarks>
	void GetData(Data* pToday = nullptr, Data* pYesterday = nullptr, const SYSTEMTIME* pLocalTime = nullptr);

	///	<summary>
	/// Returns the data for the specified day.
	///	</summary>
	///	<param name="pToday">Optional pointer to receive data for today.</param>
	///	<param name="pYesterday">Optional pointer to receive data for pYesterday.</param>
	///	<param name="pLocalTime">Optional pointer to the LOCAL system time.</param>
	///	<remarks>
	///	This will never trigger a roll.
	///	</remarks>
	void GetDataNoRoll(Data* pToday = nullptr, Data* pYesterday = nullptr, const SYSTEMTIME* pLocalTime = nullptr) const;

	///	<summary>
	/// If not set, increments today's counter, sets isSet flag and set time.
	///	</summary>
	///	<param name="pLocalTime">An optional pointer to the LOCAL system time.</param>
	///	<remarks>
	///	This may trigger a roll.
	///	</remarks>
	void Set(const SYSTEMTIME* pLocalTime = nullptr);

	///	<summary>
	/// Calls set or reset.
	///	</summary>
	///	<param name="set">If <b>true</b> call Set(), else call Reset().</param>
	///	<param name="pLocalTime">An optional pointer to the LOCAL system time.</param>
	///	<remarks>
	///	This may trigger a roll.
	///	</remarks>
	void Set(bool set, const SYSTEMTIME* pLocalTime = nullptr);

	///	<summary>
	/// If set, calculates the set time, clears the isSet flag and sets the reset time.
	///	</summary>
	///	<param name="pLocalTime">An optional pointer to the LOCAL system time.</param>
	///	<remarks>
	///	This may trigger a roll.
	///	</remarks>
	void Reset(const SYSTEMTIME* pLocalTime = nullptr);

private:
	void CheckRoll(const SYSTEMTIME& time);

	// These are used only in testing
	void Set(int day, unsigned count, unsigned milliseconds, bool isSet);
	unsigned operator[](int day) const;

	friend SentinelNativeUnitTests::CountHistoryTests;
	friend SentinelNativeUnitTests::ProgressiveLevelTests;
	friend SentinelNativeUnitTests::WebDiagnostics_Progressive;
};
