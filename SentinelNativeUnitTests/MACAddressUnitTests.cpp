#if 000//TODO-PORT

#include "stdafx.h"
#include "MACAddress.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(MACAddressUnitTests)
	{
	public:
		TEST_METHOD(MACAddressSetGetCStringUnitTest)
		{
			CMACAddress *mac = new CMACAddress();
			mac->SetMAC(L"01-23-45-67-89-ab");
			CString strMac;
			mac->GetMAC(strMac);
			Assert::IsTrue(strMac == L"01-23-45-67-89-AB");


			delete mac;
		}

		TEST_METHOD(MACAddressSetGetBytesUnitTest)
		{
			CMACAddress *mac = new CMACAddress();
			byte bytes[SIZE_OF_MAC_ADDRESS] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab};
			mac->SetMAC(bytes);
			CString strMac;
			byte resultBytes[SIZE_OF_MAC_ADDRESS];
			mac->GetMAC(resultBytes, SIZE_OF_MAC_ADDRESS);
			Assert::IsFalse(memcmp(bytes, resultBytes, SIZE_OF_MAC_ADDRESS));

			delete mac;
		}

	};
}

#endif //TODO-PORT