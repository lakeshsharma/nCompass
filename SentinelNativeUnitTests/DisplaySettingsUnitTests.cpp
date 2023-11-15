#if 000//TODO-PORT

#include "stdafx.h"
#include "CppUnitTest.h"

#include "Config/Config.h"
#include "VideoModeConfig.h"
#include "Hardware\DisplaySettings.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(DisplaySettingsUnitTests)
	{
	public:
		static const LONG LCD_WIDTH = 640;
		static const LONG LCD_HEIGHT = 240;
		static const LONG DEFAULT_LCD_POSITION_X = -LCD_WIDTH;
		static const LONG DEFAULT_LCD_POSITION_Y = 0;

		static const DWORD REFRESHRATE = 60;

		static const LONG DEFAULT_EGM_WIDTH = 1024;
		static const LONG DEFAULT_EGM_HEIGHT = 768;

		static const LONG DEFAULT_MIXER_WIDTH = 1024;
		static const LONG DEFAULT_MIXER_HEIGHT = 768;

		static const LONG DEFAULT_VEB_WIDTH = 1280;
		static const LONG DEFAULT_VEB_HEIGHT = 1024;

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
					{  1024*768*REFRESHRATE,    1024,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{   640*240*REFRESHRATE,     640,     240,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{   640*480*REFRESHRATE,     640,     480,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{   800*600*REFRESHRATE,     800,     600,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{ 1280*1024*REFRESHRATE,    1280,    1024,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{  1360*768*REFRESHRATE,    1360,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{  1366*768*REFRESHRATE,    1366,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
					{  1999*999*REFRESHRATE,    1999,     999,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
				};
			
				for (int n = 0; n < _countof(entries); n++)
				{
					AddVideoMode(new CVideoMode(n, entries[n], L"VEB Test Mode"));
				}
			}
		};

		TEST_METHOD(DisplaySettingsUpdateTest)
		{
			CDisplaySettings settings;

			Assert::IsFalse(settings.Updated(), L"Updated() returns 'true' before update has happened");

			bool success = settings.Update();
			Assert::IsTrue(success == settings.Updated(), L"Updated() not matching what is returned by Update()");
			Assert::IsTrue(success, L"Unable to initialize CDisplaySettings");
		}
		
		TEST_METHOD(DisplaySettingsGetLcdDesktopPositionIsDefaultTest)
		{
			CDisplaySettings settings;

			bool success = false;

			success = settings.Update();
			Assert::IsTrue(success, L"Unable to initialize CDisplaySettings");

			POINT position;
			success = settings.GetLcdDesktopPosition(position);
			Assert::IsTrue(success, L"Unable to get position");
			Assert::IsTrue(DEFAULT_LCD_POSITION_X == position.x, L"Wrong x position returned");
			Assert::IsTrue(DEFAULT_LCD_POSITION_Y == position.y, L"Wrong y position returned");
		}
		
		TEST_METHOD(DisplaySettingsGetLcdDesktopResolutionIsDefaultTest)
		{
			CDisplaySettings settings;

			bool success = false;

			success = settings.Update();
			Assert::IsTrue(success, L"Unable to initialize CDisplaySettings");

			SIZE resolution;
			success = settings.GetLcdDesktopResolution(resolution);
			Assert::IsTrue(success, L"Unable to get resolution");
			Assert::IsTrue(LCD_WIDTH == resolution.cx, L"Wrong x resolution returned");
			Assert::IsTrue(LCD_HEIGHT == resolution.cy, L"Wrong y resolution returned");
		}
		
		TEST_METHOD(DisplaySettingsSetLcdDesktopTest)
		{
			CDisplaySettings settings;

			bool success = false;

			success = settings.Update();
			Assert::IsTrue(success, L"Unable to initialize CDisplaySettings");

			POINT original;
			success = settings.GetLcdDesktopPosition(original);
			Assert::IsTrue(success, L"Unable to get original position");

			POINT position;
			position.x = 0;
			position.y = LCD_HEIGHT;

			success = settings.SetLcdDesktop(&position);
			Assert::IsTrue(success, L"SetDesktop failed trying to set a valid position");

			success = settings.GetLcdDesktopPosition(position);
			Assert::IsTrue(success, L"Unable to get position");
			Assert::IsTrue(0 == position.x, L"Wrong x position returned");
			Assert::IsTrue(LCD_HEIGHT == position.y, L"Wrong y position returned");

			// Restore to original position
			success = settings.SetLcdDesktop(&original);
			Assert::IsTrue(success, L"SetDesktop failed restoring original position");

			success = settings.GetLcdDesktopPosition(position);
			Assert::IsTrue(success, L"Unable to get position after restoring");
			Assert::IsTrue(original.x == position.x, L"Wrong x position restored");
			Assert::IsTrue(original.y == position.y, L"Wrong y position restored");
		}

		TEST_METHOD(DisplaySettingsSetVebDesktopNoChangeTest)
		{
			CDisplaySettings settings;

			bool success = settings.Update();
			Assert::IsTrue(success, L"Unable to initialize CVebDesktopDisplay");

			// SetDesktop is expect to return 'false' if no change is made
			success = settings.SetVebDesktop(NULL, DesktopOrientationNoChange);
			Assert::IsFalse(success, L"SetDesktop return 'true' when making no change");
		}

		TEST_METHOD(DisplaySettingsGetSetVebDesktopOrientationTest)
		{
			CDisplaySettings settings;

			bool success = false;

			success = settings.Update();
			Assert::IsTrue(success, L"Unable to initialize CVebDesktopDisplay");

			DesktopOrientationType original;
			success = settings.GetVebDesktopOrientation(original);
			Assert::IsTrue(success, L"Unable to get original orientation");

			DesktopOrientationType current = original;

			// Rotate counter-clockwise
			for (int n = DesktopOrientation90 + 1; n <= DesktopOrientation270 + 1; n++)
			{
				DesktopOrientationType test = (DesktopOrientationType)((original + n) % (DesktopOrientation270 + 1));
				success = settings.SetVebDesktop(NULL, test);
				Assert::IsTrue(success, L"SetDesktop failed attempting to rotate display");

				success = settings.GetVebDesktopOrientation(current);
				Assert::IsTrue(success, L"Unable to get orientation after setting");
				Assert::IsTrue(test == current, L"Set to wrong orientation");
			}

			// Rotate clockwise
			for (int n = 3; n >= 0; n--)
			{
				DesktopOrientationType test = (DesktopOrientationType)((original + n) % 4);
				success = settings.SetVebDesktop(NULL, test);
				Assert::IsTrue(success, L"SetDesktop failed attempting to rotate display");

				success = settings.GetVebDesktopOrientation(current);
				Assert::IsTrue(success, L"Unable to get orientation after setting");
				Assert::IsTrue(test == current, L"Set to wrong orientation");
			}

			// Restore to original orientation. Note that SetDesktop is expect to return 'false' if original == current
			success = (settings.SetVebDesktop(NULL, original) == (current != original));
			Assert::IsTrue(success, L"SetDesktop failed restoring original orientation");

			success = settings.GetVebDesktopOrientation(current);
			Assert::IsTrue(success, L"Unable to get orientation after restoring");
			Assert::IsTrue(original == current, L"Wrong orientation restored");
		}
		
		void TestGetSetVebDesktopPostion(CDisplaySettings& settings, LONG x, LONG y)
		{
			bool success = false;

			POINT test;
			test.x = x;
			test.y = y;

			success = settings.SetVebDesktop(NULL, DesktopOrientationDefault, &test);
			Assert::IsTrue(success, L"SetDesktop failed setting position");

			POINT current;
			success = settings.GetVebDesktopPosition(current);

			Assert::IsTrue(success, L"Unable to get position after setting");
			Assert::IsTrue(test.x == current.x, L"Wrong x position set");
			Assert::IsTrue(test.y == current.y, L"Wrong y position set");
		}
		
		TEST_METHOD(DisplaySettingsGetSetVebDesktopPositionTest)
		{
			CDisplaySettings settings;

			bool success = false;

			success = settings.Update();
			Assert::IsTrue(success, L"Unable to initialize CDisplaySettings");

			POINT original;
			success = settings.GetVebDesktopPosition(original);
			Assert::IsTrue(success, L"Unable to get original position");

			TestGetSetVebDesktopPostion(settings, LCD_WIDTH, 0);

			TestGetSetVebDesktopPostion(settings, 0, LCD_HEIGHT);

			TestGetSetVebDesktopPostion(settings, -DEFAULT_VEB_WIDTH, 0);

			TestGetSetVebDesktopPostion(settings, 0, -DEFAULT_VEB_HEIGHT);

			// Restore to original position
			POINT current;
			success = settings.GetVebDesktopPosition(current);
			Assert::IsTrue(success, L"Unable to get position before restoring");

			// Note that SetDesktop is expect to return 'false' if original == current
			success = (settings.SetVebDesktop(NULL, DesktopOrientationDefault, &original) == (original.x != current.x || original.y != current.y));
			Assert::IsTrue(success, L"SetVebDesktop failed restoring original position");

			success = settings.GetVebDesktopPosition(current);
			Assert::IsTrue(success, L"Unable to get position after restoring");
			Assert::IsTrue(original.x == current.x, L"Wrong x position restored");
			Assert::IsTrue(original.y == current.y, L"Wrong y position restored");
		}

		TEST_METHOD(DisplaySettingsGetSetVebDesktopResolutionNoChangeTest)
		{
			CDisplaySettings settings;

			bool success = false;

			success = settings.Update();
			Assert::IsTrue(success, L"Unable to initialize CDisplaySettings");

			SIZE original;
			success = settings.GetVebDesktopResolution(original);
			Assert::IsTrue(success, L"Unable to get original resolution");

			CTestVideoModeTable table;
			table.BuildYourself();
	
			CVideoMode vmode;
			int result;

			result = table.FindVideoModeFromRes(DEFAULT_VEB_WIDTH, DEFAULT_VEB_HEIGHT, REFRESHRATE, vmode);
			Assert::IsTrue(result >= 0, L"Default Resolution not found in CVideoModeTable");

			settings.SetVebDesktop(&vmode, DesktopOrientationNoChange);

			// Test that SetVebDesktop returns 'false' when resolution is unchanged
			success = settings.SetVebDesktop(&vmode, DesktopOrientationNoChange);
			Assert::IsFalse(success, L"SetVebDesktop failed setting resolution");

			// Restore to original position
			SIZE current;
			success = settings.GetVebDesktopResolution(current);
			Assert::IsTrue(success, L"Unable to get resolution before restoring");

			// Note that SetDesktop is expect to return 'false' if original == current
			result = table.FindVideoModeFromRes((USHORT)original.cx, (USHORT)original.cy, REFRESHRATE, vmode);
			Assert::IsTrue(result >= 0, L"Original Resolution not found in CVideoModeTable");
			success = (settings.SetVebDesktop(&vmode, DesktopOrientationNoChange) == (original.cx != current.cx || original.cy != current.cy));
			Assert::IsTrue(success, L"SetDesktop failed restoring original resolution");

			success = settings.GetVebDesktopResolution(current);
			Assert::IsTrue(success, L"Unable to get resolution after restoring");
			Assert::IsTrue(original.cx == current.cx, L"Wrong x resolution restored");
			Assert::IsTrue(original.cy == current.cy, L"Wrong y resolution restored");
		}

		void TestGetSetVebDesktopResolution(CDisplaySettings& settings, CVideoMode vmode, DWORD expectedWidth, DWORD expectedHeight)
		{
			bool success = false;

			success = settings.SetVebDesktop(&vmode, DesktopOrientationNoChange);
			Assert::IsTrue(success, L"SetVebDesktop failed setting resolution");

			SIZE current;
			success = settings.GetVebDesktopResolution(current);
			Assert::IsTrue(success, L"Unable to get resolution after setting");
			Assert::IsTrue(expectedWidth == current.cx, L"Wrong x resolution set");
			Assert::IsTrue(expectedHeight == current.cy, L"Wrong y resolution set");
		}

		TEST_METHOD(DisplaySettingsGetSetVebDesktopResolutionTest)
		{
			CDisplaySettings settings;

			bool success = false;

			success = settings.Update();
			Assert::IsTrue(success, L"Unable to initialize CDisplaySettings");

			SIZE original;
			success = settings.GetVebDesktopResolution(original);
			Assert::IsTrue(success, L"Unable to get original resolution");

			CTestVideoModeTable table;
			table.BuildYourself();
	
			CVideoMode vmode;
			int result;

			// Test basic resolution change
			result = table.FindVideoModeFromRes(LOWEST_VEB_WIDTH, LOWEST_VEB_HEIGHT, REFRESHRATE, vmode);
			Assert::IsTrue(result >= 0, L"Lowest Resolution not found in CVideoModeTable");
			TestGetSetVebDesktopResolution(settings, vmode, LOWEST_VEB_WIDTH, LOWEST_VEB_HEIGHT);

			// Test 1366 vs 1360 workaround
			result = table.FindVideoModeFromRes(WORKAROUND1_VEB_WIDTH, WORKAROUND_VEB_HEIGHT, REFRESHRATE, vmode);
			Assert::IsTrue(result >= 0, L"Workaround Resolution not found in CVideoModeTable");
			TestGetSetVebDesktopResolution(settings, vmode, WORKAROUND0_VEB_WIDTH, WORKAROUND_VEB_HEIGHT);

			// Test wrong resolution
			result = table.FindVideoModeFromRes(WRONG_VEB_WIDTH, WRONG_VEB_HEIGHT, REFRESHRATE, vmode);
			Assert::IsTrue(result >= 0, L"Wrong Resolution not found in CVideoModeTable");
			TestGetSetVebDesktopResolution(settings, vmode, ACCEPTABLE_VEB_WIDTH, ACCEPTABLE_VEB_HEIGHT);

			// Restore to original position
			SIZE current;
			success = settings.GetVebDesktopResolution(current);
			Assert::IsTrue(success, L"Unable to get resolution before restoring");

			// Note that SetDesktop is expect to return 'false' if original == current
			result = table.FindVideoModeFromRes((USHORT)original.cx, (USHORT)original.cy, REFRESHRATE, vmode);
			Assert::IsTrue(result >= 0, L"Original Resolution not found in CVideoModeTable");
			success = (settings.SetVebDesktop(&vmode, DesktopOrientationNoChange) == (original.cx != current.cx || original.cy != current.cy));
			Assert::IsTrue(success, L"SetDesktop failed restoring original resolution");

			success = settings.GetVebDesktopResolution(current);
			Assert::IsTrue(success, L"Unable to get resolution after restoring");
			Assert::IsTrue(original.cx == current.cx, L"Wrong x resolution restored");
			Assert::IsTrue(original.cy == current.cy, L"Wrong y resolution restored");
		}

		TEST_METHOD(DisplaySettingsGetMonitorResolutionTest)
		{
			CDisplaySettings settings;

			bool success = false;

			success = settings.Update();
			Assert::IsTrue(success, L"Unable to initialize CDisplaySettings");

			SIZE current;
			success = settings.GetMonitorResolution(current);
			Assert::IsTrue(success, L"Unable to get current monitor resolution");
			Assert::IsTrue(DEFAULT_VEB_WIDTH == current.cx, L"Wrong x monitor resolution");
			Assert::IsTrue(DEFAULT_VEB_HEIGHT == current.cy, L"Wrong y monitor resolution");
		}

		TEST_METHOD(DisplaySettingsGetEgmVideoResolutionTest)
		{
			CDisplaySettings settings;

			bool success = false;

			success = settings.Update();
			Assert::IsTrue(success, L"Unable to initialize CDisplaySettings");

			SIZE current;
			success = settings.GetEgmVideoResolution(current);
			Assert::IsTrue(success, L"Unable to get current monitor resolution");
			Assert::IsTrue(DEFAULT_EGM_WIDTH == current.cx, L"Wrong x monitor resolution");
			Assert::IsTrue(DEFAULT_EGM_HEIGHT == current.cy, L"Wrong y monitor resolution");
		}

		TEST_METHOD(DisplaySettingsGetMixerVideoResolutionTest)
		{
			CDisplaySettings settings;

			bool success = false;

			success = settings.Update();
			Assert::IsTrue(success, L"Unable to initialize CDisplaySettings");

			SIZE current;
			success = settings.GetMixerVideoResolution(current);
			Assert::IsTrue(success, L"Unable to get current monitor resolution");

			CString failedMsg;
			failedMsg.Format(_T("Wrong x monitor resolution, %d x %d"), (int)current.cx, (int)current.cy);
			Assert::IsTrue(DEFAULT_MIXER_WIDTH == current.cx, failedMsg);
			Assert::IsTrue(DEFAULT_MIXER_HEIGHT == current.cy, failedMsg);
		}

		TEST_METHOD(DisplaySettingsGetSetMixerVideoAnalogOffsetsTest)
		{
			CDisplaySettings settings;

			bool success = false;

			success = settings.Update();
			Assert::IsTrue(success, L"Unable to initialize CDisplaySettings");

			POINT original;
			success = settings.GetMixerVideoAnalogOffsets(original);
			Assert::IsTrue(success, L"Unable to get original offsets");

			POINT newOffsets(original);
			newOffsets.x += 1;
			newOffsets.y += 1;

			success = settings.SetMixerVideoAnalogOffsets(newOffsets);
			Assert::IsTrue(success, L"Setting new offests not reported successful");

			POINT current;
			success = settings.GetMixerVideoAnalogOffsets(current);
			Assert::IsTrue(success, L"Unable to get current offsets");
			Assert::IsTrue(newOffsets.x == current.x, L"Wrong x offset");
			Assert::IsTrue(newOffsets.y == current.y, L"Wrong y offset");

			// Restore Analog Offsets
			success = settings.SetMixerVideoAnalogOffsets(original);
			Assert::IsTrue(success, L"Restoring offests not reported successful");

			success = settings.GetMixerVideoAnalogOffsets(current);
			Assert::IsTrue(success, L"Unable to get current offsets");
			Assert::IsTrue(original.x == current.x, L"Wrong x offset");
			Assert::IsTrue(original.y == current.y, L"Wrong y offset");
		}

		TEST_METHOD(DisplaySettingsGetSetEgmVideoAnalogOffsetsTest)
		{
			CDisplaySettings settings;

			bool success = false;

			success = settings.Update();
			Assert::IsTrue(success, L"Unable to initialize CDisplaySettings");

			POINT original;
			success = settings.GetEgmVideoAnalogOffsets(original);
			Assert::IsTrue(success, L"Unable to get original offsets");

			POINT newOffsets(original);
			newOffsets.x += 1;
			newOffsets.y += 1;

			success = settings.SetEgmVideoAnalogOffsets(newOffsets);
			Assert::IsTrue(success, L"Setting new offests not reported successful");

			POINT current;
			success = settings.GetEgmVideoAnalogOffsets(current);
			Assert::IsTrue(success, L"Unable to get current offsets");
			Assert::IsTrue(newOffsets.x == current.x, L"Wrong x offset");
			Assert::IsTrue(newOffsets.y == current.y, L"Wrong y offset");

			// Restore Analog Offsets
			success = settings.SetEgmVideoAnalogOffsets(original);
			Assert::IsTrue(success, L"Restoring offests not reported successful");

			success = settings.GetEgmVideoAnalogOffsets(current);
			Assert::IsTrue(success, L"Unable to get current offsets");
			Assert::IsTrue(original.x == current.x, L"Wrong x offset");
			Assert::IsTrue(original.y == current.y, L"Wrong y offset");
		}

		TEST_METHOD(DisplaySettingsGetSetMixerSignalTypeTest)
		{
			static const byte SIGNAL_TYPE_COUNT = VIDEO_SIGNAL_TYPE_BOTH + 1;

			byte original = CDisplaySettings::GetMixerVideoSignalType();

			byte newType = (original + 1) % SIGNAL_TYPE_COUNT;

			CDisplaySettings::SetMixerVideoSignalType(newType);

			byte current = CDisplaySettings::GetMixerVideoSignalType();
			Assert::IsTrue(newType == current, L"Signal Type not set");

			// Restore Signal Type
			CDisplaySettings::SetMixerVideoSignalType(original);
			current = CDisplaySettings::GetMixerVideoSignalType();
			Assert::IsTrue(original == current, L"Signal Type not restored");
		}

		TEST_METHOD(DisplaySettingsGetSetEgmSignalTypeTest)
		{
			static const byte SIGNAL_TYPE_COUNT = VIDEO_SIGNAL_TYPE_DIGITAL + 1;

			byte original = CDisplaySettings::GetEgmVideoSignalType();

			byte newType = (original + 1) % SIGNAL_TYPE_COUNT;

			CDisplaySettings::SetEgmVideoSignalType(newType);

			byte current = CDisplaySettings::GetEgmVideoSignalType();
			Assert::IsTrue(newType == current, L"Signal Type not set");

			// Restore Signal Type
			CDisplaySettings::SetEgmVideoSignalType(original);
			current = CDisplaySettings::GetEgmVideoSignalType();
			Assert::IsTrue(original == current, L"Signal Type not restored");
		}

		TEST_METHOD(DisplaySettingsGenerateInputVideoModeConfigTest)
		{
			CDisplaySettings settings;

			bool success = false;

			success = settings.Update();
			Assert::IsTrue(success, L"Unable to initialize CDisplaySettings");

			CConfig config;

			CVideoModeConfig* videoConfig = settings.GenerateVideoModeConfig(config, true);
			Assert::IsNotNull(videoConfig, L"Failed to generate Input VideoModeConfig");
			Assert::IsFalse(videoConfig->HasChangeBeenMade(), L"Input VideoModeConfig created with changes flag set true.");

			switch (videoConfig->GetSignalType())
			{
				case VIDEO_MODE_SIGNAL_AUTO:
					success = (VIDEO_SIGNAL_TYPE_AUTO == CDisplaySettings::GetEgmVideoSignalType());
					break;
				case VIDEO_MODE_SIGNAL_ANALOG:
					success = (VIDEO_SIGNAL_TYPE_ANALOGUE == CDisplaySettings::GetEgmVideoSignalType());
					break;
				case VIDEO_MODE_SIGNAL_DIGITAL:
					success = (VIDEO_SIGNAL_TYPE_DIGITAL == CDisplaySettings::GetEgmVideoSignalType());
					break;
				case VIDEO_MODE_SIGNAL_BOTH:
					success = (VIDEO_SIGNAL_TYPE_BOTH == CDisplaySettings::GetEgmVideoSignalType());
					break;
				default:
					Assert::Fail(L"GetSignalType returned an undefined type.");
			}
			Assert::IsTrue(success, L"VideoModeConfig Input Signal Type does not match EGM Signal Type");

			success = (videoConfig->IsVideoModeAuto() == config.IsVideoInputModeAutoDetect());
			Assert::IsTrue(success, L"VideoModeConfig Input Auto Detect does not CConfig");

			success = (videoConfig->GetAutoConfigVideoModeIndex() == videoConfig->GetSelectedVideoModeIndex());
			Assert::IsTrue(success, L"VideoModeConfig Input Autoconfig Video Mode does not match Selected Video Mode");

			delete videoConfig;
		}

	};
}

#endif //TODO-PORT
