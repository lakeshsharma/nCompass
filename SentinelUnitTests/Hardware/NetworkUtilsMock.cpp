
#include "stdafx.h"
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "NetworkUtilsMock.h"
       
 
NetworkUtilsTestsMock::NetworkUtilsTestsMock(void) : m_instance(NetworkUtils::Instance()),  cmd("0")
{}
 
string NetworkUtilsTestsMock::GetCmdForNetworkCableStateMocked( void )
{
    return cmd;
}





