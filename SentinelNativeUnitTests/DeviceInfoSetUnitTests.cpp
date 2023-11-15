#if 000//TODO-PORT

#include "stdafx.h"
#include "DeviceInfoSet.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(DeviceInfoSet)
	{
// #pragma region NumberOfDevicesByClassGUID
		class CDeviceInfoSet_TestNumDevices : public CDeviceInfoSet
		{
		public:
			CDeviceInfoSet_TestNumDevices(CPlatform &platform, CDisplayManager &displayManager)
				:
				CDeviceInfoSet(platform, displayManager),
				m_numberOfDevicesByClassGUID(0)
			{
			}

			void SetNumberOfDevicesByClassGUID(int numberOfDevicesByClassGUID)
			{
				m_numberOfDevicesByClassGUID = numberOfDevicesByClassGUID;
			}

			virtual int NumberOfDevicesByClassGUID(GUID classGuid, DWORD *pdwError = NULL)
			{
				UNREFERENCED_PARAMETER(classGuid);
				UNREFERENCED_PARAMETER(pdwError);
				return m_numberOfDevicesByClassGUID;
			}

		private:
			int m_numberOfDevicesByClassGUID;
		};

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevels::GetHardwareTFT
		/// Test that GetHardwareTFT() function is called as a virtual function and returns false.
		/// </summary>
		TEST_METHOD(NumberOfDevicesByClassGUID)
		{
			// Data.
			GUID classGuid = { 0 };

			// Setup.
			CPlatform platform(_T(""), 0, _T(""), _T(""));
			CDisplayManager_TestTft displayManager(false);
			CDeviceInfoSet_TestNumDevices objTest(platform, displayManager);
			CDeviceInfoSet &obj(objTest);

			// Perform operation(s) and test(s).
			objTest.SetNumberOfDevicesByClassGUID(-1);
			Assert::AreEqual((int)-1, objTest.NumberOfDevicesByClassGUID(classGuid), L"-1 objTest");
			Assert::AreEqual((int)-1, obj.NumberOfDevicesByClassGUID(classGuid), L"-1 obj");

			objTest.SetNumberOfDevicesByClassGUID(0);
			Assert::AreEqual((int)0, objTest.NumberOfDevicesByClassGUID(classGuid), L"0 objTest");
			Assert::AreEqual((int)0, obj.NumberOfDevicesByClassGUID(classGuid), L"0 obj");

			objTest.SetNumberOfDevicesByClassGUID(1);
			Assert::AreEqual((int)1, objTest.NumberOfDevicesByClassGUID(classGuid), L"1 objTest");
			Assert::AreEqual((int)1, obj.NumberOfDevicesByClassGUID(classGuid), L"1 obj");

			objTest.SetNumberOfDevicesByClassGUID(2);
			Assert::AreEqual((int)2, objTest.NumberOfDevicesByClassGUID(classGuid), L"2 objTest");
			Assert::AreEqual((int)2, obj.NumberOfDevicesByClassGUID(classGuid), L"2 obj");

			objTest.SetNumberOfDevicesByClassGUID(3);
			Assert::AreEqual((int)3, objTest.NumberOfDevicesByClassGUID(classGuid), L"3 objTest");
			Assert::AreEqual((int)3, obj.NumberOfDevicesByClassGUID(classGuid), L"3 obj");
		}
// #pragma endregion NumberOfDevicesByClassGUID

		class CDisplayManager_TestTft : public CDisplayManager
		{
		public:
			CDisplayManager_TestTft(bool hardwareTFT)
				: m_hardwareTFT(hardwareTFT)
			{
			}

			virtual bool GetHardwareTFT()
			{
				return m_hardwareTFT;
			}

		private:
			bool m_hardwareTFT;
		};

// #pragma region GetDefaultMouseCount
		/// <summary>
		/// Functional test for static constant platform ID values:
		/// </summary>
		TEST_METHOD(CDeviceInfoSet_ConstDefaultMouseCount)
		{
			// Perform operation(s) and test(s).
			Assert::AreEqual((int)1, CDeviceInfoSet::DEFAULT_MOUSE_COUNT, L"DEFAULT_MOUSE_COUNT");
			Assert::AreEqual((int)2, CDeviceInfoSet::BRAEMAC_BLUE_VIDEO_MIXER_DEFAULT_MOUSE_COUNT, L"BRAEMAC_BLUE_VIDEO_MIXER_DEFAULT_MOUSE_COUNT");
		}

		/// <summary>
		/// Functional test for:
		///     CDeviceInfoSet::GetDefaultMouseCount()
		/// Test with non-blue box platform with no video mixer.
		/// </summary>
		TEST_METHOD(GetDefaultMouseCount_NotBlueNoMixer)
		{
			// Data.
			const UINT platformId(CPlatform::AXIOMTEK_PLATFORMID);
			const bool hardwareTFT(true);
			const int defaultMouseCountExpected(1);

			// Setup.
			CPlatform platform(_T(""), platformId, _T(""), _T(""));
			CDisplayManager_TestTft displayManager(hardwareTFT);
			CDeviceInfoSet obj(platform, displayManager);

			// Perform operation(s) and test(s).
			Assert::AreEqual(defaultMouseCountExpected, obj.GetDefaultMouseCount(), L"obj.GetDefaultMouseCount");
		}

		/// <summary>
		/// Functional test for:
		///     CDeviceInfoSet::GetDefaultMouseCount()
		/// Test with non-blue box platform with video mixer.
		/// </summary>
		TEST_METHOD(GetDefaultMouseCount_NotBlueMixer)
		{
			// Data.
			const UINT platformId(CPlatform::AXIOMTEK_PLATFORMID);
			const bool hardwareTFT(false);
			const int defaultMouseCountExpected(1);

			// Setup.
			CPlatform platform(_T(""), platformId, _T(""), _T(""));
			CDisplayManager_TestTft displayManager(hardwareTFT);
			CDeviceInfoSet obj(platform, displayManager);

			// Perform operation(s) and test(s).
			Assert::AreEqual(defaultMouseCountExpected, obj.GetDefaultMouseCount(), L"obj.GetDefaultMouseCount");
		}

		/// <summary>
		/// Functional test for:
		///     CDeviceInfoSet::GetDefaultMouseCount()
		/// Test with blue box platform with no video mixer.
		/// </summary>
		TEST_METHOD(GetDefaultMouseCount_BlueNoMixer)
		{
			// Data.
			const UINT platformId(CPlatform::BRAEMAC_BLUE_PLATFORM_ID);
			const bool hardwareTFT(true);
			const int defaultMouseCountExpected(1);

			// Setup.
			CPlatform platform(_T(""), platformId, _T(""), _T(""));
			CDisplayManager_TestTft displayManager(hardwareTFT);
			CDeviceInfoSet obj(platform, displayManager);

			// Perform operation(s) and test(s).
			Assert::AreEqual(defaultMouseCountExpected, obj.GetDefaultMouseCount(), L"obj.GetDefaultMouseCount");
		}

		/// <summary>
		/// Functional test for:
		///     CDeviceInfoSet::GetDefaultMouseCount()
		/// Test with blue box platform with video mixer.
		/// </summary>
		TEST_METHOD(GetDefaultMouseCount_BlueMixer)
		{
			// Data.
			const UINT platformId(CPlatform::BRAEMAC_BLUE_PLATFORM_ID);
			const bool hardwareTFT(false);
			const int defaultMouseCountExpected(2);

			// Setup.
			CPlatform platform(_T(""), platformId, _T(""), _T(""));
			CDisplayManager_TestTft displayManager(hardwareTFT);
			CDeviceInfoSet obj(platform, displayManager);

			// Perform operation(s) and test(s).
			Assert::AreEqual(defaultMouseCountExpected, obj.GetDefaultMouseCount(), L"obj.GetDefaultMouseCount");
		}
// #pragma endregion GetDefaultMouseCount

// #pragma region CheckForUsbMouse
		/// <summary>
		/// Functional test for:
		///     CDeviceInfoSet::CheckForUsbMouse()
		/// Test with non-blue box platform with no video mixer.
		/// </summary>
		TEST_METHOD(CheckForUsbMouse_NotBlueNoMixer)
		{
			// Data.
			const UINT platformId(CPlatform::AXIOMTEK_PLATFORMID);
			const bool hardwareTFT(true);
			const int defaultMouseCountExpected(CDeviceInfoSet::DEFAULT_MOUSE_COUNT);

			// Setup.
			CPlatform platform(_T(""), platformId, _T(""), _T(""));
			CDisplayManager_TestTft displayManager(hardwareTFT);
			CDeviceInfoSet_TestNumDevices objTest(platform, displayManager);
			CDeviceInfoSet &obj(objTest);

			// Perform operation(s) and test(s).
			objTest.SetNumberOfDevicesByClassGUID(defaultMouseCountExpected);
			Assert::AreEqual(false, obj.CheckForUsbMouse(), L"obj.CheckForUsbMouse false");

			objTest.SetNumberOfDevicesByClassGUID(defaultMouseCountExpected + 1);
			Assert::AreEqual(true, obj.CheckForUsbMouse(), L"obj.CheckForUsbMouse true");
		}

		/// <summary>
		/// Functional test for:
		///     CDeviceInfoSet::CheckForUsbMouse()
		/// Test with non-blue box platform with video mixer.
		/// </summary>
		TEST_METHOD(CheckForUsbMouse_NotBlueMixer)
		{
			// Data.
			const UINT platformId(CPlatform::AXIOMTEK_PLATFORMID);
			const bool hardwareTFT(false);
			const int defaultMouseCountExpected(CDeviceInfoSet::DEFAULT_MOUSE_COUNT);

			// Setup.
			CPlatform platform(_T(""), platformId, _T(""), _T(""));
			CDisplayManager_TestTft displayManager(hardwareTFT);
			CDeviceInfoSet_TestNumDevices objTest(platform, displayManager);
			CDeviceInfoSet &obj(objTest);

			// Perform operation(s) and test(s).
			objTest.SetNumberOfDevicesByClassGUID(defaultMouseCountExpected);
			Assert::AreEqual(false, obj.CheckForUsbMouse(), L"obj.CheckForUsbMouse false");

			objTest.SetNumberOfDevicesByClassGUID(defaultMouseCountExpected + 1);
			Assert::AreEqual(true, obj.CheckForUsbMouse(), L"obj.CheckForUsbMouse true");
		}

		/// <summary>
		/// Functional test for:
		///     CDeviceInfoSet::CheckForUsbMouse()
		/// Test with blue box platform with no video mixer.
		/// </summary>
		TEST_METHOD(CheckForUsbMouse_BlueNoMixer)
		{
			// Data.
			const UINT platformId(CPlatform::BRAEMAC_BLUE_PLATFORM_ID);
			const bool hardwareTFT(true);
			const int defaultMouseCountExpected(CDeviceInfoSet::DEFAULT_MOUSE_COUNT);

			// Setup.
			CPlatform platform(_T(""), platformId, _T(""), _T(""));
			CDisplayManager_TestTft displayManager(hardwareTFT);
			CDeviceInfoSet_TestNumDevices objTest(platform, displayManager);
			CDeviceInfoSet &obj(objTest);

			// Perform operation(s) and test(s).
			objTest.SetNumberOfDevicesByClassGUID(defaultMouseCountExpected);
			Assert::AreEqual(false, obj.CheckForUsbMouse(), L"obj.CheckForUsbMouse false");

			objTest.SetNumberOfDevicesByClassGUID(defaultMouseCountExpected + 1);
			Assert::AreEqual(true, obj.CheckForUsbMouse(), L"obj.CheckForUsbMouse true");
		}

		/// <summary>
		/// Functional test for:
		///     CDeviceInfoSet::CheckForUsbMouse()
		/// Test with blue box platform with video mixer.
		/// </summary>
		TEST_METHOD(CheckForUsbMouse_BlueMixer)
		{
			// Data.
			const UINT platformId(CPlatform::BRAEMAC_BLUE_PLATFORM_ID);
			const bool hardwareTFT(false);
			const int defaultMouseCountExpected(CDeviceInfoSet::BRAEMAC_BLUE_VIDEO_MIXER_DEFAULT_MOUSE_COUNT);

			// Setup.
			CPlatform platform(_T(""), platformId, _T(""), _T(""));
			CDisplayManager_TestTft displayManager(hardwareTFT);
			CDeviceInfoSet_TestNumDevices objTest(platform, displayManager);
			CDeviceInfoSet &obj(objTest);

			// Perform operation(s) and test(s).
			objTest.SetNumberOfDevicesByClassGUID(defaultMouseCountExpected);
			Assert::AreEqual(false, obj.CheckForUsbMouse(), L"obj.CheckForUsbMouse false");

			objTest.SetNumberOfDevicesByClassGUID(defaultMouseCountExpected + 1);
			Assert::AreEqual(true, obj.CheckForUsbMouse(), L"obj.CheckForUsbMouse true");
		}
// #pragma endregion CheckForUsbMouse
	};
}

#endif //TODO-PORT