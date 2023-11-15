#include "stdafx.h"
#include "../Sentinel/IPAddress.h"

// Unit testing includes.
#include "UnitTestingUtils.h"
#include "gtest/gtest.h"

using namespace std;

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a helper method.
#define AssertResult5h(result, resultSize, expected, expectedSize, resultName) \
	CUnitTestingUtils::AssertEqual(result, resultSize, expected, expectedSize, resultName, testIndex, functionName, lineNumber, extraIndex);
#define AssertResult3h(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, functionName, lineNumber, extraIndex);
#define AssertResult2h(result, expected) AssertResult3h(result, expected, _T(#result))

// AssertResult macro for calling CUnitTestingUtils (with an extra index) from a test method.
#define AssertResult3x(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__, &extraIndex);
#define AssertResult2x(result, expected) AssertResult3x(result, expected, _T(#result))

class IPAddressUnitTest : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

};
	
static const size_t ADDRESS_SIZE_EXPECTED = 4;
const BYTE m_addressBytesExpected[4] = { 0, 0, 0, 0 };
// Default expected result(s).
//static const BYTE CIPAddress::m_addressBytesExpected[ADDRESS_SIZE_EXPECTED] = { 0, 0, 0, 0 };
static const long m_portExpected = 0;
static const CIPAddress m_defaultIPAddress;

// Calculate expected string results at runtime, as they can be determined from the expected raw results.
void AssertEqual(const CIPAddress &obj,
    const BYTE addressBytesExpected[ADDRESS_SIZE_EXPECTED],
    long portExpected,
    int testIndex,
    LPCTSTR functionName,
    int lineNumber,
    const int* extraIndex = NULL)
{
    string varAddressStringExpected;
    varAddressStringExpected = FormatString(_T("%u.%u.%u.%u"),
        addressBytesExpected[0],
        addressBytesExpected[1],
        addressBytesExpected[2],
        addressBytesExpected[3]);
    const string& addressStringExpected(varAddressStringExpected);

    ASSERT_TRUE(IP_ADDRESS_SIZE == ADDRESS_SIZE_EXPECTED)<< L"IP_ADDRESS_SIZE";

    // *** VERIFY CIPAddress::GetPort();
    AssertResult2h((DWORD)obj.GetPort(), (DWORD)portExpected);

    // *** VERIFY CIPAddress::GetIPAddressPrefix();
    AssertResult2h(obj.GetIPAddressPrefix(), addressBytesExpected[0]);

    // *** VERIFY CIPAddress::GetIP(BYTE* lpAddress, int length);
    BYTE addressBytesInitial[ADDRESS_SIZE_EXPECTED + 1];
    addressBytesInitial[0] = (BYTE)(0xff ^ addressBytesExpected[0]);
    addressBytesInitial[1] = (BYTE)(0xff ^ addressBytesExpected[1]);
    addressBytesInitial[2] = (BYTE)(0xff ^ addressBytesExpected[2]);
    addressBytesInitial[3] = (BYTE)(0xff ^ addressBytesExpected[3]);
    addressBytesInitial[4] = 0xbb;

    // Verify that GetIP(BYTE*) writes zero bytes if called with a length that is too small.
    BYTE addressBytes[_countof(addressBytesInitial)];
    memcpy_s(addressBytes, sizeof(addressBytes), addressBytesInitial, sizeof(addressBytesInitial));

    for (int length(0); length <= ADDRESS_SIZE_EXPECTED - 1; ++length)
    {
        obj.GetIP(addressBytes, length);
        string resultName;
        resultName = FormatString(_T("obj.GetIP(BYTE*, length=%d)"), (int)length);
        AssertResult5h(addressBytes, sizeof(addressBytes), addressBytesInitial, sizeof(addressBytes), resultName.c_str());
    }

    // Verify that GetIP(BYTE*) writes exactly ADDRESS_SIZE_EXPECTED bytes if called with a length that greater than or larger than ADDRESS_SIZE_EXPECTED.
    memcpy_s(addressBytesInitial, sizeof(addressBytesInitial), addressBytesExpected, ADDRESS_SIZE_EXPECTED);
    for (int length(ADDRESS_SIZE_EXPECTED); length <= sizeof(addressBytes); ++length)
    {
        memcpy_s(addressBytes, sizeof(addressBytes), addressBytesInitial, sizeof(addressBytesInitial));
        obj.GetIP(addressBytes, length);
        string resultName;
        resultName = FormatString(_T("obj.GetIP(BYTE*, length=%d)"), (int)length);
        AssertResult5h(addressBytes, sizeof(addressBytes), addressBytesInitial, sizeof(addressBytes), resultName.c_str());
    }

    // *** VERIFY CIPAddress::GetIP(CString &strAddress);
    string addressString("xyz");
    obj.GetIP(addressString);
    AssertResult2h(addressString, addressStringExpected);

    // *** VERIFY CIPAddress::GetIP(char* lpAddress, size_t length);
    const int addressBufferSize = addressStringExpected.length() + 1 + 2; // Add some extra length for testing.
    char* addressBufferExpected(new char[addressBufferSize]);
    char* addressBuffer(new char[addressBufferSize]);

    // Verify that the addressBuffer is set to an empty string when the length of the buffer is too small.
    // Never mind.  The sprintf_s() behavior is different for Debug builds and release builds when the length is too small.

    // Update addressBufferExpected to match expected result.
    for (int index(0); index < addressStringExpected.length(); ++index)
    {
        addressBufferExpected[index] = (char)addressStringExpected[index];
    }
    addressBufferExpected[addressStringExpected.length()] = (char)0;

    // Verify that the addressBuffer is set to the correct string when the length of the buffer is large enough.
    // Don't verify the whole buffer. Anything past the trailing NUL character can be undefined behavior.
    for (int length(addressStringExpected.length() + 1); length <= addressBufferSize; ++length)
    {
        memset(addressBuffer, 0xbb, addressBufferSize);
        obj.GetIP(addressBuffer, length);
        string resultName;
        resultName = FormatString(_T("obj.GetIP(char*, length=%d)"), (int)length);
        AssertResult5h(addressBuffer, addressStringExpected.length() + 1, addressBufferExpected, addressStringExpected.length() + 1, resultName.c_str());
    }

    // Cleanup.
    delete[] addressBufferExpected;
    delete[] addressBuffer;
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddress::CIPAddress()
///     CIPAddress::GetPort()
///     CIPAddress::GetIPAddressPrefix()
///     CIPAddress::GetIP(BYTE* lpAddress, int length)
///     CIPAddress::GetIP(CString &strAddress)
///     CIPAddress::GetIP(char* lpAddress, size_t length)
/// Code coverage for:
///      CIPAddress::~CIPAddress()
/// </summary>
TEST_F(IPAddressUnitTest, CIPAddress_DefaultTest)
{
    // Setup.
    const int testIndex(0);

    // Perform operation(s) to be tested.
    const CIPAddress obj;

    // Test(s).
    AssertEqual(obj,
        m_addressBytesExpected,
        m_portExpected,
        testIndex, __TFUNCTION__, __LINE__);
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddress::SetIP(const BYTE*)
///     CIPAddress::SetPort()
///     CIPAddress::CIPAddress(CIPAddress const &RHS)
///     CIPAddress::GetIP(BYTE*, int)
///     CIPAddress::GetPort()
/// </summary>
TEST_F(IPAddressUnitTest, CopyConstructorTest)
{
    typedef struct
    {
        // Input(s).
        const BYTE m_addressBytes[4];
        int m_port;

        // Expected result(s).
        const BYTE m_addressBytesExpected[4];
        int m_portExpected;
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { { 1, 2, 3, 4 }, -1, { 1, 2, 3, 4 }, -1 },
        { { 255, 255, 255, 255 }, 0, { 255, 255, 255, 255 }, 0 },
        { { 0, 0, 0, 0 }, 1, { 0, 0, 0, 0 }, 1 },
        { { 0, 0, 0, 0 }, 0, { 0, 0, 0, 0 }, 0 },
        { { 11, 22, 33, 44 }, 2, { 11, 22, 33, 44 }, 2 },
        { { 123, 234, 56, 78 }, 255, { 123, 234, 56, 78 }, 255 },
        { { 21, 22, 23, 24 }, 256, { 21, 22, 23, 24 }, 256 },
        { { 31, 32, 33, 34 }, 257, { 31, 32, 33, 34 }, 257 },
        { { 41, 42, 43, 44 }, 1023, { 41, 42, 43, 44 }, 1023 },
        { { 51, 52, 53, 54 }, 1024, { 51, 52, 53, 54 }, 1024 },
        { { 61, 62, 63, 64 }, 1025, { 61, 62, 63, 64 }, 1025 },
        { { 71, 72, 73, 74 }, 65535, { 71, 72, 73, 74 }, 65535 },
        { { 211, 212, 213, 214 }, 65536, { 211, 212, 213, 214 }, 65536 },
        { { 221, 222, 223, 224 }, 65537, { 221, 222, 223, 224 }, 65537 },
        { { 231, 232, 233, 234 }, INT_MAX, { 231, 232, 233, 234 }, INT_MAX },
        { { 241, 242, 243, 244 }, INT_MIN, { 241, 242, 243, 244 }, INT_MIN },
        { { 251, 252, 253, 254 }, (int)DWORD_MAX, { 251, 252, 253, 254 }, (int)DWORD_MAX },
        { { 255, 255, 255, 255 }, (int)0xffffffff, { 255, 255, 255, 255 }, (int)0xffffffff },
    };

    // Setup. Perform each test with the same instance.
    CIPAddress objA;

    for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Perform each test with a new object.
        CIPAddress objB;

        objA.SetIP(TEST_DATA[testIndex].m_addressBytes);
        objB.SetIP(TEST_DATA[testIndex].m_addressBytes);
        objA.SetPort(TEST_DATA[testIndex].m_port);
        objB.SetPort(TEST_DATA[testIndex].m_port);

        // Perform operation(s) to be tested.
        const CIPAddress copyA(objA);
        const CIPAddress copyB(objB);

        // Test(s).
        AssertEqual(copyA,
            TEST_DATA[testIndex].m_addressBytesExpected,
            TEST_DATA[testIndex].m_portExpected,
            testIndex, __TFUNCTION__, __LINE__);
        AssertEqual(copyB,
            TEST_DATA[testIndex].m_addressBytesExpected,
            TEST_DATA[testIndex].m_portExpected,
            testIndex, __TFUNCTION__, __LINE__);
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddress::SetPort()
///     CIPAddress::GetPort()
/// </summary>
TEST_F(IPAddressUnitTest, SetPortTest)
{
    typedef struct
    {
        // Input(s).
        int m_port;

        // Expected result(s).
        int m_portExpected;
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { -1, -1 },
        { 0, 0 },
        { 1, 1 },
        { 2, 2 },
        { 255, 255 },
        { 256, 256 },
        { 257, 257 },
        { 1023, 1023 },
        { 1024, 1024 },
        { 1025, 1025 },
        { 65535, 65535 },
        { 65536, 65536 },
        { 65537, 65537 },
        { INT_MAX, INT_MAX },
        { INT_MIN, INT_MIN },
        { (int)DWORD_MAX, (int)DWORD_MAX },
    };

    // Setup. Perform each test with the same instance.
    CIPAddress objA;

    for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Perform each test with a new object.
        CIPAddress objB;

        // Perform operation(s) to be tested.
        objA.SetPort(TEST_DATA[testIndex].m_port);
        objB.SetPort(TEST_DATA[testIndex].m_port);

        // Test(s).
        AssertEqual(objA,
            m_addressBytesExpected,
            TEST_DATA[testIndex].m_portExpected,
            testIndex, __TFUNCTION__, __LINE__);
        AssertEqual(objB,
            m_addressBytesExpected,
            TEST_DATA[testIndex].m_portExpected,
            testIndex, __TFUNCTION__, __LINE__);
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddress::SetIP(const BYTE*)
///     CIPAddress::GetIP(BYTE*, int)
/// </summary>
TEST_F(IPAddressUnitTest, SetIPByteArrayTest)
{

    typedef struct
    {
        // Input(s).
        const BYTE m_addressBytes[4];

        // Expected result(s).
        const BYTE m_addressBytesExpected[4];
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { { 1, 2, 3, 4 }, { 1, 2, 3, 4 } },
        { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
        { { 255, 255, 255, 255 }, { 255, 255, 255, 255 } },
        { { 11, 22, 33, 44 }, { 11, 22, 33, 44 } },
        { { 123, 234, 56, 78 }, { 123, 234, 56, 78 } },
    };

    // Setup. Perform each test with the same instance.
    CIPAddress objA;

    for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Perform each test with a new object.
        CIPAddress objB;

        // Perform operation(s) to be tested.
        objA.SetIP(TEST_DATA[testIndex].m_addressBytes);
        objB.SetIP(TEST_DATA[testIndex].m_addressBytes);

        // Test(s).
        AssertEqual(objA,
            TEST_DATA[testIndex].m_addressBytesExpected,
            m_portExpected,
            testIndex, __TFUNCTION__, __LINE__);
        AssertEqual(objB,
            TEST_DATA[testIndex].m_addressBytesExpected,
            m_portExpected,
            testIndex, __TFUNCTION__, __LINE__);
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddress::SetIP((LPCTSTR)NULL0)
///     CIPAddress::GetIP(BYTE*, int)
/// Test setting the IP address using a NULL LPCTSTR.
/// </summary>
TEST_F(IPAddressUnitTest, SetIPString_NullTest)
{
    typedef BYTE TIPAddressBytes[4];

    // Setup.
    const int testIndex(0);

    const string addressString1234("1.2.3.4");
    TIPAddressBytes addressBytes1234 = { 1, 2, 3, 4 };

    const string addressStringNull("");
    TIPAddressBytes addressBytes0000 = { 0, 0, 0, 0 };

    // Set the instance to a value different than the expected result.
    CIPAddress obj;
    obj.SetIP(addressBytes1234);

    // Verify setup.
    AssertEqual(obj, addressBytes1234, m_portExpected,
        testIndex, __TFUNCTION__, __LINE__);

    // Test(s).
    obj.SetIP(addressStringNull);
    AssertEqual(obj, addressBytes0000, m_portExpected,
        testIndex, __TFUNCTION__, __LINE__);
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddress::SetIP(LPCTSTR)
///     CIPAddress::GetIP(BYTE*, int)
/// </summary>
TEST_F(IPAddressUnitTest, SetIPStringTest)
{
    typedef BYTE TIPAddressBytes [4];

    typedef struct
    {
        // Input(s).
        LPCTSTR m_addressString;

        // Expected result(s).
        const TIPAddressBytes m_addressBytesExpected;
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { _T("1.2.3.4"), { 1, 2, 3, 4 } },
        { _T("0.0.0.0"), { 0, 0, 0, 0 } },
        { _T("255.255.255.255"), { 255, 255, 255, 255 } },
        { _T("11.22.33.44"), { 11, 22, 33, 44 } },
        { _T("123.234.56.78"), { 123, 234, 56, 78 } },

        { _T("256.2.3.4"), { 0, 0, 0, 0 } },
        { _T("1.256.3.4"), { 0, 0, 0, 0 } },
        { _T("1.2.256.4"), { 0, 0, 0, 0 } },
        { _T("1.2.3.256"), { 0, 0, 0, 0 } },

        { _T("123456789012345678901234567890.2.3.4"), { 0, 0, 0, 0 } },
        { _T("1.123456789012345678901234567890.3.4"), { 0, 0, 0, 0 } },
        { _T("1.2.123456789012345678901234567890.4"), { 0, 0, 0, 0 } },
        { _T("1.2.256.123456789012345678901234567890"), { 0, 0, 0, 0 } },

        { _T(" 123.234.56.78"), { 123, 234, 56, 78 } },
        { _T("1.2.34"), { 0, 0, 0, 0 } },
        { _T(" 00123. 00234. 0056. 0078"), { 123, 234, 56, 78 } }, // Parse as decimal base 10, not octal.
        { _T(" 00123. 00234. 0056. 0099"), { 123, 234, 56, 99 } }, // Parse as valid decimal base 10, not invalid octal.
        { _T(" 123. 234.56  78"), { 0, 0, 0, 0 } },
        { _T(" 123. 234. 56. 78"), { 123, 234, 56, 78 } },
        { _T(" 123 . 234. 56. 78"), { 0, 0, 0, 0 } },
        { _T("1.2.3.4.5"), { 1, 2, 3, 4 } },
        { _T(" 123. 234. 56. 78.90"), { 123, 234, 56, 78 } },
        { _T("0x1.0x2.0x3.0x4"), { 0, 0, 0, 0 } }, // No hexadecimal support.

        { _T("-4294967173.234.56.78"), { 123, 234, 56, 78 } }, // Negative value in range, 123 - (UINT_MAX + 1).
        { _T("-123. 234. 56. 78.90"), { 0, 0, 0, 0 } }, // Negative value out of range.
    };

    // Setup. Perform each test with the same instance.
    CIPAddress objA;

    for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Perform each test with a new object.
        CIPAddress objB;

        // Perform operation(s) to be tested.
        objA.SetIP(TEST_DATA[testIndex].m_addressString);
        objB.SetIP(TEST_DATA[testIndex].m_addressString);

        // Test(s).
        AssertEqual(objA,
            TEST_DATA[testIndex].m_addressBytesExpected,
            m_portExpected,
            testIndex, __TFUNCTION__, __LINE__);
        AssertEqual(objB,
            TEST_DATA[testIndex].m_addressBytesExpected,
            m_portExpected,
            testIndex, __TFUNCTION__, __LINE__);
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddress::SetIP(const BYTE*)
///     CIPAddress::SetPort()
///     CIPAddress::operator=(CIPAddress const &RHS)
///     CIPAddress::GetIP(BYTE*, int)
///     CIPAddress::GetPort()
/// </summary>
TEST_F(IPAddressUnitTest, AssignmentOperatorTest)
{
    typedef struct
    {
        // Input(s).
        const BYTE m_addressBytes[4];
        int m_port;

        // Expected result(s).
        const BYTE m_addressBytesExpected[4];
        int m_portExpected;
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { { 1, 2, 3, 4 }, -1, { 1, 2, 3, 4 }, -1 },
        { { 255, 255, 255, 255 }, 0, { 255, 255, 255, 255 }, 0 },
        { { 0, 0, 0, 0 }, 1, { 0, 0, 0, 0 }, 1 },
        { { 0, 0, 0, 0 }, 0, { 0, 0, 0, 0 }, 0 },
        { { 11, 22, 33, 44 }, 2, { 11, 22, 33, 44 }, 2 },
        { { 123, 234, 56, 78 }, 255, { 123, 234, 56, 78 }, 255 },
        { { 21, 22, 23, 24 }, 256, { 21, 22, 23, 24 }, 256 },
        { { 31, 32, 33, 34 }, 257, { 31, 32, 33, 34 }, 257 },
        { { 41, 42, 43, 44 }, 1023, { 41, 42, 43, 44 }, 1023 },
        { { 51, 52, 53, 54 }, 1024, { 51, 52, 53, 54 }, 1024 },
        { { 61, 62, 63, 64 }, 1025, { 61, 62, 63, 64 }, 1025 },
        { { 71, 72, 73, 74 }, 65535, { 71, 72, 73, 74 }, 65535 },
        { { 211, 212, 213, 214 }, 65536, { 211, 212, 213, 214 }, 65536 },
        { { 221, 222, 223, 224 }, 65537, { 221, 222, 223, 224 }, 65537 },
        { { 231, 232, 233, 234 }, INT_MAX, { 231, 232, 233, 234 }, INT_MAX },
        { { 241, 242, 243, 244 }, INT_MIN, { 241, 242, 243, 244 }, INT_MIN },
        { { 251, 252, 253, 254 }, (int)DWORD_MAX, { 251, 252, 253, 254 }, (int)DWORD_MAX },
        { { 255, 255, 255, 255 }, (int)0xffffffff, { 255, 255, 255, 255 }, (int)0xffffffff },
    };

    // Setup. Perform each test with the same instance.
    CIPAddress copyA;

    for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Perform each test with a new object.
        CIPAddress copyB;

        CIPAddress objA;
        CIPAddress objB;
        objA.SetIP(TEST_DATA[testIndex].m_addressBytes);
        objB.SetIP(TEST_DATA[testIndex].m_addressBytes);
        objA.SetPort(TEST_DATA[testIndex].m_port);
        objB.SetPort(TEST_DATA[testIndex].m_port);

        // Perform operation(s) to be tested.
        copyA = objA;
        copyB = objB;

        // Test(s).
        AssertEqual(copyA,
            TEST_DATA[testIndex].m_addressBytesExpected,
            TEST_DATA[testIndex].m_portExpected,
            testIndex, __TFUNCTION__, __LINE__);
        AssertEqual(copyB,
            TEST_DATA[testIndex].m_addressBytesExpected,
            TEST_DATA[testIndex].m_portExpected,
            testIndex, __TFUNCTION__, __LINE__);
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddress::SetIP(const BYTE*)
///     CIPAddress::SetPort()
///     CIPAddress::operator==(CIPAddress const &RHS)
///     CIPAddress::operator!=(CIPAddress const &RHS)
/// Test both equality and inquality operators.
/// </summary>
TEST_F(IPAddressUnitTest, EqualityOperatorTest)
{
    typedef struct
    {
        // Input(s).
        const BYTE m_addressBytesA[4];
        int m_portA;
        const BYTE m_addressBytesB[4];
        int m_portB;

        // Expected result(s).
        const bool m_resultExpected;
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { { 1, 2, 3, 4 }, -1, { 1, 2, 3, 4 }, -1, true },
        { { 1, 2, 3, 4 }, -1, { 2, 2, 3, 4 }, -1, false },
        { { 1, 2, 3, 4 }, -1, { 1, 3, 3, 4 }, -1, false },
        { { 1, 2, 3, 4 }, -1, { 1, 2, 4, 4 }, -1, false },
        { { 1, 2, 3, 4 }, -1, { 1, 2, 3, 5 }, -1, false },
        { { 1, 2, 3, 4 }, -1, { 1, 2, 3, 4 }, -2, false },

        { { 255, 255, 255, 255 }, 0, { 255, 255, 255, 255 }, 0, true },
        { { 254, 255, 255, 255 }, 0, { 255, 255, 255, 255 }, 0, false },
        { { 255, 254, 255, 255 }, 0, { 255, 255, 255, 255 }, 0, false },
        { { 255, 255, 254, 255 }, 0, { 255, 255, 255, 255 }, 0, false },
        { { 255, 255, 255, 254 }, 0, { 255, 255, 255, 255 }, 0, false },
        { { 255, 255, 255, 255 }, 65536, { 255, 255, 255, 255 }, 0, false },

        { { 0, 0, 0, 0 }, 1, { 0, 0, 0, 0 }, 1, true },
        { { 0, 0, 0, 0 }, 0, { 0, 0, 0, 0 }, 0, true },
        { { 11, 22, 33, 44 }, 2, { 11, 22, 33, 44 }, 2, true },
        { { 123, 234, 56, 78 }, 255, { 123, 234, 56, 78 }, 255, true },
        { { 21, 22, 23, 24 }, 256, { 21, 22, 23, 24 }, 256, true },
        { { 31, 32, 33, 34 }, 257, { 31, 32, 33, 34 }, 257, true },
        { { 41, 42, 43, 44 }, 1023, { 41, 42, 43, 44 }, 1023, true },
        { { 51, 52, 53, 54 }, 1024, { 51, 52, 53, 54 }, 1024, true },
        { { 61, 62, 63, 64 }, 1025, { 61, 62, 63, 64 }, 1025, true },
        { { 71, 72, 73, 74 }, 65535, { 71, 72, 73, 74 }, 65535, true },
        { { 211, 212, 213, 214 }, 65536, { 211, 212, 213, 214 }, 65536, true },
        { { 221, 222, 223, 224 }, 65537, { 221, 222, 223, 224 }, 65537, true },
        { { 231, 232, 233, 234 }, INT_MAX, { 231, 232, 233, 234 }, INT_MAX, true },
        { { 241, 242, 243, 244 }, INT_MIN, { 241, 242, 243, 244 }, INT_MIN, true },
        { { 251, 252, 253, 254 }, (int)DWORD_MAX, { 251, 252, 253, 254 }, (int)DWORD_MAX, true },
        { { 255, 255, 255, 255 }, (int)0xffffffff, { 255, 255, 255, 255 }, (int)0xffffffff, true },
    };

    // Setup. Perform each test with the same instance.
    CIPAddress varObjA;

    for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Perform each test with a new object.
        CIPAddress varObjB;

        // Set objects in different orders.
        varObjA.SetIP(TEST_DATA[testIndex].m_addressBytesA);
        varObjA.SetPort(TEST_DATA[testIndex].m_portA);
        varObjB.SetPort(TEST_DATA[testIndex].m_portB);
        varObjB.SetIP(TEST_DATA[testIndex].m_addressBytesB);

        const CIPAddress& objA(varObjA);
        const CIPAddress& objB(varObjB);

        // Perform test(s).

        // Test #1.
        {
            bool result = objA == objB;
            if (result != TEST_DATA[testIndex].m_resultExpected)
            {
                string failedMsg;
                failedMsg = FormatString(_T("objA == objB: result=%d expected=%d testIndex=%d in %s(%d)"),
                    (int)result,
                    (int)TEST_DATA[testIndex].m_resultExpected,
                    (int)testIndex,
                    static_cast<LPCTSTR>(__TFUNCTION__),
                    (int)__LINE__);
                ASSERT_TRUE(false)<< failedMsg;
            }
        }

        // Test #2.
        {
            bool result = objB == objA;
            if (result != TEST_DATA[testIndex].m_resultExpected)
            {
                string failedMsg;
                failedMsg = FormatString(_T("objB == objA: result=%d expected=%d testIndex=%d in %s(%d)"),
                    (int)result,
                    (int)TEST_DATA[testIndex].m_resultExpected,
                    (int)testIndex,
                    static_cast<LPCTSTR>(__TFUNCTION__),
                    (int)__LINE__);
                ASSERT_TRUE(false)<< failedMsg;
            }
        }

        // Test #3.
        {
            bool result = objA != objB;
            if (result != !TEST_DATA[testIndex].m_resultExpected)
            {
                string failedMsg;
                failedMsg = FormatString(_T("objA != objB: result=%d expected=%d testIndex=%d in %s(%d)"),
                    (int)result,
                    (int)!TEST_DATA[testIndex].m_resultExpected,
                    (int)testIndex,
                    static_cast<LPCTSTR>(__TFUNCTION__),
                    (int)__LINE__);
                ASSERT_TRUE(false)<< failedMsg;
            }
        }

        // Test #4.
        {
            bool result = objB != objA;
            if (result != !TEST_DATA[testIndex].m_resultExpected)
            {
                string failedMsg;
                failedMsg = FormatString(_T("objB != objA: result=%d expected=%d testIndex=%d in %s(%d)"),
                    (int)result,
                    (int)!TEST_DATA[testIndex].m_resultExpected,
                    (int)testIndex,
                    static_cast<LPCTSTR>(__TFUNCTION__),
                    (int)__LINE__);
                ASSERT_TRUE(false)<< failedMsg;
            }
        }
    }
}