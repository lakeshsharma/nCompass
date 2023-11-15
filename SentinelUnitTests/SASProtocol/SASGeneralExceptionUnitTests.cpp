
#include "stdafx.h"
#include "SASProtocol/SASGeneralPoll.h"
#include "SASProtocol/SASGeneralException.h"
#include "gtest/gtest.h"
// Unit testing includes.
#include "UnitTestingUtils.h"

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))


namespace SentinelNativeUnitTests
{
	
    class SASGeneralPoll : public ::testing::Test
	{
    public:
		// Class member(s) used by test (data) methods.
		int m_testIndex;

		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		virtual void SetUp()
		{
			// Test method initialization code.
			m_testIndex = 0;
		}

		/// <summary>
		/// Cleanup class member(s) before each test method is run.
		/// </summary>
		virtual void TearDown()
		{
			// Test method cleanup code.
		}
    };
		/// <summary>
		/// Code coverage and functional test for:
		///     CSASGeneralException::CSASGeneralException()
		///     CSASGeneralException::GetDescriptionFor()
		/// Code coverage for:
		///     CSASGeneralException::CSASGeneralException()
		/// Test GetDescriptionFor() with one value.
		/// </summary>
		TEST_F(SASGeneralPoll, GetDescriptionFor)
		{
			// Data.
			const GeneralPollException generalPollException(GPE_SLOTDOOR_OPENED);
			const std::string descriptionFor("Slot Door Opened");

			const CSASGeneralException obj;
			AssertResult2(obj.GetDescriptionFor(generalPollException), descriptionFor);
            //ASSERT_EQ(obj.GetDescriptionFor(generalPollException), descriptionFor);
		}
	
}
