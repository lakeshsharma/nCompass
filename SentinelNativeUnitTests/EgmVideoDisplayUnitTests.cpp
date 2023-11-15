#if 000//TODO-PORT
#include "stdafx.h"
#include "CppUnitTest.h"

#include "Hardware\EgmVideoDisplay.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(EgmVideoDisplayUnitTests)
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
		
		TEST_METHOD(EgmVideoDisplayUpdateTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CEgmVideoDisplay egm(table);

			Assert::IsFalse(egm.Updated(), L"Updated() returns 'true' before update has happened");

			bool success = egm.Update();
			Assert::IsTrue(success == egm.Updated(), L"Updated() not matching what is returned by Update()");
			Assert::IsTrue(success, L"Unable to initialize CEgmVideoDisplay");
		}

		TEST_METHOD(EgmVideoDisplayGetResolutionTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CEgmVideoDisplay egm(table);

			bool success = false;

			success = egm.Update();
			Assert::IsTrue(success, L"Unable to initialize CEgmVideoDisplay");

			SIZE current;
			success = egm.GetResolution(current);
			Assert::IsTrue(success, L"Unable to get current egm resolution");
			Assert::IsTrue(DEFAULT_EGM_WIDTH == current.cx, L"Wrong x egm resolution");
			Assert::IsTrue(DEFAULT_EGM_HEIGHT == current.cy, L"Wrong y egm resolution");
		}

		TEST_METHOD(EgmVideoDisplayGetMatchingModeTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CEgmVideoDisplay egm(table);

			bool success = false;

			success = egm.Update();
			Assert::IsTrue(success, L"Unable to initialize CEgmVideoDisplay");

			CVideoMode vmode = egm.GetMatchingVideoMode();
			Assert::IsTrue(DEFAULT_EGM_WIDTH == vmode.GetVideoTiming().GetHorizontal(), L"Wrong x egm resolution");
			Assert::IsTrue(DEFAULT_EGM_HEIGHT == vmode.GetVideoTiming().GetVertical(), L"Wrong y egm resolution");
		}

		TEST_METHOD(EgmVideoDisplayGetSetVideoModeIdTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CEgmVideoDisplay egm(table);

			bool success = false;

			success = egm.Update();
			Assert::IsTrue(success, L"Unable to initialize CEgmVideoDisplay");

			byte original = egm.GetVideoModeId();
			byte newModeId = original + 1;

			bool modeChanged = egm.SetVideoModeId(newModeId);
			Assert::IsTrue(modeChanged, L"Vidoe Mode Id reports not changed");

			byte current = egm.GetVideoModeId();
			Assert::IsTrue(newModeId == current, L"Video Mode Id remains unchanged");

			SIZE resolution;
			success = egm.GetResolution(resolution);
			Assert::IsTrue(success, L"Unable to get current egm resolution");
			Assert::IsTrue(entries[current].XActive == resolution.cx, L"Wrong x egm resolution");
			Assert::IsTrue(entries[current].YActive == resolution.cy, L"Wrong y egm resolution");

			CVideoMode vmode = egm.GetMatchingVideoMode();
			Assert::IsTrue(entries[current].XActive == vmode.GetVideoTiming().GetHorizontal(), L"Wrong x egm resolution");
			Assert::IsTrue(entries[current].YActive == vmode.GetVideoTiming().GetVertical(), L"Wrong y egm resolution");

			// Restore Video Mode Id
			modeChanged = egm.SetVideoModeId(original);
			Assert::IsTrue(modeChanged, L"Vidoe Mode Id reports not restored");

			current = egm.GetVideoModeId();
			Assert::IsTrue(original == current, L"Video Mode Id remains not resoted");

			success = egm.GetResolution(resolution);
			Assert::IsTrue(success, L"Unable to get current egm resolution");
			Assert::IsTrue(entries[original].XActive == resolution.cx, L"Wrong x egm resolution");
			Assert::IsTrue(entries[original].YActive == resolution.cy, L"Wrong y egm resolution");

			vmode = egm.GetMatchingVideoMode();
			Assert::IsTrue(entries[original].XActive == vmode.GetVideoTiming().GetHorizontal(), L"Wrong x egm resolution");
			Assert::IsTrue(entries[original].YActive == vmode.GetVideoTiming().GetVertical(), L"Wrong y egm resolution");
		}

		TEST_METHOD(EgmVideoDisplayGetSetAnalogOffsetsTest)
		{
			CTestVideoModeTable table;
			table.BuildYourself();

			CEgmVideoDisplay egm(table);

			bool success = false;

			success = egm.Update();
			Assert::IsTrue(success, L"Unable to initialize CEgmVideoDisplay");

			POINT original;
			success = egm.GetAnalogOffsets(original);
			Assert::IsTrue(success, L"Unable to get original offsets");

			POINT newOffsets(original);
			newOffsets.x += 1;
			newOffsets.y += 1;

			success = egm.SetAnalogOffsets(newOffsets);
			Assert::IsTrue(success, L"Setting new offests not reported successful");

			POINT current;
			success = egm.GetAnalogOffsets(current);
			Assert::IsTrue(success, L"Unable to get current offsets");
			Assert::IsTrue(newOffsets.x == current.x, L"Wrong x offset");
			Assert::IsTrue(newOffsets.y == current.y, L"Wrong y offset");

			// Restore Analog Offsets
			success = egm.SetAnalogOffsets(original);
			Assert::IsTrue(success, L"Restoring offests not reported successful");

			success = egm.GetAnalogOffsets(current);
			Assert::IsTrue(success, L"Unable to get current offsets");
			Assert::IsTrue(original.x == current.x, L"Wrong x offset");
			Assert::IsTrue(original.y == current.y, L"Wrong y offset");
		}

		TEST_METHOD(EgmVideoDisplayGetSetSignalTypeTest)
		{
			static const byte SIGNAL_TYPE_COUNT = VIDEO_SIGNAL_TYPE_DIGITAL + 1;

			byte original = CEgmVideoDisplay::GetSignalType();

			byte newType = (original + 1) % SIGNAL_TYPE_COUNT;

			CEgmVideoDisplay::SetSignalType(newType);

			byte current = CEgmVideoDisplay::GetSignalType();
			Assert::IsTrue(newType == current, L"Signal Type not set");

			// Restore Signal Type
			CEgmVideoDisplay::SetSignalType(original);
			current = CEgmVideoDisplay::GetSignalType();
			Assert::IsTrue(original == current, L"Signal Type not restored");
		}

	};

	const VID_TABLE_ENTRY EgmVideoDisplayUnitTests::entries[] = {
	//                  PllFreq, XActive, YActive, XFront, XSync, XBlank, YFront, YSync, YBlank, HRefreshFreq, HSyncPolarity, VSyncPolarity
		{  1024*768*REFRESHRATE,    1024,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{   640*480*REFRESHRATE,     640,     480,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{   800*600*REFRESHRATE,     800,     600,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{ 1280*1024*REFRESHRATE,    1280,    1024,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{  1360*768*REFRESHRATE,    1360,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
		{  1366*768*REFRESHRATE,    1366,     768,      0,     0,      0,      0,     0,      0,  REFRESHRATE,             0,             0 },
	};

	void EgmVideoDisplayUnitTests::CTestVideoModeTable::BuildYourself()
	{
		for (int n = 0; n < _countof(entries); n++)
		{
			AddVideoMode(new CVideoMode(n, entries[n], L"EGM Test Mode"));
		}
	}
}
#endif //TODO-PORT