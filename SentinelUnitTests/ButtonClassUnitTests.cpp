#include "stdafx.h"
#include "UI/ButtonInfo.h"
#include "UI/ScreenDefs.h"

// Unit testing includes.
#include "gtest/gtest.h"
using namespace std;

int buttonId(969);
RECT location(1, 2, 3, 4);
string upButtonName("upButtonName");
string downButtonName("downButtonName");

namespace SentinelNativeUnitTests
{

		void SetupButtonInfo(CButtonInfo& buttonInfo)
		{
			buttonInfo.SetButtonID(buttonId);
			buttonInfo.SetLocation(location);
			buttonInfo.SetImages(upButtonName, downButtonName);
		}
		TEST(ButtonClassUnitTests, ButtonClassConstructorUnitTest)
		{
			//Setup
			CButtonInfo buttonInfo;
			SetupButtonInfo(buttonInfo);

			//Tests
			bool areEqual = false;
			ASSERT_EQ(buttonId, buttonInfo.GetButtonID()) << "m_btnID.";
			areEqual = upButtonName == buttonInfo.GetUpButtonFilename();
			ASSERT_TRUE(areEqual) << "m_strUpButton";
			areEqual = downButtonName == buttonInfo.GetDownButtonFilename();
			ASSERT_TRUE(areEqual) << "m_strDownButton";
			areEqual = location == buttonInfo.GetLocation();
			ASSERT_TRUE(areEqual) << "m_location";
		}

		TEST(ButtonClassUnitTests, ButtonInfoAssignmentOperator)
		{
			CButtonInfo buttonInfo;
			buttonInfo.SetButtonID(100);
			buttonInfo.SetLocation(RECT(1, 1, 10, 100));
			buttonInfo.SetImages("Up", "Down");

			CButtonInfo copy = buttonInfo;
			ASSERT_TRUE(copy.GetButtonID() == 100);
			ASSERT_TRUE(copy.GetLocation() == RECT(1, 1, 10, 100));
			ASSERT_TRUE(copy.GetUpButtonFilename() == "Up");
			ASSERT_TRUE(copy.GetDownButtonFilename() == "Down");
		}

		TEST(ButtonClassUnitTests, ButtonInfoCopyConstructor)
		{
			CButtonInfo buttonInfo;
			buttonInfo.SetButtonID(100);
			buttonInfo.SetLocation(RECT(1, 1, 10, 100));
			buttonInfo.SetImages("Up", "Down");

			CButtonInfo copy(buttonInfo);
			ASSERT_TRUE(copy.GetButtonID() == 100);
			ASSERT_TRUE(copy.GetLocation() == RECT(1, 1, 10, 100));
			ASSERT_TRUE(copy.GetUpButtonFilename() == "Up");
			ASSERT_TRUE(copy.GetDownButtonFilename() == "Down");
		}
}

