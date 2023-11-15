#if 000//TODO-PORT
#include "stdafx.h"
#include "CppUnitTest.h"

#include "Hardware\MonitorDisplay.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(MonitorDisplayUnitTests)
	{
		static const DWORD REFRESHRATE = 60;

		static const LONG DEFAULT_VEB_WIDTH = 1280;
		static const LONG DEFAULT_VEB_HEIGHT = 1024;

		class CTestVideoModeTable : public CVideoModeTable
		{
		public:
			void BuildYourself()
			{
				const VID_TABLE_ENTRY entries[] = {
				//                  PllFreq, XActive, YActive, XFront, XSync, XBlank, YFront, YSync, YBlank, HRefreshFreq, HSyncPolarity, VSyncPolarity
					{  1024*768*REFRESHRATE,    1024,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{   640*480*REFRESHRATE,     640,     480,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{   800*600*REFRESHRATE,     800,     600,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{ 1280*1024*REFRESHRATE,    1280,    1024,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{  1360*768*REFRESHRATE,    1360,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{  1366*768*REFRESHRATE,    1366,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
				};
			
				for (int n = 0; n < _countof(entries); n++)
				{
					AddVideoMode(new CVideoMode(n, entries[n], L"Monitor Test Mode"));
				}
			}
		};

	public:
		
		TEST_METHOD(MonitorDisplayUpdateTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CMonitorDisplay monitor(table);

			Assert::IsFalse(monitor.Updated(), L"Updated() returns 'true' before update has happened");

			bool success = monitor.Update();
			Assert::IsTrue(success == monitor.Updated(), L"Updated() not matching what is returned by Update()");
			Assert::IsTrue(success, L"Unable to initialize CMonitorDisplay");
		}

		TEST_METHOD(MonitorDisplayGetResolutionTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CMonitorDisplay monitor(table);

			bool success = false;

			success = monitor.Update();
			Assert::IsTrue(success, L"Unable to initialize CMonitorDisplay");

			SIZE current;
			success = monitor.GetResolution(current);
			Assert::IsTrue(success, L"Unable to get current monitor resolution");
			Assert::IsTrue(DEFAULT_VEB_WIDTH == current.cx, L"Wrong x monitor resolution");
			Assert::IsTrue(DEFAULT_VEB_HEIGHT == current.cy, L"Wrong y monitor resolution");
		}

		TEST_METHOD(MonitorDisplayGetMatchingModeTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CMonitorDisplay monitor(table);

			bool success = false;

			success = monitor.Update();
			Assert::IsTrue(success, L"Unable to initialize CMonitorDisplay");

			CVideoMode vmode = monitor.GetMatchingVideoMode();
			Assert::IsTrue(DEFAULT_VEB_WIDTH == vmode.GetVideoTiming().GetHorizontal(), L"Wrong x monitor resolution");
			Assert::IsTrue(DEFAULT_VEB_HEIGHT == vmode.GetVideoTiming().GetVertical(), L"Wrong y monitor resolution");
		}

	};
}

#endif //TODO-PORT