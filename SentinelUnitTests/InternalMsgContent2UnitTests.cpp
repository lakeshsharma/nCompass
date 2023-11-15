#include "stdafx.h"
#include "InternalMsgContent.h"

#include "UIConfig.h"
#include "gtest/gtest.h"

class InternalMsgContentUnitTests : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

};
TEST_F(InternalMsgContentUnitTests, CUIConfig_MsgContentTypeTest)
{
    ASSERT_TRUE(CUIConfig::MsgContentType() == INT_MSG_CONTENT_UI_CONFIG);
}

TEST_F(InternalMsgContentUnitTests, CUIConfig_GetContentTypeTest)
{
    CUIConfig obj;
    ASSERT_TRUE(obj.GetContentType() == INT_MSG_CONTENT_UI_CONFIG);
}