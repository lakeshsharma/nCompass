#if 000//TODO-PORT
#include "stdafx.h"
#include "CppUnitTest.h"

#include "Hardware\HIDContext.h"
#include "..\CardReaderLayerDll\CardReaderSimulator.h"
#include "UnitTestingUtils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define USB_VENDOR_ID_UIC						0x6352
#define USB_PRODUCT_ID_UIC_CRDRDR1				0x240A
#define USB_PRODUCT_ID_UIC_CRDRDR2				0x240B
#define USB_PRODUCT_ID_UIC_CRDRDR3				0x240C
#define USB_VENDOR_ID_XST						0x2724
#define USB_PRODUCT_ID_XST_CRDRDR1				0x6001

namespace SentinelNativeUnitTests
{
	TEST_CLASS(HidContextUnitTests)
	{
		static const LPCWSTR TESTDIR;

	public:
		
		void TestDeviceNotFound(CHIDContext& context)
		{
			Assert::IsTrue(INVALID_HANDLE_VALUE == context.GetDeviceHandle(), L"Device Handle not cleared after failing to find device.");
			Assert::IsTrue(INVALID_HANDLE_VALUE == context.GetReadHandle(), L"Read Handle not cleared after failing to find device.");
			Assert::IsTrue(0 == context.GetFeatureReportByteLength(), L"Feature Report Length not cleared after failing to find device.");
			Assert::IsTrue(0 == context.GetInputReportByteLength(), L"Input Report Length not cleared after failing to find device.");
			Assert::IsFalse(context.IsReady(), L"Reported as 'ready' after failing to find device.");
		}

		void TestDeviceFound(CHIDContext& context)
		{
			Assert::IsFalse(INVALID_HANDLE_VALUE == context.GetDeviceHandle(), L"Device Handle invalid after finding device.");
			Assert::IsFalse(INVALID_HANDLE_VALUE == context.GetReadHandle(), L"Read Handle invalid after finding device.");
			Assert::IsFalse(0 == context.GetFeatureReportByteLength(), L"Feature Report Length is 0 after finding device.");
			Assert::IsFalse(0 == context.GetInputReportByteLength(), L"Input Report Length is 0 after finding device.");
			Assert::IsTrue(context.IsReady(), L"Not reported as 'ready' after finding device.");
		}

		TEST_CLASS_CLEANUP(HIDContextClassCleanup)
		{
			CCardReaderSimulator::CleanupInstance();
		}

		TEST_METHOD_INITIALIZE(HIDContextInitializeTest)
		{
			CString testDir;
			CUnitTestingUtils::GetTestDir(TESTDIR, testDir);
			CCardReaderSimulator::Instance().Start(testDir);
		}

		TEST_METHOD_CLEANUP(HIDContextCleanupTest)
		{
			CCardReaderSimulator::Instance().End();
		}

		TEST_METHOD(HIDContextDefault)
		{
			CHIDContext context;
			TestDeviceNotFound(context);
		}

		TEST_METHOD(HIDContextFindDeviceInvalidProductTest)
		{
			CHIDContext context;
			bool found = context.FindDevice(USB_VENDOR_ID_UIC, USB_INVALID_PRODUCT_ID);
			Assert::IsFalse(found, L"Invalid product reported as found");
			TestDeviceNotFound(context);
		}

		TEST_METHOD(HIDContextFindDeviceInvalidVendorTest)
		{
			CHIDContext context;
			bool found = context.FindDevice(USB_INVALID_VENDOR_ID, USB_PRODUCT_ID_UIC_CRDRDR1);
			Assert::IsFalse(found, L"Invalid vendor reported as found");
			TestDeviceNotFound(context);
		}

		TEST_METHOD(HIDContextFindDeviceNotConnectedTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, false);
			Assert::IsFalse(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate card reader as not connected.");

			CHIDContext context;
			bool found = context.FindDevice(USB_VENDOR_ID_UIC, USB_PRODUCT_ID_UIC_CRDRDR1);
			Assert::IsFalse(found, L"Unconnected device reported as found");
			TestDeviceNotFound(context);
		}

		TEST_METHOD(HIDContextFindDeviceUICConnectedTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate card reader as connected.");

			CHIDContext context;
			bool found = context.FindDevice(USB_VENDOR_ID_UIC, USB_PRODUCT_ID_UIC_CRDRDR1);
			Assert::IsTrue(found, L"Connected device not found");
			TestDeviceFound(context);
		}

		TEST_METHOD(HIDContextFindDeviceXSTConnectedTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_XST, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_XST), L"Unable to simulate card reader as connected.");

			CHIDContext context;
			bool found = context.FindDevice(USB_VENDOR_ID_XST, USB_PRODUCT_ID_XST_CRDRDR1);
			Assert::IsTrue(found, L"Connected device not found");
			TestDeviceFound(context);
		}

		TEST_METHOD(HIDContextCleanup)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_UIC, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_UIC), L"Unable to simulate card reader as connected.");

			CHIDContext context;
			bool found = context.FindDevice(USB_VENDOR_ID_UIC, USB_PRODUCT_ID_UIC_CRDRDR1);
			Assert::IsTrue(found, L"Connected device not found");
			TestDeviceFound(context);

			context.Cleanup();
			TestDeviceNotFound(context);
		}

		TEST_METHOD(HIDContextFindDeviceLargeDevicePathConnectedTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_TEST, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_TEST), L"Unable to simulate card reader as connected.");

			CHIDContext context;
			bool found = context.FindDevice(USB_VENDOR_ID_TEST, USB_PRODUCT_ID_LARGEDEVICEPATH);
			Assert::IsTrue(found, L"Connected device not found");
			TestDeviceFound(context);
		}

		TEST_METHOD(HIDContextFindDeviceBadFeatureAndInputLengthsPathConnectedTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_TEST, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_TEST), L"Unable to simulate card reader as connected.");

			CHIDContext context;
			bool found = context.FindDevice(USB_VENDOR_ID_TEST, USB_PRODUCT_ID_BADLENGTHS);
			Assert::IsFalse(found, L"Device with bad feature and input lengths reported as found");
			TestDeviceNotFound(context);
		}

		TEST_METHOD(HIDContextFindDeviceBadReadHandlePathConnectedTest)
		{
			CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
			simulator.SetConnected(USB_VENDOR_ID_TEST, true);
			Assert::IsTrue(simulator.GetConnected(USB_VENDOR_ID_TEST), L"Unable to simulate card reader as connected.");

			CHIDContext context;
			bool found = context.FindDevice(USB_VENDOR_ID_TEST, USB_PRODUCT_ID_BADREADHANDLE);
			Assert::IsFalse(found, L"Device with bad read handle reported as found");
			TestDeviceNotFound(context);
		}

	};

	const LPCWSTR HidContextUnitTests::TESTDIR = L"\\Test_HIDContext";
}
#endif //TODO-PORT