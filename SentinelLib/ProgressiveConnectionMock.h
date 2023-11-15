#pragma once

#include "gmock/gmock.h"
#include "ProgressiveProtocol\ProgressiveKeys.h"
#include "ProgressiveProtocol\ProgressiveConfig.h"
#include "ProgressiveProtocol\ProgressiveConnection.h"
#include "ProgressiveProtocol\ProgressiveDataConsumer.h"
// #include "activemq\\commands\\ActiveMQBytesMessage.h"//TODO-PORT Move third-party includes to stdafx.h

class CProgressiveConnectionMock : public CProgressiveConnection
{
public:
	std::string m_lastMessageBodyToOneLink;

	CProgressiveConnectionMock(
		CProgressiveDataConsumer &dataConsumer,
		CProgressiveConfig *progConfig,
		const CString& paltronicsCertificatePath,
		IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

	~CProgressiveConnectionMock();

	MOCK_METHOD0(SendCardStatus, void());
	MOCK_METHOD0(IsTimeForConnectionAttempt, bool());
	MOCK_METHOD0(IsTimeForAddressCheck, bool());
	MOCK_METHOD0(AddressCheck, void());
	MOCK_METHOD0(IsTimeForRediscovery, bool());
	MOCK_METHOD0(WatchdogCheckIn, void());
	MOCK_METHOD0(CreateBytesMessage, cms::BytesMessage*());
	MOCK_METHOD0(SendCardOutNoPlayer, void());
	MOCK_METHOD1(GetMessageBody, std::string(const cms::BytesMessage*));
	MOCK_METHOD3(InternalSendMessageToOneLink, bool(ToOneLink::Type, const char*, const std::string&));
	MOCK_METHOD2(SendMessageToOneLink, bool(ToOneLink::Type, const std::string&));
	MOCK_METHOD2(QueueToOneLink, void(ProgressiveQueueToOneLink*, int));

	ProgressiveConnectionState GetConnectionState();
	bool IsMessageFactoryCreated();
	bool IsGamesCreated();
	cms::BytesMessage* CreateMockBytesMessage(const char* pMessageType, const std::string& messageBody);
	bool MockInternalSendMessageToOneLink(ToOneLink::Type messageType, const char* pMessageType, const std::string& messageBody);

	// Use these method(s) to call base (parent) methods.
	void BaseSendCardStatus();
	bool BaseSendMessageToOneLink(ToOneLink::Type messageType, const std::string& messageBody);

private:
	void ConnectToConcentrator(std::string url);
};

