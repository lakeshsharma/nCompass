#include "stdafx.h"
#include "ProgressiveEligibleMinorInfo.h"
#include "Util/Currency.h"

using namespace std;

ProgressiveEligibleMinorInfo::ProgressiveEligibleMinorInfo(const rapidjson::Value& minor)
	: m_name(JsonGetCStringEx(minor, pKeyName))
	, m_percentEligible(JsonGetDoubleEx(minor, pKeyPercentEligible))
	, m_awardType(AwardType::emap.At(JsonGetString(minor, pKeyAwardType), AwardType::Invalid))
	, m_min(Currency::ToInt64(JsonGetDoubleEx(minor, pKeyMin)))
	, m_max(Currency::ToInt64(JsonGetDoubleEx(minor, pKeyMax)))
	, m_sequence(JsonGetIntEx(minor, pKeySequence))
	, m_isEligible(JsonGetBoolEx(minor, pKeyEligible))
{
}

ProgressiveEligibleMinorInfo::ProgressiveEligibleMinorInfo()
	: m_percentEligible(0.0),
	m_awardType(AwardType::Invalid),
	m_min(0),
	m_max(0),
	m_sequence(0),
	m_isEligible(false)
{
}

ProgressiveEligibleMinorInfo::ProgressiveEligibleMinorInfo(const ProgressiveEligibleMinorInfo& other)
{
	*this = other;
}

ProgressiveEligibleMinorInfo & ProgressiveEligibleMinorInfo::operator=(const ProgressiveEligibleMinorInfo &other)
{
	m_name = other.m_name;
	m_percentEligible = other.m_percentEligible;
	m_awardType = other.m_awardType;
	m_min = other.m_min;
	m_max = other.m_max;
	m_sequence = other.m_sequence;
	m_isEligible = other.m_isEligible;

	return *this;
}

const std::string& ProgressiveEligibleMinorInfo::GetName() const
{
	return m_name;
}

double  ProgressiveEligibleMinorInfo::GetPercentEligible() const
{
	return m_percentEligible;
}

AwardType::Type ProgressiveEligibleMinorInfo::GetAwardType() const
{
	return m_awardType;
}

int64_t ProgressiveEligibleMinorInfo::GetMin() const
{
	return m_min;
}

int64_t ProgressiveEligibleMinorInfo::GetMax() const
{
	return m_max;
}

int ProgressiveEligibleMinorInfo::GetSequence() const
{
	return m_sequence;
}

bool ProgressiveEligibleMinorInfo::IsEligible() const
{
	return m_isEligible;
}

string ProgressiveEligibleMinorInfo::GetJsonString() const
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	
	writer.Key(pKeyMinors);
	{
		writer.StartArray();
		writer.StartObject();
		writer.Key(pKeySequence);			writer.Int(1);
		writer.Key(pKeyEligible);			writer.Bool(m_isEligible);
		writer.Key(pKeyPercentEligible);	writer.Double(m_percentEligible);
		writer.Key(pKeyName);				writer.String(m_name);
		writer.Key(pKeyMin);				writer.Double((double)m_min / ONE_DOLLAR_IN_CENTS);
		writer.Key(pKeyMax);				writer.Double((double)m_max / ONE_DOLLAR_IN_CENTS);
		string awardTypeStr(AwardType::emap.At(m_awardType));
		writer.Key(pKeyAwardType);			writer.String(awardTypeStr);
		writer.EndObject();
		writer.EndArray();
	}

    return writeStream.m_str;
}