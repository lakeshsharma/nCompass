#include "SentinelMock.h"

CSentinelMock::CSentinelMock() 
    : internalMsg(nullptr)
{

}

CSentinelMock::~CSentinelMock() 
{
	
}

void CSentinelMock::SendMessageToProtocolMocked (ProtocolType protocolType, CInternalMessage *msg, int priorityLevel )
{
    internalMsg = msg;
    return;
}

CInternalMessage *CSentinelMock::GetInternalMsg()
{
    return internalMsg;
}
