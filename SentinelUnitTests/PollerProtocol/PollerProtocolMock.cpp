#include "stdafx.h"
#include "PollerProtocolMock.h"


CPollerProtocolMock::CPollerProtocolMock(const CPairingInformation& mobilePairingInfo, CQueueList &scfToInternalsQueue) :
	CPollerProtocol(mobilePairingInfo, scfToInternalsQueue)
{
	m_IsConnectToPollerCalled = false;
}


CPollerProtocolMock::~CPollerProtocolMock()
{
}

void CPollerProtocolMock::AddToPollerQueueMocked(CPollerMessage *pollerMsg, int priority)
{
    m_pollerMsg = pollerMsg;
}

CPollerMessage *CPollerProtocolMock::GetPollerMsg()
{
    return m_pollerMsg;
}

void CPollerProtocolMock::ConnectToPollerMocked(void)
{
	m_IsConnectToPollerCalled = true;
}

bool CPollerProtocolMock::IsConnectToPollerCalled()
{
	return m_IsConnectToPollerCalled;
}
