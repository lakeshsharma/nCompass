#pragma once

#include "Sentinel.h"
#include "gmock/gmock.h" 

class CSentinelMock : public IProtocolManager {
 public:
     CSentinelMock();
     virtual ~CSentinelMock();

    MOCK_METHOD0(InitInstance, bool());
    MOCK_METHOD1(StartProtocol, void(ProtocolType));
    MOCK_METHOD3(SendMessageToProtocol, void(ProtocolType protocolType, CInternalMessage *msg, int priorityLevel));
    MOCK_METHOD1(RemoveProtocol, void(ProtocolType protocolType));

    MOCK_METHOD1(WaitForProtocol, void(ProtocolType protocolType));
    MOCK_METHOD2(ClearInboundQueueStarvation, void(ProtocolType, MessagePersistType));
    MOCK_METHOD2(SetInboundQueueEqualPriorityInterrupt, void(IProtocolManager::ProtocolType, bool));

    MOCK_METHOD1(InboundQueueForProtocolIsReady, bool(ProtocolType));
    MOCK_METHOD2(IsQueueStarved, bool(ProtocolType, MessagePersistType));
    MOCK_METHOD1(IsProtocolInitialized, bool(ProtocolType));
    MOCK_METHOD2(FlushAllPersistedButTypeFromInboundQueue, void(ProtocolType, int));
    MOCK_METHOD3(PeekMessageWithPersisContentWindowTypeInUI, void(InternalMsgType&, unsigned int&, unsigned int&));
    MOCK_METHOD0(FlushMsgWithPersistContentWindowType, void());

    virtual void SendMessageToProtocolMocked (ProtocolType protocolType, CInternalMessage *msg, int priorityLevel);

    CInternalMessage *GetInternalMsg();

private:
    CInternalMessage *internalMsg;
};
