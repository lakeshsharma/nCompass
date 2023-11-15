#if 000//TODO-PORT
#include "stdafx.h"
#include "CppUnitTest.h"

#include "Hardware\VideoTiming.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(VideoTimingUnitTests)
	{
	public:
		
		TEST_METHOD(VideoTimingDefaultConstructorTest)
		{
			CVideoTiming timing;
			Assert::IsTrue(0 == timing.GetHorizontal(), L"Horizontal not default.");
			Assert::IsTrue(0 == timing.GetHorzBlank(), L"HorzBlank not default.");
			Assert::IsTrue(0 == timing.GetVertical(), L"Vertical not default.");
			Assert::IsTrue(0 == timing.GetVertBlank(), L"VertBlank not default.");
			Assert::IsTrue(0 == timing.GetPixelClock(), L"PixelClock not default.");
			Assert::IsTrue(0 == timing.GetRefreshRate(), L"RefreshRate not default.");
		}

		TEST_METHOD(VideoTimingVidTableEntryConstructorTest)
		{
			VID_TABLE_ENTRY entry = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

			CVideoTiming timing(entry);

			Assert::IsTrue(entry.XActive == timing.GetHorizontal(), L"Horizontal not what was passed.");
			Assert::IsTrue(entry.XBlank == timing.GetHorzBlank(), L"HorzBlank not what was passed.");
			Assert::IsTrue(entry.YActive == timing.GetVertical(), L"Vertical not what was passed.");
			Assert::IsTrue(entry.YBlank == timing.GetVertBlank(), L"VertBlank not what was passed.");
			Assert::IsTrue(entry.PllFreq == timing.GetPixelClock(), L"PixelClock not what was passed.");
			Assert::IsTrue(RefreshRate(entry) == timing.GetRefreshRate(), L"RefreshRate not what was passed.");
		}

		TEST_METHOD(VideoTimingClockXYConstructorTest)
		{
			CVideoTiming timing(0, 1, 2, 3, 4);

			Assert::IsTrue(1 == timing.GetHorizontal(), L"Horizontal not what was passed.");
			Assert::IsTrue(2 == timing.GetHorzBlank(), L"HorzBlank not what was passed.");
			Assert::IsTrue(3 == timing.GetVertical(), L"Vertical not what was passed.");
			Assert::IsTrue(4 == timing.GetVertBlank(), L"VertBlank not what was passed.");
			Assert::IsTrue(0 == timing.GetPixelClock(), L"PixelClock not what was passed.");
			Assert::IsTrue(RefreshRate(0, 1 + 2, 3 + 4) == timing.GetRefreshRate(), L"RefreshRate not what was passed.");
		}

		TEST_METHOD(VideoTimingXYFConstructorTest)
		{
			CVideoTiming timing(1024, 768, 60);

			Assert::IsTrue(1024 == timing.GetHorizontal(), L"Horizontal not what was passed.");
			Assert::IsTrue(768 == timing.GetVertical(), L"Vertical not what was passed.");
			Assert::IsTrue(60 == timing.GetRefreshRate(), L"RefreshRate not what was passed.");
		}

		TEST_METHOD(VideoTimingCopyConstructorTest)
		{
			VID_TABLE_ENTRY entry = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

			CVideoTiming timing0(entry);
			CVideoTiming timing1 = timing0;

			Assert::IsTrue(entry.XActive == timing1.GetHorizontal(), L"Horizontal not what was copied.");
			Assert::IsTrue(entry.XBlank == timing1.GetHorzBlank(), L"HorzBlank not what was copied.");
			Assert::IsTrue(entry.YActive == timing1.GetVertical(), L"Vertical not what was copied.");
			Assert::IsTrue(entry.YBlank == timing1.GetVertBlank(), L"VertBlank not what was copied.");
			Assert::IsTrue(entry.PllFreq == timing1.GetPixelClock(), L"PixelClock not what was copied.");
			Assert::IsTrue(RefreshRate(entry) == timing1.GetRefreshRate(), L"RefreshRate not what was copied.");
		}

		TEST_METHOD(VideoTimingAssignmentTest)
		{
			VID_TABLE_ENTRY entry = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

			CVideoTiming timing0(entry);
			CVideoTiming timing1;
			timing1 = timing0;

			Assert::IsTrue(entry.XActive == timing1.GetHorizontal(), L"Horizontal not what was copied.");
			Assert::IsTrue(entry.XBlank == timing1.GetHorzBlank(), L"HorzBlank not what was copied.");
			Assert::IsTrue(entry.YActive == timing1.GetVertical(), L"Vertical not what was copied.");
			Assert::IsTrue(entry.YBlank == timing1.GetVertBlank(), L"VertBlank not what was copied.");
			Assert::IsTrue(entry.PllFreq == timing1.GetPixelClock(), L"PixelClock not what was copied.");
			Assert::IsTrue(RefreshRate(entry) == timing1.GetRefreshRate(), L"RefreshRate not what was copied.");
		}

		void TestRefreshRate(ULONG pllFreq, USHORT horz, USHORT vert, ULONG refreshRate)
		{
			ULONG result = 0;

			result = RefreshRate(pllFreq, horz, vert);
			Assert::IsTrue(refreshRate == result, L"RefreshRate not calcuated correctly using x, y, and frequency.");

			VID_TABLE_ENTRY entry;
			entry.PllFreq = pllFreq;
			entry.XActive = horz - 10;
			entry.XBlank = 10;
			entry.YActive = vert - 10;
			entry.YBlank = 10;

			result = RefreshRate(entry);
			Assert::IsTrue(refreshRate == result, L"RefreshRate not calcuated correctly using VID_TABLE_ENTRY.");

			VID_PARAMS params;
			params.DotClock = pllFreq;
			params.XTotal = horz;
			params.YTotal = vert;

			result = RefreshRate(params);
			Assert::IsTrue(refreshRate == result, L"RefreshRate not calcuated correctly using VID_PARAMS.");
		}

		TEST_METHOD(RefreshRateTest)
		{
			const DWORD HORZ = 1024;
			const DWORD VERT = 768;
			const DWORD REFRESHRATE = 60;
			const ULONG PLLFREQ = HORZ*768*REFRESHRATE;

			// Standard RefreshRate call
			TestRefreshRate(PLLFREQ, HORZ, VERT, REFRESHRATE);

			ULONG rounding = (ULONG)(HORZ*VERT*0.5) - 1;

			// RefreshRate Round down
			TestRefreshRate(PLLFREQ + rounding, HORZ, VERT, REFRESHRATE);

			// RefreshRate Round up
			TestRefreshRate(PLLFREQ - rounding, HORZ, VERT, REFRESHRATE);
		}
	};
}
#endif //TODO-PORT