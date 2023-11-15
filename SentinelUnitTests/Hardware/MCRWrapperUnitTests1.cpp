#include "stdafx.h"
#include "../Sentinel/Hardware/MCRWrapper.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class MCRWrapperUnitTests : public::testing::Test
	{
	public:
		CMCRWrapper *m_obj;
		virtual void SetUp()
		{
			CConfig config(true, "");
			m_obj = new CMCRWrapper(config);
		}
		virtual void TearDown()
		{
			delete m_obj;
		}
	};

	/// <summary>
	/// Test the GetBezelStyle method of CMCRWrapper class.
	/// </summary>
	TEST_F(MCRWrapperUnitTests, GetBezelStyle)
	{
		m_obj->SetBezelStyle(CARD_STYLE::CARD_BAD_READ);
		ASSERT_EQ(m_obj->GetBezelStyle(), CARD_STYLE::CARD_BAD_READ);

		m_obj->SetBezelStyle(CARD_STYLE::CARD_STYLE_PLAYER);
		ASSERT_EQ(m_obj->GetBezelStyle(), CARD_STYLE::CARD_STYLE_PLAYER);
	}
}
