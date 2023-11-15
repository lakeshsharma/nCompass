#pragma once
#if 000//TODO-PORT
/// <summary>
/// IXmlReader functionality wrapper and helper functions for unit tests.
/// </summary>
class CXmlReaderTest
{
public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CXmlReaderTest"/> class.
	/// </summary>
	/// <param name="testIndex">IN - The XML data to be read.</param>
	/// <param name="testIndex">IN - Index of the test.</param>
	CXmlReaderTest(LPCWSTR data, const int *testIndex = NULL);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CXmlReaderTest"/> class.
	/// </summary>
	~CXmlReaderTest();

	/// <summary>
	/// Moves the reader to the next element that matches targetLocalName and targetNamespaceUri.
	/// </summary>
	/// <param name="name">IN - The target name.</param>
	/// <param name="targetNamespaceUri">The target namespace URI.</param>
	/// <returns>true if the element was found, false if end-of-file was found, otherwise an AssertFailedException is thrown</returns>
	/// <remarks>
	/// If true is returned then the next attribute to get will be the first attribute of the element.
	/// </remarks>
	bool MoveToNextElement(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as a CString value.
	/// </summary>
	/// <param name="targetLocalName">Name of the target local.</param>
	/// <param name="targetNamespaceUri">The target namespace URI.</param>
	/// <returns>The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</returns>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	CString GetNextAttributeAsString(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as a CString value.
	/// </summary>
	/// <param name="value">OUT - The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</param>
	/// <param name="targetLocalName">IN - Name of the target local.</param>
	/// <param name="targetNamespaceUri">IN - The target namespace URI.</param>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	void GetNextAttribute(CString &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as a bool value.
	/// </summary>
	/// <param name="targetLocalName">Name of the target local.</param>
	/// <param name="targetNamespaceUri">The target namespace URI.</param>
	/// <returns>The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</returns>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	bool GetNextAttributeAsBool(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as a bool value.
	/// </summary>
	/// <param name="value">OUT - The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</param>
	/// <param name="targetLocalName">IN - Name of the target local.</param>
	/// <param name="targetNamespaceUri">IN - The target namespace URI.</param>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	void GetNextAttribute(bool &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as a BYTE value.
	/// </summary>
	/// <param name="targetLocalName">Name of the target local.</param>
	/// <param name="targetNamespaceUri">The target namespace URI.</param>
	/// <returns>The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</returns>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	BYTE GetNextAttributeAsBYTE(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as a BYTE value.
	/// </summary>
	/// <param name="value">OUT - The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</param>
	/// <param name="targetLocalName">IN - Name of the target local.</param>
	/// <param name="targetNamespaceUri">IN - The target namespace URI.</param>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	void GetNextAttribute(BYTE &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as a short value.
	/// </summary>
	/// <param name="targetLocalName">Name of the target local.</param>
	/// <param name="targetNamespaceUri">The target namespace URI.</param>
	/// <returns>The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</returns>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	int GetNextAttributeAsShort(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as a short value.
	/// </summary>
	/// <param name="value">OUT - The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</param>
	/// <param name="targetLocalName">IN - Name of the target local.</param>
	/// <param name="targetNamespaceUri">IN - The target namespace URI.</param>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	void GetNextAttribute(short &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as a WORD value.
	/// </summary>
	/// <param name="targetLocalName">Name of the target local.</param>
	/// <param name="targetNamespaceUri">The target namespace URI.</param>
	/// <returns>The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</returns>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	WORD GetNextAttributeAsWORD(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as a WORD value.
	/// </summary>
	/// <param name="value">OUT - The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</param>
	/// <param name="targetLocalName">IN - Name of the target local.</param>
	/// <param name="targetNamespaceUri">IN - The target namespace URI.</param>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	void GetNextAttribute(WORD &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as an int value.
	/// </summary>
	/// <param name="targetLocalName">Name of the target local.</param>
	/// <param name="targetNamespaceUri">The target namespace URI.</param>
	/// <returns>The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</returns>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	int GetNextAttributeAsInt(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as an int value.
	/// </summary>
	/// <param name="value">OUT - The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</param>
	/// <param name="targetLocalName">IN - Name of the target local.</param>
	/// <param name="targetNamespaceUri">IN - The target namespace URI.</param>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	void GetNextAttribute(int &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as a DWORD value.
	/// </summary>
	/// <param name="targetLocalName">Name of the target local.</param>
	/// <param name="targetNamespaceUri">The target namespace URI.</param>
	/// <returns>The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</returns>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	DWORD GetNextAttributeAsDWORD(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as a DWORD value.
	/// </summary>
	/// <param name="value">OUT - The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</param>
	/// <param name="targetLocalName">IN - Name of the target local.</param>
	/// <param name="targetNamespaceUri">IN - The target namespace URI.</param>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	void GetNextAttribute(DWORD &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as an __int64 value.
	/// </summary>
	/// <param name="targetLocalName">Name of the target local.</param>
	/// <param name="targetNamespaceUri">The target namespace URI.</param>
	/// <returns>The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</returns>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	__int64 GetNextAttributeAsInt64(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as an __int64 value.
	/// </summary>
	/// <param name="value">OUT - The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</param>
	/// <param name="targetLocalName">IN - Name of the target local.</param>
	/// <param name="targetNamespaceUri">IN - The target namespace URI.</param>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	void GetNextAttribute(__int64 &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Gets the next attribute (of the current element) that matches targetLocalName and targetNamespaceUri.
	/// The attribute value is returned as a ULONGLONG value.
	/// </summary>
	/// <param name="targetLocalName">Name of the target local.</param>
	/// <param name="targetNamespaceUri">The target namespace URI.</param>
	/// <returns>The attribute value if the attribute is found, otherwise an AssertFailedException is thrown.</returns>
	/// <remarks>
	/// Attributes are processed in order, so the XML data is processed more efficiently.
	/// </remarks>
	ULONGLONG GetNextAttributeAsQWORD(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri = NULL);

	/// <summary>
	/// Asserts that end-of-file is true for the reader.
	/// </summary>
	void AssertEof();

private:
	static const LPCTSTR FALSE_STRING;
	static const LPCTSTR TRUE_STRING;
	static const int RADIX = 10; // Only support base 10 for numeric values.

	CComPtr<IStream> m_stream;
	IXmlReader *m_reader;
	volatile const int *m_testIndex;
	bool m_startWithFirstAttribute;

	HRESULT GetNextAttribute(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri, LPCWSTR &value);
	bool GetInt64FromString(LPCWSTR stringValue, __int64 &variable);
	bool GetUInt64FromString(LPCWSTR stringValue, unsigned __int64 &variable);
	void AssertHresultMsg(HRESULT hr, LPCTSTR format, va_list argList);
	void AssertHresultMsg(HRESULT hr, LPCTSTR format, ...);
	void AssertHresultOk(HRESULT hr, LPCTSTR format, ...);
	void AssertHresultSuccess(HRESULT hr, LPCTSTR format, ...);
};
#endif //TODO-PORT