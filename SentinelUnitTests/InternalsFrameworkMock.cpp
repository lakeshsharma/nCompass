#include "InternalsFrameworkMock.h"

CInternalsFrameworkMock::CInternalsFrameworkMock(IProtocolManager *protocolManager, time_t startupTime, bool isLcd, const std::string& firmwareVersion) 
    : CInternalsFramework(protocolManager, startupTime, isLcd, firmwareVersion)
{
	m_scfToInternalsQueue = nullptr;
	m_gcfToInternalsQueue = nullptr;
	m_uiToInternalsQueue = nullptr;
	m_progressiveToInternalsQueue = nullptr;
	m_transLog = nullptr;
	m_player = nullptr;
	Initialize();
}

CInternalsFrameworkMock::~CInternalsFrameworkMock() 
{

}

void CInternalsFrameworkMock::Initialize( void )
{
    if (!m_scfToInternalsQueue) 
    {
        m_scfToInternalsQueue = new CQueueList(SCF2INT_MessageQueueLogIdentifier);
    }
	
	if (!m_gcfToInternalsQueue)
	{
		m_gcfToInternalsQueue = new CQueueList(GCF2INT_MessageQueueLogIdentifier);
	}

	if (!m_uiToInternalsQueue)
	{
		m_uiToInternalsQueue = new CQueueList(SUI2INT_MessageQueueLogIdentifier);
	}

	if (!m_progressiveToInternalsQueue)
	{
		m_progressiveToInternalsQueue = new CInternalMessageQueueList(PROGINT_MessageQueueLogIdentifier);
	}

	if ( !m_transLog) 
	{
        m_transLog = new CLogs( MEMORY_NONE, LOG_TRANS, TRANSACTION_LOG);
    }

    if (!m_player)
    {
        CPlayer *volatile player = new CPlayer(m_config, m_EGMConfig);
        m_player = player;
    }

}

CJackpot* CInternalsFrameworkMock::GetJackpot()
{
	return m_jackpot;
}

void CInternalsFrameworkMock::SetJackpot(CJackpot *jackpot)
{
	m_jackpot = jackpot;
}

CPlayer* CInternalsFrameworkMock::GetPlayer()
{
	return m_player;
}

void CInternalsFrameworkMock::SetPlayer(CPlayer *player)
{
	m_player = player;
}

void CInternalsFrameworkMock::ProcessPersonalBankerInfo(const CPersonalBankerInfo *pbInfo, const CConfig &config, bool gameOnline, bool bIsEFT)
{
	m_config = config;
	m_personalBanker->CreateTransfer(CUtilities::GetCurrentTime());
	m_personalBanker->SetState(PBStateInfoReceived);
	m_personalBanker->ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);
}

CPersonalBankerInfo* CInternalsFrameworkMock::GetPBInfoCopy()
{
	return m_personalBanker->GetInfoCopy();
}

bool CInternalsFrameworkMock::HasPBTransfer()
{
	return m_personalBanker->HasTransfer();
}

void CInternalsFrameworkMock::SetMobilePBTBalances(DWORD cashable, DWORD cashPromo, DWORD nonCashablePromo)
{
	CMobilePBTSessionInfo &mobilePBTInfo = CMobilePBTSessionInfo::Instance();
	mobilePBTInfo.SetBalances(cashable, cashPromo, nonCashablePromo);
}

void CInternalsFrameworkMock::SendMCFundsTransferBalanceToPollerProtocolMocked()
{
	m_config.SetMobileConnectCommunicationEnabled(true);
	SendMCFundsTransferBalanceToPollerProtocol();
}

__int64 CInternalsFrameworkMock::CalculateNonNCEPPlayedMocked(__int64 coinIn, __int64 ncepPlayed)
{
	return CalculateNonNCEPPlayed(coinIn, ncepPlayed);
}
