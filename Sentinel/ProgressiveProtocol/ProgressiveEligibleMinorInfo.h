#pragma once

#include "RapidJsonHelper.h"
#include "ProgressiveKeys.h"

namespace SentinelNativeUnitTests { class ProgressiveEligibleMinorInfoTests; }

class ProgressiveEligibleMinorInfo
{
private:
	std::string m_name;
	double  m_percentEligible;
	AwardType::Type m_awardType;
	int64_t m_min;
	int64_t m_max;
	int     m_sequence;
	bool    m_isEligible;

public:
	///	<summary>
	/// ProgressiveEligibleMinorInfo constructor.
	///	</summary>
	///	<param name="minor">The rapidjson minor iterator.</param>
	explicit ProgressiveEligibleMinorInfo(const rapidjson::Value& minor);

	///	<summary>
	/// ProgressiveEligibleMinorInfo default constructor.
	///	</summary>
	ProgressiveEligibleMinorInfo();

	///	<summary>
	/// ProgressiveEligibleMinorInfo copy constructor.
	///	</summary>
	ProgressiveEligibleMinorInfo(const ProgressiveEligibleMinorInfo& other);

	///	<summary>
	/// ProgressiveEligibleMinorInfo assignment operator.
	///	</summary>
	ProgressiveEligibleMinorInfo &operator=(const ProgressiveEligibleMinorInfo &other);

	///	<summary>
	/// Returns the name.
	///	</summary>
	///	<returns>The name.</returns>
	const std::string &GetName() const;

	///	<summary>
	/// Returns the percent eligible.
	///	</summary>
	///	<returns>The percent eligible.</returns>
	double  GetPercentEligible() const;

	///	<summary>
	///	Returns the award type.
	///	</summary>
	///	<returns>The award type.</returns>
	AwardType::Type GetAwardType() const;

	///	<summary>
	/// Returns the min amount (in cents).
	///	</summary>
	///	<returns>The min amount (in cents).</returns>
	int64_t GetMin() const;

	///	<summary>
	/// Returns the max amount (in cents).
	///	</summary>
	///	<returns>The max amount (in cents).</returns>
	int64_t GetMax() const;

	///	<summary>
	/// Returns the sequence.
	///	</summary>
	///	<returns>The sequence.</returns>
	int GetSequence() const;

	///	<summary>
	/// Indicates whether this is eligible.
	///	</summary>
	///	<returns><b>true</b> if eligible, else <b>false</b>.</returns>
	bool IsEligible() const;

	///	<summary>
	/// Get JSON string value
	///	</summary>
	///	<returns>JSON string value of the object</returns>
	std::string GetJsonString () const;

private:

	friend SentinelNativeUnitTests::ProgressiveEligibleMinorInfoTests;
};

