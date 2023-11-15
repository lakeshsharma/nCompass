#if 000//TODO-PORT
#include "stdafx.h"
#include "UnitTestingXmlLite.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

const LPCWSTR CXmlReaderTest::FALSE_STRING(L"false");
const LPCWSTR CXmlReaderTest::TRUE_STRING(L"true");

CXmlReaderTest::CXmlReaderTest(LPCWSTR data, const int *testIndex) :
	m_stream(NULL),
	m_reader(NULL),
	m_testIndex(testIndex),
	m_startWithFirstAttribute(true)
{
	HRESULT hr(S_OK);
		
	hr = CreateStreamOnHGlobal(NULL, FALSE, &m_stream);
	AssertHresultOk(hr, _T("CreateStreamOnHGlobal()"));
	Assert::IsTrue(m_stream != NULL, _T("m_stream"));

	hr = m_stream->Write(data, wcslen(data) * sizeof(WCHAR), NULL);
	AssertHresultOk(hr, _T("m_stream->Write()"));

	// Reset the stream to read from the beginning.
	LARGE_INTEGER pos;
	pos.QuadPart = 0;
	hr = m_stream->Seek(pos, STREAM_SEEK_SET, NULL);

	hr = CreateXmlReader(__uuidof(IXmlReader), (void**)&m_reader, NULL);
	AssertHresultOk(hr, _T("CreateXmlReader()"));
	Assert::IsNotNull(m_reader, _T("m_reader"));

	hr = m_reader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit);
	AssertHresultOk(hr, _T("m_reader->SetProperty()"));

	hr = m_reader->SetInput(m_stream);
	AssertHresultOk(hr, _T("m_reader->SetInput()"));
}

CXmlReaderTest::~CXmlReaderTest()
{
	if (m_reader != NULL)
	{
		m_reader->Release();
	}

	if (m_stream != NULL)
	{
		m_stream.Release();
	}
}

bool CXmlReaderTest::MoveToNextElement(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	Assert::IsNotNull(m_reader, _T("m_reader"));
	Assert::IsNotNull(targetLocalName, _T("targetLocalName"));

	bool found(false);
	HRESULT hr(S_OK);

	while (!found && S_OK == hr)
	{
		XmlNodeType nodeType;
		hr = m_reader->Read(&nodeType);
		if (S_OK == hr)
		{
			if (XmlNodeType_Element == nodeType)
			{
				const WCHAR *namespaceUri;
				UINT namespaceUriLength;
				hr = m_reader->GetNamespaceUri(&namespaceUri, &namespaceUriLength);
				if (S_OK == hr)
				{
					// Check if this element matches the target namespace.
					if (targetNamespaceUri == NULL && 0 == namespaceUriLength ||
						targetNamespaceUri != NULL && 0 == wcscmp(namespaceUri, targetNamespaceUri))
					{
						const WCHAR *localName;
						hr = m_reader->GetLocalName(&localName, NULL);
						if (S_OK == hr)
						{
							// Check if this element matches the target name.
							if (0 == wcscmp(localName, targetLocalName))
							{
								found = true;
								m_startWithFirstAttribute = true;
							}
						}
					}
				}
			}
		}
	}

	AssertHresultSuccess(hr, _T("MoveToNextElement() %s:%s"),
		static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
		static_cast<LPCTSTR>(CW2CT(targetLocalName)));
	return found;
}

HRESULT CXmlReaderTest::GetNextAttribute(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri, LPCWSTR &value)
{
	bool found(false);
	HRESULT hr(S_OK);
	value = NULL;

	while (!found && S_OK == hr)
	{
		if (m_startWithFirstAttribute)
		{
			m_startWithFirstAttribute = false;
			hr = m_reader->MoveToFirstAttribute();
		}
		else
		{
			hr = m_reader->MoveToNextAttribute();
		}

		if (S_OK == hr)
		{
			const WCHAR *namespaceUri;
			UINT namespaceUriLength;
			hr = m_reader->GetNamespaceUri(&namespaceUri, &namespaceUriLength);
			if (S_OK == hr)
			{
				// Check if this element matches the target namespace.
				if (targetNamespaceUri == NULL && 0 == namespaceUriLength ||
					targetNamespaceUri != NULL && 0 == wcscmp(namespaceUri, targetNamespaceUri))
				{
					const WCHAR *localName;
					hr = m_reader->GetLocalName(&localName, NULL);
					if (S_OK == hr)
					{
						// Check if this element matches the target name.
						if (0 == wcscmp(localName, targetLocalName))
						{
							found = true;

							const WCHAR *attributeValue;
							hr = m_reader->GetValue(&attributeValue, NULL);
							if (S_OK == hr)
							{
								value = attributeValue;
							}
						}
					}
				}
			}
		}
	}

	AssertHresultSuccess(hr, _T("GetNextAttribute() %s:%s found=%d"),
		static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
		static_cast<LPCTSTR>(CW2CT(targetLocalName)),
		static_cast<int>(found));
	return hr;
}

CString CXmlReaderTest::GetNextAttributeAsString(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	CString returnValue;

	LPCWSTR value;
	HRESULT hr(GetNextAttribute(targetLocalName, targetNamespaceUri, value));

	if (value != NULL && S_OK == hr)
	{
		returnValue = value;
	}
	else
	{
		AssertHresultMsg(hr, _T("GetNextAttributeAsString() %s:%s found=%d"),
			static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
			static_cast<LPCTSTR>(CW2CT(targetLocalName)),
			static_cast<int>(value != NULL));
	}

	return returnValue;
}

void CXmlReaderTest::GetNextAttribute(CString &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	value = GetNextAttributeAsString(targetLocalName, targetNamespaceUri);
}

bool CXmlReaderTest::GetNextAttributeAsBool(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	bool returnValue(false);

	LPCWSTR value;
	HRESULT hr(GetNextAttribute(targetLocalName, targetNamespaceUri, value));

	if (value != NULL && S_OK == hr)
	{
		if (0 == wcscmp(value, FALSE_STRING))
		{
			returnValue = false;
		}
		else if (0 == wcscmp(value, TRUE_STRING))
		{
			returnValue = true;
		}
		else
		{
			AssertHresultMsg(hr, _T("GetNextAttributeAsBool() %s:%s value=%s"),
				static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
				static_cast<LPCTSTR>(CW2CT(targetLocalName)),
				static_cast<LPCTSTR>(value));
		}
	}
	else
	{
		AssertHresultMsg(hr, _T("GetNextAttributeAsBool() %s:%s found=%d"),
			static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
			static_cast<LPCTSTR>(CW2CT(targetLocalName)),
			static_cast<int>(value != NULL));
	}

	return returnValue;
}

void CXmlReaderTest::GetNextAttribute(bool &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	value = GetNextAttributeAsBool(targetLocalName, targetNamespaceUri);
}

bool CXmlReaderTest::GetInt64FromString(LPCWSTR stringValue, __int64 &variable)
{
	bool ok(false);

	size_t stringLength(_tcslen(stringValue));
	if (stringLength > 0)
	{
		LPTSTR endPtr(NULL);
		errno = 0; // Clear errno so we can tell if the following function sets it.
		__int64 numericValue = _wcstoi64(stringValue, &endPtr, RADIX);
		if (errno != ERANGE)
		{
			if (endPtr == static_cast<LPCTSTR>(stringValue)+stringLength)
			{
				variable = numericValue;
				ok = true;
			}
		}
	}

	return ok;
}

bool CXmlReaderTest::GetUInt64FromString(LPCWSTR stringValue, unsigned __int64 &variable)
{
	bool ok(false);

	size_t stringLength(_tcslen(stringValue));
	if (stringLength > 0)
	{
		LPTSTR endPtr(NULL);
		errno = 0; // Clear errno so we can tell if the following function sets it.
		unsigned __int64 numericValue = _wcstoui64(stringValue, &endPtr, RADIX);
		if (errno != ERANGE)
		{
			if (endPtr == static_cast<LPCTSTR>(stringValue)+stringLength)
			{
				variable = numericValue;
				ok = true;
			}
		}
	}

	return ok;
}

BYTE CXmlReaderTest::GetNextAttributeAsBYTE(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	BYTE returnValue(0);

	LPCWSTR value;
	HRESULT hr(GetNextAttribute(targetLocalName, targetNamespaceUri, value));

	if (value != NULL && S_OK == hr)
	{
		unsigned __int64 numericValue;
		bool ok(GetUInt64FromString(value, numericValue));

		if (ok)
		{
			if (numericValue <= BYTE_MAX)
			{
				returnValue = (BYTE)numericValue;
			}
			else
			{
				ok = false;
			}
		}

		if (!ok)
		{
			AssertHresultMsg(hr, _T("GetNextAttributeAsBYTE() %s:%s value=%s"),
				static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
				static_cast<LPCTSTR>(CW2CT(targetLocalName)),
				static_cast<LPCTSTR>(value));
		}
	}
	else
	{
		AssertHresultMsg(hr, _T("GetNextAttributeAsBYTE() %s:%s found=%d"),
			static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
			static_cast<LPCTSTR>(CW2CT(targetLocalName)),
			static_cast<int>(value != NULL));
	}

	return returnValue;
}

void CXmlReaderTest::GetNextAttribute(BYTE &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	value = GetNextAttributeAsBYTE(targetLocalName, targetNamespaceUri);
}

int CXmlReaderTest::GetNextAttributeAsShort(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	int returnValue(0);

	LPCWSTR value;
	HRESULT hr(GetNextAttribute(targetLocalName, targetNamespaceUri, value));

	if (value != NULL && S_OK == hr)
	{
		__int64 numericValue;
		bool ok(GetInt64FromString(value, numericValue));

		if (ok)
		{
			if (SHRT_MIN <= numericValue && numericValue <= SHRT_MAX)
			{
				returnValue = (int)numericValue;
			}
			else
			{
				ok = false;
			}
		}

		if (!ok)
		{
			AssertHresultMsg(hr, _T("GetNextAttributeAsShort() %s:%s value=%s"),
				static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
				static_cast<LPCTSTR>(CW2CT(targetLocalName)),
				static_cast<LPCTSTR>(value));
		}
	}
	else
	{
		AssertHresultMsg(hr, _T("GetNextAttributeAsShort() %s:%s found=%d"),
			static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
			static_cast<LPCTSTR>(CW2CT(targetLocalName)),
			static_cast<int>(value != NULL));
	}

	return returnValue;
}

void CXmlReaderTest::GetNextAttribute(short &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	value = GetNextAttributeAsShort(targetLocalName, targetNamespaceUri);
}

WORD CXmlReaderTest::GetNextAttributeAsWORD(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	WORD returnValue(0);

	LPCWSTR value;
	HRESULT hr(GetNextAttribute(targetLocalName, targetNamespaceUri, value));

	if (value != NULL && S_OK == hr)
	{
		unsigned __int64 numericValue;
		bool ok(GetUInt64FromString(value, numericValue));

		if (ok)
		{
			if (numericValue <= WORD_MAX)
			{
				returnValue = (WORD)numericValue;
			}
			else
			{
				ok = false;
			}
		}

		if (!ok)
		{
			AssertHresultMsg(hr, _T("GetNextAttributeAsWORD() %s:%s value=%s"),
				static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
				static_cast<LPCTSTR>(CW2CT(targetLocalName)),
				static_cast<LPCTSTR>(value));
		}
	}
	else
	{
		AssertHresultMsg(hr, _T("GetNextAttributeAsWORD() %s:%s found=%d"),
			static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
			static_cast<LPCTSTR>(CW2CT(targetLocalName)),
			static_cast<int>(value != NULL));
	}

	return returnValue;
}

void CXmlReaderTest::GetNextAttribute(WORD &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	value = GetNextAttributeAsWORD(targetLocalName, targetNamespaceUri);
}

int CXmlReaderTest::GetNextAttributeAsInt(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	int returnValue(0);

	LPCWSTR value;
	HRESULT hr(GetNextAttribute(targetLocalName, targetNamespaceUri, value));

	if (value != NULL && S_OK == hr)
	{
		__int64 numericValue;
		bool ok(GetInt64FromString(value, numericValue));

		if (ok)
		{
			if (INT_MIN <= numericValue && numericValue <= INT_MAX)
			{
				returnValue = (int)numericValue;
			}
			else
			{
				ok = false;
			}
		}

		if (!ok)
		{
			AssertHresultMsg(hr, _T("GetNextAttributeAsInt() %s:%s value=%s"),
				static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
				static_cast<LPCTSTR>(CW2CT(targetLocalName)),
				static_cast<LPCTSTR>(value));
		}
	}
	else
	{
		AssertHresultMsg(hr, _T("GetNextAttributeAsInt() %s:%s found=%d"),
			static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
			static_cast<LPCTSTR>(CW2CT(targetLocalName)),
			static_cast<int>(value != NULL));
	}

	return returnValue;
}

void CXmlReaderTest::GetNextAttribute(int &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	value = GetNextAttributeAsInt(targetLocalName, targetNamespaceUri);
}

DWORD CXmlReaderTest::GetNextAttributeAsDWORD(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	DWORD returnValue(0);

	LPCWSTR value;
	HRESULT hr(GetNextAttribute(targetLocalName, targetNamespaceUri, value));

	if (value != NULL && S_OK == hr)
	{
		unsigned __int64 numericValue;
		bool ok(GetUInt64FromString(value, numericValue));

		if (ok)
		{
			if (numericValue <= DWORD_MAX)
			{
				returnValue = (DWORD)numericValue;
			}
			else
			{
				ok = false;
			}
		}

		if (!ok)
		{
			AssertHresultMsg(hr, _T("GetNextAttributeAsDWORD() %s:%s value=%s"),
				static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
				static_cast<LPCTSTR>(CW2CT(targetLocalName)),
				static_cast<LPCTSTR>(value));
		}
	}
	else
	{
		AssertHresultMsg(hr, _T("GetNextAttributeAsDWORD() %s:%s found=%d"),
			static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
			static_cast<LPCTSTR>(CW2CT(targetLocalName)),
			static_cast<int>(value != NULL));
	}

	return returnValue;
}

void CXmlReaderTest::GetNextAttribute(DWORD &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	value = GetNextAttributeAsDWORD(targetLocalName, targetNamespaceUri);
}

__int64 CXmlReaderTest::GetNextAttributeAsInt64(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	__int64 returnValue(0);

	LPCWSTR value;
	HRESULT hr(GetNextAttribute(targetLocalName, targetNamespaceUri, value));

	if (value != NULL && S_OK == hr)
	{
		__int64 numericValue;
		bool ok(GetInt64FromString(value, numericValue));

		if (ok)
		{
			if (_I64_MIN <= numericValue && numericValue <= _I64_MAX)
			{
				returnValue = (__int64)numericValue;
			}
			else
			{
				ok = false;
			}
		}

		if (!ok)
		{
			AssertHresultMsg(hr, _T("GetNextAttributeAsInt64() %s:%s value=%s"),
				static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
				static_cast<LPCTSTR>(CW2CT(targetLocalName)),
				static_cast<LPCTSTR>(value));
		}
	}
	else
	{
		AssertHresultMsg(hr, _T("GetNextAttributeAsInt64() %s:%s found=%d"),
			static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
			static_cast<LPCTSTR>(CW2CT(targetLocalName)),
			static_cast<int>(value != NULL));
	}

	return returnValue;
}

void CXmlReaderTest::GetNextAttribute(__int64 &value, LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	value = GetNextAttributeAsInt64(targetLocalName, targetNamespaceUri);
}

ULONGLONG CXmlReaderTest::GetNextAttributeAsQWORD(LPCWSTR targetLocalName, LPCWSTR targetNamespaceUri)
{
	ULONGLONG returnValue(0);

	LPCWSTR value;
	HRESULT hr(GetNextAttribute(targetLocalName, targetNamespaceUri, value));

	if (value != NULL && S_OK == hr)
	{
		unsigned __int64 numericValue;
		bool ok = GetUInt64FromString(value, numericValue);
		if (!ok)
		{
			AssertHresultMsg(hr, _T("GetNextAttributeAsQWORD() %s:%s value=%s"),
				static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
				static_cast<LPCTSTR>(CW2CT(targetLocalName)),
				static_cast<LPCTSTR>(value));
		}
	}
	else
	{
		AssertHresultMsg(hr, _T("GetNextAttributeAsQWORD() %s:%s found=%d"),
			static_cast<LPCTSTR>(CW2CT(targetNamespaceUri != NULL ? targetNamespaceUri : L"")),
			static_cast<LPCTSTR>(CW2CT(targetLocalName)),
			static_cast<int>(value != NULL));
	}

	return returnValue;
}

void CXmlReaderTest::AssertEof()
{
	Assert::IsNotNull(m_reader, _T("m_reader"));
	Assert::IsTrue(m_reader->IsEOF(), _T("m_reader->IsEOF()"));
}

/// <summary>
/// Asserts false for hr with the provided information.
/// </summary>
/// <param name="hr">IN - The HRESULT.</param>
/// <param name="format">IN - The format string for the optional variable arguments.</param>
/// <param name="">IN/OUT - The variable arguments.</param>
void CXmlReaderTest::AssertHresultMsg(HRESULT hr, LPCTSTR format, va_list argList)
{
	CString failedMsg;
	failedMsg.Format(_T("hr=%8.8x "), (unsigned)hr);
	if (m_testIndex != NULL)
	{
		failedMsg.AppendFormat(_T("testIndex=%d "), static_cast<int>(*m_testIndex));
	}

	failedMsg.AppendFormatV(format, argList);

	Assert::IsTrue(false, failedMsg);
}

/// <summary>
/// Asserts false for hr with the provided information.
/// </summary>
/// <param name="hr">IN - The HRESULT.</param>
/// <param name="format">IN - The format string for the optional variable arguments.</param>
/// <param name="">IN/OUT - The optional variable arguments.</param>
void CXmlReaderTest::AssertHresultMsg(HRESULT hr, LPCTSTR format, ...)
{
	va_list argList;
	va_start(argList, format);
	AssertHresultMsg(hr, format, argList);
}


/// <summary>
/// Asserts that the hr==S_OK.
/// </summary>
/// <param name="hr">IN - The HRESULT.</param>
/// <param name="format">IN - The format string for the optional variable arguments.</param>
/// <param name="">IN/OUT - The optional variable arguments.</param>
void CXmlReaderTest::AssertHresultOk(HRESULT hr, LPCTSTR format, ...)
{
	if (S_OK != hr)
	{
		va_list argList;
		va_start(argList, format);
		AssertHresultMsg(hr, format, argList);
	}
}

/// <summary>
/// Asserts that the SUCCEEDED(hr)==true.
/// </summary>
/// <param name="hr">IN - The HRESULT.</param>
/// <param name="format">IN - The format string for the optional variable arguments.</param>
/// <param name="">IN/OUT - The optional variable arguments.</param>
void CXmlReaderTest::AssertHresultSuccess(HRESULT hr, LPCTSTR format, ...)
{
	if (!SUCCEEDED(hr))
	{
		va_list argList;
		va_start(argList, format);
		AssertHresultMsg(hr, format, argList);
	}

}
#endif //TODO-PORT