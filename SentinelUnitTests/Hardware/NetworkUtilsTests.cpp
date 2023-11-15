// Unit testing includes.
#include "gtest/gtest.h"
#include "NetworkUtilsMock.h"

using ::testing::Mock;
using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::NiceMock;


class NetworkUtilsTests : public ::testing::Test
{
    private:
        NiceMock<NetworkUtilsTestsMock> m_NetworkUtilsMock;

    public:
        virtual void SetUp() 
        {
            ON_CALL(m_NetworkUtilsMock, GetCmdForNetworkCableState()).WillByDefault(Invoke(&m_NetworkUtilsMock, &NetworkUtilsTestsMock::GetCmdForNetworkCableStateMocked));
        }

        virtual void TearDown() 
        {}

        void SetNetworkCablePluggedCmd( string cmd )
        {
            m_NetworkUtilsMock.SetCmdForNetworkCableState(cmd);
   
        }

        void f_NetworkUtilsTestsConst( void )
        {
           ASSERT_FALSE( m_NetworkUtilsMock.GetNetworkServiceRestarted())<< "Network service restarted"; 
           ASSERT_FALSE( m_NetworkUtilsMock.GetNetworkCableUnPlugged())<< "Network cable is unplugged"; 
           ASSERT_FALSE( m_NetworkUtilsMock.GetNetworkCableToggleState()) << "Network cable toggle state is true"; 
        }

        void f_GetNetworkCableToggleStateTest( void )
        {
            m_NetworkUtilsMock.SetNetworkCableToggleState(true);
            ASSERT_TRUE(m_NetworkUtilsMock.GetNetworkCableToggleState()) << "Network Cable state is not toggled";

            m_NetworkUtilsMock.SetNetworkCableToggleState(false);
            ASSERT_FALSE( m_NetworkUtilsMock.GetNetworkCableToggleState())<< "Network Cable state is toggled";
        }

        void f_GetNetworkServiceRestartedTest( void )
        {
            m_NetworkUtilsMock.SetNetworkServiceRestarted(true);
            ASSERT_TRUE(m_NetworkUtilsMock.GetNetworkServiceRestarted()) << "Network service not restarted";

            m_NetworkUtilsMock.SetNetworkServiceRestarted(false);
            ASSERT_FALSE(m_NetworkUtilsMock.GetNetworkServiceRestarted())<< "Network service restarted";
        }

        void f_GetNetworkCableUnPluggedTest(void)
        {
            m_NetworkUtilsMock.SetNetworkCableUnPlugged(true);
            ASSERT_TRUE(m_NetworkUtilsMock.GetNetworkCableUnPlugged()) << "Network Cable state is not unplugged";

            m_NetworkUtilsMock.SetNetworkCableUnPlugged(false);
            ASSERT_FALSE(m_NetworkUtilsMock.GetNetworkCableUnPlugged())<< "Network Cable state is plugged";
        }

        void f_NetWorkSvcRestartIfNetworkCableChangedTest(void)
        {
            SetNetworkCablePluggedCmd("0");
            bool IsNetworkSvcToRestart =  m_NetworkUtilsMock.NetWorkSvcRestartIfNetworkCableChanged();

            ASSERT_TRUE( m_NetworkUtilsMock.GetNetworkCableUnPlugged())<< "Network Cable state is not unplugged";
            ASSERT_FALSE( m_NetworkUtilsMock.GetNetworkCableToggleState())<< "Network Cable state is toggled";

            SetNetworkCablePluggedCmd("1");

            IsNetworkSvcToRestart =  m_NetworkUtilsMock.NetWorkSvcRestartIfNetworkCableChanged();
            ASSERT_FALSE( m_NetworkUtilsMock.GetNetworkCableUnPlugged())<< "Network Cable state is unplugged";
            ASSERT_TRUE( m_NetworkUtilsMock.GetNetworkCableToggleState())<< "Network Cable state is not toggled";

            ASSERT_TRUE(IsNetworkSvcToRestart)<< "Network service will not restart";

            //For invalid state of command
            SetNetworkCablePluggedCmd("abababab");
            IsNetworkSvcToRestart =  m_NetworkUtilsMock.NetWorkSvcRestartIfNetworkCableChanged();
            ASSERT_FALSE( m_NetworkUtilsMock.GetNetworkCableUnPlugged())<< "Network Cable state is unplugged";

             //For invalid state of command
            SetNetworkCablePluggedCmd("abababab22212");
            IsNetworkSvcToRestart =  m_NetworkUtilsMock.NetWorkSvcRestartIfNetworkCableChanged();
            ASSERT_FALSE( m_NetworkUtilsMock.GetNetworkCableUnPlugged())<< "Network Cable state is unplugged";

           //For invalid state of command
            SetNetworkCablePluggedCmd("");
            IsNetworkSvcToRestart =  m_NetworkUtilsMock.NetWorkSvcRestartIfNetworkCableChanged();
            ASSERT_FALSE( m_NetworkUtilsMock.GetNetworkCableUnPlugged())<< "Network Cable state is unplugged";

        }
};

TEST_F(NetworkUtilsTests, NetworkUtilsTestsConst)
{
    f_NetworkUtilsTestsConst();
}

TEST_F(NetworkUtilsTests, GetNetworkCableToggleStateTest)
{
    f_GetNetworkCableToggleStateTest();
}

TEST_F(NetworkUtilsTests, GetNetworkServiceRestartedTest)
{
    f_GetNetworkServiceRestartedTest();
}

TEST_F(NetworkUtilsTests, GetNetworkCableUnPluggedTest)
{
  f_GetNetworkCableUnPluggedTest();
}

TEST_F(NetworkUtilsTests, NetWorkSvcRestartIfNetworkCableChangedTest)
{
    f_NetWorkSvcRestartIfNetworkCableChangedTest();
}



