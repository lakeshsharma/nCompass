#pragma once


#include "ProgressiveProtocol/ProgressiveKeys.h"
#include "ProgressiveProtocol/ProgressiveLevel.h"
#include "ProgressiveProtocol/ProgressiveLevelInfo.h"
#include "ProgressiveProtocol/ProgressiveLevelsInfo.h"
#include "ProgressiveProtocol/ProgressiveLevelManager.h"
#include "OptionalVal.h"

namespace SentinelNativeUnitTests
{
	namespace ProgressiveLevelTestHelpers
	{
		const uint64_t MillisecondsPerSecond = 1000;
		namespace Intervals
		{
			const uint64_t PerMillisecond = 1000000 / 100;
			const uint64_t PerSecond = PerMillisecond * MillisecondsPerSecond;
			const uint64_t PerMinute = PerSecond * 60;
			const uint64_t PerHour = PerMinute * 60;
		}		
				
		class _ICardConfig5
		{
		public:
			struct _ICardGameLevel
			{
				int64_t LevelId;
				int LevelSequence;
			};

			struct _ICardGame5
			{
				int GameNumber;
				double Denomination;
				int MaxBetCredits;
				double TheoPercentage;
				const char* GameName;
				const char* Paytable;
				bool QueryCards;
				std::vector<std::unique_ptr<_ICardGameLevel>> Levels;
			};

			struct _MessageChannel
			{
				//const char* Url;
				const char* Destination;
				const char* MulticastDestination;
				//const char* EncryptionKey;
			};

			struct _ICardLevel
			{
				int64_t Id;
				int64_t Version;
				ProgressiveLevelType::Type Type;
				const char* pCardLevelType;	// ProgressiveKeys ProgressiveLevelTypeStr
				_MessageChannel Channel;
				const char* Name;
				int MediaMinSeconds;
				int MediaMaxSeconds;
			};

			struct _File
			{
				const char* FileName;
				const char* MD5;
			};

			struct _PlaylistItem
			{
				_File Content;
				bool Enabled;
				time_t BeginDate;
				time_t EndDate;
				const char* BeginTime;
				const char* EndTime;
				bool ActiveSunday;
				bool ActiveMonday;
				bool ActiveTuesday;
				bool ActiveWednesday;
				bool ActiveThursday;
				bool ActiveFriday;
				bool ActiveSaturday;
				LONG64 SequenceId;
				const char* AudioAdjustment;
				bool Mute;
				bool Image;
				LONG64 Duration;
			};

			struct _Playlist
			{
				const char* Name;
				std::unique_ptr<std::vector<std::unique_ptr<_PlaylistItem>>> Items; // optional
				_Playlist *pParentPlaylist;
				bool Shuffle;
				//const char* Description;
			};

			struct _EventChannel
			{
				const char* Destination;
				const char* MulticastDestination;
			};

			int64_t										SourceDeviceId;
			int64_t										SourceDeviceConfigVersion;
			const char*									SourceAsset;
			int64_t										Timestamp;
			const char*									MessageId;
			int64_t										DeviceId;
			int64_t										DeviceConfigVersion;
			const char*									DeviceAddress;
			const char*									OneLinkSystemId;
			_EventChannel								EventChannel;
			const char*									AssetNumber;
			const char*									BankName;
			const char*									TimeZone;
			std::vector<std::unique_ptr<_ICardGame5>>	Games;
			std::vector<std::unique_ptr<_ICardLevel>>	CardLevels;
			const char*									ProtocolType; // pValSAS
			int											ProgressiveGroupId;
			const char*									PaymentMethod;
			OptionalVal<bool>							UseAFTLockOnTransfers;
			bool										MeterBankLevel;
			bool										MeterMachineOnly;
			OptionalVal<std::string>					ContentHttpUrl;
			std::unique_ptr<_Playlist>					Playlist;
			// std::vector<std::unique_ptr<_IMMLevel>>	IMMLevels;

		public:
			_ICardConfig5(int dataSet = 1);
			_ICardConfig5(int ProgressiveGroupId, bool useAftOnTransfers);

			void SetUseAftOnTransfers(bool useAftOnTransfers);

			void Create1();
			void Create2();

			void CreateJsonDoc(rapidjson::Document& jsonDoc);
			std::string CreateJson() const;
			void CreateLevelsInfo(shared_ptr<ProgressiveLevelsInfo>& levelsInfo, const DWORD* pReceivedTickCount = nullptr, const SYSTEMTIME* pReceivedTime = nullptr);
			std::shared_ptr<ProgressiveLevelManager> CreateLevelManager(const DWORD* pReceivedTickCount = nullptr, const SYSTEMTIME* pReceivedTime = nullptr) ;
			std::shared_ptr<ProgressiveLevelManager> CreateLevelManager(DWORD receivedTickCount, const SYSTEMTIME* pReceivedTime = nullptr) 
			{
				return CreateLevelManager(&receivedTickCount, pReceivedTime);
			}

			void Verify(ProgressiveLevelsInfo& levels);
			void Verify(ProgressiveLevelManager& levels);

		};

		struct LevelData
		{
			int line;
			DWORD updatedTickCount;
			__int64	id;
			ProgressiveLevelType::Type type;
			const char* pName;
			const char* pUrl;
			size_t	numSasLevels;
			BYTE	sasLevels[3];
			size_t	expectedNumSasLevels;

			void Verify(int line, CProgressiveLevel* pLevel) const;
			std::shared_ptr<CProgressiveLevel> CreateProgressiveLevel() const;
		};
	}
}
