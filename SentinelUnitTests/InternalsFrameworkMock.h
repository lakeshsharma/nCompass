#pragma once

#include "Sentinel.h"
#include "gmock/gmock.h" 

namespace SentinelNativeUnitTests
{
	class InternalsProcessingForUiUnitTests;
	class InternalsProcessingForGCFUnitTests;
}

class CInternalsFrameworkMock : public CInternalsFramework {
 public:

    CInternalsFrameworkMock(IProtocolManager *protocolManager, time_t startupTime, bool isLcd, const std::string& firmwareVersion);
    virtual ~CInternalsFrameworkMock();

    MOCK_METHOD0(EnsureLatestCampaign, void());
    MOCK_METHOD0(EnsureLatestEventPackageM, void());
    //MOCK_METHOD2(GetMediaFiles, void(MediaFileType mediaFileType, CFileList *fileList));
    MOCK_METHOD0(InitializePlayer, void ());

    void Initialize( void );

	CJackpot *GetJackpot();
	void SetJackpot(CJackpot *jackpot);

	CPlayer *GetPlayer();
	void SetPlayer(CPlayer *player);

	void ProcessPersonalBankerInfo(const CPersonalBankerInfo *pbInfo, const CConfig &config, bool gameOnline, bool bIsEFT);

	CPersonalBankerInfo* GetPBInfoCopy();
	bool HasPBTransfer();

	void SetMobilePBTBalances(DWORD cashable, DWORD cashPromo, DWORD nonCashablePromo);
	void SendMCFundsTransferBalanceToPollerProtocolMocked();
	__int64 CalculateNonNCEPPlayedMocked(__int64 coinIn, __int64 ncepPlayed);

	//To give Access to UT cases.
	friend class SentinelNativeUnitTests::InternalsProcessingForUiUnitTests;
	friend class SentinelNativeUnitTests::InternalsProcessingForGCFUnitTests;
};
