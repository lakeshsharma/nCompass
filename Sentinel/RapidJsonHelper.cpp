#include "stdafx.h"
#include "RapidJsonHelper.h"
#include "Utilities.h"
#include "Util/Currency.h"

using namespace std;
using namespace rapidjson;

JsonException::JsonException(ParseErrorCode code, const char* pMsg, const char* pFilename) throw()
	: SentinelFileException(pFilename, code, pMsg)
{
}

JsonException::JsonException(int code, const char* pMsg, const char* pFilename) throw()
	: SentinelFileException(pFilename, code, pMsg)
{
}

JsonException::JsonException(const JsonException& other) throw()
	: SentinelFileException(other)
{
}

JsonException& JsonException::operator=(const JsonException& other) throw()
{
	SentinelFileException::operator=(other);
	return *this;
}

const char* JsonException::codestring() const throw()
{
	return GetParseErrorString((ParseErrorCode) code());
}

const char* JsonException::GetParseErrorString(ParseErrorCode parseErrorCode)
{
	return ((int)parseErrorCode < (int)__start__JsonErrorCode__) ?
		rapidjson::GetParseError_En(parseErrorCode) :
		GetJsonErrorCodeString((JsonErrorCode) parseErrorCode);
}

static const char* JsonErrorStrings[] =
{
	"JSON File not found",
	"DuplicateFilename",
	"Member not found",
	"String value is null or empty",
	"Value is not a string",
	"Value is not an Int",
	"Value is not a UInt",
	"Value is not an Int64",
	"Value is not a UInt64",
	"Value is not a double",
	"Value is not a number",
	"Value is not a bool",
	"Value is not an array",
	"Value is not an object",
	"Unknown error"
};

const char* JsonException::GetJsonErrorCodeString(JsonErrorCode helperErrorCode)
{
	if (helperErrorCode < __start__JsonErrorCode__)
		return GetParseErrorString((ParseErrorCode) helperErrorCode);

	if (helperErrorCode > __last__JsonErrorCode__)
		helperErrorCode = __last__JsonErrorCode__;

	return JsonErrorStrings[helperErrorCode - __start__JsonErrorCode__];
}

JsonBaseStream::JsonBaseStream(const string& streamName)
	: m_streamName(streamName)
{
}

JsonBaseStream::~JsonBaseStream()
{
}

const char* JsonBaseStream::GetStreamName() const
{
	return m_streamName.c_str();
}

JsonWriteStream::JsonWriteStream(const string& streamName)
	: JsonBaseStream(streamName)
{
}

JsonWriteStream::~JsonWriteStream()
{
}

void JsonWriteStream::Flush()
{
}

char* JsonWriteStream::PutBegin()
{ 
	return nullptr; 
}

size_t JsonWriteStream::PutEnd(char*)
{ 
	return 0;
}

char JsonWriteStream::Peek()
{
	return 0;
}

char JsonWriteStream::Take()
{
	return 0;
}

size_t Tell()
{
	return 0;
}

JsonWriteStreamFile::JsonWriteStreamFile(const string& filename)
	: JsonWriteStream(filename)
	, m_pFile(nullptr)
{
	m_pFile = fopen(filename.c_str(), "wbS");
	if (m_pFile == nullptr)
		throw JsonException(-1, nullptr, filename.c_str());
}

JsonWriteStreamFile::~JsonWriteStreamFile()
{
	if (m_pFile)
		fclose(m_pFile);
}

void JsonWriteStreamFile::Put(char ch)
{
	if (m_pFile)
		fputc(ch, m_pFile);
}

void JsonWriteStreamFile::Flush()
{
	if (m_pFile)
		fflush(m_pFile);
}

void JsonWriteStreamFile::Put(const char* pStr)
{
	if (m_pFile && pStr && *pStr)
		fwrite(pStr, 1, strlen(pStr), m_pFile);
}

JsonWriteStreamString::JsonWriteStreamString(size_t reserve, const string& streamName) 
	: JsonWriteStream(streamName)
{
	m_str.reserve(reserve);
}

void JsonWriteStreamString::Put(char ch)
{
	m_str += ch;
}

void JsonWriteStreamString::Put(const char* pStr)
{
	if (pStr != nullptr && *pStr != 0)
		m_str += pStr;
}

JsonWriteStreamString::operator const char*() const
{
	return m_str.c_str();
}

JsonWriteStreamString::operator string&()
{
	return m_str;
}

JsonReadStream::JsonReadStream(const string& streamName)
	: JsonBaseStream(streamName)
{
}

JsonReadStream::~JsonReadStream()
{
}

size_t JsonReadStream::Take(char*, size_t)
{
	return 0;
}

char* JsonReadStream::PutBegin()
{
	return nullptr;
}

size_t JsonReadStream::PutEnd(char*)
{
	return 0;
}

void JsonReadStream::Put(char)
{
}

void JsonReadStream::Flush()
{
}

// #pragma endregion JsonReadStream

// #pragma region JsonReadStreamFile

JsonReadStreamFile::JsonReadStreamFile(const string& filename)
	: JsonReadStream(filename)
	, m_pFile(nullptr)
{
	m_pFile = fopen(filename.c_str(), "rbS");
	if (m_pFile == nullptr)
		throw JsonException(-1, nullptr, filename.c_str());
}

JsonReadStreamFile::~JsonReadStreamFile()
{
	if (m_pFile)
		fclose(m_pFile);
}

char JsonReadStreamFile::Peek() const
{
	if (!m_pFile)
		return 0;

	int ch = fgetc(m_pFile);
	ungetc(ch, m_pFile);

	return ch == EOF ? 0 : (char) ch;
}

char JsonReadStreamFile::Take()
{
	if (!m_pFile || feof(m_pFile))
		return 0;

	int ch = fgetc(m_pFile);
	return ch == EOF ? 0 : (char) ch;
}

size_t JsonReadStreamFile::Tell()
{
	return m_pFile ? ftell(m_pFile) : 0;
}

size_t JsonReadStreamFile::Take(char* pBuffer, size_t bufferLen)
{
	return m_pFile && pBuffer && bufferLen ?
		fread(pBuffer, 1, bufferLen, m_pFile) : 0;
}

// #pragma endregion JsonReadStreamFile

// #pragma region JsonReadStreamString

JsonReadStreamString::JsonReadStreamString(const char* pStr, size_t stringLen, const string& streamName) 
	: JsonReadStream(streamName)
	, m_pStr(pStr)
	, m_pCur(pStr)
	, m_pEnd(pStr)
{
	if (m_pStr != nullptr && stringLen != 0)
	{
		if (stringLen == SIZE_MAX)
		{
			m_pEnd = &m_pStr[strlen(m_pStr)];
		}
		else
		{
			m_pEnd = &m_pStr[stringLen];
		}
	}
}

JsonReadStreamString::JsonReadStreamString(const string& str, const string& streamName) 
	: JsonReadStream(streamName)
	, m_pStr(str.c_str())
	, m_pCur(m_pStr)
	, m_pEnd(&m_pStr[str.size()])
{
}

char JsonReadStreamString::Peek() const
{
	return m_pCur < m_pEnd ? *m_pCur : 0;
}

char JsonReadStreamString::Take()
{
	return m_pCur < m_pEnd ? *m_pCur++ : 0;
}

size_t JsonReadStreamString::Tell()
{
	return m_pCur - m_pEnd;
}

size_t JsonReadStreamString::Take(char* pBuffer, size_t bufferLen)
{
	size_t remainder = m_pEnd - m_pStr;
	if (remainder > 0)
	{
		if (remainder > bufferLen)
			remainder = bufferLen;

		if (pBuffer != nullptr)
			memcpy(pBuffer, m_pCur, remainder);

		m_pCur += remainder;
	}
	return remainder;
}

JsonWriter::JsonWriter(JsonWriteStream& os)
	: Writer(os)
{
}

bool JsonWriter::String(const wchar_t* str)
{ 
	return Writer::String(Convert::WStrToUtf8(str));
}

bool JsonWriter::String(const wchar_t* str, rapidjson::SizeType length)
{ 
	return Writer::String(Convert::WStrToUtf8(str, length));
}

bool JsonWriter::String(const wstring& str)
{ 
	return Writer::String(Convert::WStrToUtf8(str));
}

bool JsonWriter::String(const char* str)
{ 
	return Writer::String(str);
}

bool JsonWriter::String(const char* str, rapidjson::SizeType length, bool copy_)
{ 
	return Writer::String(str, length, copy_);
}

bool JsonWriter::String(const string& str)
{ 
	return Writer::String(str);
}

bool JsonWriter::Currency(int64_t val)
{
	return Writer::Double(Currency::Int64ToCurrency(val));
}

bool JsonWriter::Bool(bool val)
{
	return Writer::Bool(val);
}

JsonConditionalObject::JsonConditionalObject(JsonWriter& writer, const char* pKey)
	: m_pWriter(&writer)
	, m_pKey(pKey)
{
}

JsonConditionalObject::~JsonConditionalObject()
{
	End();
}

void JsonConditionalObject::Start()
{
	if (m_pWriter != nullptr && m_pKey != nullptr)
	{
		m_pWriter->Key(m_pKey);
		m_pWriter->StartObject();
		m_pKey = nullptr;
	}
}

void JsonConditionalObject::End()
{
	if (m_pWriter != nullptr && m_pKey == nullptr)
	{
		m_pWriter->EndObject();
		m_pWriter = nullptr;
	}
}

JsonConditionalArray::JsonConditionalArray(JsonWriter& writer, const char* pKey)
	: m_pWriter(&writer)
	, m_pKey(pKey)
{
}

JsonConditionalArray::~JsonConditionalArray()
{
	End();
}

void JsonConditionalArray::Start()
{
	if (m_pWriter != nullptr && m_pKey != nullptr)
	{
		m_pWriter->Key(m_pKey);
		m_pWriter->StartArray();
		m_pKey = nullptr;
	}
}

void JsonConditionalArray::End()
{
	if (m_pWriter != nullptr && m_pKey == nullptr)
	{
		m_pWriter->EndArray();
		m_pWriter = nullptr;
	}
}
