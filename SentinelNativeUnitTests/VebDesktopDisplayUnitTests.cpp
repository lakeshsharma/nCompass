#if 000//TODO-PORT
#include "stdafx.h"
#include "CppUnitTest.h"

#include "Hardware\VebDesktopDisplay.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(VebDesktopDisplayUnitTest)
	{
	private:
		static const LONG LCD_WIDTH = 640;
		static const LONG LCD_HEIGHT = 240;

		static const DWORD REFRESHRATE = 60;

		static const LONG DEFAULT_VEB_WIDTH = 1024;
		static const LONG DEFAULT_VEB_HEIGHT = 768;

		static const DWORD LOWEST_VEB_WIDTH = 640;
		static const DWORD LOWEST_VEB_HEIGHT = 480;

		static const DWORD WORKAROUND0_VEB_WIDTH = 1360;
		static const DWORD WORKAROUND1_VEB_WIDTH = 1366;
		static const DWORD WORKAROUND_VEB_HEIGHT = 768;

		static const DWORD WRONG_VEB_WIDTH = 1999;
		static const DWORD WRONG_VEB_HEIGHT = 999;

		static const DWORD ACCEPTABLE_VEB_WIDTH = 1280;
		static const DWORD ACCEPTABLE_VEB_HEIGHT = 1024;

		class CTestVideoModeTable : public CVideoModeTable
		{
		public:
			void BuildYourself()
			{
				const VID_TABLE_ENTRY entries[] = {
				//                  PllFreq, XActive, YActive, XFront, XSync, XBlank, YFront, YSync, YBlank, HRefreshFreq, HSyncPolarity, VSyncPolarity
					{   640*480*REFRESHRATE,     640,     480,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{   800*600*REFRESHRATE,     800,     600,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{  1024*768*REFRESHRATE,    1024,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{ 1280*1024*REFRESHRATE,    1280,    1024,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{  1360*768*REFRESHRATE,    1360,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{  1366*768*REFRESHRATE,    1366,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
				};
			
				for (int n = 0; n < _countof(entries); n++)
				{
					AddVideoMode(new CVideoMode(n, entries[n], L"VEB Test Mode"));
				}
			}
		};

	public:

		TEST_METHOD(VebDesktopDisplayUpdateTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CVebDesktopDisplay veb(table);

			Assert::IsFalse(veb.Updated(), L"Updated() returns 'true' before update has happened");

			bool success = veb.Update();
			Assert::IsTrue(success == veb.Updated(), L"Updated() not matching what is returned by Update()");
			Assert::IsTrue(success, L"Unable to initialize CVebDesktopDisplay");
		}

		TEST_METHOD(VebDesktopDisplaySetDesktopNoChangeTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CVebDesktopDisplay veb(table);

			bool success = veb.Update();
			Assert::IsTrue(success, L"Unable to initialize CVebDesktopDisplay");

			// SetDesktop is expect to return 'false' if no change is made
			success = veb.SetDesktop(NULL, DesktopOrientationNoChange);
			Assert::IsFalse(success, L"SetDesktop return 'true' when making no change");
		}

		TEST_METHOD(VebDesktopDisplayGetSetOrientationTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CVebDesktopDisplay veb(table);

			bool success = false;

			success = veb.Update();
			Assert::IsTrue(success, L"Unable to initialize CVebDesktopDisplay");

			DesktopOrientationType original;
			success = veb.GetOrientation(original);
			Assert::IsTrue(success, L"Unable to get original orientation");

			DesktopOrientationType current = original;

			// Rotate counter-clockwise
			for (int n = 1; n <= 4; n++)
			{
				DesktopOrientationType test = (DesktopOrientationType)((original + n) % 4);
				success = veb.SetDesktop(NULL, test);
				Assert::IsTrue(success, L"SetDesktop failed attempting to rotate display");

				success = veb.GetOrientation(current);
				Assert::IsTrue(success, L"Unable to get orientation after setting");
				Assert::IsTrue(test == current, L"Set to wrong orientation");
			}

			// Rotate clockwise
			for (int n = 3; n >= 0; n--)
			{
				DesktopOrientationType test = (DesktopOrientationType)((original + n) % 4);
				success = veb.SetDesktop(NULL, test);
				Assert::IsTrue(success, L"SetDesktop failed attempting to rotate display");

				success = veb.GetOrientation(current);
				Assert::IsTrue(success, L"Unable to get orientation after setting");
				Assert::IsTrue(test == current, L"Set to wrong orientation");
			}

			// Restore to original orientation. Note that SetDesktop is expect to return 'false' if original == current
			success = (veb.SetDesktop(NULL, original) == (current != original));
			Assert::IsTrue(success, L"SetDesktop failed restoring original orientation");

			success = veb.GetOrientation(current);
			Assert::IsTrue(success, L"Unable to get orientation after restoring");
			Assert::IsTrue(original == current, L"Wrong orientation restored");
		}
		
		void TestGetSetPostion(CVebDesktopDisplay& veb, LONG x, LONG y)
		{
			bool success = false;

			POINT test;
			test.x = x;
			test.y = y;

			success = veb.SetDesktop(NULL, DesktopOrientationDefault, &test);
			Assert::IsTrue(success, L"SetDesktop failed setting position");

			POINT current;
			success = veb.GetPosition(current);

			Assert::IsTrue(success, L"Unable to get position after setting");
			Assert::IsTrue(test.x == current.x, L"Wrong x position set");
			Assert::IsTrue(test.y == current.y, L"Wrong y position set");
		}
		
		TEST_METHOD(VebDesktopDisplayGetSetPositionTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CVebDesktopDisplay veb(table);

			bool success = false;

			success = veb.Update();
			Assert::IsTrue(success, L"Unable to initialize CVebDesktopDisplay");

			POINT original;
			success = veb.GetPosition(original);
			Assert::IsTrue(success, L"Unable to get original position");

			TestGetSetPostion(veb, LCD_WIDTH, 0);

			TestGetSetPostion(veb, 0, LCD_HEIGHT);

			TestGetSetPostion(veb, -DEFAULT_VEB_WIDTH, 0);

			TestGetSetPostion(veb, 0, -DEFAULT_VEB_HEIGHT);

			// Restore to original position
			POINT current;
			success = veb.GetPosition(current);
			Assert::IsTrue(success, L"Unable to get position before restoring");

			// Note that SetDesktop is expect to return 'false' if original == current
			success = (veb.SetDesktop(NULL, DesktopOrientationDefault, &original) == (original.x != current.x || original.y != current.y));
			Assert::IsTrue(success, L"SetDesktop failed restoring original position");

			success = veb.GetPosition(current);
			Assert::IsTrue(success, L"Unable to get position after restoring");
			Assert::IsTrue(original.x == current.x, L"Wrong x position restored");
			Assert::IsTrue(original.y == current.y, L"Wrong y position restored");
		}

		void TestGetSetResolution(CVebDesktopDisplay& veb, DWORD width, DWORD height, DWORD expectedWidth, DWORD expectedHeight)
		{
			bool success = false;

			CVideoTiming test((USHORT)width, (USHORT)height, REFRESHRATE);

			success = veb.SetDesktop(&test, DesktopOrientationNoChange);
			Assert::IsTrue(success, L"SetDesktop failed setting resolution");

			SIZE current;
			success = veb.GetResolution(current);
			Assert::IsTrue(success, L"Unable to get resolution after setting");
			Assert::IsTrue(expectedWidth == current.cx, L"Wrong x resolution set");
			Assert::IsTrue(expectedHeight == current.cy, L"Wrong y resolution set");
		}

		TEST_METHOD(VebDesktopDisplayGetSetResolutionTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CVebDesktopDisplay veb(table);

			bool success = false;

			success = veb.Update();
			Assert::IsTrue(success, L"Unable to initialize CVebDesktopDisplay");

			SIZE original;
			success = veb.GetResolution(original);
			Assert::IsTrue(success, L"Unable to get original resolution");

			// Test basic resolution change
			TestGetSetResolution(veb, LOWEST_VEB_WIDTH, LOWEST_VEB_HEIGHT, LOWEST_VEB_WIDTH, LOWEST_VEB_HEIGHT);

			// Test 1366 vs 1360 workaround
			TestGetSetResolution(veb, WORKAROUND1_VEB_WIDTH, WORKAROUND_VEB_HEIGHT, WORKAROUND0_VEB_WIDTH, WORKAROUND_VEB_HEIGHT);

			// Test wrong resolution
			TestGetSetResolution(veb, WRONG_VEB_WIDTH, WRONG_VEB_HEIGHT, ACCEPTABLE_VEB_WIDTH, ACCEPTABLE_VEB_HEIGHT);

			// Restore to original position
			SIZE current;
			success = veb.GetResolution(current);
			Assert::IsTrue(success, L"Unable to get resolution before restoring");

			// Note that SetDesktop is expect to return 'false' if original == current
			CVideoTiming timing((USHORT)original.cx, (USHORT)original.cy, REFRESHRATE);
			success = (veb.SetDesktop(&timing, DesktopOrientationNoChange) == (original.cx != current.cx || original.cy != current.cy));
			Assert::IsTrue(success, L"SetDesktop failed restoring original resolution");

			success = veb.GetResolution(current);
			Assert::IsTrue(success, L"Unable to get resolution after restoring");
			Assert::IsTrue(original.cx == current.cx, L"Wrong x resolution restored");
			Assert::IsTrue(original.cy == current.cy, L"Wrong y resolution restored");
		}

	};
}
#endif //TODO-PORT