#include "stdafx.h"
#include "PersonalBankerAutoUpload.h"

CPersonalBankerAutoUpload::CPersonalBankerAutoUpload(void) : 
	m_pbtAutoInProcess(false),
	m_pbtAutoUploadPending(false),
	m_pbtAutoStartedOnce(false),
	m_startUploadOver(false),
	m_pbtOrigCard(NULL),
	m_pbtUploadTimerStart(0),
	m_pbtUploadTimerPeriod(0),
	m_pendingAutoNCEPAmount(0),
	m_pendingAutoWATAmount(0),
	m_pendingAutoCEPAmount(0)
{
}

CPersonalBankerAutoUpload::~CPersonalBankerAutoUpload(void)
{
	CleanUp();
}

CPersonalBankerAutoUpload::CPersonalBankerAutoUpload(const CPersonalBankerAutoUpload &rhs)
{
	m_pbtOrigCard = NULL;
	CopyObj(rhs);
}

CPersonalBankerAutoUpload& CPersonalBankerAutoUpload::operator=(const CPersonalBankerAutoUpload &rhs)
{
	if( this != &rhs)
	{
		CleanUp();
		CopyObj(rhs);
	}
	return *this;
}

void CPersonalBankerAutoUpload::CleanUp()
{
	DeleteCard();
}

void CPersonalBankerAutoUpload::CopyObj(const CPersonalBankerAutoUpload& rhs)
{
	m_pbtAutoInProcess = rhs.m_pbtAutoInProcess;
	m_pbtAutoUploadPending = rhs.m_pbtAutoUploadPending;
	m_pbtAutoStartedOnce = rhs.m_pbtAutoStartedOnce;
	m_startUploadOver = rhs.m_startUploadOver;	
	if(m_pbtOrigCard == NULL && rhs.m_pbtOrigCard != NULL)
	{
		m_pbtOrigCard = new CCard(*(const_cast<CPersonalBankerAutoUpload&>(rhs).GetCard()));
	}
	m_pbtUploadTimerStart = rhs.m_pbtUploadTimerStart;
	m_pbtUploadTimerPeriod = rhs.m_pbtUploadTimerPeriod;
	m_pendingAutoNCEPAmount = rhs.m_pendingAutoNCEPAmount;
	m_pendingAutoWATAmount = rhs.m_pendingAutoWATAmount;
	m_pendingAutoCEPAmount = rhs.m_pendingAutoCEPAmount;
}

void CPersonalBankerAutoUpload::DeleteCard()
{
	if(m_pbtOrigCard != NULL)
	{
		delete m_pbtOrigCard;
		m_pbtOrigCard = NULL;
	}
}

void CPersonalBankerAutoUpload::SetAutoInProcess(bool autoInProcess)
{
	m_pbtAutoInProcess = autoInProcess;
}

bool CPersonalBankerAutoUpload::IsAutoInProcess() const
{
	return m_pbtAutoInProcess;
}

void CPersonalBankerAutoUpload::SetAutoUploadPending(bool autoNcepOutPending)
{
	m_pbtAutoUploadPending = autoNcepOutPending;
}

bool CPersonalBankerAutoUpload::IsAutoUploadPending() const
{
	return m_pbtAutoUploadPending;
}

void CPersonalBankerAutoUpload::SetAutoStartedOnce(bool autoStartedOnce)
{
	m_pbtAutoStartedOnce = autoStartedOnce;
}

bool CPersonalBankerAutoUpload::IsAutoStartedOnce() const
{
	return m_pbtAutoStartedOnce;
}

void CPersonalBankerAutoUpload::SetStartUploadOver(bool startUploadOver)
{
	m_startUploadOver = startUploadOver;
}

bool CPersonalBankerAutoUpload::IsStartUploadOver() const
{
	return m_startUploadOver;
}

CCard* CPersonalBankerAutoUpload::CreateCard()
{
	DeleteCard();
	m_pbtOrigCard = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetUTCTime());
	return m_pbtOrigCard;
}

CCard* CPersonalBankerAutoUpload::CreateCard(const CCard* card)
{
	if(card != NULL)
	{
		DeleteCard();
		m_pbtOrigCard = new CCard(*card);
	}
	return m_pbtOrigCard;
}

CCard* CPersonalBankerAutoUpload::GetCard()
{
	return m_pbtOrigCard;
}

void CPersonalBankerAutoUpload::SetUploadTimerExpired()
{
	m_pbtUploadTimerPeriod = 0;
}

void CPersonalBankerAutoUpload::SetUploadDelay(DWORD tickCount, DWORD delayPeriod)
{
	m_pbtUploadTimerStart = tickCount;
	m_pbtUploadTimerPeriod = delayPeriod;
}

bool CPersonalBankerAutoUpload::IsUploadTimerExpired(DWORD tickCount) const
{
	DWORD ticksSinceStart = tickCount - m_pbtUploadTimerStart;
	bool expired = ticksSinceStart >= m_pbtUploadTimerPeriod;
	return expired;
}

void CPersonalBankerAutoUpload::SetPendingAutoNcepAmount(DWORD amount)
{
	m_pendingAutoNCEPAmount = amount;
}

DWORD CPersonalBankerAutoUpload::GetPendingAutoNcepAmount() const
{
	return m_pendingAutoNCEPAmount;
}

void CPersonalBankerAutoUpload::SetPendingAutoWatAmount(DWORD amount)
{
	m_pendingAutoWATAmount = amount;
}

DWORD CPersonalBankerAutoUpload::GetPendingAutoWatAmount() const
{
	return m_pendingAutoWATAmount;
}

void CPersonalBankerAutoUpload::SetPendingAutoCepAmount(DWORD amount)
{
	m_pendingAutoCEPAmount = amount;
}

DWORD CPersonalBankerAutoUpload::GetPendingAutoCepAmount() const
{
	return m_pendingAutoCEPAmount;
}

DWORD CPersonalBankerAutoUpload::GetPendingCashToAutoUpload() const
{
	return m_pendingAutoWATAmount + m_pendingAutoCEPAmount;
}

bool CPersonalBankerAutoUpload::IsAutoUploadInProgress() const
{
	return m_pbtAutoUploadPending || m_pbtAutoInProcess;
}
