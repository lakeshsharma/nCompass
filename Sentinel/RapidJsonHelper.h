#pragma once

#include "SentinelException.h"
#include "Utilities.h"
#include "OptionalVal.h"
#include "Util/Convert.h"

class JsonException : public SentinelFileException
{
public:
	enum JsonErrorCode
	{
		__start__JsonErrorCode__ = 1024,
		JsonFileNotFound = __start__JsonErrorCode__,
		DuplicateFilename,
		MemberNotFound,
		StringValueEmpty,
		ValueIsNotString,
		ValueIsNotInt,
		ValueIsNotUint,
		ValueIsNotInt64,
		ValueIsNotUint64,
		ValueIsNotDouble,
		ValueIsNotNumber,
		ValueIsNotBool,
		ValueIsNotArray,
		ValueIsNotObject,
		__last__JsonErrorCode__
	};

public:
	///	<summary>
	/// JsonException constructor.
	///	</summary>
	///	<param name="parseErrorCode">The rapidjson ParseErrorCode to save.</param>
	///	<param name="pMsg">The message to save with this instance. If nullptr is
	///		passed an empty string will be saved.</param>
	///	<param name="pFilename">The filename to save with this instance.</param>
	JsonException(rapidjson::ParseErrorCode parseErrorCode, const char* pMsg, const char* pFilename = "") throw();

	///	<summary>
	/// JsonException constructor.
	///	</summary>
	///	<param name="code">The code to save with this instance.</param>
	///	<param name="pMsg">The message to save with this instance as a UTF8 string.
	///		If nullptr is passed in empty string will be saved.</param>
	///	<param name="pFilename">The filename to save with this instance.</param>
	JsonException(int errorCode, const char* pMsg, const char* pFilename = "") throw();

	///	<summary>
	/// JsonException copy constructor.
	///	</summary>
	///	<param name="other">The JsonException object to copy.</param>
	JsonException(const JsonException& other) throw();

	///	<summary>
	/// JsonException assignment operator.
	///	</summary>
	///	<param name="other">The JsonException object to copy.</param>
	///	<returns>A reference to this instance.</returns>
	JsonException& operator=(const JsonException& other) throw();

	///	<summary>
	///	Returns the string associated with the stored 'code()'.
	///	</summary>
	///	<returns>The string associated with the stored 'code()'.</returns>
	const char* codestring() const throw();

	///	<summary>
	///	Returns the string associated with the passed parseErrorCode.
	///	</summary>
	///	<returns>The string associated with the passed parseErrorCode.</returns>
	static const char* GetParseErrorString(rapidjson::ParseErrorCode parseErrorCode);

	///	<summary>
	///	Returns the string associated with the passed helperErrorCode.
	///	</summary>
	///	<returns>The string associated with the passed helperErrorCode.</returns>
	static const char* GetJsonErrorCodeString(JsonErrorCode jsonErrorCode);
};

///	<summary>
/// Indicates whether the value associated with pName is a null.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<returns><b>true</b> if the member is found and it's a null, else <b>false</b>.</return>
template<typename Encoding, typename Allocator>
bool JsonIsNull(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName)
{
	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	return value.FindMember(pName) == value.MemberEnd() ? false : memberIt->value.IsNull();
}

///	<summary>
/// Returns the pointer to the string value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pDefaultVal">The default value.</param>
///	<returns>The value if the member is found and it's a string, else pDefaultVal.</returns>
/// <remarks>If you need to convert a UTF-8 string, use <see cref="JsonGetCString"/>.</remarks>
template<typename Encoding, typename Allocator> 
const char* JsonGetString(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pDefaultVal = nullptr)
{
	std::string emptyStr;
	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	return memberIt == value.MemberEnd() ? pDefaultVal : JsonIsNull(value, pName) ? emptyStr.c_str() : memberIt->value.GetString();
}

///	<summary>
/// Returns a CStringW instance with a properly converted string.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pDefaultVal">The default value.</param>
///	<returns>The value as a CStringW if the member is found and it's a string, else pDefaultVal.</returns>
/// <remarks>Use this function if you need to convert a UTF-8 string.</remarks>
template<typename Encoding, typename Allocator> 
std::string JsonGetCString(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pDefaultVal = "")
{
	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	return memberIt == value.MemberEnd() ? pDefaultVal : memberIt->value.GetString();
}

///	<summary>
/// Returns an int value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="defaultVal">The default value.</param>
///	<returns>The value if the member is found and it's an int, else defaultVal.</returns>
template<typename Encoding, typename Allocator> 
int JsonGetInt(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, int defaultVal = 0)
{
	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	return memberIt == value.MemberEnd() ? defaultVal : memberIt->value.GetInt();
}

///	<summary>
/// Returns an unsigned int value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="defaultVal">The default value.</param>
///	<returns>The value if the member is found and it's a uint, else defaultVal.</returns>
template<typename Encoding, typename Allocator> 
unsigned JsonGetUint(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, unsigned defaultVal = 0)
{
	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	return memberIt == value.MemberEnd() ? defaultVal : memberIt->value.GetUint();
}

///	<summary>
/// Returns an int64 value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="defaultVal">The default value.</param>
///	<returns>The value if the member is found and it's an int64, else defaultVal.</returns>
template<typename Encoding, typename Allocator> 
int64_t JsonGetInt64(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, int64_t defaultVal = 0)
{
	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	return memberIt == value.MemberEnd() ? defaultVal : memberIt->value.GetInt64();
}

///	<summary>
/// Returns the unsigned int64 value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="defaultVal">The default value.</param>
///	<returns>The value if the member is found and it's a uint64, else defaultVal.</returns>
template<typename Encoding, typename Allocator> 
uint64_t JsonGetUint64(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, uint64_t defaultVal = 0)
{
	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	return memberIt == value.MemberEnd() ? defaultVal : memberIt->value.GetUint64();
}

///	<summary>
/// Returns the double value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="defaultVal">The default value.</param>
///	<returns>The value if the member is found and it's a double, else defaultVal.</returns>
template<typename Encoding, typename Allocator> 
double JsonGetDouble(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, double defaultVal = 0.0)
{
	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	return memberIt == value.MemberEnd() ? defaultVal : memberIt->value.GetDouble();
}

///	<summary>
/// Returns the bool value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="defaultVal">The default value.</param>
///	<returns>The value if the member is found and it's a bool, else defaultVal.</returns>
template<typename Encoding, typename Allocator> 
bool JsonGetBool(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, bool defaultVal = false)
{
	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	return memberIt == value.MemberEnd() ? defaultVal : memberIt->value.GetBool();
}

///	<summary>
/// Indicates whether there is a value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<returns><b>true</b> if the member is found, else <b>false</b></returns>
template<typename Encoding, typename Allocator> 
bool JsonMemberExists(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName)
{
	return value.FindMember(pName) != value.MemberEnd();
}

///	<summary>
/// Indicates whether the value associated with pName is an array.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<returns><b>true</b> if the member is found and it's an array, else <b>false</b>.</return>
template<typename Encoding, typename Allocator> 
bool JsonIsArray(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName)
{
	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	return value.FindMember(pName) == value.MemberEnd() ? false : memberIt->value.IsArray();
}

///	<summary>
/// Indicates whether the value associated with pName is a number.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<returns><b>true</b> if the member is found and it's a number, else <b>false</b>.</return>
template<typename Encoding, typename Allocator> 
bool JsonIsNumber(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName)
{
	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	return value.FindMember(pName) == value.MemberEnd() ? false : memberIt->value.IsNumber();
}

///	<summary>
/// Returns the Value::ConstMemberIterator for the matching value.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>If the member is found, the Value::ConstMemberIterator for that value.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found.</exception>
template<typename Encoding, typename Allocator> 
rapidjson::Value::ConstMemberIterator JsonFindMemberEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	if (memberIt == value.MemberEnd())
		throw JsonException(JsonException::MemberNotFound, pName, pStreamName);

	return memberIt;
}

///	<summary>
/// Returns the pointer to the string value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="failIfEmpty">Fails if the string is nullptr or empty.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>A char pointer if the member is found and it is type string.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found, isn't a
///		string or, if failIfEmpty is set, if value is empty.</exception>
template<typename Encoding, typename Allocator> 
const char* JsonGetStringEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, bool failIfEmpty = false, const char* pStreamName = nullptr)
{
	rapidjson::Value::ConstMemberIterator memberIt = JsonFindMemberEx(value, pName);
	if (!memberIt->value.IsString())
		throw JsonException(JsonException::ValueIsNotString, pName, pStreamName);

	const char* pStr = memberIt->value.GetString();
	if (failIfEmpty && *pStr == 0)
		throw JsonException(JsonException::StringValueEmpty, pName, pStreamName);

	return pStr;
}

///	<summary>
/// Returns a CStringW instance with a properly converted string.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="failIfEmpty">Fails if the string is nullptr or empty.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>An instance of CStringW.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found, isn't a
///		string or, if failIfEmpty is set, if value is empty.</exception>
template<typename Encoding, typename Allocator> 
std::string JsonGetCStringEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, bool failIfEmpty = false, const char* pStreamName = nullptr)
{
	return JsonGetStringEx(value, pName, failIfEmpty, pStreamName);
}

///	<summary>
/// Indicates whether the value associated with pName in a number.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns><b>true</b> if the member is found and it is a number.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found.</exception>
template<typename Encoding, typename Allocator>
bool JsonIsNumberEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	rapidjson::Value::ConstMemberIterator memberIt = JsonFindMemberEx(value, pName, pStreamName);
	return memberIt->value.IsNumber();
}

///	<summary>
/// Returns an int64 value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>The value if the member is found and it is type int64.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found or isn't an int64.</exception>
template<typename Encoding, typename Allocator> 
int64_t JsonGetInt64Ex(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	rapidjson::Value::ConstMemberIterator memberIt = JsonFindMemberEx(value, pName, pStreamName);
	if (!memberIt->value.IsInt64())
		throw JsonException(JsonException::ValueIsNotInt64, pName, pStreamName);

	return memberIt->value.GetInt64();
}

///	<summary>
/// Returns a string instance with a properly converted string.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="failIfEmpty">Fails if the string is nullptr or empty.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>An instance of wstring.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found, isn't a
///		string or, if failIfEmpty is set, if value is empty.</exception>
template<typename Encoding, typename Allocator>
std::string JsonGetIdEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, bool failIfEmpty = false, const char* pStreamName = nullptr)
{
    UNREFERENCED_PARAMETER(&failIfEmpty); 
    UNREFERENCED_PARAMETER(&pStreamName); 

	std::string idString;
	if (JsonIsNumberEx(value, pName))
	{
		idString = std::to_string(JsonGetInt64Ex(value, pName));
	}
	else
	{
		idString = JsonGetStringEx(value, pName);
		//make sure this string value is a number
		if (idString.find_first_not_of("0123456789") != std::string::npos)
		{
			idString.clear();
		}
	}

	return idString;
}

///	<summary>
/// Returns an int value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">The optional stream name, for logging and debugging purposes.</param>
///	<returns>The value if the member is found and it is type int.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found or isn't an
///		int.</exception>
template<typename Encoding, typename Allocator> 
int JsonGetIntEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	rapidjson::Value::ConstMemberIterator memberIt = JsonFindMemberEx(value, pName, pStreamName);
	if (!memberIt->value.IsInt())
		throw JsonException(JsonException::ValueIsNotInt, pName, pStreamName);

	return memberIt->value.GetInt();
}

///	<summary>
/// Returns an unsigned int value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>The value if the member is found and it is type unsigned.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found or isn't an
///		unsigned int.</exception>
template<typename Encoding, typename Allocator> 
unsigned JsonGetUintEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	rapidjson::Value::ConstMemberIterator memberIt = JsonFindMemberEx(value, pName, pStreamName);
	if (!memberIt->value.IsUint())
		throw JsonException(JsonException::ValueIsNotUint, pName, pStreamName);

	return memberIt->value.GetUint();
}

///	<summary>
/// Returns the unsigned int64 value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>The value if the member is found and it is type uint64.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found or isn't a uint64.</exception>
template<typename Encoding, typename Allocator> 
uint64_t JsonGetUint64Ex(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	rapidjson::Value::ConstMemberIterator memberIt = JsonFindMemberEx(value, pName, pStreamName);
	if (!memberIt->value.IsUint64())
		throw JsonException(JsonException::ValueIsNotUint64, pName, pStreamName);

	return memberIt->value.GetUint64();
}

///	<summary>
/// Returns the double value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="strict">If <b>true</b>, the value <b>must</b> contain a decimal
///		point, otherwise any number will be valid.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>The value if the member is found and it is type double.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found or isn't a double.</exception>
template<typename Encoding, typename Allocator> 
double JsonGetDoubleEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, bool strict = false, const char* pStreamName = nullptr)
{
	rapidjson::Value::ConstMemberIterator memberIt = JsonFindMemberEx(value, pName, pStreamName);
	if (strict)
	{
		if (!memberIt->value.IsDouble())
			throw JsonException(JsonException::ValueIsNotDouble, pName, pStreamName);
	}
	else
	{
		if (!memberIt->value.IsNumber())
			throw JsonException(JsonException::ValueIsNotNumber, pName, pStreamName);
	}

	return memberIt->value.GetDouble();
}

///	<summary>
/// Returns the bool value associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>The value if the member is found and it is type bool.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found or isn't a bool.</exception>
template<typename Encoding, typename Allocator> 
bool JsonGetBoolEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	rapidjson::Value::ConstMemberIterator memberIt = JsonFindMemberEx(value, pName, pStreamName);
	if (!memberIt->value.IsBool())
		throw JsonException(JsonException::ValueIsNotBool, pName, pStreamName);

	return memberIt->value.GetBool();
}

///	<summary>
/// Returns the array object associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>The value if the member is found and it is type array.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found or isn't an array.</exception>
template<typename Encoding, typename Allocator> 
const rapidjson::Value& JsonGetArrayEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	rapidjson::Value::ConstMemberIterator memberIt = JsonFindMemberEx(value, pName, pStreamName);
	if (!memberIt->value.IsArray())
		throw JsonException(JsonException::ValueIsNotArray, pName, pStreamName);

	return memberIt->value;
}

///	<summary>
/// Returns the object associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>The value if the member is found and it is type object.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found or isn't an object.</exception>
template<typename Encoding, typename Allocator> 
const rapidjson::Value& JsonGetObjectEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	rapidjson::Value::ConstMemberIterator memberIt = JsonFindMemberEx(value, pName, pStreamName);
	if (!memberIt->value.IsObject())
		throw JsonException(JsonException::ValueIsNotObject, pName, pStreamName);

	return memberIt->value;
}

///	<summary>
/// Indicates whether the value associated with pName is a null.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns><b>true</b> if the member is found and it is a null. <b>false</b>.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found .</exception>
template<typename Encoding, typename Allocator> 
bool JsonIsNullEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	rapidjson::Value::ConstMemberIterator memberIt = JsonFindMemberEx(value, pName, pStreamName);
	return memberIt->value.IsNull();
}

///	<summary>
/// Indicates whether the value associated with pName in an array.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns><b>true</b> if the member is found and it is an array.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found.</exception>
template<typename Encoding, typename Allocator> 
bool JsonIsArrayEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	rapidjson::Value::ConstMemberIterator memberIt = JsonFindMemberEx(value, pName, pStreamName);
	return memberIt->value.IsArray();
}

///	<summary>
/// Returns an OptionalVal of type int associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>An OptionalVal<int> instance.</returns>
/// <exception cref="JsonException">Thrown if the value exists and isn't an int64_t.</exception>
template<typename Encoding, typename Allocator> 
OptionalVal<int64_t> JsonGetOptionalInt64Ex(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	OptionalVal<int64_t> optionalVal;

	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	if (memberIt == value.MemberEnd())
	{
		// Not finding the member is okay.
	}
	else if (!memberIt->value.IsInt64())
	{
		throw JsonException(JsonException::ValueIsNotInt64, pName, pStreamName);
	}
	else
	{
		optionalVal.isValid = true;
		optionalVal.val = memberIt->value.GetInt64();
	}

	return optionalVal;
}

///	<summary>
/// Returns an OptionalVal of type int associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>An OptionalVal<int> instance.</returns>
/// <exception cref="JsonException">Thrown if the value exists and isn't an int.</exception>
template<typename Encoding, typename Allocator> 
OptionalVal<int> JsonGetOptionalIntEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	OptionalVal<int> optionalVal;

	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	if (memberIt == value.MemberEnd())
	{
		// Not finding the member is okay.
	}
	else if (!memberIt->value.IsInt())
	{
		throw JsonException(JsonException::ValueIsNotInt, pName, pStreamName);
	}
	else
	{
		optionalVal.isValid = true;
		optionalVal.val = memberIt->value.GetInt();
	}

	return optionalVal;
};

///	<summary>
/// Returns an OptionalVal of type bool associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>An OptionalVal<int> instance.</returns>
/// <exception cref="JsonException">Thrown if the value exists and isn't an int.</exception>
template<typename Encoding, typename Allocator> 
OptionalVal<bool> JsonGetOptionalBoolEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	OptionalVal<bool> optionalVal;

	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	if (memberIt == value.MemberEnd())
	{
		// Not finding the member is okay.
	}
	else if (!memberIt->value.IsBool())
	{
		throw JsonException(JsonException::ValueIsNotBool, pName, pStreamName);
	}
	else
	{
		optionalVal.isValid = true;
		optionalVal.val = memberIt->value.GetBool();
	}

	return optionalVal;
};

///	<summary>
/// Returns an OptionalVal of type string associated with pName.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>An OptionalVal<int> instance.</returns>
/// <exception cref="JsonException">Thrown if the value exists and isn't an int.</exception>
template<typename Encoding, typename Allocator>
std::string JsonGetOptionalStringEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	std::string optionalVal;

	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	if (memberIt == value.MemberEnd())
	{
		// Not finding the member is okay.
	}
	else if (!memberIt->value.IsString())
	{
		throw JsonException(JsonException::ValueIsNotString, pName, pStreamName);
	}
	else
	{
		optionalVal = memberIt->value.GetString();
	}

	return optionalVal;
};

///	<summary>
/// Returns an optional array object associated with pName.  Returns nullptr if the array object is not present.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>The value if the member is found and it is type array, nullptr if the member is not found.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found or isn't an array.</exception>
template<typename Encoding, typename Allocator>
const rapidjson::Value* JsonGetOptionalArrayEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pStreamName = nullptr)
{
	const rapidjson::Value *returnValue(nullptr);

	rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
	if (memberIt == value.MemberEnd())
	{
		// Not finding the member is okay.
	}
	else if (!memberIt->value.IsArray())
	{
		throw JsonException(JsonException::ValueIsNotArray, pName, pStreamName);
	}
	else
	{
		returnValue = &memberIt->value;
	}

	return returnValue;
}

///	<summary>
/// Returns a string instance with a properly converted string.
///	</summary>
///	<param name="value">A rapidjson::GenericValue.</param>
///	<param name="pName">The name of the value.</param>
///	<param name="failIfEmpty">Fails if the string is nullptr or empty.</param>
///	<param name="pStreamName">An optional stream name, for logging and debugging purposes.</param>
///	<returns>An instance of wstring.</returns>
/// <exception cref="JsonException">Thrown if key cannot be found, isn't a
///		string or, if failIfEmpty is set, if value is empty.</exception>
template<typename Encoding, typename Allocator>
std::string JsonGetIdStringEx(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, bool failIfEmpty = false, const char* pStreamName = nullptr)
{
    UNREFERENCED_PARAMETER(&failIfEmpty); 
    UNREFERENCED_PARAMETER(&pStreamName); 

	const int64_t ALL_ID_VALUE = -1;
	std::string idString;

	if (JsonIsNumberEx(value, pName))
	{
		int64_t whereId(JsonGetInt64Ex(value, pName));
		if (ALL_ID_VALUE == whereId)
		{
			idString = SUBSCRIPTION_WHERE_ALL_STRING;
		}
		else
		{
			idString = std::to_string(whereId);
		}		
	}
	else
	{
		idString = JsonGetStringEx(value, pName);
	}

	return idString;
}


///	<summary>
/// Base class used when implementing a custom stream.
///	</summary>
///	<remarks>
/// rapidjson's streaming uses templates. This provides a mostly non-template
/// way of using streams.
///	</remarks>
class JsonBaseStream
{
public:
	typedef char Ch;

private:
	const std::string m_streamName;

public:
	///	<summary>
	/// JsonBaseStream constructor.
	///	</summary>
	JsonBaseStream(const std::string& pStreamName);

	///	<summary>
	/// JsonBaseStream constructor.
	///	</summary>
	virtual ~JsonBaseStream();

	///	<summary>
	///	Returns a pointer to the stream name.
	///	</summary>
	///	<returns>A pointer to the stream name. This will never be nullptr.</returns>
	virtual const char* GetStreamName() const;

private:
	JsonBaseStream(const JsonBaseStream&);
	void operator=(const JsonBaseStream&);
};

///	<summary>
/// Base class used when implementing a custom write stream.
///	</summary>
///	<remarks>
/// rapidjson's streaming uses templates. This provides a mostly non-template
/// way of using write streams.
///	</remarks>
class JsonWriteStream : public JsonBaseStream
{
public:
	///	<summary>
	/// JsonWriteStream constructor.
	///	</summary>
	///	<param name="pStreamName">The stream name. Can be nullptr.</param>
	JsonWriteStream(const std::string& streamName);

	///	<summary>
	/// JsonWriteStream constructor.
	///	</summary>
	virtual ~JsonWriteStream();

	///	<summary>
	/// Request to write a character to a stream.
	///	</summary>
	///	<param name="ch">The character to write.</param>
	virtual void Put(char ch) = 0;

	///	<summary>
	/// Request to flush the stream.
	///	</summary>
	///	<remarks>
	/// Not all streams can be flushed, so the default behavior is to do nothing.
	///	</remarks>
	virtual void Flush();

	///	<summary>
	/// Request to write a UTF8 string to a stream.
	///	</summary>
	///	<param name="pStr">The UTF8 string to write.</param>
	///	<remarks>
	/// This is not called by rapidjson, but is here for convenience.
	///	</remarks>
	virtual void Put(const char* pStr) = 0;

public:
	// The following are required by the interface, but are NOPs
	char* PutBegin();
	size_t PutEnd(char*);
	char Peek();
	char Take();
	size_t Tell();

private:
	JsonWriteStream(const JsonWriteStream&);
	void operator=(const JsonWriteStream&);
};

class JsonWriteStreamFile : public JsonWriteStream
{
protected:
	FILE* m_pFile;

public:
	///	<summary>
	/// JsonWriteStreamFile constructor.
	///	</summary>
	///	<param name="pFilename">The file name to create.</param>
	/// <exception cref="SentinelFileException">Thrown if file cannot be opened.</exception>
	JsonWriteStreamFile(const std::string& filename);

	///	<summary>
	/// JsonWriteStreamFile constructor.
	///	</summary>
	virtual ~JsonWriteStreamFile();

	///	<summary>
	/// Writes a character to file stream.
	///	</summary>
	///	<param name="ch">The character to write.</param>
	void Put(char ch);

	///	<summary>
	/// Flushes the file stream.
	///	</summary>
	void Flush();

	///	<summary>
	/// Write a UTF8 string to file stream.
	///	</summary>
	///	<param name="pStr">The UTF-8 string to write.</param>
	///	<remarks>
	/// This is not called by rapidjson, but is here for convenience.
	///	</remarks>
	virtual void Put(const char* pStr);

private:
	JsonWriteStreamFile(const JsonWriteStreamFile&);
	void operator=(const JsonWriteStreamFile&);
};

class JsonWriteStreamString : public JsonWriteStream
{
public:
	static const size_t DefaultReserve = 1024;
	std::string m_str;

public:
	///	<summary>
	/// JsonWriteStreamString constructor.
	///	</summary>
	///	<param name="reserve">The space to reserve.</param>
	JsonWriteStreamString(size_t reserve = DefaultReserve, const std::string& streamName = "");

	///	<summary>
	/// Write a character to the string.
	///	</summary>
	///	<param name="ch">The character to write.</param>
	void Put(char ch);

	///	<summary>
	/// Write a UTF-8 string to the string.
	///	</summary>
	///	<param name="pStr">The UTF-8 string to write.</param>
	///	<remarks>
	/// This is not called by rapidjson, but is here for convenience.
	///	</remarks>
	void Put(const char* pStr);

	///	<summary>
	/// Returns a pointer to the contained string.
	///	</summary>
	///	<returns>A pointer to the contained string.</remarks>
	operator const char*() const;

	///	<summary>
	/// Returns a reference to the contained string.
	///	</summary>
	///	<returns>A pointer to the contained string.</remarks>
	operator std::string&();

private:
	JsonWriteStreamString(const JsonWriteStreamString&);
	void operator=(const JsonWriteStreamString&);
};

///	<summary>
/// Base class used when implementing a custom read stream.
///	</summary>
///	<remarks>
/// rapidjson's streaming uses templates. This provides a mostly non-template
/// way of using write streams.
///	</remarks>
class JsonReadStream : public JsonBaseStream
{
public:
	///	<summary>
	/// JsonReadStream constructor.
	///	</summary>
	///	<param name="pStreamName">The stream name. Can be nullptr.</param>
	JsonReadStream(const std::string& streamName);

	///	<summary>
	/// JsonReadStream constructor.
	///	</summary>
	virtual ~JsonReadStream();

	///	<summary>
	/// Request to peek at the next character in a stream.
	///	</summary>
	/// <returns>The next character. Zero (0) indicates the end
	/// 	of the stream.</returns>
	virtual char Peek() const = 0;
		
	///	<summary>
	/// Request to get the next character in a stream.
	///	</summary>
	/// <returns>The next character. Zero (0) indicates the end
	/// 	of the stream.</returns>
	virtual char Take() = 0;

	///	<summary>
	/// Returns the current read cursor.
	///	</summary>
	/// <returns>Number of characters read from start.</returns>
	virtual size_t Tell() = 0;

	///	<summary>
	/// Request to read a UTF8 string from a stream.
	///	</summary>
	///	<param name="pStr">A pointer to a buffer to receive the string.</param>
	///	<param name="bufferLen">The length of the string buffer.</param>
	/// <returns>Number of characters actually read.</returns>
	///	<remarks>
	/// This is not called by rapidjson, but is here for convenience.
	///	</remarks>
	virtual size_t Take(char* pStr, size_t bufferLen);

public:
	// The following are required by the interface, but are NOPs
	char* PutBegin();
	size_t PutEnd(char*);
	void Put(char);
	virtual void Flush();

private:
	JsonReadStream(const JsonReadStream&);
	void operator=(const JsonReadStream&);
};

class JsonReadStreamFile : public JsonReadStream
{
protected:
	FILE* m_pFile;

public:
	///	<summary>
	/// JsonReadStreamFile constructor.
	///	</summary>
	///	<param name="pFilename">The file name to create.</param>
	/// <exception cref="SentinelFileException">Thrown if file cannot be opened.</exception>
	JsonReadStreamFile(const std::string& filename);

	///	<summary>
	/// JsonReadStreamFile constructor.
	///	</summary>
	virtual ~JsonReadStreamFile();

	///	<summary>
	/// Returns the next character in a stream.
	///	</summary>
	/// <returns>The next character. Zero (0) indicates the end
	/// 	of the stream.</returns>
	char Peek() const;

	///	<summary>
	/// Returns the next character in a stream.
	///	</summary>
	/// <returns>The next character. Zero (0) indicates the end
	/// 	of the stream.</returns>
	char Take();

	///	<summary>
	/// Returns the current read cursor.
	///	</summary>
	/// <returns>Number of characters read from start.</returns>
	size_t Tell();

	///	<summary>
	/// Reads a UTF-8 string from a stream.
	///	</summary>
	///	<param name="pBuffer">A pointer to a buffer to receive the string.</param>
	///	<param name="bufferLen">The length of the string buffer.</param>
	/// <returns>Number of characters actually read.</returns>
	///	<remarks>
	/// This is not called by rapidjson, but is here for convenience.
	///	</remarks>
	size_t Take(char* pBuffer, size_t bufferLen);

private:
	JsonReadStreamFile(const JsonReadStreamFile&);
	void operator=(const JsonReadStreamFile&);
};

class JsonReadStreamString : public JsonReadStream
{
private:
	const char* m_pStr;
	const char* m_pCur;
	const char* m_pEnd;

public:
	///	<summary>
	/// JsonReadStreamString constructor.
	///	</summary>
	///	<param name="pStr">The UTF-8 string to read.</param>
	///	<param name="stringLen">The length of the string. If SIZE_MAX, the length
	///		will be calculated using strlen.</param>
	///	<param name="pStreamName">The optional stream name.</param>
	explicit JsonReadStreamString(const char* pStr, size_t stringLen = (size_t)SIZE_MAX, const std::string& streamName = "");

	///	<summary>
	/// JsonReadStreamString constructor.
	///	</summary>
	///	<param name="str">The UTF-8 string to read.</param>
	///	<param name="pStreamName">The optional stream name.</param>
	explicit JsonReadStreamString(const std::string& str, const std::string& streamName = "");

	///	<summary>
	/// Returns the next character in a stream.
	///	</summary>
	/// <returns>The next character. Zero (0) indicates the end of the stream.</returns>
	char Peek() const;

	///	<summary>
	/// Returns the next character in a stream.
	///	</summary>
	/// <returns>The next character. Zero (0) indicates the end of the stream.</returns>
	char Take();

	///	<summary>
	/// Returns the current read cursor.
	///	</summary>
	/// <returns>Number of characters read from start.</returns>
	size_t Tell();

	///	<summary>
	/// Reads a UTF-8 string from a stream.
	///	</summary>
	///	<param name="pStr">A pointer to a buffer to receive the string.</param>
	///	<param name="bufferLen">The length of the string buffer.</param>
	/// <returns>Number of characters actually read.</returns>
	///	<remarks>
	/// This is not called by rapidjson, but is here for convenience.
	///	</remarks>
	size_t Take(char* pStr, size_t bufferLen);

private:
	JsonReadStreamString(const JsonReadStreamString&);
	void operator=(const JsonReadStreamString&);
};

///	<summary>
/// A writer with overloaded string methods.
///	</summary>
/// <remarks>
/// Although the Writer methods return a bool, only the String functions ever return
/// false and only when not using the default of UTF-8 for source and target AND a
/// conversion fails. In other words, the return can be ignored.
/// </remarks>
class JsonWriter : public rapidjson::Writer<JsonWriteStream>
{
public:
	JsonWriter(JsonWriteStream& os);

	///	<summary>
	/// Write a string to the JsonWriteStream
	///	</summary>
	///	<param name="pStr">A pointer to wchar_t string.</param>
	///	<returns><b>true</b> if successful, else <b>false</b>.</returns>
	bool String(const wchar_t* pStr);

	///	<summary>
	/// Write a string to the JsonWriteStream
	///	</summary>
	///	<param name="pStr">A pointer to wchar_t string.</param>
	///	<param name="length">The length of the string in characters.</param>
	///	<returns><b>true</b> if successful, else <b>false</b>.</returns>
	bool String(const wchar_t* pStr, rapidjson::SizeType length);

	///	<summary>
	/// Write a string to the JsonWriteStream
	///	</summary>
	///	<param name="str">A reference to a wstring.</param>
	///	<returns><b>true</b> if successful, else <b>false</b>.</returns>
	bool String(const std::wstring& str);

	///	<summary>
	/// Write a string to the JsonWriteStream
	///	</summary>
	///	<param name="pStr">A pointer to char string.</param>
	///	<param name="length">The length of the string in characters.</param>
	///	<param name="copy">Unused; needed for interface.</param>
	///	<returns><b>true</b> if successful, else <b>false</b>.</returns>
	bool String(const char* pStr, rapidjson::SizeType length, bool copy_ = true);

	///	<summary>
	/// Write a string to the JsonWriteStream
	///	</summary>
	///	<param name="pStr">A pointer to a char string.</param>
	///	<returns><b>true</b> if successful, else <b>false</b>.</returns>
	bool String(const char* pStr);

	///	<summary>
	/// Write a string to the JsonWriteStream
	///	</summary>
	///	<param name="str">A reference to a string.</param>
	///	<returns><b>true</b> if successful, else <b>false</b>.</returns>
	bool String(const std::string& str);

	///	<summary>
	/// Write currency to the JsonWriteStream
	///	</summary>
	///	<param name="val">The value, in cents, to write.</param>
	///	<returns><b>true</b> if successful, else <b>false</b>.</returns>
	/// <remarks>
	/// Currency is stored as a double in dollars. This will convert the value.
	/// <remarks>
	bool Currency(int64_t val);

	///	<summary>
	/// Write a bool to the JsonWriteStream
	///	</summary>
	///	<param name="val">A bool value to write</param>
	///	<returns><b>true</b> if successful, else <b>false</b>.</returns>
	bool Bool(bool val);

private:
	JsonWriter(const JsonWriter& other);
	void operator=(const JsonWriter& other);
};

class JsonConditionalObject
{
private:
	JsonWriter* m_pWriter;
	const char* m_pKey;

public:
	///	<summary>
	/// JsonConditionalObject constructor.
	///	</summary>
	///	<param name="writer">A reference to a JsonWriter instance.</param>
	///	<param name="pKey">The key to use if the object is to be written.</param>
	JsonConditionalObject(JsonWriter& writer, const char* pKey);

	///	<summary>
	/// JsonConditionalObject destructor; ends the object if it hasn't
	/// yet been ended.
	///	</summary>
	~JsonConditionalObject();

	///	<summary>
	///	Starts the object if it hasn't been started yet.
	///	</summary>
	void Start();

	///	<summary>
	///	Ends the object if it hasn't been ended yet.
	///	</summary>
	void End();

private:
	JsonConditionalObject(const JsonConditionalObject& other);
	void operator=(const JsonConditionalObject& other);
};

class JsonConditionalArray
{
private:
	JsonWriter* m_pWriter;
	const char* m_pKey;

public:
	///	<summary>
	/// JsonConditionalArray constructor.
	///	</summary>
	///	<param name="writer">A reference to a JsonWriter instance.</param>
	///	<param name="pKey">The key to use if the array is to be written.</param>
	JsonConditionalArray(JsonWriter& writer, const char* pKey);

	///	<summary>
	/// JsonConditionalObject destructor; ends the array if it hasn't
	/// yet been ended.
	///	</summary>
	~JsonConditionalArray();

	///	<summary>
	///	Starts the array if it hasn't been started yet.
	///	</summary>
	void Start();

	///	<summary>
	///	Ends the array if it hasn't been ended yet.
	///	</summary>
	void End();

private:
	JsonConditionalArray(const JsonConditionalArray& other);
	void operator=(const JsonConditionalArray& other);
};
