#if 000//TODO-PORT
#include "stdafx.h"
#include "CppUnitTest.h"

#include "Config/Config.h"
#include "VideoModeConfig.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(VideoModeConfigUnitTests)
	{

		static const DWORD REFRESHRATE = 60;

		static const VID_TABLE_ENTRY entries[];

		void ConfirmDefaults(CVideoModeConfig& videoConfig)
		{
			Assert::IsFalse(videoConfig.HasChangeBeenMade(), L"VideoModeConfig 'has been changed' not defaulted to 'false'");
			Assert::IsTrue(VIDEO_MODE_SIGNAL_AUTO == videoConfig.GetSignalType(), L"Video Mode Config 'signal type' not defaulted to 'auto'");
			Assert::IsTrue(videoConfig.IsVideoModeAuto(), L"VideoModeConfig 'video mode' not defaulted to 'auto'");
			Assert::IsFalse(videoConfig.IsVideoModeNative(), L"VideoModeConfig 'video mode' defaulted to 'native'");
			Assert::IsTrue(INVALID_VIDEO_MODE_INDEX == videoConfig.GetSelectedVideoModeIndex(), L"Video Mode Config 'selected mode' not defaulted to 'invalid'");
			Assert::IsTrue(INVALID_VIDEO_MODE_INDEX == videoConfig.GetAutoConfigVideoModeIndex(), L"Video Mode Config 'auto config mode' not defaulted to 'invalid'");
		}

	public:
		
		TEST_METHOD(VideoModeConfigConstructorDefaultTest)
		{
			CVideoModeConfig default;
			ConfirmDefaults(default);
			Assert::IsFalse(default.IsConfigForInput(), L"VideoModeConfig default to 'input'");

			CVideoModeConfig input(true);
			ConfirmDefaults(input);
			Assert::IsTrue(input.IsConfigForInput(), L"VideoModeConfig not initialized to 'input'");

			CVideoModeConfig output(false);
			ConfirmDefaults(output);
			Assert::IsTrue(output.IsConfigForOutput(), L"VideoModeConfig not initialized to 'output'");
		}
		
		TEST_METHOD(VideoModeConfigHasBeenChangedTest)
		{
			CVideoModeConfig videoConfig;
			ConfirmDefaults(videoConfig);

			videoConfig.SetVideoModeToAuto(!videoConfig.IsVideoModeAuto());
			Assert::IsTrue(videoConfig.HasChangeBeenMade(), L"VideoModeConfig not reporting being changed");

			videoConfig.SetChangesMade(false);
			Assert::IsFalse(videoConfig.HasChangeBeenMade(), L"VideoModeConfig 'change' flag not cleared");
		}
		
		TEST_METHOD(VideoModeConfigGetSetConfigForInputOutputTest)
		{
			CVideoModeConfig videoConfig;
			ConfirmDefaults(videoConfig);

			// Set for 'input'
			videoConfig.SetConfigForInput();
			Assert::IsFalse(videoConfig.HasChangeBeenMade(), L"VideoModeConfig reporting being changed for input/output");
			Assert::IsTrue(videoConfig.IsConfigForInput(), L"VideoModeConfig not successfully set to 'input'");

			videoConfig.SetChangesMade(false);

			// Set for 'output'
			videoConfig.SetConfigForOutput();
			Assert::IsFalse(videoConfig.HasChangeBeenMade(), L"VideoModeConfig reporting being changed for input/output");
			Assert::IsTrue(videoConfig.IsConfigForOutput(), L"VideoModeConfig not successfully set to 'output'");
		}

		TEST_METHOD(VideoModeConfigGetSetSignalTypeTest)
		{
			CVideoModeConfig videoConfig;
			ConfirmDefaults(videoConfig);

			//Set Signal Type to VIDEO_MODE_SIGNAL_ANALOG
			videoConfig.SetSignalType(VIDEO_MODE_SIGNAL_ANALOG);
			Assert::IsTrue(videoConfig.HasChangeBeenMade(), L"VideoModeConfig not reporting being changed");
			Assert::IsTrue(VIDEO_MODE_SIGNAL_ANALOG == videoConfig.GetSignalType(), L"Video Mode Config 'signal type' not set to 'analog'");

			videoConfig.SetChangesMade(false);

			//Set Signal Type to VIDEO_MODE_SIGNAL_DIGITAL
			videoConfig.SetSignalType(VIDEO_MODE_SIGNAL_DIGITAL);
			Assert::IsTrue(videoConfig.HasChangeBeenMade(), L"VideoModeConfig not reporting being changed");
			Assert::IsTrue(VIDEO_MODE_SIGNAL_DIGITAL == videoConfig.GetSignalType(), L"Video Mode Config 'signal type' not set to 'digital'");

			videoConfig.SetChangesMade(false);

			//Set Signal Type to VIDEO_MODE_SIGNAL_BOTH
			videoConfig.SetSignalType(VIDEO_MODE_SIGNAL_BOTH);
			Assert::IsTrue(videoConfig.HasChangeBeenMade(), L"VideoModeConfig not reporting being changed");
			Assert::IsTrue(VIDEO_MODE_SIGNAL_BOTH == videoConfig.GetSignalType(), L"Video Mode Config 'signal type' not set to 'both'");

			videoConfig.SetChangesMade(false);

			//Set Signal Type to VIDEO_MODE_SIGNAL_AUTO
			videoConfig.SetSignalType(VIDEO_MODE_SIGNAL_AUTO);
			Assert::IsTrue(videoConfig.HasChangeBeenMade(), L"VideoModeConfig not reporting being changed");
			Assert::IsTrue(VIDEO_MODE_SIGNAL_AUTO == videoConfig.GetSignalType(), L"Video Mode Config 'signal type' not set to 'auto'");
		}

		TEST_METHOD(VideoModeConfigAddVideoModeTest)
		{
			CVideoModeConfig videoConfig;
			ConfirmDefaults(videoConfig);

			Assert::IsTrue(0 == videoConfig.GetVideoModeCount(), L"New VideoModeConfig not empty");

			CVideoMode videoMode;
			videoConfig.AddVideoMode(videoMode);

			Assert::IsTrue(1 == videoConfig.GetVideoModeCount(), L"VideoMode not added to VideoModeConfig");
		}
		
		TEST_METHOD(VideoModeConfigSelectVideoModeTest)
		{
			CVideoModeConfig videoConfig;
			ConfirmDefaults(videoConfig);

			Assert::IsTrue(0 == videoConfig.GetVideoModeCount(), L"New VideoModeConfig not empty");

			CVideoMode videoMode(25, entries[0], L"Test");
			videoConfig.AddVideoMode(videoMode);

			Assert::IsTrue(1 == videoConfig.GetVideoModeCount(), L"VideoMode not added to VideoModeConfig");

			// Selecting VideoMode should not work if still 'auto'
			videoConfig.SetSelectedVideoModeByIndex(0);
			Assert::IsFalse(0 == videoConfig.GetSelectedVideoModeIndex(), L"VideoMode selected when 'auto'");

			// Selecting VideoMode shoudl work now.
			videoConfig.SetVideoModeToAuto(false);
			videoConfig.SetSelectedVideoModeByIndex(0);

			Assert::IsTrue(0 == videoConfig.GetSelectedVideoModeIndex(), L"VideoMode not selected");
			Assert::IsTrue(videoMode.Id() == videoConfig.GetSelectedVideoModeID(), L"VideoMode not correct 'id'");
			Assert::IsTrue(0 == videoConfig.GetSelectedVideoModeName().Compare(L"25 Test"), L"VideoMode not correct 'name'");
		}

		TEST_METHOD(VideoModeConfigHasValidChangeTest)
		{
			CVideoModeConfig videoConfig;
			ConfirmDefaults(videoConfig);
			videoConfig.SetVideoModeToAuto(false);
			Assert::IsTrue(videoConfig.HasChangeBeenMade(), L"VideoModeConfig not report 'auto' has changed");

			// Change not valid until video mode selected
			Assert::IsFalse(videoConfig.HasValidChange(), L"VideoModeCongfig reporting valid before video mode selected");

			Assert::IsTrue(0 == videoConfig.GetVideoModeCount(), L"New VideoModeConfig not empty");
			CVideoMode videoMode(25, entries[0], L"Test");
			videoConfig.AddVideoMode(videoMode);
			Assert::IsTrue(1 == videoConfig.GetVideoModeCount(), L"VideoMode not added to VideoModeConfig");
			videoConfig.SetSelectedVideoModeByIndex(0);

			// Change should be valid now
			Assert::IsTrue(videoConfig.HasValidChange(), L"VideoModeCongfig not reporting valid change");
		}
		
		TEST_METHOD(VideoModeConfigGetSetAutoTest)
		{
			CVideoModeConfig videoConfig;
			ConfirmDefaults(videoConfig);

			CVideoMode videoMode(25, entries[0], L"Test");
			videoConfig.AddVideoMode(videoMode);
			Assert::IsTrue(1 == videoConfig.GetVideoModeCount(), L"VideoMode not added to VideoModeConfig");

			// Set Auto Detect to 'false'
			videoConfig.SetVideoModeToAuto(false);
			Assert::IsTrue(videoConfig.HasChangeBeenMade(), L"VideoModeConfig not reporting being changed");
			Assert::IsFalse(videoConfig.IsVideoModeAuto(), L"VideoModeConfig Auto Detect not successfully set to 'false'");

			// Should only work if notin 'auto' mode
			videoConfig.SetSelectedVideoModeByIndex(0);
			Assert::IsTrue(0 == videoConfig.GetSelectedVideoModeIndex(), L"VideoMode not selected");
			Assert::IsFalse(videoConfig.GetSelectedVideoModeIndex() == videoConfig.GetAutoConfigVideoModeIndex(), L"VideoMode selected should not match 'auto'");

			videoConfig.SetChangesMade(false);

			// Set Auto Detect to 'true'
			videoConfig.SetVideoModeToAuto(true);
			Assert::IsTrue(videoConfig.HasChangeBeenMade(), L"VideoModeConfig not reporting being changed");
			Assert::IsTrue(videoConfig.IsVideoModeAuto(), L"VideoModeConfig Auto Detect not successfully set to 'true'");
			Assert::IsTrue(videoConfig.GetSelectedVideoModeIndex() == videoConfig.GetAutoConfigVideoModeIndex(), L"VideoMode selected should match 'auto'");
		}
		
		TEST_METHOD(VideoModeConfigGetSetNativeTest)
		{
			CVideoModeConfig videoConfig;
			ConfirmDefaults(videoConfig);

			CVideoMode videoMode(25, entries[0], L"Test");
			videoConfig.AddVideoMode(videoMode);
			Assert::IsTrue(1 == videoConfig.GetVideoModeCount(), L"VideoMode not added to VideoModeConfig");

			// Set Native to 'true'
			videoConfig.SetVideoModeToNative(true);
			Assert::IsTrue(videoConfig.HasChangeBeenMade(), L"VideoModeConfig not reporting being changed");
			Assert::IsTrue(videoConfig.IsVideoModeNative(), L"VideoModeConfig Native not successfully set to 'true'");

			// Should only work if in 'native' mode
			videoConfig.SetNativeConfigVideoModeByID(25);
			Assert::IsTrue(videoMode.Id() == videoConfig.GetSelectedVideoModeID(), L"VideoMode not selected");

			videoConfig.SetChangesMade(false);

			// Set Native to 'false'
			videoConfig.SetVideoModeToNative(false);
			Assert::IsTrue(videoConfig.HasChangeBeenMade(), L"VideoModeConfig not reporting being changed");
			Assert::IsFalse(videoConfig.IsVideoModeNative(), L"VideoModeConfig Native not successfully set to 'false'");
			
			// Should not work if in 'native' mode
			videoConfig.SetVideoModeToAuto(false); // Can't be 'auto' either. Otherwise, won't select.
			videoConfig.SetSelectedVideoModeByIndex(INVALID_VIDEO_MODE_INDEX);
			videoConfig.SetNativeConfigVideoModeByID(25);
			Assert::IsTrue(INVALID_VIDEO_MODE_INDEX == videoConfig.GetSelectedVideoModeIndex(), L"VideoMode set by 'native' mode");
		}

		TEST_METHOD(VideoModeConfigCopytConstructorTest)
		{
			CVideoModeConfig start;
			ConfirmDefaults(start);
			Assert::IsTrue(0 == start.GetVideoModeCount(), L"New VideoModeConfig not empty");

			start.SetConfigForInput();
			start.SetSignalType(VIDEO_MODE_SIGNAL_ANALOG);
			start.SetVideoModeToAuto(false);
			start.SetVideoModeToNative(true);

			CVideoMode videoMode(25, entries[0], L"Test");
			start.AddVideoMode(videoMode);
			Assert::IsTrue(1 == start.GetVideoModeCount(), L"VideoMode not added to VideoModeConfig");

			start.SetNativeConfigVideoModeByID(25);

			CVideoModeConfig videoConfig0(start);
			Assert::IsTrue(videoConfig0.IsConfigForInput(), L"VideoModeConfig not copied as 'input'");
			Assert::IsTrue(VIDEO_MODE_SIGNAL_ANALOG == videoConfig0.GetSignalType(), L"VideoModeConfig 'signal type' not copied as 'analog'");
			Assert::IsTrue(videoConfig0.HasChangeBeenMade(), L"VideoModeConfig not copied as 'has changed'");
			Assert::IsFalse(videoConfig0.IsVideoModeAuto(), L"VideoModeConfig Auto Detect not copied as 'false'");
			Assert::IsTrue(videoConfig0.IsVideoModeNative(), L"VideoModeConfig Native not copied 'true'");
			Assert::IsTrue(videoMode.Id() == videoConfig0.GetSelectedVideoModeID(), L"VideoMode not selected");

			CVideoModeConfig videoConfig1 = start;
			Assert::IsTrue(videoConfig1.IsConfigForInput(), L"VideoModeConfig not copied as 'input'");
			Assert::IsTrue(VIDEO_MODE_SIGNAL_ANALOG == videoConfig1.GetSignalType(), L"VideoModeConfig 'signal type' not copied as 'analog'");
			Assert::IsTrue(videoConfig1.HasChangeBeenMade(), L"VideoModeConfig not copied as 'has changed'");
			Assert::IsFalse(videoConfig1.IsVideoModeAuto(), L"VideoModeConfig Auto Detect not copied as 'false'");
			Assert::IsTrue(videoConfig1.IsVideoModeNative(), L"VideoModeConfig Native not copied 'true'");
			Assert::IsTrue(videoMode.Id() == videoConfig1.GetSelectedVideoModeID(), L"VideoMode not selected");
		}

	};

	const VID_TABLE_ENTRY VideoModeConfigUnitTests::entries[] = {
	//                  PllFreq, XActive, YActive, XFront, XSync, XBlank, YFront, YSync, YBlank, HRefreshFreq, HSyncPolarity, VSyncPolarity
		{  1024*768*REFRESHRATE,    1024,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{   640*480*REFRESHRATE,     640,     480,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{   800*600*REFRESHRATE,     800,     600,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{ 1280*1024*REFRESHRATE,    1280,    1024,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{  1360*768*REFRESHRATE,    1360,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{  1366*768*REFRESHRATE,    1366,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
	};

}
#endif //TODO-PORT