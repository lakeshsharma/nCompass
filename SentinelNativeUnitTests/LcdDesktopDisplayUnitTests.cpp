#if 000//TODO-PORT

#include "stdafx.h"
#include "CppUnitTest.h"

#include "Hardware\LcdDesktopDisplay.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(LcdDesktopDisplayUnitTest)
	{
	private:
		static const LONG WIDTH = 640;
		static const LONG HEIGHT = 240;
		static const LONG POSITION_X_DEFAULT = -WIDTH;
		static const LONG POSITION_Y_DEFAULT = 0;

		class CTestVideoModeTable : public CVideoModeTable
		{
		public:
			void BuildYourself()
			{
				VID_TABLE_ENTRY entry = {
					60*640*240 /*PllFreq*/, 640 /*XActive*/, 240 /*YActive*/,
					0 /*XFront*/, 0 /*XSync*/, 0 /*XBlank*/,  0 /*YFront*/, 0 /*YSync*/, 0 /*YBlank*/,
					60 /*HRefreshFreq*/, 0 /*HSyncPolarity*/, 0 /*VSyncPolarity*/
				};
				AddVideoMode(new CVideoMode(0, entry, L"LCD Test Mode"));
			}
		};

	public:
		
		TEST_METHOD(LcdDesktopDisplayUpdateTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CLcdDesktopDisplay lcd(table);

			Assert::IsFalse(lcd.Updated(), L"Updated() returns 'true' before update has happened");

			bool success = lcd.Update();
			Assert::IsTrue(success == lcd.Updated(), L"Updated() not matching what is returned by Update()");
			Assert::IsTrue(success, L"Unable to initialize CLcdDesktopDisplay");

			CVideoMode vmode = lcd.GetMatchingVideoMode();
			Assert::IsTrue(vmode.GetVideoTiming().GetHorizontal() == WIDTH, L"Matching Video Mode has wrong width");
			Assert::IsTrue(vmode.GetVideoTiming().GetVertical() == HEIGHT, L"Matching Video Mode has wrong height");
		}
		
		TEST_METHOD(LcdDesktopDisplayGetOrientationTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CLcdDesktopDisplay lcd(table);

			bool success = false;

			success = lcd.Update();
			Assert::IsTrue(success, L"Unable to initialize CLcdDesktopDisplay");

			DesktopOrientationType orientation;
			success = lcd.GetOrientation(orientation);
			Assert::IsTrue(success, L"Unable to get orientation");
			Assert::IsTrue(DesktopOrientationDefault == orientation, L"Wrong orientation returned");
		}
		
		TEST_METHOD(LcdDesktopDisplayGetPositionIsDefaultTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CLcdDesktopDisplay lcd(table);

			bool success = false;

			success = lcd.Update();
			Assert::IsTrue(success, L"Unable to initialize CLcdDesktopDisplay");

			POINT position;
			success = lcd.GetPosition(position);
			Assert::IsTrue(success, L"Unable to get position");
			Assert::IsTrue(POSITION_X_DEFAULT == position.x, L"Wrong x position returned");
			Assert::IsTrue(POSITION_Y_DEFAULT == position.y, L"Wrong y position returned");
		}
		
		TEST_METHOD(LcdDesktopDisplayGetResolutionIsDefaultTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CLcdDesktopDisplay lcd(table);

			bool success = false;

			success = lcd.Update();
			Assert::IsTrue(success, L"Unable to initialize CLcdDesktopDisplay");

			SIZE resolution;
			success = lcd.GetResolution(resolution);
			Assert::IsTrue(success, L"Unable to get resolution");
			Assert::IsTrue(WIDTH == resolution.cx, L"Wrong x resolution returned");
			Assert::IsTrue(HEIGHT == resolution.cy, L"Wrong y resolution returned");
		}
		
		TEST_METHOD(LcdDesktopDisplaySetDesktopTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CLcdDesktopDisplay lcd(table);

			bool success = false;

			success = lcd.Update();
			Assert::IsTrue(success, L"Unable to initialize CLcdDesktopDisplay");

			POINT original;
			success = lcd.GetPosition(original);
			Assert::IsTrue(success, L"Unable to get original position");

			POINT position;
			position.x = 0;
			position.y = HEIGHT;

			success = lcd.SetDesktop(NULL, DesktopOrientationNoChange, &position);
			Assert::IsTrue(success, L"SetDesktop failed trying to set a valid position");

			success = lcd.GetPosition(position);
			Assert::IsTrue(success, L"Unable to get position");
			Assert::IsTrue(0 == position.x, L"Wrong x position returned");
			Assert::IsTrue(HEIGHT == position.y, L"Wrong y position returned");

			// Restore to original position
			success = lcd.SetDesktop(NULL, DesktopOrientationNoChange, &original);
			Assert::IsTrue(success, L"SetDesktop failed restoring original position");

			success = lcd.GetPosition(position);
			Assert::IsTrue(success, L"Unable to get position after restoring");
			Assert::IsTrue(original.x == position.x, L"Wrong x position restored");
			Assert::IsTrue(original.y == position.y, L"Wrong y position restored");
		}

		TEST_METHOD(LcdGetDisplayFrequencyTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CLcdDesktopDisplay lcd(table);

			lcd.GetDisplayFrequency(LcdModel_None);

			Assert::IsTrue(CLcdDesktopDisplay::DISPLAY_FREQUENCEY_OLD_HITACHI == lcd.GetDisplayFrequency(LcdModel_None), 
				L"GetDisplayFrequency returned the wrong expected value.");

			Assert::IsTrue(CLcdDesktopDisplay::DISPLAY_FREQUENCEY_OLD_HITACHI == lcd.GetDisplayFrequency(LcdModel_OldHitachi),
				L"GetDisplayFrequency returned the wrong expected value.");

			Assert::IsTrue(CLcdDesktopDisplay::DISPLAY_FREQUENCEY_NEW_HITACHI == lcd.GetDisplayFrequency(LcdModel_NewHitachi),
				L"GetDisplayFrequency returned the wrong expected value.");

			Assert::IsTrue(CLcdDesktopDisplay::DISPLAY_FREQUENCEY_KYOCERA == lcd.GetDisplayFrequency(LcdModel_Kyocera),
				L"GetDisplayFrequency returned the wrong expected value.");
		}

	};
}
#endif //TODO-PORT