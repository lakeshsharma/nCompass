
#include "stdafx.h"
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Hardware/NetworkUtils.h"

class NetworkUtilsTestsMock : public NetworkUtils
{
    public:
        NetworkUtilsTestsMock(void);

        MOCK_METHOD0(GetCmdForNetworkCableState, string(void));
        void SetCmdForNetworkCableState( string pCmd )
        {
            cmd = pCmd;
        }

        NetworkUtils& Instance()
        {
            return m_instance;
        }

        string GetCmdForNetworkCableStateMocked(void);

    private : 
        NetworkUtils &m_instance; 
        string cmd;
        
};

