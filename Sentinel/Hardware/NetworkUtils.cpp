#include "NetworkUtils.h"

using namespace std;

std::recursive_mutex NetworkUtils::m_criticalSection;

NetworkUtils::NetworkUtils( void ):
		m_bNetworkCableUnPlugged(false),
		m_bNetworkCableToggle(false),
		m_bNetworkServiceRestarted(false)
{}

bool NetworkUtils::GetNetworkCableToggleState()
{
    return m_bNetworkCableToggle;
}

bool NetworkUtils::GetNetworkServiceRestarted()
{
    return m_bNetworkServiceRestarted;
}

bool NetworkUtils::GetNetworkCableUnPlugged()
{
    return m_bNetworkCableUnPlugged;
}

void NetworkUtils::SetNetworkCableToggleState(bool NetworkCableToggle)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	m_bNetworkCableToggle = NetworkCableToggle;
}

void NetworkUtils::SetNetworkServiceRestarted(bool NetworkServiceRestarted)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
     m_bNetworkServiceRestarted = NetworkServiceRestarted;
}

void NetworkUtils::SetNetworkCableUnPlugged(bool NetworkCableUnPlugged)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
     m_bNetworkCableUnPlugged = NetworkCableUnPlugged;
}

string NetworkUtils::GetCmdForNetworkCableState(void)
{ 
    string ethFile = string(CDirectoryManager::DIRECTORY_KERNEL_NET_ETH) + "/carrier";
	string cmd;
	string cmd_status;

	//To get the event of plugging and unplugging network cable
	if (CUtilities::FileExists(ethFile))
	{
		cmd = string("cat ") + ethFile;
		cmd_status = CUtilities::GetSystemResultString(cmd.c_str());
	}

	return cmd_status;
}

bool NetworkUtils::NetWorkSvcRestartIfNetworkCableChanged(void)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	int ret_i = -1;

	//To make the code unit testable the function is made
	string cmd_status = GetCmdForNetworkCableState();
	if (!cmd_status.empty())
	{
       std::stringstream convertor;
 
       convertor << cmd_status;
       convertor >> ret_i;

        if(!convertor.fail())
        {
            //Network cable unplugged
            if (!m_bNetworkServiceRestarted && !m_bNetworkCableToggle && !m_bNetworkCableUnPlugged && (ret_i == 0))
            {
                LogString(NETWORKING_LOG, "Network cable unplugged");
                m_bNetworkCableUnPlugged = true;
            }

            //Network cable plugged after unplugging
            if (m_bNetworkCableUnPlugged && (ret_i == 1))
            {
                    LogString(NETWORKING_LOG, "Network cable plugged again");
                    m_bNetworkCableToggle = true;
                    m_bNetworkCableUnPlugged = false;
            }
        }
	}

    m_bNetworkServiceRestarted = false;
    return m_bNetworkCableToggle;
}

