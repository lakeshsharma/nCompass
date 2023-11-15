#include "stdafx.h"
#include "../Sentinel/IPAddressArray.h"

// Unit testing includes.
#include "UnitTestingUtils.h"
#include "gtest/gtest.h"

using namespace std;

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a helper method.
#define AssertResult3h(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, functionName, lineNumber, extraIndex);
#define AssertResult2h(result, expected) AssertResult3h(result, expected, _T(#result))

// AssertResult macro for calling CUnitTestingUtils (with an extra index) from a test method.
#define AssertResult3x(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__, &extraIndex);
#define AssertResult2x(result, expected) AssertResult3x(result, expected, _T(#result))

// Default expected result(s).
// This test class is for CIPAddressArray, so just check for CIPAddess instance equality,
// without concern for the actutal CIPAddress instance values.
static const int m_countExpected = 0;
static const CIPAddress* m_ipAddressesExpected;
static const CIPAddress m_defaultIPAddress;

class IPAddressArray : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

};		

void AssertEqual(const CIPAddressArray &obj,
    int countExpected,
    const CIPAddress ipAddressesExpected[],
    int testIndex,
    LPCTSTR functionName,
    int lineNumber,
    const int* extraIndex = NULL)
{
    AssertResult2h((int)obj.GetCount(), countExpected);

    AssertResult2h(obj.GetAt(-1), m_defaultIPAddress);
    for (int index(0); index < countExpected; ++index)
    {
        string resultName;
        resultName = FormatString(_T("obj.GetAt(index=%d)"), (int)index);
        AssertResult3h(obj.GetAt(index), ipAddressesExpected[index], resultName.c_str());
    }
    AssertResult2h(obj.GetAt(obj.GetCount()), m_defaultIPAddress);
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddressArray::CIPAddressArray()
///     CIPAddressArray::GetCount()
///     CIPAddressArray::GetAt()
/// Code coverage for:
///      CIPAddressArray::~CIPAddressArray()
/// </summary>
TEST_F(IPAddressArray, CIPAddressArray_DefaultTest)
{
    // Setup.
    const int testIndex(0);

    // Perform operation(s) to be tested.
    const CIPAddressArray obj;

    // Test(s).
    AssertEqual(obj,
        m_countExpected,
        m_ipAddressesExpected,
        testIndex, __TFUNCTION__, __LINE__);
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddressArray::CIPAddressArray()
///     CIPAddressArray::CIPAddressArray(const CIPAddressArray &rhs)
///     CIPAddressArray::GetCount()
///     CIPAddressArray::GetAt()
///     CIPAddressArray::Add(const BYTE*, int);
/// </summary>
TEST_F(IPAddressArray, CopyConstructorTest)
{
    typedef BYTE TIPAddressBytes[4];
    typedef struct {
        TIPAddressBytes m_ipAddressBytes;
        int m_port;
    } TIPAddressBytesPort;

    typedef struct
    {
        // Input(s).
        const TIPAddressBytesPort* const m_ipAddressBytesPort;
        const int m_ipAddressCount;

        // Expected result(s).
        // Calculated at runtime.
    } TestDataType;

    TIPAddressBytesPort addr1[] = {
        { { 1, 2, 3, 4 }, 12345 },
    };

    TIPAddressBytesPort addr2[] = {
        { { 1, 2, 3, 4 }, INT_MIN },
        { { 0, 0, 0, 0 }, 0 },
        { { 255, 255, 255, 255 }, INT_MAX },
    };

    TIPAddressBytesPort addr3[] = {
        { { 11, 22, 33, 44 }, -1 },
        { { 22, 33, 44, 55 }, 1 },
        { { 123, 234, 56, 78 }, 0x12345678 },
        { { 123, 234, 56, 78 }, 0x12345678 },
        { { 123, 234, 56, 78 }, 0x12345678 },
        { { 255, 255, 255, 255 }, 1234567890 },
    };

    const TestDataType TEST_DATA[] =
    {
        { NULL, 0 },
        { addr1, _countof(addr1) },
        { addr2, _countof(addr2) },
        { addr3, _countof(addr3) },
    };

    // Setup. Perform each test with the same instance.
    CIPAddressArray objA;

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Calculate expected accumulated result.
        int countExpectedA(0);
        for (int indexA(0); indexA <= testIndex; ++indexA)
        {
            countExpectedA += TEST_DATA[indexA].m_ipAddressCount;
        }
        CIPAddress* const ipAddressesExpectedA(new CIPAddress[countExpectedA]);
        int indexA(0);
        for (int testIndexA(0); testIndexA <= testIndex; ++testIndexA)
        {
            for (int index(0); index < TEST_DATA[testIndexA].m_ipAddressCount; ++index, ++indexA)
            {
                ipAddressesExpectedA[indexA].SetIP(TEST_DATA[testIndexA].m_ipAddressBytesPort[index].m_ipAddressBytes);
                ipAddressesExpectedA[indexA].SetPort(TEST_DATA[testIndexA].m_ipAddressBytesPort[index].m_port);
            }
        }

        // Setup. Perform each test with a new object.
        const int countExpectedB(TEST_DATA[testIndex].m_ipAddressCount);
        CIPAddress* const ipAddressesExpectedB(new CIPAddress[countExpectedB]);
        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            ipAddressesExpectedB[index].SetIP(TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_ipAddressBytes);
            ipAddressesExpectedB[index].SetPort(TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_port);
        }

        CIPAddressArray objB;

        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            objA.Add(TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_ipAddressBytes,
                TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_port);
            objB.Add(TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_ipAddressBytes,
                TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_port);
        }

        // Perform operation(s) to be tested.
        CIPAddressArray copyA(objA);
        CIPAddressArray copyB(objB);


        // Test(s).
        AssertEqual(copyA,
            countExpectedA,
            ipAddressesExpectedA,
            testIndex, __TFUNCTION__, __LINE__);

        AssertEqual(copyB,
            countExpectedB,
            ipAddressesExpectedB,
            testIndex, __TFUNCTION__, __LINE__);

        // Cleanup.
        delete[] ipAddressesExpectedA;
        delete[] ipAddressesExpectedB;
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddressArray::CIPAddressArray()
///     CIPAddressArray::operator=(const CIPAddressArray &rhs)
///     CIPAddressArray::GetCount()
///     CIPAddressArray::GetAt()
///     CIPAddressArray::Add(const BYTE*, int);
/// </summary>
TEST_F(IPAddressArray, AssignmentOperatorTest)
{
    typedef BYTE TIPAddressBytes[4];
    typedef struct {
        TIPAddressBytes m_ipAddressBytes;
        int m_port;
    } TIPAddressBytesPort;

    typedef struct
    {
        // Input(s).
        const TIPAddressBytesPort* const m_ipAddressBytesPort;
        const int m_ipAddressCount;

        // Expected result(s).
        // Calculated at runtime.
    } TestDataType;

    TIPAddressBytesPort addr1[] = {
        { { 1, 2, 3, 4 }, 12345 },
    };

    TIPAddressBytesPort addr2[] = {
        { { 1, 2, 3, 4 }, INT_MIN },
        { { 0, 0, 0, 0 }, 0 },
        { { 255, 255, 255, 255 }, INT_MAX },
    };

    TIPAddressBytesPort addr3[] = {
        { { 11, 22, 33, 44 }, -1 },
        { { 22, 33, 44, 55 }, 1 },
        { { 123, 234, 56, 78 }, 0x12345678 },
        { { 123, 234, 56, 78 }, 0x12345678 },
        { { 123, 234, 56, 78 }, 0x12345678 },
        { { 255, 255, 255, 255 }, 1234567890 },
    };

    const TestDataType TEST_DATA[] =
    {
        { NULL, 0 },
        { addr1, _countof(addr1) },
        { addr2, _countof(addr2) },
        { addr3, _countof(addr3) },
    };

    // Setup. Perform each test with the same instance.
    CIPAddressArray objA;

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Calculate expected accumulated result.
        int countExpectedA(0);
        for (int indexA(0); indexA <= testIndex; ++indexA)
        {
            countExpectedA += TEST_DATA[indexA].m_ipAddressCount;
        }
        CIPAddress* const ipAddressesExpectedA(new CIPAddress[countExpectedA]);
        int indexA(0);
        for (int testIndexA(0); testIndexA <= testIndex; ++testIndexA)
        {
            for (int index(0); index < TEST_DATA[testIndexA].m_ipAddressCount; ++index, ++indexA)
            {
                ipAddressesExpectedA[indexA].SetIP(TEST_DATA[testIndexA].m_ipAddressBytesPort[index].m_ipAddressBytes);
                ipAddressesExpectedA[indexA].SetPort(TEST_DATA[testIndexA].m_ipAddressBytesPort[index].m_port);
            }
        }

        // Setup. Perform each test with a new object.
        const int countExpectedB(TEST_DATA[testIndex].m_ipAddressCount);
        CIPAddress* const ipAddressesExpectedB(new CIPAddress[countExpectedB]);
        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            ipAddressesExpectedB[index].SetIP(TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_ipAddressBytes);
            ipAddressesExpectedB[index].SetPort(TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_port);
        }

        CIPAddressArray objB;

        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            objA.Add(TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_ipAddressBytes,
                TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_port);
            objB.Add(TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_ipAddressBytes,
                TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_port);
        }

        // Perform operation(s) to be tested.
        CIPAddressArray copyA;
        AssertEqual(copyA, m_countExpected, m_ipAddressesExpected, testIndex, __TFUNCTION__, __LINE__); // Verify default construction.
        copyA = objA;
        CIPAddressArray copyB;
        AssertEqual(copyB, m_countExpected, m_ipAddressesExpected, testIndex, __TFUNCTION__, __LINE__); // Verify default construction.
        copyB = objB;

        // Test(s).
        AssertEqual(copyA,
            countExpectedA,
            ipAddressesExpectedA,
            testIndex, __TFUNCTION__, __LINE__);

        AssertEqual(copyB,
            countExpectedB,
            ipAddressesExpectedB,
            testIndex, __TFUNCTION__, __LINE__);

        // Perform more operation(s) to be tested.  Test that old information is overridden.
        copyB = objA;
        copyA = objB;

        // More test(s).
        AssertEqual(copyB,
            countExpectedA,
            ipAddressesExpectedA,
            testIndex, __TFUNCTION__, __LINE__);

        AssertEqual(copyA,
            countExpectedB,
            ipAddressesExpectedB,
            testIndex, __TFUNCTION__, __LINE__);

        // Cleanup.
        delete[] ipAddressesExpectedA;
        delete[] ipAddressesExpectedB;
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddressArray::CIPAddressArray()
///     CIPAddressArray::operator==()
///     CIPAddressArray::operator!=()
///     CIPAddressArray::Add(const BYTE*);
/// </summary>
TEST_F(IPAddressArray, EqualityOperatorTest)
{
    typedef BYTE TIPAddressBytes[4];
    typedef struct {
        TIPAddressBytes m_ipAddressBytes;
        int m_port;
    } TIPAddressBytesPort;

    typedef struct
    {
        // Input(s).
        const TIPAddressBytesPort* const m_ipAddressBytesPortA;
        const int m_ipAddressCountA;
        const TIPAddressBytesPort* const m_ipAddressBytesPortB;
        const int m_ipAddressCountB;

        // Expected result(s).
        // Expect equivalent results for a==b, b==a, a!=b, and b!=b.
        const bool m_resultExpected; // Result expected for equality tests.
    } TestDataType;

    // Different number of elements.
    TIPAddressBytesPort addr1a[] = {
        { { 1, 2, 3, 4 }, 12345 },
    };

    // Same.
    TIPAddressBytesPort addr2a[] = {
        { { 1, 2, 3, 4 }, 12345 },
    };
    TIPAddressBytesPort addr2b[] = {
        { { 1, 2, 3, 4 }, 12345 },
    };

    // Different address.
    TIPAddressBytesPort addr3a[] = {
        { { 1, 2, 3, 4 }, 12345 },
    };
    TIPAddressBytesPort addr3b[] = {
        { { 1, 2, 3, 5 }, 12345 },
    };

    // Different port.
    TIPAddressBytesPort addr4a[] = {
        { { 1, 2, 3, 4 }, 12345 },
    };
    TIPAddressBytesPort addr4b[] = {
        { { 1, 2, 3, 4 }, 12346 },
    };

    // A in B, but B not in A.
    TIPAddressBytesPort addr5a[] = {
        { { 0, 0, 0, 0 }, 0 },
        { { 0, 0, 0, 0 }, 0 },
    };
    TIPAddressBytesPort addr5b[] = {
        { { 0, 0, 0, 0 }, 0 },
        { { 1, 1, 1, 1 }, 1 },
    };

    // A in B and B in A, but different lengths.
    TIPAddressBytesPort addr6a[] = {
        { { 0, 0, 0, 0 }, 0 },
        { { 1, 1, 1, 1 }, 1 },
    };
    TIPAddressBytesPort addr6b[] = {
        { { 0, 0, 0, 0 }, 0 },
        { { 1, 1, 1, 1 }, 1 },
        { { 0, 0, 0, 0 }, 0 },
    };

    // A in B and B in A, but quantities of the same elements.
    TIPAddressBytesPort addr7a[] = {
        { { 0, 0, 0, 0 }, 0 },
        { { 0, 0, 0, 0 }, 0 },
        { { 1, 1, 1, 1 }, 1 },
    };
    TIPAddressBytesPort addr7b[] = {
        { { 0, 0, 0, 0 }, 0 },
        { { 1, 1, 1, 1 }, 1 },
        { { 1, 1, 1, 1 }, 1 },
    };

    // Same elements, but different order.
    TIPAddressBytesPort addr8a[] = {
        { { 11, 12, 13, 14 }, 1234 },
        { { 21, 22, 23, 24 }, 2345 },
        { { 31, 32, 33, 34 }, 3456 },
        { { 41, 42, 43, 44 }, 4567 },
    };
    TIPAddressBytesPort addr8b[] = {
        { { 31, 32, 33, 34 }, 3456 },
        { { 11, 12, 13, 14 }, 1234 },
        { { 41, 42, 43, 44 }, 4567 },
        { { 21, 22, 23, 24 }, 2345 },
    };

    const TestDataType TEST_DATA[] =
    {
        { NULL, 0, NULL, 0, true },
        { addr1a, _countof(addr1a), NULL, 0, false },
        { addr2a, _countof(addr2a), addr2b, _countof(addr2b), true },
        { addr3a, _countof(addr3a), addr3b, _countof(addr3b), false },
        { addr4a, _countof(addr4a), addr4b, _countof(addr4b), false },
        { addr5a, _countof(addr5a), addr5b, _countof(addr5b), false },
        { addr6a, _countof(addr6a), addr6b, _countof(addr6b), false },
        { addr7a, _countof(addr7a), addr7b, _countof(addr7b), true },
        { addr8a, _countof(addr8a), addr8b, _countof(addr8b), true },
    };

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Calculate expected accumulated result.
        CIPAddressArray objA;
        CIPAddressArray objB;

        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCountA; ++index)
        {
            objA.Add(TEST_DATA[testIndex].m_ipAddressBytesPortA[index].m_ipAddressBytes,
                TEST_DATA[testIndex].m_ipAddressBytesPortA[index].m_port);
        }

        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCountB; ++index)
        {
            objB.Add(TEST_DATA[testIndex].m_ipAddressBytesPortB[index].m_ipAddressBytes,
                TEST_DATA[testIndex].m_ipAddressBytesPortB[index].m_port);
        }

        // Perform operation(s) to be tested.

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

/// <summary>
/// Code coverage and functional test for:
///     CIPAddressArray::CIPAddressArray()
///     CIPAddressArray::GetCount()
///     CIPAddressArray::GetAt()
///     CIPAddressArray::Add(const BYTE*);
/// Code coverage for:
///      CIPAddressArray::~CIPAddressArray()
/// </summary>
TEST_F(IPAddressArray, AddByteArrayTest)
{
    typedef BYTE TIPAddressBytes [4];

    typedef struct
    {
        // Input(s).
        const TIPAddressBytes* const m_ipAddressBytes;
        const int m_ipAddressCount;

        // Expected result(s).
        // Calculated at runtime.
    } TestDataType;

    TIPAddressBytes addr1[] = {
        { 1, 2, 3, 4 },
    };

    TIPAddressBytes addr2[] = {
        { 1, 2, 3, 4 },
        { 0, 0, 0, 0 },
        { 255, 255, 255, 255 },
    };

    TIPAddressBytes addr3[] = {
        { 11, 22, 33, 44 },
        { 123, 234, 56, 78 },
        { 123, 234, 56, 78 },
        { 123, 234, 56, 78 },
        { 255, 255, 255, 255 },
    };

    const TestDataType TEST_DATA[] =
    {
        { NULL, 0 },
        { addr1, _countof(addr1) },
        { addr2, _countof(addr2) },
        { addr3, _countof(addr3) },
    };

    // Setup. Perform each test with the same instance.
    CIPAddressArray objA;

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Calculate expected accumulated result.
        int countExpectedA(0);
        for (int indexA(0); indexA <= testIndex; ++indexA)
        {
            countExpectedA += TEST_DATA[indexA].m_ipAddressCount;
        }
        CIPAddress* const ipAddressesExpectedA(new CIPAddress[countExpectedA]);
        int indexA(0);
        for (int testIndexA(0); testIndexA <= testIndex; ++testIndexA)
        {
            for (int index(0); index < TEST_DATA[testIndexA].m_ipAddressCount; ++index, ++indexA)
            {
                ipAddressesExpectedA[indexA].SetIP(TEST_DATA[testIndexA].m_ipAddressBytes[index]);
            }
        }

        // Setup. Perform each test with a new object.
        const int countExpectedB(TEST_DATA[testIndex].m_ipAddressCount);
        CIPAddress* const ipAddressesExpectedB(new CIPAddress[countExpectedB]);
        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            ipAddressesExpectedB[index].SetIP(TEST_DATA[testIndex].m_ipAddressBytes[index]);
        }

        CIPAddressArray objB;

        // Perform operation(s) to be tested.
        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            objA.Add(TEST_DATA[testIndex].m_ipAddressBytes[index]);
            objB.Add(TEST_DATA[testIndex].m_ipAddressBytes[index]);
        }

        // Test(s).
        AssertEqual(objA,
            countExpectedA,
            ipAddressesExpectedA,
            testIndex, __TFUNCTION__, __LINE__);

        AssertEqual(objB,
            countExpectedB,
            ipAddressesExpectedB,
            testIndex, __TFUNCTION__, __LINE__);

        // Cleanup.
        delete[] ipAddressesExpectedA;
        delete[] ipAddressesExpectedB;
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddressArray::CIPAddressArray()
///     CIPAddressArray::GetCount()
///     CIPAddressArray::GetAt()
///     CIPAddressArray::Add(const BYTE*, int);
/// </summary>
TEST_F(IPAddressArray, AddByteArrayPortTest)
{
    typedef BYTE TIPAddressBytes [4];
    typedef struct {
        TIPAddressBytes m_ipAddressBytes;
        int m_port;
    } TIPAddressBytesPort;

    typedef struct
    {
        // Input(s).
        const TIPAddressBytesPort* const m_ipAddressBytesPort;
        const int m_ipAddressCount;

        // Expected result(s).
        // Calculated at runtime.
    } TestDataType;

    TIPAddressBytesPort addr1[] = {
        { { 1, 2, 3, 4 }, 12345 },
    };

    TIPAddressBytesPort addr2[] = {
        { { 1, 2, 3, 4 }, INT_MIN },
        { { 0, 0, 0, 0 }, 0 },
        { { 255, 255, 255, 255 }, INT_MAX },
    };

    TIPAddressBytesPort addr3[] = {
        { { 11, 22, 33, 44 }, -1 },
        { { 22, 33, 44, 55 }, 1 },
        { { 123, 234, 56, 78 }, 0x12345678 },
        { { 123, 234, 56, 78 }, 0x12345678 },
        { { 123, 234, 56, 78 }, 0x12345678 }, 
        { { 255, 255, 255, 255 }, 1234567890 },
    };

    const TestDataType TEST_DATA[] =
    {
        { NULL, 0 },
        { addr1, _countof(addr1) },
        { addr2, _countof(addr2) },
        { addr3, _countof(addr3) },
    };

    // Setup. Perform each test with the same instance.
    CIPAddressArray objA;

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Calculate expected accumulated result.
        int countExpectedA(0);
        for (int indexA(0); indexA <= testIndex; ++indexA)
        {
            countExpectedA += TEST_DATA[indexA].m_ipAddressCount;
        }
        CIPAddress* const ipAddressesExpectedA(new CIPAddress[countExpectedA]);
        int indexA(0);
        for (int testIndexA(0); testIndexA <= testIndex; ++testIndexA)
        {
            for (int index(0); index < TEST_DATA[testIndexA].m_ipAddressCount; ++index, ++indexA)
            {
                ipAddressesExpectedA[indexA].SetIP(TEST_DATA[testIndexA].m_ipAddressBytesPort[index].m_ipAddressBytes);
                ipAddressesExpectedA[indexA].SetPort(TEST_DATA[testIndexA].m_ipAddressBytesPort[index].m_port);
            }
        }

        // Setup. Perform each test with a new object.
        const int countExpectedB(TEST_DATA[testIndex].m_ipAddressCount);
        CIPAddress* const ipAddressesExpectedB(new CIPAddress[countExpectedB]);
        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            ipAddressesExpectedB[index].SetIP(TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_ipAddressBytes);
            ipAddressesExpectedB[index].SetPort(TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_port);
        }

        CIPAddressArray objB;

        // Perform operation(s) to be tested.
        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            objA.Add(TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_ipAddressBytes,
                TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_port);
            objB.Add(TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_ipAddressBytes,
                TEST_DATA[testIndex].m_ipAddressBytesPort[index].m_port);
        }

        // Test(s).
        AssertEqual(objA,
            countExpectedA,
            ipAddressesExpectedA,
            testIndex, __TFUNCTION__, __LINE__);

        AssertEqual(objB,
            countExpectedB,
            ipAddressesExpectedB,
            testIndex, __TFUNCTION__, __LINE__);

        // Cleanup.
        delete[] ipAddressesExpectedA;
        delete[] ipAddressesExpectedB;
    }
}

/// <summary>
/// </summary>
/// <param name="address">IP address in the form of a 4 byte array</param>
/// <summary>
/// Code coverage and functional test for:
///     CIPAddressArray::CIPAddressArray()
///     CIPAddressArray::GetCount()
///     CIPAddressArray::GetAt()
///     CIPAddressArray::Add(LPCTSTR);
/// Code coverage for:
///      CIPAddressArray::~CIPAddressArray()
/// </summary>
TEST_F(IPAddressArray, AddStringTest)
{
    typedef struct
    {
        // Input(s).
        const LPCTSTR* const m_ipAddressStrings;
        const int m_ipAddressCount;

        // Expected result(s).
        // Calculated at runtime.
    } TestDataType;

    LPCTSTR addr1[] = {
        _T("1.2.3.4"),
    };

    LPCTSTR addr2[] = {
        _T("1.2.3.4"),
        _T("0.0.0.0"),
        _T("255.255.255.255"),
    };

    LPCTSTR addr3[] = {
        _T("11.22.33.44"),
        _T("123.234.56.78"),
        _T("123.234.56.78"),
        _T("123.234.56.78"),
        _T("255.255.255.255"),
    };

    const TestDataType TEST_DATA[] =
    {
        { NULL, 0 },
        { addr1, _countof(addr1) },
        { addr2, _countof(addr2) },
        { addr3, _countof(addr3) },
    };

    // Setup. Perform each test with the same instance.
    CIPAddressArray objA;

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Calculate expected accumulated result.
        int countExpectedA(0);
        for (int indexA(0); indexA <= testIndex; ++indexA)
        {
            countExpectedA += TEST_DATA[indexA].m_ipAddressCount;
        }
        CIPAddress* const ipAddressesExpectedA(new CIPAddress[countExpectedA]);
        int indexA(0);
        for (int testIndexA(0); testIndexA <= testIndex; ++testIndexA)
        {
            for (int index(0); index < TEST_DATA[testIndexA].m_ipAddressCount; ++index, ++indexA)
            {
                ipAddressesExpectedA[indexA].SetIP(TEST_DATA[testIndexA].m_ipAddressStrings[index]);
            }
        }

        // Setup. Perform each test with a new object.
        const int countExpectedB(TEST_DATA[testIndex].m_ipAddressCount);
        CIPAddress* const ipAddressesExpectedB(new CIPAddress[countExpectedB]);
        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            ipAddressesExpectedB[index].SetIP(TEST_DATA[testIndex].m_ipAddressStrings[index]);
        }

        CIPAddressArray objB;

        // Perform operation(s) to be tested.
        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            objA.Add(TEST_DATA[testIndex].m_ipAddressStrings[index]);
            objB.Add(TEST_DATA[testIndex].m_ipAddressStrings[index]);
        }

        // Test(s).
        AssertEqual(objA,
            countExpectedA,
            ipAddressesExpectedA,
            testIndex, __TFUNCTION__, __LINE__);

        AssertEqual(objB,
            countExpectedB,
            ipAddressesExpectedB,
            testIndex, __TFUNCTION__, __LINE__);

        // Cleanup.
        delete[] ipAddressesExpectedA;
        delete[] ipAddressesExpectedB;
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddressArray::CIPAddressArray()
///     CIPAddressArray::GetCount()
///     CIPAddressArray::GetAt()
///     CIPAddressArray::Add(LPCTSTR, int);
/// </summary>
TEST_F(IPAddressArray, AddStringPortTest)
{
    typedef struct {
        LPCTSTR const m_ipAddressString;
        int m_port;
    } TIPAddressStringPort;

    typedef struct
    {
        // Input(s).
        const TIPAddressStringPort* const m_ipAddressStringPort;
        const int m_ipAddressCount;

        // Expected result(s).
        // Calculated at runtime.
    } TestDataType;

    TIPAddressStringPort addr1[] = {
        { _T("1.2.3.4"), 12345 },
    };

    TIPAddressStringPort addr2[] = {
        { _T("1.2.3.4"), INT_MIN },
        { _T("0.0.0.0"), 0 },
        { _T("255.255.255.255"), INT_MAX },
    };

    TIPAddressStringPort addr3[] = {
        { _T("11.22.33.44"), -1 },
        { _T("22.33.44.55"), 1 },
        { _T("123.234.56.78"), 0x12345678 },
        { _T("123.234.56.78"), 0x12345678 },
        { _T("123.234.56.78"), 0x12345678 },
        { _T("255.255.255.255"), 1234567890 },
    };

    const TestDataType TEST_DATA[] =
    {
        { NULL, 0 },
        { addr1, _countof(addr1) },
        { addr2, _countof(addr2) },
        { addr3, _countof(addr3) },
    };

    // Setup. Perform each test with the same instance.
    CIPAddressArray objA;

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Calculate expected accumulated result.
        int countExpectedA(0);
        for (int indexA(0); indexA <= testIndex; ++indexA)
        {
            countExpectedA += TEST_DATA[indexA].m_ipAddressCount;
        }
        CIPAddress* const ipAddressesExpectedA(new CIPAddress[countExpectedA]);
        int indexA(0);
        for (int testIndexA(0); testIndexA <= testIndex; ++testIndexA)
        {
            for (int index(0); index < TEST_DATA[testIndexA].m_ipAddressCount; ++index, ++indexA)
            {
                ipAddressesExpectedA[indexA].SetIP(TEST_DATA[testIndexA].m_ipAddressStringPort[index].m_ipAddressString);
                ipAddressesExpectedA[indexA].SetPort(TEST_DATA[testIndexA].m_ipAddressStringPort[index].m_port);
            }
        }

        // Setup. Perform each test with a new object.
        const int countExpectedB(TEST_DATA[testIndex].m_ipAddressCount);
        CIPAddress* const ipAddressesExpectedB(new CIPAddress[countExpectedB]);
        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            ipAddressesExpectedB[index].SetIP(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString);
            ipAddressesExpectedB[index].SetPort(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
        }

        CIPAddressArray objB;

        // Perform operation(s) to be tested.
        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            objA.Add(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString,
                TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
            objB.Add(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString,
                TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
        }

        // Test(s).
        AssertEqual(objA,
            countExpectedA,
            ipAddressesExpectedA,
            testIndex, __TFUNCTION__, __LINE__);

        AssertEqual(objB,
            countExpectedB,
            ipAddressesExpectedB,
            testIndex, __TFUNCTION__, __LINE__);

        // Cleanup.
        delete[] ipAddressesExpectedA;
        delete[] ipAddressesExpectedB;
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddressArray::CIPAddressArray()
///     CIPAddressArray::GetCount()
///     CIPAddressArray::GetAt()
///     CIPAddressArray::Add(CIPAddress);
///     CIPAddressArray::Remove();
/// Remove all elements in a non-sequential order, verifying the array after each element removed.
/// </summary>
TEST_F(IPAddressArray, RemoveTest)
{
    typedef struct {
        LPCTSTR const m_ipAddressString;
        int m_port;
    } TIPAddressStringPort;

    typedef struct
    {
        // Input(s).
        const TIPAddressStringPort* const m_ipAddressStringPort;
        const int m_ipAddressCount;

        // Expected result(s).
        // Calculated at runtime.
    } TestDataType;

    TIPAddressStringPort addr1[] = {
        { _T("1.2.3.4"), 12345 },
    };

    TIPAddressStringPort addr2[] = {
        { _T("1.2.3.4"), INT_MIN },
        { _T("0.0.0.0"), 0 },
        { _T("255.255.255.255"), INT_MAX },
    };

    TIPAddressStringPort addr3[] = {
        { _T("11.22.33.44"), -1 },
        { _T("22.33.44.55"), 1 },
        { _T("123.234.56.78"), 0x12345678 },
        { _T("123.234.56.78"), 0x12345678 },
        { _T("123.234.56.78"), 0x12345678 },
        { _T("255.255.255.255"), 1234567890 },
    };

    const TestDataType TEST_DATA[] =
    {
        { NULL, 0 },
        { addr1, _countof(addr1) },
        { addr2, _countof(addr2) },
        { addr3, _countof(addr3) },
    };

    // Setup. Perform each test with the same instance.
    CIPAddressArray objA;

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Perform each test with a new object.
        int countExpectedB(TEST_DATA[testIndex].m_ipAddressCount);
        CIPAddress* const ipAddressesExpectedB(new CIPAddress[countExpectedB]);
        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            ipAddressesExpectedB[index].SetIP(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString);
            ipAddressesExpectedB[index].SetPort(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
        }

        CIPAddressArray objB;

        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            CIPAddress ipAddress;
            ipAddress.SetIP(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString);
            ipAddress.SetPort(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
            objA.Add(ipAddress);
            objB.Add(ipAddress);
        }

        // Verify setup.
        AssertEqual(objA,
            countExpectedB,
            ipAddressesExpectedB,
            testIndex, __TFUNCTION__, __LINE__);
        AssertEqual(objB,
            countExpectedB,
            ipAddressesExpectedB,
            testIndex, __TFUNCTION__, __LINE__);

        for (int extraIndex(1); extraIndex <= TEST_DATA[testIndex].m_ipAddressCount; ++extraIndex)
        {
            // Setup for next test.
            const int removalIndex((extraIndex * 3) % countExpectedB);
            for (int index(removalIndex); index < countExpectedB - 1; ++index)
            {
                ipAddressesExpectedB[index] = ipAddressesExpectedB[index + 1];
            }
            --countExpectedB;

            // Perform operation(s) to be tested.
            objA.RemoveAt(removalIndex);
            objB.RemoveAt(removalIndex);

            // Test(s).
            AssertEqual(objA,
                countExpectedB,
                ipAddressesExpectedB,
                testIndex, __TFUNCTION__, __LINE__, &extraIndex);
            AssertEqual(objB,
                countExpectedB,
                ipAddressesExpectedB,
                testIndex, __TFUNCTION__, __LINE__, &extraIndex);
        }

        // Cleanup.
        delete[] ipAddressesExpectedB;
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CIPAddressArray::CIPAddressArray()
///     CIPAddressArray::GetCount()
///     CIPAddressArray::GetAt()
///     CIPAddressArray::Add(CIPAddress);
///     CIPAddressArray::RemoveAll();
/// </summary>
TEST_F(IPAddressArray, RemoveAllTest)
{
    typedef struct {
        LPCTSTR const m_ipAddressString;
        int m_port;
    } TIPAddressStringPort;

    typedef struct
    {
        // Input(s).
        const TIPAddressStringPort* const m_ipAddressStringPort;
        const int m_ipAddressCount;

        // Expected result(s).
        // Calculated at runtime.
    } TestDataType;

    TIPAddressStringPort addr1[] = {
        { _T("1.2.3.4"), 12345 },
    };

    TIPAddressStringPort addr2[] = {
        { _T("1.2.3.4"), INT_MIN },
        { _T("0.0.0.0"), 0 },
        { _T("255.255.255.255"), INT_MAX },
    };

    TIPAddressStringPort addr3[] = {
        { _T("11.22.33.44"), -1 },
        { _T("22.33.44.55"), 1 },
        { _T("123.234.56.78"), 0x12345678 },
        { _T("123.234.56.78"), 0x12345678 },
        { _T("123.234.56.78"), 0x12345678 },
        { _T("255.255.255.255"), 1234567890 },
    };

    const TestDataType TEST_DATA[] =
    {
        { NULL, 0 },
        { addr1, _countof(addr1) },
        { addr2, _countof(addr2) },
        { addr3, _countof(addr3) },
    };

    // Setup. Perform each test with the same instance.
    CIPAddressArray objA;

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup. Perform each test with a new object.
        const int countExpectedB(TEST_DATA[testIndex].m_ipAddressCount);
        CIPAddress* const ipAddressesExpectedB(new CIPAddress[countExpectedB]);
        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            ipAddressesExpectedB[index].SetIP(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString);
            ipAddressesExpectedB[index].SetPort(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
        }

        CIPAddressArray objB;

        for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
        {
            CIPAddress varIPAddress;
            varIPAddress.SetIP(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString);
            varIPAddress.SetPort(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
            const CIPAddress& ipAddress(varIPAddress);
            objA.Add(ipAddress);
            objB.Add(ipAddress);
        }

        // Verify setup for objA.
        AssertEqual(objA,
            countExpectedB,
            ipAddressesExpectedB,
            testIndex, __TFUNCTION__, __LINE__);

        // Perform operation(s) to be tested for objA.
        objA.RemoveAll();

        // Test(s) for objA.
        AssertEqual(objA,
            m_countExpected,
            m_ipAddressesExpected,
            testIndex, __TFUNCTION__, __LINE__);

        // Verify setup for objB.
        AssertEqual(objB,
            countExpectedB,
            ipAddressesExpectedB,
            testIndex, __TFUNCTION__, __LINE__);

        // Perform operation(s) to be tested for objB.
        objB.RemoveAll();

        // Test(s) for objB.
        AssertEqual(objB,
            m_countExpected,
            m_ipAddressesExpected,
            testIndex, __TFUNCTION__, __LINE__);

        // Cleanup.
        delete[] ipAddressesExpectedB;
    }
}
