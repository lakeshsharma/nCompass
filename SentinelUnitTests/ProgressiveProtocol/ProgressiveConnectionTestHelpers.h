
#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveConnection.h"
#include "ProgressiveProtocol/ProgressiveProtocol.h"
#include <activemq/commands/ActiveMQBytesMessage.h>
#include "Platform.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace cms;
using namespace activemq;
using namespace activemq::util;
using namespace activemq::commands;

namespace SentinelUnitTests
{
	static const char* pValidSystemId = "a35ef42d-8995-4060-befe-f09e75210a56";
	/// \class FakeProgressiveProtocol
	/// \brief A fake progressive protocol needed to be able to call the existing
	/// 	   ProgressiveConnection class and pass data to it.
	/// 	    This class cannot be inherited..
	class FakeProgressiveProtocol final : public CProgressiveDataConsumer
	{
		//Stores the levels broadcasted after ProgressiveLevelBroadcast is called.
		CProgressiveLevelBroadcast* pLevelBroadcast;
		CProgressiveJackpotAnnouncement* pJackpotAnnouncement;
		CProgressiveTotalizerAnnouncement* pTotalizerAnnouncement;

		void SendMsgToInternals(CInternalMessage* pMsg, int priority = QUEUE_PRIORITY_NORMAL){};
		void QueueToOneLink(ProgressiveQueueToOneLink* pMsg, int priority = QUEUE_PRIORITY_HIGH){};
		void ConnectionUpdate(const CProgressiveConnectionInfo& connectionInfo){};
		void ProgressiveHitSentStatusUpdate(const CEGMAward& award, bool wasSent){};
		void SendJackpotPaidRequestStatusUpdate(const CEGMAward& award, bool wasSent){};
		void SendConfigFileDownload(CConfigFilePackage* configFilePackage) {};
		void SendCampaignDownload(CCampaignPackage* campaignPackage) {};
		void SendSkinDownload(CSkinPackage* skinPackage) {};
		void SendScheduleEnd(int64_t levelId, const std::string& levelName) {};
		void HostUpdate(const CProgressiveHost* host){};
		void GetProgressiveGameInfo(){};
		void JackpotHit(CEGMAward* egmAward){};
		void SendRemoteReboot(){};
		void SendPromoGameRequest(CPromoGameRequest* promoGameRequest){};
		void ProgressiveJackpotAnnouncement(CProgressiveJackpotAnnouncement* pAnnouncement) 
		{
			pJackpotAnnouncement = pAnnouncement;
		};
		void ProgressiveTotalizerAnnouncement(CProgressiveTotalizerAnnouncement* pAnnouncement) 
		{
			pTotalizerAnnouncement = pAnnouncement;
		};
		void ClaimableAward(CProgressiveClaimableAward* pClaimableAward){};
		void SendFirmwareDownload(CFirmwarePackage* firmwarePackage) {};
		void ProgressiveLevelBroadcast(CProgressiveLevelBroadcast* pLevelBroadcast)
		{
			this->pLevelBroadcast = pLevelBroadcast;
		};
		void SendAnnouncementsConfiguration(CProgressiveAnnouncementsInfo* pAnnouncmentsConfiguration) {};
		void SendProgressiveLevelsInfo(ProgressiveLevelsInfo* pProgressiveLevelsInfo){};
		void SendProgressiveConfiguration(ProgressiveLevelsInfo* pProgressiveLevelsInfo, CProgressiveAnnouncementsInfo* pAnnouncmentsConfiguration, CFirmwarePackage* pFirmwarePackage, CCampaignConfig* pCampaignConfig, CSkinConfig* pSkinConfig) {};
		void ProgressiveBonusingAvailable(CProgressiveBonusingAvailable* progresiveBonusingAvailable, CProgressiveEligibleLevelsInfo* progressiveEligibleLevelsInfo, ProgressivePatronSessionInfo* progressivePatronSessionInfo){};
		void ProgressiveWinnerPick(CProgressiveWinnerPick* pWinnerPick){};
		void ProgressiveWinnerPicked(CProgressiveWinnerPicked* pWinnerPicked){};
		void EgmStateRequest(bool egmStateRequest, int64_t jackpotId){};

	public:

		/// \fn FakeProgressiveProtocol()
		/// \brief Initializes a new instance of the <see cref="ProgressiveConnectionDecryptionTests"/> class
		FakeProgressiveProtocol() : CProgressiveDataConsumer() {}

		/// \fn CProgressiveLevelBroadcast* GetProgLevels()
		/// \brief Gets prog levels
		/// \returns Null if it fails, else the prog levels.
		CProgressiveLevelBroadcast* GetProgLevels()
		{
			return this->pLevelBroadcast;
		}

		CProgressiveJackpotAnnouncement* GetJackpotAnnouncement()
		{
			return this->pJackpotAnnouncement;
		}

		CProgressiveTotalizerAnnouncement* GetTotalizerAnnouncement()
		{
			return this->pTotalizerAnnouncement;
		}
	};

	/// \class FakeProgressiveMulticastFrame
	/// \brief A fake progressive multicast frame.
	class FakeProgressiveMulticastFrame : public ProgressiveMulticastFrame
	{
	public:
		FakeProgressiveMulticastFrame(const std::string& id) : ProgressiveMulticastFrame(id)
		{}
	};

	/// \class FakeProgressiveConnection
	/// \brief A fake progressive connection.
	class FakeProgressiveConnection : public CProgressiveConnection
	{
	public:
		FakeProgressiveConnection(CProgressiveDataConsumer& dataConsumer, CProgressiveConfig* progConfig,
			const std::string& paltronicsCertificatePathname,
			const std::string& firmwareVersion = "1",
			IOperatingSystem& operatingSystem = COperatingSystem::m_operatingSystem) : 
			CProgressiveConnection(dataConsumer,progConfig,paltronicsCertificatePathname,firmwareVersion,operatingSystem)
		{
			CConnection connection;
			this->m_messageFactory = new CProgressiveMessageFactory(connection, firmwareVersion);
		}

		/// \fn void CallOnMultiCast(ProgressiveMulticastFrame& frame)
		/// \brief Calls the OnMulticastMessage on ProgressiveConnection class
		/// 	   So we can test the decryption logic.
		/// \param [in,out] frame The frame.
		void CallOnMultiCast(ProgressiveMulticastFrame& frame)
		{
			this->OnMulticastMessage(frame);
		}
	};
}