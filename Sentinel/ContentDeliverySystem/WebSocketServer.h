#pragma once
#include "FlashComServer.h"
#include "../UI/UIProtocol.h"

class CWebSocketServer :
    public CFlashComServer
{
public:
    CWebSocketServer(unsigned short port, char packetTerminator);
    int Write(BYTE* data, size_t dataLength);
};

