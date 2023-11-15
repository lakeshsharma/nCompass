#pragma once

#include "ProgressiveKeys.h"
#include "MediaPackageManager.h"

const int64_t InvalidProgressiveLevelId = 0;
const BYTE    MinSasLevel     = 1;              // Zero is used for bonusing; 1-32 for SAS IDs.
const BYTE    MaxSasLevels    = 33;             // Zero is used for bonusing; 1-32 for SAS IDs.
const BYTE    InvalidSasLevel = 255;
const bool    DefaultMulticastActive = true; // true to indicate adequate multicast performance, until proven otherwise.

namespace SentinelNativeUnitTests
{
class ProgressiveLevelTests;
class ProgressiveLevelInfoTests;
class ProgressiveLevelsInfoTests;
namespace ProgressiveLevelTestHelpers
{
class _ICardConfig5;
struct LevelData;
}
//	class WebDiagnostics_Progressive;
//	class EGMAwardManager;
//	class ProgressiveLevelTests;
//	class ProgressiveLevelsTests;
//	class ProgressiveLevelForwardTests;
//	class ProgressiveLevelBroadcastTests;
}

class CMediaPackageManager;
class ProgressiveLevelsInfo;
class ProgressiveLevelManager;
class CProgressiveLevel;
class CSlotsContentPackage;

class CMinorAward
{
	friend ProgressiveLevelManager;
	friend CMediaPackageManager;
protected:
	int64_t m_minorId;
	int64_t m_sequence;
	std::string m_name;
	std::shared_ptr<CSlotsContentPackage> m_promoGameContentPackage;
	std::shared_ptr<CSlotsContentPackage> m_celebrationContentPackage;

	// None of these parameters are checked since only CSlotsContentPackage class calls it with guaranteed valid data.
public:
	CMinorAward(const rapidjson::Value& minorAwardValue, int64_t levelId);

protected:
	CMinorAward(const CMinorAward& other) = delete;
	void operator=(const CMinorAward& other) = delete;
private:
};

class ProgressiveLevelInfo
{
protected:
   int64_t m_id;
   ProgressiveLevelType::Type m_type;
   std::string m_name;
   std::string m_url;
   std::vector<BYTE> m_sasLevels;
   std::vector<int> m_queryCardsGames;
   std::vector<std::unique_ptr<CSlotsContentPackage>> m_slotsContentPackages;
   std::vector<std::unique_ptr<CMinorAward>> m_minorAwards;

   friend class CMediaPackageManager;

   // None of these parameters are checked since only ProgressiveLevelsInfo class calls it with guaranteed valid data.
public:
   ProgressiveLevelInfo(int64_t levelId, ProgressiveLevelType::Type type, const rapidjson::Value& level);
   virtual ~ProgressiveLevelInfo();
protected:
   ProgressiveLevelInfo(const ProgressiveLevelInfo& other) = delete;
   void operator=(const ProgressiveLevelInfo& other) = delete;

   void AddQueryCardsGame(int gameNumber);
   void AppendQueryCardsSet(std::set<DWORD>& queryCardsSet) const;

   void AddSasLevel(BYTE sasLevel);
   void SortSASLevels();
   void AddContentPackage(const rapidjson::Value& level, SlotsContentType::Type type);

private:
   friend ProgressiveLevelsInfo;
   friend ProgressiveLevelManager;
   friend CProgressiveLevel;
   friend SentinelNativeUnitTests::ProgressiveLevelTests;
   friend SentinelNativeUnitTests::ProgressiveLevelInfoTests;
   friend SentinelNativeUnitTests::ProgressiveLevelsInfoTests;
   friend SentinelNativeUnitTests::ProgressiveLevelTestHelpers::_ICardConfig5;
   friend SentinelNativeUnitTests::ProgressiveLevelTestHelpers::LevelData;
};
