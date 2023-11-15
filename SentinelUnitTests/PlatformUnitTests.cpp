#include "stdafx.h"
#include "Platform.h"


#include "gtest/gtest.h"
namespace SentinelUnitTests
{
    class Platform :public ::testing::Test
    {
        public:
            virtual void SetUp() 
            {
            }

            virtual void TearDown() 
            {
            }
            void CPlatform_ConstPlatformID()
            {
                // Perform operation(s) and TEST_F(s).
                ASSERT_EQ((UINT)3, CPlatform::AXIOMTEK_PLATFORMID) << "AXIOMTEK_PLATFORMID";
                ASSERT_EQ((UINT)4, CPlatform::BRAEMAC_BLUE_PLATFORM_ID) << "BRAEMAC_BLUE_PLATFORM_ID";
            }
            void GetPlatformID_Zero()
            {
                // Data.
    			const UINT platformId(0);

    			// Setup.
    			CPlatform obj("", platformId, "", "");

    			// Perform operation(s) and TEST_F(s).
    			ASSERT_EQ(platformId, obj.GetPlatformID()) << "obj.GetPlatformID";
            }

            void GetPlatformID_AXIOMTEK_PLATFORMID()
            {
                // Data.
        		const UINT platformId(CPlatform::AXIOMTEK_PLATFORMID);

        			// Setup.
        		CPlatform obj("", platformId,"", "");

        			// Perform operation(s) and TEST_F(s).
        		ASSERT_EQ(platformId, obj.GetPlatformID()) << "obj.GetPlatformID";
            }

            void GetPlatformID_BRAEMAC_BLUE_PLATFORM_ID()
            {
                // Data.
        		const UINT platformId(CPlatform::BRAEMAC_BLUE_PLATFORM_ID);

               // Setup.
        		CPlatform obj("", platformId, "", "");

        		// Perform operation(s) and TEST_F(s).
        		ASSERT_EQ(platformId, obj.GetPlatformID()) << "obj.GetPlatformID";
            }
    };
    // #pragma region GetPlatformID
	/// <summary>
	/// Functional test for static constant platform ID values:
	/// </summary>
	TEST_F(Platform, CPlatform_ConstPlatformID)
    {
        CPlatform_ConstPlatformID();
    }

	/// <summary>
	/// Functional test for:
	///     CPlatform::CPlatform(LPCTSTR osVersion, UINT platformID, LPCTSTR platformName, LPCTSTR upgradeSMDFilename)
	///     CPlatform::GetPlatformID()
	/// Test that (mock) constructor and GetPlatformID() use the specified platformID.
	/// </summary>
	TEST_F(Platform, GetPlatformID_Zero)
	{
        GetPlatformID_Zero();
	}

	/// <summary>
	/// Functional test for:
	///     CPlatform::CPlatform(LPCTSTR osVersion, UINT platformID, LPCTSTR platformName, LPCTSTR upgradeSMDFilename)
	///     CPlatform::GetPlatformID()
	/// Test that (mock) constructor and GetPlatformID() use the specified platformID.
	/// </summary>
	TEST_F(Platform, GetPlatformID_AXIOMTEK_PLATFORMID)
	{
        GetPlatformID_AXIOMTEK_PLATFORMID();
	}

	/// <summary>
	/// Functional test for:
	///     CPlatform::CPlatform(LPCTSTR osVersion, UINT platformID, LPCTSTR platformName, LPCTSTR upgradeSMDFilename)
	///     CPlatform::GetPlatformID()
	/// Test that (mock) constructor and GetPlatformID() use the specified platformID.
	/// </summary>
	TEST_F(Platform, GetPlatformID_BRAEMAC_BLUE_PLATFORM_ID)
	{
        GetPlatformID_BRAEMAC_BLUE_PLATFORM_ID();
	}

// #pragma endregion GetHardwareTFT
}

