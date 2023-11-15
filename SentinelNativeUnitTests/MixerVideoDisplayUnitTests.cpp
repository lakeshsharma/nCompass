#if 000//TODO-PORT

#include "stdafx.h"
#include "CppUnitTest.h"

#include "Hardware\MixerVideoDisplay.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(MixerVideoDisplayUnitTests)
	{
		static const DWORD REFRESHRATE = 60;

		static const LONG DEFAULT_EGM_WIDTH = 1024;
		static const LONG DEFAULT_EGM_HEIGHT = 768;

		static const VID_TABLE_ENTRY entries[];

		class CTestVideoModeTable : public CVideoModeTable
		{
		public:
			void BuildYourself();
		};
			
	public:
			
		TEST_METHOD(MixerVideoDisplayUpdateTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CMixerVideoDisplay mixer(table);

			Assert::IsFalse(mixer.Updated(), L"Updated() returns 'true' before update has happened");

			bool success = mixer.Update();
			Assert::IsTrue(success == mixer.Updated(), L"Updated() not matching what is returned by Update()");
			Assert::IsTrue(success, L"Unable to initialize CMixerVideoDisplay");
		}

		TEST_METHOD(MixerVideoDisplayGetResolutionTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CMixerVideoDisplay mixer(table);

			bool success = false;

			success = mixer.Update();
			Assert::IsTrue(success, L"Unable to initialize CMixerVideoDisplay");

			SIZE current;
			success = mixer.GetResolution(current);
			Assert::IsTrue(success, L"Unable to get current mixer resolution");
			Assert::IsTrue(DEFAULT_EGM_WIDTH == current.cx, L"Wrong x mixer resolution");
			Assert::IsTrue(DEFAULT_EGM_HEIGHT == current.cy, L"Wrong y mixer resolution");
		}

		TEST_METHOD(MixerVideoDisplayGetMatchingModeTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CMixerVideoDisplay mixer(table);

			bool success = false;

			success = mixer.Update();
			Assert::IsTrue(success, L"Unable to initialize CMixerVideoDisplay");

			CVideoMode vmode = mixer.GetMatchingVideoMode();
			Assert::IsTrue(DEFAULT_EGM_WIDTH == vmode.GetVideoTiming().GetHorizontal(), L"Wrong x mixer resolution");
			Assert::IsTrue(DEFAULT_EGM_HEIGHT == vmode.GetVideoTiming().GetVertical(), L"Wrong y mixer resolution");
		}

		TEST_METHOD(MixerVideoDisplayGetSetVideoModeIdTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CMixerVideoDisplay mixer(table);

			bool success = false;

			success = mixer.Update();
			Assert::IsTrue(success, L"Unable to initialize CMixerVideoDisplay");

			byte original = mixer.GetVideoModeId();
			byte newModeId = original + 1;

			bool modeChanged = mixer.SetVideoModeId(newModeId);
			Assert::IsTrue(modeChanged, L"Vidoe Mode Id reports not changed");

			byte current = mixer.GetVideoModeId();
			Assert::IsTrue(newModeId == current, L"Video Mode Id remains unchanged");

			SIZE resolution;
			success = mixer.GetResolution(resolution);
			Assert::IsTrue(success, L"Unable to get current mixer resolution");
			Assert::IsTrue(entries[current].XActive == resolution.cx, L"Wrong x mixer resolution");
			Assert::IsTrue(entries[current].YActive == resolution.cy, L"Wrong y mixer resolution");

			CVideoMode vmode = mixer.GetMatchingVideoMode();
			Assert::IsTrue(entries[current].XActive == vmode.GetVideoTiming().GetHorizontal(), L"Wrong x mixer resolution");
			Assert::IsTrue(entries[current].YActive == vmode.GetVideoTiming().GetVertical(), L"Wrong y mixer resolution");

			// Restore Video Mode Id
			modeChanged = mixer.SetVideoModeId(original);
			Assert::IsTrue(modeChanged, L"Vidoe Mode Id reports not restored");

			current = mixer.GetVideoModeId();
			Assert::IsTrue(original == current, L"Video Mode Id remains not resoted");

			success = mixer.GetResolution(resolution);
			Assert::IsTrue(success, L"Unable to get current mixer resolution");
			Assert::IsTrue(entries[original].XActive == resolution.cx, L"Wrong x mixer resolution");
			Assert::IsTrue(entries[original].YActive == resolution.cy, L"Wrong y mixer resolution");

			vmode = mixer.GetMatchingVideoMode();
			Assert::IsTrue(entries[original].XActive == vmode.GetVideoTiming().GetHorizontal(), L"Wrong x mixer resolution");
			Assert::IsTrue(entries[original].YActive == vmode.GetVideoTiming().GetVertical(), L"Wrong y mixer resolution");
		}

		TEST_METHOD(MixerVideoDisplayGetSetAnalogOffsetsTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CMixerVideoDisplay mixer(table);

			bool success = false;

			success = mixer.Update();
			Assert::IsTrue(success, L"Unable to initialize CMixerVideoDisplay");

			POINT original;
			success = mixer.GetAnalogOffsets(original);
			Assert::IsTrue(success, L"Unable to get original offsets");

			POINT newOffsets(original);
			newOffsets.x += 1;
			newOffsets.y += 1;

			success = mixer.SetAnalogOffsets(newOffsets);
			Assert::IsTrue(success, L"Setting new offests not reported successful");

			POINT current;
			success = mixer.GetAnalogOffsets(current);
			Assert::IsTrue(success, L"Unable to get current offsets");
			Assert::IsTrue(newOffsets.x == current.x, L"Wrong x offset");
			Assert::IsTrue(newOffsets.y == current.y, L"Wrong y offset");

			// Restore Analog Offsets
			success = mixer.SetAnalogOffsets(original);
			Assert::IsTrue(success, L"Restoring offests not reported successful");

			success = mixer.GetAnalogOffsets(current);
			Assert::IsTrue(success, L"Unable to get current offsets");
			Assert::IsTrue(original.x == current.x, L"Wrong x offset");
			Assert::IsTrue(original.y == current.y, L"Wrong y offset");
		}

		TEST_METHOD(MixerVideoDisplayGetSetSignalTypeTest)
		{
			static const byte SIGNAL_TYPE_COUNT = VIDEO_SIGNAL_TYPE_DIGITAL + 1;

			byte original = CMixerVideoDisplay::GetSignalType();

			byte newType = (original + 1) % SIGNAL_TYPE_COUNT;

			CMixerVideoDisplay::SetSignalType(newType);

			byte current = CMixerVideoDisplay::GetSignalType();
			Assert::IsTrue(newType == current, L"Signal Type not set");

			// Restore Signal Type
			CMixerVideoDisplay::SetSignalType(original);
			current = CMixerVideoDisplay::GetSignalType();
			Assert::IsTrue(original == current, L"Signal Type not restored");
		}

	};

	const VID_TABLE_ENTRY MixerVideoDisplayUnitTests::entries[] = {
	//                  PllFreq, XActive, YActive, XFront, XSync, XBlank, YFront, YSync, YBlank, HRefreshFreq, HSyncPolarity, VSyncPolarity
		{  1024*768*REFRESHRATE,    1024,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{   640*480*REFRESHRATE,     640,     480,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{   800*600*REFRESHRATE,     800,     600,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{ 1280*1024*REFRESHRATE,    1280,    1024,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{  1360*768*REFRESHRATE,    1360,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{  1366*768*REFRESHRATE,    1366,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
	};

	void MixerVideoDisplayUnitTests::CTestVideoModeTable::BuildYourself()
	{
		for (int n = 0; n < _countof(entries); n++)
		{
			AddVideoMode(new CVideoMode(n, entries[n], L"EGM Test Mode"));
		}
	}
}

#endif //TODO-PORT