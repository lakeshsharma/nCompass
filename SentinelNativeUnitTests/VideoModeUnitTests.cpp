#if 000//TODO-PORT
#include "stdafx.h"
#include "CppUnitTest.h"

#include "Hardware/VideoMode.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(VideoModeUnitTests)
	{
	public:
		
		void VideoTimingEquals(const CVideoTiming timing0, const CVideoTiming timing1)
		{
			Assert::IsTrue(timing0.GetHorizontal() == timing1.GetHorizontal(), L"Horizontal wrong.");
			Assert::IsTrue(timing0.GetHorzBlank() == timing1.GetHorzBlank(), L"HorzBalnk wrong.");
			Assert::IsTrue(timing0.GetVertical() == timing1.GetVertical(), L"Vertical wrong.");
			Assert::IsTrue(timing0.GetVertBlank() == timing1.GetVertBlank(), L"VertBlank wrong.");
			Assert::IsTrue(timing0.GetPixelClock() == timing1.GetPixelClock(), L"PixelClock wrong.");
			Assert::IsTrue(timing0.GetRefreshRate() == timing1.GetRefreshRate(), L"RefreshRate wrong.");
		}

		TEST_METHOD(VideoModeDefaultConstructorTest)
		{
			CVideoMode mode;
			Assert::IsTrue(AUTODETECT_VIDEOMODE == mode.Id(), L"CVideoMode default Id() not AUTODETECT_VIDEOMODE.");
			Assert::IsTrue(0 == lstrcmp(L"", mode.GetName()), L"CVideoMode default GeName() not empty.");

			CVideoTiming timing;
			VideoTimingEquals(timing, mode.GetVideoTiming());
		}

		TEST_METHOD(VideoModeVidTableEntryConstructorTest)
		{
			VID_TABLE_ENTRY entry = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

			CVideoMode mode(10, entry, L"test");
			Assert::IsTrue(10 == mode.Id(), L"CVideoMode Id() not what was passed.");
			Assert::IsTrue(0 == lstrcmp(L"test", mode.GetName()), L"CVideoMode GeName() not what was passed.");

			CVideoTiming timing(entry);
			VideoTimingEquals(timing, mode.GetVideoTiming());
		}

		TEST_METHOD(VideoModeCopyConstructorTest)
		{
			VID_TABLE_ENTRY entry = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

			CVideoMode mode0(10, entry, L"test");
			CVideoMode mode1 = mode0;
			Assert::IsTrue(10 == mode1.Id(), L"CVideoMode Id() not what was copied.");
			Assert::IsTrue(0 == lstrcmp(L"test", mode1.GetName()), L"CVideoMode GeName() not what was copied.");

			VideoTimingEquals(mode0.GetVideoTiming(), mode1.GetVideoTiming());
		}

		TEST_METHOD(VideoModeAssignmentTest)
		{
			VID_TABLE_ENTRY entry = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

			CVideoMode mode0(10, entry, L"test");
			CVideoMode mode1;
			mode1 = mode0;

			Assert::IsTrue(10 == mode1.Id(), L"CVideoMode Id() not what was copied.");
			Assert::IsTrue(0 == lstrcmp(L"test", mode1.GetName()), L"CVideoMode GeName() not what was copied.");

			VideoTimingEquals(mode0.GetVideoTiming(), mode1.GetVideoTiming());
		}

	};
}
#endif //TODO-PORT