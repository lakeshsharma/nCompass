#if 000//TODO-PORT

#include "stdafx.h"
#include "Hardware\Edid.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(EdidUnitTests)
	{
	public:
		static const bool EXPECTED_STANDARDTIMING = true;
		static const USHORT EXPECTED_HORIZONTAL = 1280;
		static const USHORT EXPECTED_VERTICAL = 1024;
		static const ULONG EXPECTED_REFRESHRATE = 60;
		static const UINT EXPECTED_EDID_LENGTH = 128;

		TEST_METHOD(EgmEdidUpdateTest)
		{
			CEgmEdid edid;

			Assert::IsFalse(edid.Updated(), L"Updated() returns 'true' before update has happened");
			Assert::IsTrue(EXPECTED_EDID_LENGTH == edid.m_len, L"Edid reported buffer length other than expected");

			bool success = edid.Update();
			Assert::IsTrue(success == edid.Updated(), L"Updated() not matching what is returned by Update()");
			Assert::IsTrue(success, L"Unable to initialize CEgmEdid");
			Assert::IsTrue(EXPECTED_STANDARDTIMING == edid.StandardTiming(), L"Standard Timing not reported as expected from VirtualLiteDLL");

			CVideoTiming expected(EXPECTED_HORIZONTAL, EXPECTED_VERTICAL, EXPECTED_REFRESHRATE);
			CVideoTiming timing = edid.GetVideoTiming();
			Assert::IsTrue(expected.GetHorizontal() == timing.GetHorizontal(), L"CSdvoEdid reports wrong horizontal from VirtualLiteDLL");
			Assert::IsTrue(expected.GetVertical() == timing.GetVertical(), L"CSdvoEdid reports wrong vertical from VirtualLiteDLL");
			Assert::IsTrue(expected.GetRefreshRate() == timing.GetRefreshRate(), L"CSdvoEdid reports wrong refreshrate from VirtualLiteDLL");
		}

		TEST_METHOD(EgmEdidEqualsTest)
		{
			CEgmEdid edid0;
			CEgmEdid edid1;

			bool success = false;

			success = edid0.Update();
			Assert::IsTrue(success, L"Unable to initialize first CEgmEdid");

			success = edid1.Update();
			Assert::IsTrue(success, L"Unable to initialize second CEgmEdid");

			Assert::IsTrue(edid0 == edid1, L"EGM Edid created from the same source not equal");
		}

		TEST_METHOD(MonitorEdidUpdateTest)
		{
			CMonitorEdid edid;

			Assert::IsFalse(edid.Updated(), L"Updated() returns 'true' before update has happened");
			Assert::IsTrue(EXPECTED_EDID_LENGTH == edid.m_len, L"Edid reported buffer length other than expected");

			bool success = edid.Update();
			Assert::IsTrue(success == edid.Updated(), L"Updated() not matching what is returned by Update()");
			Assert::IsTrue(success, L"Unable to initialize CMonitorEdid");
			Assert::IsTrue(EXPECTED_STANDARDTIMING == edid.StandardTiming(), L"Standard Timing not reported as expected from VirtualLiteDLL");

			CVideoTiming expected(EXPECTED_HORIZONTAL, EXPECTED_VERTICAL, EXPECTED_REFRESHRATE);
			CVideoTiming timing = edid.GetVideoTiming();
			Assert::IsTrue(expected.GetHorizontal() == timing.GetHorizontal(), L"CSdvoEdid reports wrong horizontal from VirtualLiteDLL");
			Assert::IsTrue(expected.GetVertical() == timing.GetVertical(), L"CSdvoEdid reports wrong vertical from VirtualLiteDLL");
			Assert::IsTrue(expected.GetRefreshRate() == timing.GetRefreshRate(), L"CSdvoEdid reports wrong refresh rate from VirtualLiteDLL");
		}

		TEST_METHOD(MonitorEdidEqualsTest)
		{
			CMonitorEdid edid0;
			CMonitorEdid edid1;

			bool success = false;

			success = edid0.Update();
			Assert::IsTrue(success, L"Unable to initialize first CMonitorEdid");

			success = edid1.Update();
			Assert::IsTrue(success, L"Unable to initialize second CMonitorEdid");

			Assert::IsTrue(edid0 == edid1, L"EGM Edid created from the same source not equal");
		}

		TEST_METHOD(SdvoEdidUpdateTest)
		{
			CSdvoEdid edid;

			Assert::IsFalse(edid.Updated(), L"Updated() returns 'true' before update has happened");
			Assert::IsTrue(EXPECTED_EDID_LENGTH == edid.m_len, L"Edid reported buffer length other than expected");

			bool success = edid.Update();
			Assert::IsTrue(success == edid.Updated(), L"Updated() not matching what is returned by Update()");
			Assert::IsTrue(success, L"Unable to initialize CSdvoEdid");
			Assert::IsTrue(EXPECTED_STANDARDTIMING == edid.StandardTiming(), L"Standard Timing not reported as expected from VirtualLiteDLL");

			CVideoTiming expected(EXPECTED_HORIZONTAL, EXPECTED_VERTICAL, EXPECTED_REFRESHRATE);
			CVideoTiming timing = edid.GetVideoTiming();
			Assert::IsTrue(expected.GetHorizontal() == timing.GetHorizontal(), L"CSdvoEdid reports wrong horizontal from VirtualLiteDLL");
			Assert::IsTrue(expected.GetVertical() == timing.GetVertical(), L"CSdvoEdid reports wrong vertical from VirtualLiteDLL");
			Assert::IsTrue(expected.GetRefreshRate() == timing.GetRefreshRate(), L"CSdvoEdid reports wrong refresh rate from VirtualLiteDLL");
		}

		TEST_METHOD(SdvoEdidEqualsTest)
		{
			CSdvoEdid edid0;
			CSdvoEdid edid1;

			bool success = false;

			success = edid0.Update();
			Assert::IsTrue(success, L"Unable to initialize first CSdvoEdid");

			success = edid1.Update();
			Assert::IsTrue(success, L"Unable to initialize second CSdvoEdid");

			Assert::IsTrue(edid0 == edid1, L"EGM Edid created from the same source not equal");
		}

		TEST_METHOD(SdvoEdid_Format_log)
		{
			CSdvoEdid edid;

			ostringstream ostr;
			edid.Format(ostr, LogManipulators());

			OutputDebugStringA("#### CSdvoEdid Format ####\n");
			OutputDebugStringA(ostr.str().c_str());
			OutputDebugStringA("\n#### CSdvoEdid Format ####\n");
		}

		TEST_METHOD(SdvoEdid_Format_html)
		{
			CSdvoEdid edid;

			ostringstream ostr;
			edid.Format(ostr, HtmlManipulators());

			OutputDebugStringA("#### CSdvoEdid Format ####\n");
			OutputDebugStringA(ostr.str().c_str());
			OutputDebugStringA("\n#### CSdvoEdid Format ####\n");
		}
	};
}
#endif //TODO-PORT
