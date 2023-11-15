#pragma once
#if 000//TODO-PORT
/// <summary>
/// XML data helper functions for unit tests.
/// </summary>
class CUnitTestingXmlData
{
public:
	// Test data element and attribute name(s).
	static const CStringW TestData_BeginRootElement;
	static const CStringW TestData_DataElementName;
	static const LPCWSTR TestData_ContentName;
	static const LPCWSTR TestData_ValueName;
	static const LPCWSTR TestData_ValidName;
	static const CStringW TestData_EndRootElement;

	/// <summary>
	/// Gets test data for testing bool values.
	/// </summary>
	/// <param name="parameterName">IN - The name of the parameter.</param>
	/// <returns>Test data for the parameterName.</returns>
	static CStringW GetBoolTestData(const LPCWSTR &parameterName);

	/// <summary>
	/// Gets test data for testing file names, path names, web addresses, and other (displayable) string values.
	/// </summary>
	/// <param name="parameterName">IN - The name of the parameter.</param>
	/// <returns>Test data for the parameterName.</returns>
	static CStringW GetFilePathTestData(const LPCWSTR &parameterName);

	/// <summary>
	/// Gets XML-formatted test data containing all of the bool values.
	/// </summary>
	/// <returns>XML-formatted test data containing all of the bool values.</returns>
	static CStringW GetBoolData();

	/// <summary>
	/// Gets XML-formatted test data containing all of the bool values.
	/// </summary>
	/// <param name="ignoredA">IN - Ignored.  Used for compile-time type information.</param>
	/// <param name="ignoredB">IN - Ignored.  Used for compile-time type information.</param>
	/// <returns>XML-formatted test data containing all of the bool values.</returns>
	static CStringW GetData(bool ignoredA, bool ignoredB = false);

	/// <summary>
	/// Gets XML-formatted test data containing BYTE values.
	/// </summary>
	/// <returns>XML-formatted test data containing BYTE values.</returns>
	static CStringW GetByteData(BYTE extraThresholdA = 0, BYTE extraThresholdB = 0);

	/// <summary>
	/// Gets XML-formatted test data containing BYTE values.
	/// </summary>
	/// <param name="extraThresholdA">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <param name="extraThresholdB">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <returns>XML-formatted test data containing BYTE values.</returns>
	static CStringW GetData(BYTE extraThresholdA, BYTE extraThresholdB = 0);

	/// <summary>
	/// Gets XML-formatted test data containing short values.
	/// </summary>
	/// <param name="extraThresholdA">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <param name="extraThresholdB">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <returns>XML-formatted test data containing short values.</returns>
	static CStringW GetShortData(short extraThresholdA = 0, short extraThresholdB = 0);

	/// <summary>
	/// Gets XML-formatted test data containing short values.
	/// </summary>
	/// <param name="extraThresholdA">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <param name="extraThresholdB">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <returns>XML-formatted test data containing WORD values.</returns>
	static CStringW GetData(short extraThresholdA, short extraThresholdB = 0);

	/// <summary>
	/// Gets XML-formatted test data containing WORD values.
	/// </summary>
	/// <param name="extraThresholdA">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <param name="extraThresholdB">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <returns>XML-formatted test data containing WORD values.</returns>
	static CStringW GetWordData(WORD extraThresholdA = 0, WORD extraThresholdB = 0);

	/// <summary>
	/// Gets XML-formatted test data containing WORD values.
	/// </summary>
	/// <param name="extraThresholdA">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <param name="extraThresholdB">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <returns>XML-formatted test data containing WORD values.</returns>
	static CStringW GetData(WORD extraThresholdA, WORD extraThresholdB = 0);

	/// <summary>
	/// Gets XML-formatted test data containing int values.
	/// </summary>
	/// <param name="extraThresholdA">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <param name="extraThresholdB">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <returns>XML-formatted test data containing int values.</returns>
	static CStringW GetIntData(int extraThresholdA = 0, int extraThresholdB = 0);

	/// <summary>
	/// Gets XML-formatted test data containing int values.
	/// </summary>
	/// <param name="extraThresholdA">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <param name="extraThresholdB">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <returns>XML-formatted test data containing int values.</returns>
	static CStringW GetData(int extraThresholdA, int extraThresholdB = 0);

	/// <summary>
	/// Gets XML-formatted test data containing DWORD values.
	/// </summary>
	/// <param name="extraThresholdA">IN - If non-zero then extra values (..., threshold, ...) are included.</param>
	/// <param name="extraThresholdB">IN - If non-zero then extra values (..., threshold, ...) are included.</param>
	/// <returns>XML-formatted test data containing DWORD values.</returns>
	static CStringW GetDwordData(DWORD extraThresholdA = 0, DWORD extraThresholdB = 0);

	/// <summary>
	/// Gets XML-formatted test data containing DWORD values.
	/// </summary>
	/// <param name="extraThresholdA">IN - If non-zero then extra values (..., threshold, ...) are included.</param>
	/// <param name="extraThresholdB">IN - If non-zero then extra values (..., threshold, ...) are included.</param>
	/// <returns>XML-formatted test data containing DWORD values.</returns>
	static CStringW GetData(DWORD extraThresholdA, DWORD extraThresholdB = 0);

	/// <summary>
	/// Gets XML-formatted test data containing __int64 values.
	/// </summary>
	/// <param name="extraThresholdA">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <param name="extraThresholdB">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <returns>XML-formatted test data containing __int64 values.</returns>
	static CStringW GetIntData(__int64 extraThresholdA = 0, __int64 extraThresholdB = 0);

	/// <summary>
	/// Gets XML-formatted test data containing __int64 values.
	/// </summary>
	/// <param name="extraThresholdA">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <param name="extraThresholdB">IN - If non-zero then extra values (threshold-1, threshold, threshold+1) are included.</param>
	/// <returns>XML-formatted test data containing __int64 values.</returns>
	static CStringW GetData(__int64 extraThresholdA, __int64 extraThresholdB = 0);

	/// <summary>
	/// Gets XML-formatted test data containing string values.
	/// </summary>
	/// <returns>XML-formatted test data containing string values.</returns>
	static CStringW GetStringData();

	/// <summary>
	/// Gets an XML-formatted test data element containing the attributesString.
	/// </summary>
	/// <param name="attributesString">IN - The attributes string for the test data element.</param>
	/// <returns>An XML-formatted test data element containing the attributesString.</returns>
	static CStringW DataElement(LPCWSTR attributesString);

private:
	// Disable default constructor, default copy constructor, and default assignment operator.
	CUnitTestingXmlData();
	CUnitTestingXmlData(const CUnitTestingXmlData &rhs);
	CUnitTestingXmlData& operator=(const CUnitTestingXmlData &rhs);

	// Provide extra threshold values for testing thresholds against variable amounts.
	static const int EXTRA_THRESHOLD_VALUES = 2; // Number of extra threshold values on each side of a threshold value.
};
#endif //TODO-PORT