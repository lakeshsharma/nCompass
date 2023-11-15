#include "stdafx.h"
#include "ProgressiveEligibleLevelInfo.h"
#include "Util/Currency.h"
#include <time.h>
#include "Logging/LogString.h"

using namespace std;
const int ONE_HUNDRED_PERCENT_ELIGIBLE(1);

ProgressiveEligibleLevelInfo::ProgressiveEligibleLevelInfo(
	int64_t levelId)
	: m_levelId(levelId)
	, m_pteStatus(PteStatus::Invalid)
	, m_showInfo(false)
	, m_isEligible(false)
	, m_isPlayerQualified(false)
	, m_showOdometer(false)
	, m_pte(false)
	, m_value(0)
	, m_startTimeUtc(0)
	, m_endTimeUtc(0)
	, m_countdownStartTime(INVALID_VALUE)
	, m_countdownEndTime(INVALID_VALUE)
	, m_remainingDuration(0)
	, m_percentEligible(0.0)
	, m_awardType(AwardType::Invalid)

{
}

ProgressiveEligibleLevelInfo::ProgressiveEligibleLevelInfo(int64_t levelId, const rapidjson::Value& level)
	: m_levelId(levelId)
	, m_pteStatus(PteStatus::Invalid)
	, m_showInfo(false)
	, m_isEligible(JsonGetBoolEx(level, pKeyEligible))
	, m_isPlayerQualified(JsonGetBool(level, pKeyPlayerQualified, false))
	, m_showOdometer(JsonGetBoolEx(level, pKeyShowOdometer))
	, m_pte(false)
	, m_name(JsonGetCStringEx(level, pKeyName))
	, m_triggerType(JsonGetCStringEx(level, pKeyTriggerType))
	, m_value(0)
	, m_startTimeUtc(JsonMemberExists(level, pKeyStartTimeUtc) ? JsonGetInt64Ex(level, pKeyStartTimeUtc) : 0)
	, m_endTimeUtc(JsonMemberExists(level, pKeyEndTimeUtc) ? JsonGetInt64Ex(level, pKeyEndTimeUtc) : 0)
	, m_countdownStartTime(INVALID_VALUE)
	, m_countdownEndTime(INVALID_VALUE)
	, m_remainingDuration(0)
	, m_percentEligible(JsonGetDoubleEx(level, pKeyPercentEligible))
	, m_awardType(AwardType::emap.At(JsonGetString(level, pKeyAwardType), AwardType::Invalid))
	, m_prizeDescription(JsonMemberExists(level, pKeyPrizeDescription) ? JsonGetCStringEx(level, pKeyPrizeDescription) : "")
{
	auto pteIt = level.FindMember(pKeyPte);
	if (pteIt != level.MemberEnd() && pteIt->value.IsBool())
	{
		m_pte = pteIt->value.GetBool();
		m_pteStatus = PteStatus::emap[JsonGetStringEx(level, pKeyStatus)];
		m_info = JsonGetCStringEx(level, pKeyInfo);
		m_showInfo = JsonGetBool(level, pKeyShowInfo);
		m_valueStr = FormatString("%.2f", JsonGetDoubleEx(level, pKeyValue));
		m_value = Currency::ToInt64(JsonGetDoubleEx(level, pKeyValue));
		m_leaderboardUrl = JsonGetCString(level, pKeyLeaderboardUrl);
		m_imageUrl = JsonGetCString(level, pKeyImageUrl);

		auto minorsIt = level.FindMember(pKeyMinors);
		if (minorsIt->value.IsArray())
		{
			if (IsPlayXGetY())
			{
				ProgressiveEligibleMinorInfo minorToDisplay;
			    for (auto it = minorsIt->value.Begin(); it != minorsIt->value.End(); ++it)
				{
					//Look for highest percentage greater than 0 but less than 1.0
					ProgressiveEligibleMinorInfo tempMinor(*it);
					if ((tempMinor.GetPercentEligible() > minorToDisplay.GetPercentEligible()) 
						&& (int)tempMinor.GetPercentEligible() < ONE_HUNDRED_PERCENT_ELIGIBLE)
					{
						minorToDisplay = tempMinor;
					}
				}
				
				if (minorToDisplay.GetPercentEligible())
				{
					m_eligibleMinors.push_back(minorToDisplay);
				}
			}
			else
			{
				for (auto it = minorsIt->value.Begin(); it != minorsIt->value.End(); ++it)
				{
					m_eligibleMinors.push_back(ProgressiveEligibleMinorInfo(*it));
				}
			}
		}
	}

	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	level.Accept(writer);
	m_jsonString = move(writeStream.m_str);
}

ProgressiveEligibleLevelInfo::ProgressiveEligibleLevelInfo(
	int64_t levelId,
	bool isEligible,
	bool showOdometer,
	bool playerQualified)
	: m_levelId(levelId)
	, m_isEligible(isEligible)
    , m_isPlayerQualified(playerQualified)
	, m_showOdometer(showOdometer)
{
}

std::string ProgressiveEligibleLevelInfo::GetLogString(LPCTSTR description)
{
	ostringstream sStream;
	std::string str(description != nullptr ? description : _T(""));

	sStream << "EligibleLevelInfo:";
	sStream << " levelId=" << (long long)m_levelId;
	sStream << " pteStatus=" << (int)m_pteStatus;
	sStream << " showInfo=" << (int)m_showInfo;
	sStream << " isEligible=" << (int)m_isEligible;
	sStream << " isPlayerQualified=" << (int)m_isPlayerQualified;
	sStream << " showOdometer=" << (int)m_showOdometer;
	sStream << " pte=" << (int)m_pte;
	sStream << " name=\"" << m_name << "\"";
	sStream << " info=\"" << m_info << "\"";
	sStream << " triggerType=\"" << m_triggerType << "\"";
	sStream << " value=" << (long long)m_value;
	sStream << " startTime=" << CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(m_startTimeUtc / 1000);//m_startTimeUtc in milliseconds??
	sStream << " endTime=" << CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(m_endTimeUtc / 1000);//m_startTimeUtc in milliseconds??
	sStream << " countdownStartTime=" << m_countdownStartTime;
	sStream << " countdownEndTime=" << m_countdownEndTime;
	sStream << " percentEligible=" << (double)m_percentEligible;
	sStream << " prizeDescription=" << m_prizeDescription;
	sStream << " awardType=" << AwardType::emap.At(m_awardType);	

	return sStream.str();
}

bool ProgressiveEligibleLevelInfo::Update(const ProgressiveEligibleLevelInfo *progressiveEligibleLevelInfo)
{
	bool updated = false;
	if (progressiveEligibleLevelInfo != nullptr && m_jsonString != progressiveEligibleLevelInfo->GetJSonString())
	{
		// The jason string is changed so assume the eligible level has changed.
		updated = true;

		// Update all the items of this eligible with the one provided
		m_levelId = progressiveEligibleLevelInfo->GetLevelId();
		m_pteStatus = progressiveEligibleLevelInfo->GetPteStatus();
		m_showInfo = progressiveEligibleLevelInfo->ShowInfo();
		m_isEligible = progressiveEligibleLevelInfo->IsEligible();
		m_isPlayerQualified = progressiveEligibleLevelInfo->IsPlayerQualified();
		m_showOdometer = progressiveEligibleLevelInfo->ShowOdometer();
		m_pte = progressiveEligibleLevelInfo->IsPte();
		m_name = progressiveEligibleLevelInfo->GetName();
		m_info = progressiveEligibleLevelInfo->GetInfo();
		m_triggerType = progressiveEligibleLevelInfo->GetTriggerType();
		m_leaderboardUrl = progressiveEligibleLevelInfo->GetLeaderboardUrl();
		m_imageUrl = progressiveEligibleLevelInfo->GetImageUrl();
		m_valueStr = progressiveEligibleLevelInfo->GetValueStr();
		m_value = progressiveEligibleLevelInfo->GetValue();
		if (!IsPlayXGetY() && progressiveEligibleLevelInfo->ShowOdometer() && m_value <= 0)
		{
			m_showOdometer = false;
			LogString(PROGRESSIVE, "Do not show level=%s value=%s", m_name.c_str(), m_valueStr.c_str());
		}
		m_startTimeUtc = progressiveEligibleLevelInfo->GetStartTimeUtc();
		m_endTimeUtc = progressiveEligibleLevelInfo->GetEndTimeUtc();
		m_countdownStartTime = progressiveEligibleLevelInfo->m_countdownStartTime;
		m_countdownEndTime = progressiveEligibleLevelInfo->m_countdownEndTime;
		m_percentEligible = progressiveEligibleLevelInfo->GetPercentEligible();
		m_awardType = progressiveEligibleLevelInfo->GetAwardType();
		m_prizeDescription = progressiveEligibleLevelInfo->GetPrizeDescription();
		m_jsonString = progressiveEligibleLevelInfo->GetJSonString();

		// If the provided list of minors is smaller, remove the extra ones in this list
		if (GetNumMinors() > progressiveEligibleLevelInfo->GetNumMinors())
		{
			m_eligibleMinors.resize(progressiveEligibleLevelInfo->GetNumMinors());
		}

		// Update each minor
		unsigned int count = 0;
		progressiveEligibleLevelInfo->ForEachMinor([&](const ProgressiveEligibleMinorInfo& minor)->bool
		{
			// If we have this minor in our list of minors, update it
			if (count < m_eligibleMinors.size())
			{
				m_eligibleMinors[count] = minor;
			}

			// Add a new minor if we don't have it.
			else
			{
				m_eligibleMinors.push_back(ProgressiveEligibleMinorInfo(minor));
			}
			count++;
			return true;
		});
	}

	return updated;
}

int64_t ProgressiveEligibleLevelInfo::GetLevelId() const
{
	return m_levelId;
}

PteStatus::Type ProgressiveEligibleLevelInfo::GetPteStatus() const
{
	return m_pteStatus;
}

bool ProgressiveEligibleLevelInfo::ShowInfo() const
{
	return m_showInfo;
}

bool ProgressiveEligibleLevelInfo::IsEligible() const
{
	return m_isEligible;
}

bool ProgressiveEligibleLevelInfo::IsPlayerQualified() const
{
	return m_isPlayerQualified;
}

bool ProgressiveEligibleLevelInfo::ShowOdometer() const
{
	return m_showOdometer;
}

void ProgressiveEligibleLevelInfo::DoNotShowOdometer()
{
	m_showOdometer = false;
}

bool ProgressiveEligibleLevelInfo::IsPte() const
{
	return m_pte;
}

bool ProgressiveEligibleLevelInfo::IsScatter() const
{
	const string SCATTER_TRIGGER_TYPE("Scatter");
	return SCATTER_TRIGGER_TYPE == m_triggerType;
}

bool ProgressiveEligibleLevelInfo::DisplayScatterLevel() const
{
	return IsScatter() && m_countdownStartTime != INVALID_VALUE && m_countdownEndTime != INVALID_VALUE;
}

bool ProgressiveEligibleLevelInfo::IsPlayXGetY() const
{
	const string PLAY_X_GET_Y_TRIGGER_TYPE("PlayXGetY");
	return PLAY_X_GET_Y_TRIGGER_TYPE == m_triggerType;
}

bool ProgressiveEligibleLevelInfo::DisplayPlayXGetYLevel() const
{
	return IsPlayXGetY() && !m_eligibleMinors.empty();
}

LPCTSTR ProgressiveEligibleLevelInfo::GetName() const
{
	return m_name.c_str();
}

LPCTSTR ProgressiveEligibleLevelInfo::GetInfo() const
{
	return m_info.c_str();
}

LPCTSTR ProgressiveEligibleLevelInfo::GetTriggerType() const
{
	return m_triggerType.c_str();
}

LPCTSTR ProgressiveEligibleLevelInfo::GetLeaderboardUrl() const
{
	return m_leaderboardUrl.c_str();
}

LPCTSTR ProgressiveEligibleLevelInfo::GetImageUrl() const
{
	return m_imageUrl.c_str();
}

int64_t ProgressiveEligibleLevelInfo::GetValue() const
{
	return m_value;
}

string ProgressiveEligibleLevelInfo::GetValueStr() const
{
	return m_valueStr;
}

int64_t ProgressiveEligibleLevelInfo::GetStartTimeUtc() const
{
	return m_startTimeUtc;
}

int64_t ProgressiveEligibleLevelInfo::GetEndTimeUtc() const
{
	return m_endTimeUtc;
}

double ProgressiveEligibleLevelInfo::GetPercentEligible() const
{
	return m_percentEligible;
}

AwardType::Type ProgressiveEligibleLevelInfo::GetAwardType() const
{
	return m_awardType;
}

string ProgressiveEligibleLevelInfo::GetPrizeDescription() const
{
	return m_prizeDescription;
}

std::string ProgressiveEligibleLevelInfo::GetJSonString() const
{
	return m_jsonString;
}

size_t ProgressiveEligibleLevelInfo::GetNumMinors() const
{
	return m_eligibleMinors.size();
}

const ProgressiveEligibleMinorInfo* ProgressiveEligibleLevelInfo::operator[](size_t index) const
{
	return index < m_eligibleMinors.size() ? &m_eligibleMinors[index] : nullptr;
}

void ProgressiveEligibleLevelInfo::ForEachMinor(function<bool(const ProgressiveEligibleMinorInfo&)> callback) const
{
	if (callback != nullptr)
	{
		bool _continue = true;

		for (auto it = m_eligibleMinors.begin(); _continue && it != m_eligibleMinors.end(); ++it)
		{
			_continue = callback(*it);
		}
	}
}

void ProgressiveEligibleLevelInfo::SetScatterStartTime(int64_t time)
{
	m_countdownStartTime = time;
}

void ProgressiveEligibleLevelInfo::SetScatterEndTime(int64_t time)
{
	m_countdownEndTime = time;
}

int64_t ProgressiveEligibleLevelInfo::GetScatterStartTime() const
{
	return m_countdownStartTime;
}

int64_t ProgressiveEligibleLevelInfo::GetScatterEndTime() const
{
	return m_countdownEndTime;
}

void ProgressiveEligibleLevelInfo::ResetScatterDuration()
{
	m_remainingDuration = 0;
}

string ProgressiveEligibleLevelInfo::GetSubscriptionOdometerData(const std::string &dynamicContentPath)
{
	string subscriptionData;

	if (m_value <= 0 && !IsPlayXGetY())
	{
		//Show logo for negative value level
		JsonWriteStreamString writeStream;
		JsonWriter writer(writeStream);
		writer.StartObject();
		writer.Key(pKeyBonusLevelType);			writer.String("");
		writer.EndObject();
		subscriptionData = writeStream.m_str;
	}
	else if (IsScatter())
	{
		JsonWriteStreamString writeStream;
		JsonWriter writer(writeStream);
		writer.StartObject();
			if (!dynamicContentPath.empty())
			{
				writer.Key(pKeyDynamicContentPath);		writer.String(dynamicContentPath);
			}
			writer.Key(pKeyBonusLevelType);			writer.String(m_triggerType);
			writer.Key(pKeyBonusLevelData);
			writer.StartObject();
				if (m_remainingDuration == 0)
				{
					m_remainingDuration = (int)((time_t)m_countdownEndTime - CUtilities::GetCurrentTime());
				}
				int hours((int)m_remainingDuration / (MINUTES_PER_HOUR * SECONDS_PER_MINUTE));
				int minutes(((int)m_remainingDuration % (MINUTES_PER_HOUR * SECONDS_PER_MINUTE)) / SECONDS_PER_MINUTE);
				int seconds((int)m_remainingDuration % SECONDS_PER_MINUTE);
				writer.Key(pKeyLevelName);	writer.String(m_name);
				writer.Key(pKeyValue);		writer.String(FormatString("%02d:%02d:%02d", hours, minutes, seconds));
			writer.EndObject();
		writer.EndObject();
		subscriptionData = writeStream.m_str;
	}
	else if (IsPlayXGetY())
	{
		JsonWriteStreamString writeStream;
		JsonWriter writer(writeStream);
		writer.StartObject();
			if (!dynamicContentPath.empty())
			{
				writer.Key(pKeyDynamicContentPath);		writer.String(dynamicContentPath);
			}
			writer.Key(pKeyBonusLevelType);		writer.String(m_triggerType);
			writer.Key(pKeyBonusLevelData);
				writer.StartObject();
					writer.Key(pKeyLevelName);	writer.String(m_name);
					writer.Key(pKeyValue);		
					writer.StartObject();
						writer.Key(pKeyValue);	writer.String(m_valueStr);
						writer.Key(pKeyMinors);
						writer.StartArray();
						for (const ProgressiveEligibleMinorInfo& eligibleMinor : m_eligibleMinors) // access by const reference
						{
							writer.StartObject();
							writer.Key(pKeySequence);			writer.Int(eligibleMinor.GetSequence());
							writer.Key(pKeyName);				writer.String(eligibleMinor.GetName());
							writer.Key(pKeyEligible);			writer.Bool(eligibleMinor.IsEligible());
							writer.Key(pKeyPercentEligible);	writer.Double(eligibleMinor.GetPercentEligible());
							writer.EndObject();
						}
						writer.EndArray();
					writer.EndObject();
				writer.EndObject();
		writer.EndObject();
		subscriptionData = writeStream.m_str;
	}
	else
	{
		JsonWriteStreamString writeStream;
		JsonWriter writer(writeStream);
		writer.StartObject();
			if (!dynamicContentPath.empty())
			{
				writer.Key(pKeyDynamicContentPath);		writer.String(dynamicContentPath);
			}
			writer.Key(pKeyBonusLevelType);			writer.String(m_triggerType);
			writer.Key(pKeyBonusLevelData);
			writer.StartObject();
				writer.Key(pKeyLevelName);			writer.String(m_name);
				writer.Key(pKeyValue);				writer.String(m_valueStr);
				writer.Key(pKeyPrizeDescription);	writer.String(m_prizeDescription);
			writer.EndObject();
		writer.EndObject();
		subscriptionData = writeStream.m_str;
	}

	return subscriptionData;
}