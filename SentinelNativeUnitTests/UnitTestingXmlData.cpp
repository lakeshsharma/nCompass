#if 000//TODO-PORT
#include "stdafx.h"
#include "UnitTestingXmlData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

const CStringW CUnitTestingXmlData::TestData_BeginRootElement(L"<?xml version='1.0' encoding='UTF-16' ?><root>");
const CStringW CUnitTestingXmlData::TestData_EndRootElement(L"</root>");

const CStringW CUnitTestingXmlData::TestData_DataElementName(L"data");
const LPCWSTR CUnitTestingXmlData::TestData_ContentName(L"content");
const LPCWSTR CUnitTestingXmlData::TestData_ValueName(L"value");
const LPCWSTR CUnitTestingXmlData::TestData_ValidName(L"valid");

CStringW CUnitTestingXmlData::GetBoolTestData(const LPCWSTR &parameterName)
{
	const CStringW name(parameterName); // Use a CStringW for appending.

	// Test data:
	// "content" is the content to be read from the configuration file.  Use XML numeric character references ("&#10;") for newlines.
	// "value" is the configuration value in the content, if the content contains a correctly formatted configuration value.
	// "valid" indicates if the content is expected to have a correctly formatted configuration value.
	//     "valid" may be true even if the value is not in the valid range for the configuration field.
	const CStringW testData =
		TestData_BeginRootElement +

		// Content that isn't even close.
		DataElement(L"content='' value='false' valid='false'") +
		DataElement(L"content='ab' value='false' valid='false'") +
		DataElement(L"content='ab=cd' value='false' valid='false'") +
		DataElement(L"content='" + name + L"=cd' value='false' valid='false'") +
		DataElement(L"content='" + name + L"=' value='false' valid='false'") +
		DataElement(L"content='" + name + L"= ' value='false' valid='false'") +
		DataElement(L"content='" + name + L"=   ' value='false' valid='false'") +

		// Content that is close or equivalent to "" + name + L"=false\n".
		DataElement(L"content='" + name + L"=0' value='false' valid='false'") +
		DataElement(L"content='" + name + L"=F' value='false' valid='false'") +
		DataElement(L"content='" + name + L"=False' value='false' valid='false'") +
		DataElement(L"content='" + name + L"=fals' value='false' valid='false'") +
		DataElement(L"content='" + name + L"=falsE' value='false' valid='false'") +
		DataElement(L"content='" + name + L"=falsex' value='false' valid='false'") +
		DataElement(L"content='" + name + L"=false' value='false' valid='true'") +
		DataElement(L"content=' " + name + L" = false ' value='false' valid='true'") +
		DataElement(L"content='   " + name + L"   =   false   ' value='false' valid='true'") +
		DataElement(L"content='" + name + L"=false&#10;' value='false' valid='true'") +
		DataElement(L"content='" + name + L"=false&#10;" + name + L"=false&#10;' value='false' valid='true'") +

		// Content that is close or equivalent to "" + name + L"=false\n".
		DataElement(L"content='" + name + L"=1' value='true' valid='false'") +
		DataElement(L"content='" + name + L"=T' value='true' valid='false'") +
		DataElement(L"content='" + name + L"=True' value='true' valid='false'") +
		DataElement(L"content='" + name + L"=tru' value='true' valid='false'") +
		DataElement(L"content='" + name + L"=truE' value='true' valid='false'") +
		DataElement(L"content='" + name + L"=truex' value='true' valid='false'") +
		DataElement(L"content='" + name + L"=true' value='true' valid='true'") +
		DataElement(L"content=' " + name + L" = true ' value='true' valid='true'") +
		DataElement(L"content='   " + name + L"   =   true   ' value='true' valid='true'") +
		DataElement(L"content='" + name + L"=true&#10;' value='true' valid='true'") +
		DataElement(L"content='" + name + L"=true&#10;" + name + L"=true&#10;' value='true' valid='true'") +

		// Content that sets " + name + L" to one value and then maybe to another.
		DataElement(L"content='" + name + L"=false&#10;" + name + L"=true&#10;' value='true' valid='true'") +
		DataElement(L"content='" + name + L"=true&#10;" + name + L"=false&#10;' value='false' valid='true'") +
		DataElement(L"content='" + name + L"=false&#10;" + name + L"=tru 0&#10;' value='false' valid='true'") +
		DataElement(L"content='" + name + L"=true&#10;" + name + L"=fals&#10;' value='true' valid='true'") +

		// Content that sets " + name + L" to false and then test values that are close or equivalent to "true".
		DataElement(L"content='" + name + L"=false&#10;" + name + L"=1' value='false' valid='true'") +
		DataElement(L"content='" + name + L"=false&#10;" + name + L"=T' value='false' valid='true'") +
		DataElement(L"content='" + name + L"=false&#10;" + name + L"=True' value='false' valid='true'") +
		DataElement(L"content='" + name + L"=false&#10;" + name + L"=tru' value='false' valid='true'") +
		DataElement(L"content='" + name + L"=false&#10;" + name + L"=truE' value='false' valid='true'") +
		DataElement(L"content='" + name + L"=false&#10;" + name + L"=truex' value='false' valid='true'") +
		DataElement(L"content='" + name + L"=false&#10;" + name + L"=true' value='true' valid='true'") +
		DataElement(L"content='" + name + L"=false&#10; " + name + L" = true ' value='true' valid='true'") +
		DataElement(L"content='" + name + L"=false&#10;   " + name + L"   =   true   ' value='true' valid='true'") +
		DataElement(L"content='" + name + L"=false&#10;" + name + L"=true&#10;' value='true' valid='true'") +

		// Content that sets " + name + L" to true and then test values that are close or equivalent to "false".
		DataElement(L"content='" + name + L"=true&#10;" + name + L"=0' value='true' valid='true'") +
		DataElement(L"content='" + name + L"=true&#10;" + name + L"=F' value='true' valid='true'") +
		DataElement(L"content='" + name + L"=true&#10;" + name + L"=False' value='true' valid='true'") +
		DataElement(L"content='" + name + L"=true&#10;" + name + L"=fals' value='true' valid='true'") +
		DataElement(L"content='" + name + L"=true&#10;" + name + L"=falsE' value='true' valid='true'") +
		DataElement(L"content='" + name + L"=true&#10;" + name + L"=falsex' value='true' valid='true'") +
		DataElement(L"content='" + name + L"=true&#10;" + name + L"=false' value='false' valid='true'") +
		DataElement(L"content='" + name + L"=true&#10; " + name + L" = false ' value='false' valid='true'") +
		DataElement(L"content='" + name + L"=true&#10;   " + name + L"   =   false   ' value='false' valid='true'") +
		DataElement(L"content='" + name + L"=true&#10;" + name + L"=false&#10;' value='false' valid='true'") +

		TestData_EndRootElement;

	return testData;
}

CStringW CUnitTestingXmlData::GetFilePathTestData(const LPCWSTR &parameterName)
{
	const CStringW name(parameterName); // Use a CStringW for appending.

	// Test data:
	// "content" is the content to be read from the configuration file.  Use XML numeric character references ("&#10;") for newlines.
	// "value" is the configuration value in the content, if the content contains a correctly formatted configuration value.
	// "valid" indicates if the content is expected to have a correctly formatted configuration value.
	//     "valid" may be true even if the value is not in the valid range for the configuration field.
	const CStringW testData =
		TestData_BeginRootElement +

		// Content that isn't even close.
		DataElement(L"content='' value='' valid='false'") +
		DataElement(L"content='ab' value='' valid='false'") +
		DataElement(L"content='ab=cd' value='' valid='false'") +

		// Content that is close enough.
		DataElement(L"content='" + name + L"=cd' value='cd' valid='true'") +
		DataElement(L"content='" + name + L"=' value='' valid='true'") +
		DataElement(L"content='" + name + L"= ' value='' valid='true'") +
		DataElement(L"content='" + name + L"=   ' value='' valid='true'") +

		DataElement(L"content='" + name + L"=c' value='c' valid='true'") +
		DataElement(L"content='" + name + L"= . ' value='.' valid='true'") +
		DataElement(L"content='" + name + L"=  /  ' value='/' valid='true'") +
		DataElement(L"content='" + name + L"= \\ ' value='\\' valid='true'") +
		DataElement(L"content='" + name + L"= * ' value='*' valid='true'") +
		DataElement(L"content='" + name + L"= ? ' value='?' valid='true'") +
		DataElement(L"content='" + name + L"= : ' value=':' valid='true'") +

		DataElement(L"content='" + name + L"=C:\\' value='C:\\' valid='true'") +
		DataElement(L"content=' " + name + L" = ../Default ' value='../Default' valid='true'") +
		DataElement(L"content='   " + name + L"   =   X:/Path Name/Sub Directory.dir   ' value='X:/Path Name/Sub Directory.dir' valid='true'") +

		DataElement(L"content='   " + name + L"=10.138.96.95/ErrorDump.html'"
		L" value='10.138.96.95/ErrorDump.html' valid='true'") +
		DataElement(L"content='   " + name + L"=http://10.138.96.95/ErrorDump.html'"
		L" value='http://10.138.96.95/ErrorDump.html' valid='true'") +
		DataElement(L"content='   " + name + L"=http://123456789.123456789.123456789.123456789/ErrorDump.html'"
		L" value='http://123456789.123456789.123456789.123456789/ErrorDump.html' valid='true'") +
		DataElement(L"content='   " + name + L"=https://www.example.edu/sample test content for testing sample test content'"
		L" value='https://www.example.edu/sample test content for testing sample test content' valid='true'") +

		DataElement(L"content='" + name + L"=abc 123&#10;' value='abc 123' valid='true'") +
		DataElement(L"content='" + name + L"= def wxyz 789&#10;" + name + L"= def wxyz 789&#10;' value='def wxyz 789' valid='true'") +

		// All displayable ASCII characters, with imbedded quote and apostrophe.
		DataElement(L"content='" + name + L"= !&quot;#$%&amp;&apos;()*+,-./0123456789:;&lt;=&gt;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~'"
		L" value='!&quot;#$%&amp;&apos;()*+,-./0123456789:;&lt;=&gt;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~' valid='true'") +

		// All displayable ASCII characters, with trailing double quote.
		DataElement(L"content='" + name + L"= !#$%&amp;&apos;()*+,-./0123456789:;&lt;=&gt;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~&quot;'"
		L" value='!#$%&amp;&apos;()*+,-./0123456789:;&lt;=&gt;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~&quot;' valid='true'") +

		// All displayable ASCII characters, with trailing apostrophe.
		DataElement(L"content='" + name + L"= !&quot;#$%&amp;()*+,-./0123456789:;&lt;=&gt;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~&apos;'"
		L" value='!&quot;#$%&amp;()*+,-./0123456789:;&lt;=&gt;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~&apos;' valid='true'") +

		// Test with value length == MAXPATH-1, MAXPATH, and MAXPATH+1 (where MAXPATH=260)  Add one for the trailing NUL character.
		DataElement(L"content='" + name + L"="
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789.123456789' value='"
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789.123456789' valid='true'") +
		DataElement(L"content='" + name + L"="
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789.123456789.' value='"
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789.123456789.' valid='true'") +
		DataElement(L"content='" + name + L"="
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789.123456789.1' value='"
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789.123456789.1' valid='true'") +

		// Test with a value length of 300.
		DataElement(L"content='" + name + L"="
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789.' value='"
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789."
		L"123456789.123456789.123456789.123456789.123456789.' valid='true'") +

		// Content that sets " + name + L" to one value and then maybe to another.
		DataElement(L"content='" + name + L"=efg&#10;" + name + L"=a&#10;' value='a' valid='true'") +
		DataElement(L"content='" + name + L"=fghi&#10;" + name + L"=abcdef&#10;' value='abcdef' valid='true'") +
		DataElement(L"content='" + name + L"=ghijk&#10;" + name + L"=&#10;' value='' valid='true'") +
		DataElement(L"content='" + name + L"=hijklm&#10;" + name + L"x=abc&#10;' value='hijklm' valid='true'") +
		DataElement(L"content='" + name + L"=ijklmno&#10;FeaturesMediaPat=abc&#10;' value='ijklmno' valid='true'") +

		TestData_EndRootElement;

	return testData;
}

CStringW CUnitTestingXmlData::GetBoolData()
{
	// Each "value" attribute contains a bool value.
	// Provide a sequence of values where each value is preceed by both the same value and a different value.
	// - This is intended to facilitate testing setter functions that return the previous value.
	CStringW testData =
		TestData_BeginRootElement +

		DataElement(L"value='false'") +
		DataElement(L"value='true'") +
		DataElement(L"value='true'") +
		DataElement(L"value='false'") +
		DataElement(L"value='false'") +

		TestData_EndRootElement;

	return testData;
}

CStringW CUnitTestingXmlData::GetData(bool ignoredA, bool ignoredB)
{
	return GetBoolData();
}

CStringW CUnitTestingXmlData::GetByteData(BYTE extraThresholdA, BYTE extraThresholdB)
{
	// Each "value" attribute contains a WORD value.
	CStringW testData = TestData_BeginRootElement;

	if (extraThresholdA != 0)
	{
		// Add extra threshold-1, threshold+0, threshold+1 values.
		for (int index = -1; index <= 1; ++index)
		{
			CStringW attributeString;
			attributeString.Format(L"value='%u'", (unsigned)(BYTE)(extraThresholdA + index));
			testData += DataElement(attributeString);
		}
	}

	if (extraThresholdB != 0)
	{
		// Add extra threshold-1, threshold+0, threshold+1 values.
		for (int index = -1; index <= 1; ++index)
		{
			CStringW attributeString;
			attributeString.Format(L"value='%u'", (unsigned)(BYTE)(extraThresholdB + index));
			testData += DataElement(attributeString);
		}
	}

	testData +=
		// Smaller values.
		DataElement(L"value='0'") +
		DataElement(L"value='1'") +
		DataElement(L"value='123'") +

		// Signed data thresholds.
		DataElement(L"value='126'") + // SCHAR_MAX - 1.
		DataElement(L"value='127'") + // SCHAR_MAX.
		DataElement(L"value='128'") + // SCHAR_MIN
		DataElement(L"value='129'") + // SCHAR_MIN + 1

		// Larger values.
		DataElement(L"value='210'") +
		DataElement(L"value='254'") + // UCHAR_MAX - 1.
		DataElement(L"value='255'") + // UCHAR_MAX.

		TestData_EndRootElement;

	return testData;
}

CStringW CUnitTestingXmlData::GetData(BYTE extraThresholdA, BYTE extraThresholdB)
{
	return GetByteData(extraThresholdA, extraThresholdB);
}

CStringW CUnitTestingXmlData::GetShortData(short extraThresholdA, short extraThresholdB)
{
	// Each "value" attribute contains a short value.
	CStringW testData = TestData_BeginRootElement;

	if (extraThresholdA != 0)
	{
		// Add extra threshold-1, threshold+0, threshold+1 values.
		for (int index = -1; index <= 1; ++index)
		{
			CStringW attributeString;
			attributeString.Format(L"value='%u'", (int)(extraThresholdA + index));
			testData += DataElement(attributeString);
		}
	}

	if (extraThresholdB != 0)
	{
		// Add extra threshold-1, threshold+0, threshold+1 values.
		for (int index = -1; index <= 1; ++index)
		{
			CStringW attributeString;
			attributeString.Format(L"value='%u'", (int)(extraThresholdB + index));
			testData += DataElement(attributeString);
		}
	}

	testData +=
		// Smaller non-negative values.
		DataElement(L"value='0'") +
		DataElement(L"value='1'") +
		DataElement(L"value='12345'") +

		// Signed data thresholds.
		DataElement(L"value='32766'") + // SHRT_MAX - 1.
		DataElement(L"value='32767'") + // SHRT_MAX.
		DataElement(L"value='-32768'") + // SHRT_MIN
		DataElement(L"value='-32767'") + // SHRT_MIN + 1

		// Smaller negative values.
		DataElement(L"value='-12345'") +
		DataElement(L"value='-2'") +
		DataElement(L"value='-1'") +

		TestData_EndRootElement;

	return testData;
}

CStringW CUnitTestingXmlData::GetData(short extraThresholdA, short extraThresholdB)
{
	return GetShortData(extraThresholdA, extraThresholdB);
}

CStringW CUnitTestingXmlData::GetWordData(WORD extraThresholdA, WORD extraThresholdB)
{
	// Each "value" attribute contains a WORD value.
	CStringW testData = TestData_BeginRootElement;

	if (extraThresholdA != 0)
	{
		// Add extra threshold-1, threshold+0, threshold+1 values.
		for (int index = -1; index <= 1; ++index)
		{
			CStringW attributeString;
			attributeString.Format(L"value='%u'", (unsigned)(WORD)(extraThresholdA + index));
			testData += DataElement(attributeString);
		}
	}

	if (extraThresholdB != 0)
	{
		// Add extra threshold-1, threshold+0, threshold+1 values.
		for (int index = -1; index <= 1; ++index)
		{
			CStringW attributeString;
			attributeString.Format(L"value='%u'", (unsigned)(WORD)(extraThresholdB + index));
			testData += DataElement(attributeString);
		}
	}

	testData +=
		// Smaller values.
		DataElement(L"value='0'") +
		DataElement(L"value='1'") +
		DataElement(L"value='12345'") +

		// Signed data thresholds.
		DataElement(L"value='32766'") + // SHRT_MAX - 1.
		DataElement(L"value='32767'") + // SHRT_MAX.
		DataElement(L"value='32768'") + // SHRT_MIN
		DataElement(L"value='32769'") + // SHRT_MIN + 1

		// Larger values.
		DataElement(L"value='54321'") +
		DataElement(L"value='65534'") + // WORD_MAX - 1.
		DataElement(L"value='65535'") + // WORD_MAX.

		TestData_EndRootElement;

	return testData;
}

CStringW CUnitTestingXmlData::GetData(WORD extraThresholdA, WORD extraThresholdB)
{
	return GetWordData(extraThresholdA, extraThresholdB);
}

CStringW CUnitTestingXmlData::GetIntData(int extraThresholdA, int extraThresholdB)
{
	// Test data:
	// Each "value" attribute contains an int value.
	CStringW testData = TestData_BeginRootElement;

	if (extraThresholdA != 0)
	{
		// Add extra threshold-1, threshold+0, threshold+1 values.
		for (int index = -1; index <= 1; ++index)
		{
			CStringW attributeString;
			attributeString.Format(L"value='%d'", (int)(extraThresholdA + index));
			testData += DataElement(attributeString);
		}
	}

	if (extraThresholdB != 0)
	{
		// Add extra threshold-1, threshold+0, threshold+1 values.
		for (int index = -1; index <= 1; ++index)
		{
			CStringW attributeString;
			attributeString.Format(L"value='%d'", (int)(extraThresholdB + index));
			testData += DataElement(attributeString);
		}
	}

	testData +=
		// Smaller non-negative values.
		DataElement(L"value='0'") +
		DataElement(L"value='1'") +
		DataElement(L"value='2'") +
		DataElement(L"value='1234567890'") +

		// Signed data thresholds.
		DataElement(L"value='2147483645'") + // INT_MAX - 2.
		DataElement(L"value='2147483646'") + // INT_MAX - 1.
		DataElement(L"value='2147483647'") + // INT_MAX.
		DataElement(L"value='-2147483648'") + // INT_MIN
		DataElement(L"value='-2147483647'") + // INT_MIN + 1
		DataElement(L"value='-2147483648'") + // INT_MIN + 2

		// Smaller negative values.
		DataElement(L"value='-1234567890'") +
		DataElement(L"value='-3'") + // DWORD_MAX - 2.
		DataElement(L"value='-2'") + // DWORD_MAX - 1.
		DataElement(L"value='-1'") + // DWORD_MAX.

		TestData_EndRootElement;

	return testData;
}

CStringW CUnitTestingXmlData::GetData(int extraThresholdA, int extraThresholdB)
{
	return GetIntData(extraThresholdA, extraThresholdB);
}

CStringW CUnitTestingXmlData::GetDwordData(DWORD extraThresholdA, DWORD extraThresholdB)
{
	// Test data:
	// Each "value" attribute contains a DWORD value.
	CStringW testData = TestData_BeginRootElement;

	if (extraThresholdA != 0)
	{
		// Add extra threshold values.
		for (int index = -EXTRA_THRESHOLD_VALUES; index <= EXTRA_THRESHOLD_VALUES; ++index)
		{
			CStringW attributeString;
			attributeString.Format(L"value='%u'", (unsigned)(extraThresholdA + index));
			testData += DataElement(attributeString);
		}
	}

	if (extraThresholdB != 0)
	{
		// Add extra threshold values.
		for (int index = -EXTRA_THRESHOLD_VALUES; index <= EXTRA_THRESHOLD_VALUES; ++index)
		{
			CStringW attributeString;
			attributeString.Format(L"value='%u'", (unsigned)(extraThresholdB + index));
			testData += DataElement(attributeString);
		}
	}

	testData +=
		// Smaller values.
		DataElement(L"value='0'") +
		DataElement(L"value='1'") +
		DataElement(L"value='2'") +
		DataElement(L"value='1234567890'") +

		// Signed data thresholds.
		DataElement(L"value='2147483645'") + // INT_MAX - 2.
		DataElement(L"value='2147483646'") + // INT_MAX - 1.
		DataElement(L"value='2147483647'") + // INT_MAX.
		DataElement(L"value='2147483648'") + // INT_MIN
		DataElement(L"value='2147483649'") + // INT_MIN + 1
		DataElement(L"value='2147483650'") + // INT_MIN + 2

		// Larger values.
		DataElement(L"value='3456789012'") +
		DataElement(L"value='4294967293'") + // DWORD_MAX - 2.
		DataElement(L"value='4294967294'") + // DWORD_MAX - 1.
		DataElement(L"value='4294967295'") + // DWORD_MAX.

		TestData_EndRootElement;

	return testData;
}

CStringW CUnitTestingXmlData::GetData(DWORD extraThresholdA, DWORD extraThresholdB)
{
	return GetDwordData(extraThresholdA, extraThresholdB);
}

CStringW CUnitTestingXmlData::GetIntData(__int64 extraThresholdA, __int64 extraThresholdB)
{
	// Test data:
	// Each "value" attribute contains an int value.
	CStringW testData = TestData_BeginRootElement;

	if (extraThresholdA != 0)
	{
		// Add extra threshold-1, threshold+0, threshold+1 values.
		for (int index = -1; index <= 1; ++index)
		{
			CStringW attributeString;
			attributeString.Format(L"value='%lld'", (long long)(extraThresholdA + index));
			testData += DataElement(attributeString);
		}
	}

	if (extraThresholdB != 0)
	{
		// Add extra threshold-1, threshold+0, threshold+1 values.
		for (int index = -1; index <= 1; ++index)
		{
			CStringW attributeString;
			attributeString.Format(L"value='%lld'", (long long)(extraThresholdB + index));
			testData += DataElement(attributeString);
		}
	}

	testData +=
		// Smaller non-negative values.
		DataElement(L"value='0'") +
		DataElement(L"value='1'") +
		DataElement(L"value='1234567890'") +

		// Very large values - especially for range limited formats.
		DataElement(L"value='9999999999'") +
		DataElement(L"value='99999999999'") +
		DataElement(L"value='999999999999'") +
		DataElement(L"value='9999999999999'") +
		DataElement(L"value='99999999999999'") +
		DataElement(L"value='999999999999999'") +
		DataElement(L"value='9999999999999999'") +
		DataElement(L"value='99999999999999999'") +
		DataElement(L"value='999999999999999999'") +
		DataElement(L"value='2345678901234567890'") +

		// Signed data thresholds.
		DataElement(L"value='9223372036854775806'") + // INT_MAX - 1.
		DataElement(L"value='9223372036854775807'") + // _I64_MAX.
		DataElement(L"value='-9223372036854775808'") + // _I64_MIN.
		DataElement(L"value='-9223372036854775807'") + // _I64_MIN + 1.

		// Very small (negative) values - especially for range limited formats.
		DataElement(L"value='-2345678901234567890'") +
		DataElement(L"value='-999999999999999999'") +
		DataElement(L"value='-99999999999999999'") +
		DataElement(L"value='-9999999999999999'") +
		DataElement(L"value='-999999999999999'") +
		DataElement(L"value='-99999999999999'") +
		DataElement(L"value='-9999999999999'") +
		DataElement(L"value='-999999999999'") +
		DataElement(L"value='-99999999999'") +
		DataElement(L"value='-9999999999'") +

		// Small negative values.
		DataElement(L"value='-1234567890'") +
		DataElement(L"value='-2'") + // _UI64_MAX - 1.
		DataElement(L"value='-1'") + // _UI64_MAX.

		TestData_EndRootElement;

	return testData;
}

CStringW CUnitTestingXmlData::GetData(__int64 extraThresholdA, __int64 extraThresholdB)
{
	return GetIntData(extraThresholdA, extraThresholdB);
}

CStringW CUnitTestingXmlData::GetStringData()
{
	// Test data:
	// Each "value" attribute contains a string value.
	const CStringW testData =
		TestData_BeginRootElement +

		// Leading and/or trailing whitespace or lack thereof.
		DataElement(L"value=''") +
		DataElement(L"value=' '") +
		DataElement(L"value='  '") +
		DataElement(L"value=' ab '") +

		DataElement(L"value='abc '") +
		DataElement(L"value='abc&#9; '") +
		DataElement(L"value='abc &#9;'") +
		DataElement(L"value='abc &#9;'") +
		DataElement(L"value='abc&#10; '") +
		DataElement(L"value='abc &#10;'") +

		DataElement(L"value='abc def'") +
		DataElement(L"value='abc&#9; def'") +
		DataElement(L"value='abc &#9;def'") +
		DataElement(L"value='abc&#10; def'") +
		DataElement(L"value='abc &#10;def'") +

		DataElement(L"value=' def'") +
		DataElement(L"value='&#9; def'") +
		DataElement(L"value=' &#9;def'") +
		DataElement(L"value='&#10; def'") +
		DataElement(L"value=' &#10;def'") +

		// All displayable ASCII characters, with imbedded quote and apostrophe.
		DataElement(L"value='!&quot;#$%&amp;&apos;()*+,-./0123456789:;&lt;=&gt;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~'") +

		// All displayable ASCII characters, with trailing double quote.
		DataElement(	L" value='!#$%&amp;&apos;()*+,-./0123456789:;&lt;=&gt;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~&quot;'") +

		// All displayable ASCII characters, with trailing apostrophe.
		DataElement(L" value='!&quot;#$%&amp;()*+,-./0123456789:;&lt;=&gt;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~&apos;'") +

		// 300 characters.
		DataElement(L"value='"
			L"123456789.123456789.123456789.123456789.123456789." L"123456789.123456789.123456789.123456789.123456789."
			L"123456789.123456789.123456789.123456789.123456789." L"123456789.123456789.123456789.123456789.123456789."
			L"123456789.123456789.123456789.123456789.123456789." L"123456789.123456789.123456789.123456789.123456789."
			L"'") +

		TestData_EndRootElement;

	return testData;
}

/// <summary>
/// Gets an XML data element for the attributesString.
/// </summary>
/// <param name="attributesString">IN - The attributes string.</param>
/// <returns>An XML data element for the attributesString.</returns>
CStringW CUnitTestingXmlData::DataElement(LPCWSTR attributesString)
{
	CStringW dataElement(CStringW(L"<") + TestData_DataElementName + L" " + attributesString + L"/>");
	return dataElement;
}
#endif //TODO-PORT