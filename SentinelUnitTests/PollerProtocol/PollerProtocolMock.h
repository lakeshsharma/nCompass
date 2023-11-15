#pragma once

#include "gmock/gmock.h"
#include "PollerProtocol/PollerProtocol.h"

using namespace std;

class CPollerProtocolMock : public CPollerProtocol
{
public:
	CPollerProtocolMock(const CPairingInformation& mobilePairingInfo, CQueueList &scfToInternalsQueue);
	~CPollerProtocolMock();

	MOCK_METHOD2(AddToPollerQueue, void (CPollerMessage*, int));
	MOCK_METHOD0(ConnectToPoller, void());

    void AddToPollerQueueMocked(CPollerMessage *pollerMsg, int priority);

	//Accessor function
    CPollerMessage *GetPollerMsg();

	void ConnectToPollerMocked(void);
	bool IsConnectToPollerCalled();

private:
    CPollerMessage *m_pollerMsg;
	bool m_IsConnectToPollerCalled;
};
