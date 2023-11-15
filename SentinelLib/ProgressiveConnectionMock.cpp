#include "stdafx.h"
using namespace std;
#include "ProgressiveConnectionMock.h"
#include "activemq\\commands\\ActiveMQBytesMessage.h"

using namespace activemq::commands;
using namespace cms;
using namespace std;

CProgressiveConnectionMock::CProgressiveConnectionMock(CProgressiveDataConsumer &dataConsumer,
	CProgressiveConfig *progConfig,
	const CString& paltronicsCertificatePath,
	IOperatingSystem &operatingSystem) :
	CProgressiveConnection(dataConsumer, progConfig, paltronicsCertificatePath, operatingSystem)
{
}

CProgressiveConnectionMock::~CProgressiveConnectionMock()
{
	UpdateConnectionState(Disconnected);
}

void CProgressiveConnectionMock::ConnectToConcentrator(string url)
{
	if (NULL != m_messageFactory)
	{
		m_connectionInfo.SetConnected();
		UpdateConnectionState(Connected);
	}
}

BytesMessage* CProgressiveConnectionMock::CreateMockBytesMessage(const char* pMessageType, const string& messageBody)
{
	return CProgressiveConnection::CreateBytesMessage(pMessageType, messageBody);
}

ProgressiveConnectionState CProgressiveConnectionMock::GetConnectionState()
{
	return m_connectionInfo.GetState();
}

bool CProgressiveConnectionMock::IsMessageFactoryCreated()
{
	return NULL != m_messageFactory;
}

bool CProgressiveConnectionMock::IsGamesCreated()
{
	return NULL != m_games;
}

bool CProgressiveConnectionMock::MockInternalSendMessageToOneLink(
	ToOneLink::Type messageType,
	const char* pMessageType,
	const string& messageBody)
{
	m_lastMessageBodyToOneLink = messageBody;
	return true;
}

void CProgressiveConnectionMock::BaseSendCardStatus()
{ 
	return CProgressiveConnection::SendCardStatus();
}

bool CProgressiveConnectionMock::BaseSendMessageToOneLink(ToOneLink::Type messageType, const std::string& messageBody)
{
	return CProgressiveConnection::SendMessageToOneLink(messageType, messageBody);
}
