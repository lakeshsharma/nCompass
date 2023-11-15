
#pragma once

#include "RapidJsonHelper.h"
#include "UnitTestingUtils.h"
#include "Util/Currency.h"
#include "AssertMsg.h"
#include "gtest/gtest.h"

namespace SentinelNativeUnitTests
{

	///	<summary>
    	/// Tests a json document for a string value.
    	///	</summary>
    	///	<param name="value">A rapidjson::GenericValue.</param>
    	///	<param name="pName">The name of the value.</param>
    	///	<param name="pExpectedVal">The expected value. If nullptr, the object is expected to not be present.</param>
    	///	<param name="expectedToExist">Whether the value is optional.</param>
        /// <param name="line">The line number.</param>
    	
        template<typename Encoding, typename Allocator> 
    	void JsonGetStringTest(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, const char* pExpectedVal, int line = 0, bool testEqual = true)
    	{
    		rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
    		if (memberIt == value.MemberEnd())
    		{
    			ASSERT_TRUE(pExpectedVal==nullptr) << AssertMsg::Format(line, L"String", L"%S is missing", pName);
    		}
    		else
    		{
    			ASSERT_TRUE(pExpectedVal!=nullptr) << AssertMsg::Format(line, L"String", L"%S", pName);
    			
    			if (!memberIt->value.IsString())
    			{
    				FAIL()<<AssertMsg::Format(line, L"String", L"%S is not a string", pName);
    			}
    			else if (testEqual)
    			{
    				const char* pActualVal = memberIt->value.GetString();
    				ASSERT_STREQ(pExpectedVal, pActualVal) <<  AssertMsg::Format(line, L"String", L"%S", pName);
    			}
    		}
    	}

    	///	<summary>
    	/// Tests a json document for an int value.
    	///	</summary>
    	///	<param name="value">A rapidjson::GenericValue.</param>
    	///	<param name="pName">The name of the value.</param>
    	///	<param name="pExpectedVal">The expected value. If nullptr, the object is expected to not be present.</param>
    	///	<param name="expectedToExist">Whether the value is optional.</param>
    	///	<param name="line">The line number.</param>
        /// <param name="testEqual">true if the values should be tested
        /// for equality, false to not test the values for
        /// equality.</param>
        /// 
        template<typename Encoding, typename Allocator> 
    	void JsonGetIntTest(const rapidjson::GenericValue<Encoding, Allocator>& value,
    		const char* pName,
    		const int *pExpectedVal,
    		bool expectedToExist = true,
    		int line = 0,
    		bool testEqual = true)
    	{
    		if (nullptr == pExpectedVal)
    		{
    			expectedToExist = false;
    		}

    		rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
    		if (memberIt == value.MemberEnd())
    		{
    			if (expectedToExist)
    				FAIL()<<AssertMsg::Format(line, L"Int", L"%S is missing", pName);
    		}
    		else if (!expectedToExist)
    		{
    			FAIL()<<"tgtg";//AssertMsg::Format(line, L"Int", L"%S exists", pName);
    		}
    		else if (!memberIt->value.IsInt())
    		{
                FAIL()<<AssertMsg::Format(line, L"Int", L"%S is not an int", pName);
    		}
    		else if (testEqual)
    		{
    			int actualVal = memberIt->value.GetInt();
    			ASSERT_EQ(*pExpectedVal,
    				actualVal) << 
    				AssertMsg::Format(line, L"Int", L"%S", pName);
    		}
    	}

    	///	<summary>
    	/// Tests a json document for an int value.
    	///	</summary>
    	///	<param name="value">A rapidjson::GenericValue.</param>
    	///	<param name="pName">The name of the value.</param>
    	///	<param name="expectedVal">The expected value.</param>
    	///	<param name="expectedToExist">Whether the value is optional.</param>
    	///	<param name="line">The line number.</param>
    	///	<param name="testEqual">true if the values should be tested for equality, false to not test the values for equality.</param>
        template<typename Encoding, typename Allocator> 
        void JsonGetIntTest(const rapidjson::GenericValue<Encoding, Allocator> &value,
    		const char *pName,
    		int expectedVal,
    		bool expectedToExist = true,
    		int line = 0,
    		bool testEqual = true)
    	{
    		JsonGetIntTest(value, pName, &expectedVal, expectedToExist, line, testEqual);
    	}

    	///	<summary>
    	/// Tests a json document for an unsigned int value.
    	///	</summary>
    	///	<param name="value">A rapidjson::GenericValue.</param>
    	///	<param name="pName">The name of the value.</param>
    	///	<param name="pExpectedVal">The expected value.</param>
    	///	<param name="expectedToExist">Whether the value is optional.</param>
    	///	<param name="line">The line number.</param>
        template<typename Encoding, typename Allocator> 
        void JsonGetUintTest(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, unsigned expectedVal, bool expectedToExist = true, int line = 0, bool testEqual = true)
    	{
    		rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
    		if (memberIt == value.MemberEnd())
    		{
    			if (expectedToExist)
    				FAIL()<<AssertMsg::Format(line, L"Uint", L"%S is missing", pName);
    		}
    		else if (!expectedToExist)
    		{
    			FAIL()<<AssertMsg::Format(line, L"Uint", L"%S exists", pName);
    		}
    		else if (!memberIt->value.IsUint())
    		{
    			FAIL()<<AssertMsg::Format(line, L"Uint", L"%S is not a Uint", pName);
    		}
    		else if (testEqual)
    		{
    			unsigned actualVal = memberIt->value.GetUint();
    			ASSERT_EQ(expectedVal, actualVal) <<  AssertMsg::Format(line, L"Uint", L"%S", pName);
    		}
    	}

    	///	<summary>
    	/// Tests a json document for an Int64 value.
    	///	</summary>
    	///	<param name="value">A rapidjson::GenericValue.</param>
    	///	<param name="pName">The name of the value.</param>
    	///	<param name="pExpectedVal">The expected value.</param>
    	///	<param name="expectedToExist">Whether the value is optional.</param>
    	///	<param name="line">The line number.</param>
        template<typename Encoding, typename Allocator> 
        void JsonGetInt64Test(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, int64_t expectedVal, bool expectedToExist = true, int line = 0, bool testEqual = true)
    	{
    		rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
    		if (memberIt == value.MemberEnd())
    		{
    			if (expectedToExist)
    				FAIL()<<AssertMsg::Format(line, L"Int64", L"%S is missing", pName);
    		}
    		else if (!expectedToExist)
    		{
    			FAIL()<<AssertMsg::Format(line, L"Int64", L"%S exists", pName);
    		}
    		else if (!memberIt->value.IsInt64())
    		{
    			FAIL()<<AssertMsg::Format(line, L"Int64", L"%S is not an Int64", pName);
    		}
    		else if (testEqual)
    		{
    			int64_t actualVal = memberIt->value.GetInt64();
    			ASSERT_EQ((uint64_t) expectedVal, (uint64_t) actualVal) << AssertMsg::Format(line, L"Int64", L"%S", pName);
    		}
    	}

    	///	<summary>
    	/// Tests a json document for an Uint64 value.
    	///	</summary>
    	///	<param name="value">A rapidjson::GenericValue.</param>
    	///	<param name="pName">The name of the value.</param>
    	///	<param name="pExpectedVal">The expected value.</param>
    	///	<param name="expectedToExist">Whether the value is optional.</param>
    	///	<param name="line">The line number.</param>
        template<typename Encoding, typename Allocator> 
        void JsonGetUint64Test(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, uint64_t expectedVal, bool expectedToExist = true, int line = 0, bool testEqual = true)
    	{
    		rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
    		if (memberIt == value.MemberEnd())
    		{
    			if (expectedToExist)
    				FAIL() << AssertMsg::Format(line, L"Uint64", L"%S is missing", pName);
    		}
    		else if (!expectedToExist)
    		{
    			FAIL() << AssertMsg::Format(line, L"Uint64", L"%S exists", pName);
    		}
    		else if (!memberIt->value.IsUint64())
    		{
    			FAIL() << AssertMsg::Format(line, L"Uint64", L"%S is not a Uint64", pName);
    		}
    		else if (testEqual)
    		{
    			uint64_t actualVal = memberIt->value.GetUint64();
    			ASSERT_EQ(expectedVal, actualVal) << AssertMsg::Format(line, L"Uint64", L"%S", pName);
    		}
    	}

    	///	<summary>
    	/// Tests a json document for an double value.
    	///	</summary>
    	///	<param name="value">A rapidjson::GenericValue.</param>
    	///	<param name="pName">The name of the value.</param>
    	///	<param name="pExpectedVal">The expected value.</param>
    	///	<param name="expectedToExist">Whether the value is optional.</param>
    	///	<param name="line">The line number.</param>
        template<typename Encoding, typename Allocator> 
        void JsonGetDoubleTest(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, double expectedVal, bool expectedToExist = true, int line = 0, bool testEqual = true)
    	{
    		rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
    		if (memberIt == value.MemberEnd())
    		{
    			if (expectedToExist)
    				FAIL() << AssertMsg::Format(line, L"Double", L"%S is missing", pName);
    		}
    		else if (!expectedToExist)
    		{
    			FAIL() << AssertMsg::Format(line, L"Double", L"%S exists", pName);
    		}
    		else if (!memberIt->value.IsDouble())
    		{
    			FAIL() << AssertMsg::Format(line, L"Double", L"%S is not a Double", pName);
    		}
    		else if (testEqual)
    		{
    			double actualVal = memberIt->value.GetDouble();
    			ASSERT_EQ(expectedVal, actualVal) << AssertMsg::Format(line, L"Double", L"%S", pName);
    		}
    	}

    	///	<summary>
    	/// Tests a json document for a currency value.
    	///	</summary>
    	///	<param name="value">A rapidjson::GenericValue.</param>
    	///	<param name="pName">The name of the value.</param>
    	///	<param name="pExpectedVal">The expected value. If nullptr, the object is expected to not be present.</param>
    	///	<param name="expectedToExist">Whether the value is optional.</param>
    	///	<param name="line">The line number.</param>
    	///	<param name="testEqual">true if the values should be tested for equality, false to not test the values for equality.</param>
        template<typename Encoding, typename Allocator> 
        void JsonGetCurrencyTest(const rapidjson::GenericValue<Encoding, Allocator>& value,
    		const char* pName,
    		const int64_t *pExpectedVal,
    		bool expectedToExist = true,
    		int line = 0,
    		bool testEqual = true)
    	{
    		if (nullptr == pExpectedVal)
    		{
    			expectedToExist = false;
    		}

    		rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
    		if (memberIt == value.MemberEnd())
    		{
    			if (expectedToExist)
    				FAIL() << AssertMsg::Format(line, L"Currency", L"%S is missing", pName);
    		}
    		else if (!expectedToExist)
    		{
    			FAIL() << AssertMsg::Format(line, L"Currency", L"%S exists", pName);
    		}
    		else if (!memberIt->value.IsDouble())
    		{
    			FAIL() << AssertMsg::Format(line, L"Currency", L"%S is not a Double", pName);
    		}
    		else if (testEqual)
    		{
    			int64_t actualVal = Currency::ToInt64(memberIt->value.GetDouble());
    			ASSERT_EQ((uint64_t)*pExpectedVal, (uint64_t) actualVal) << AssertMsg::Format(line, L"Currency", L"%S", pName);
    		}
    	}

    	///	<summary>
    	///	<summary>
    	/// Tests a json document for a currency value.
    	///	</summary>
    	///	<param name="value">A rapidjson::GenericValue.</param>
    	///	<param name="pName">The name of the value.</param>
    	///	<param name="expectedVal">The expected value.</param>
    	///	<param name="expectedToExist">Whether the value is optional.</param>
    	///	<param name="line">The line number.</param>
    	///	<param name="testEqual">true if the values should be tested for equality, false to not test the values for equality.</param>
        template<typename Encoding, typename Allocator> 
        void JsonGetCurrencyTest(const rapidjson::GenericValue<Encoding, Allocator> &value,
    		const char *pName,
    		int64_t expectedVal,
    		bool expectedToExist = true,
    		int line = 0,
    		bool testEqual = true)
    	{
    		JsonGetCurrencyTest(value, pName, &expectedVal, expectedToExist, line, testEqual);
    	}

    	///	<summary>
    	/// Tests a json document for an bool value.
    	///	</summary>
    	///	<param name="value">A rapidjson::GenericValue.</param>
    	///	<param name="pName">The name of the value.</param>
    	///	<param name="pExpectedVal">The expected value.</param>
    	///	<param name="expectedToExist">Whether the value is optional.</param>
        /// <param name="line">The line number.</param>
        template<typename Encoding, typename Allocator> 
    	void JsonGetBoolTest(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, bool expectedVal, bool expectedToExist = true, int line = 0, bool testEqual = true)
    	{
    		rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
    		if (memberIt == value.MemberEnd())
    		{
    			if (expectedToExist)
    				FAIL() << AssertMsg::Format(line, L"Bool", L"%S is missing", pName);
    		}
    		else if (!expectedToExist)
    		{
    			FAIL() << AssertMsg::Format(line, L"Bool", L"%S exists", pName);
    		}
    		else if (!memberIt->value.IsBool())
    		{
    			FAIL() << AssertMsg::Format(line, L"Bool", L"%S is not a Bool", pName);
    		}
    		else if (testEqual)
    		{
    			bool actualVal = memberIt->value.GetBool();
    			ASSERT_EQ(expectedVal, actualVal) << AssertMsg::Format(line, L"Bool", L"%S", pName);
    		}
    	}

    	///	<summary>
    	/// Test for and returns the array object associated with pName.
    	///	</summary>
    	///	<param name="value">A rapidjson::GenericValue.</param>
    	///	<param name="pName">The name of the array.</param>
    	///	<param name="expectedToExist">Whether the array is optional.</param>
    	///	<param name="line">The line number.</param>
    	///	<returns>The array: it may be invalid.</returns>
        template<typename Encoding, typename Allocator> 
        const rapidjson::Value& JsonGetArrayTest(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, bool expectedToExist = true, int line = 0)
    	{
    		rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
    		if (memberIt == value.MemberEnd())
    		{
    			if (expectedToExist)
    				FAIL() << AssertMsg::Format(line, L"Array", L"%S is missing", pName);
    		}
    		else if (!expectedToExist)
    		{
    			FAIL() << AssertMsg::Format(line, L"Array", L"%S exists", pName);
    		}
    		else if (!memberIt->value.IsArray())
    		{
    			FAIL() << AssertMsg::Format(line, L"Array", L"%S is not an Array", pName);
    		}
    		return memberIt->value;
    	}

    	///	<summary>
    	/// Tests for and returns the object associated with pName.
    	///	</summary>
    	///	<param name="value">A rapidjson::GenericValue.</param>
    	///	<param name="pName">The name of the object.</param>
    	///	<param name="expectedToExist">Whether the object is optional.</param>
    	///	<param name="line">The line number.</param>
    	///	<returns>The array: it may be invalid.</returns>
        template<typename Encoding, typename Allocator> 
        const rapidjson::Value& JsonGetObjectTest(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, bool expectedToExist = true, int line = 0)
    	{
    		rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
    		if (memberIt == value.MemberEnd())
    		{
    			if (expectedToExist)
                    cout << "ghbg";
    				//FAIL()<<AssertMsg::Format(line, L"Array", L"%S is missing", pName);
    		}
    		else if (!expectedToExist)
    		{
    			//FAIL()<<AssertMsg::Format(line, L"Array", L"%S exists", pName);
    		}
    		else if (!memberIt->value.IsObject())
    		{
    			//FAIL()<<AssertMsg::Format(line, L"Object", L"%S is not an Object", pName);
    		}
    		return memberIt->value;
    	}

    	///	<summary>
    	/// Indicates whether there is a value associated with pName.
    	///	</summary>
    	///	<param name="value">A rapidjson::GenericValue.</param>
    	///	<param name="pName">The name of the value.</param>
    	///	<param name="expectedToExist">Whether the value is optional.</param>
    	///	<param name="line">The line number.</param>
        template<typename Encoding, typename Allocator> 
        void JsonExistsTest(const rapidjson::GenericValue<Encoding, Allocator>& value, const char* pName, bool expectedToExist = true, int line = 0)
    	{
    		rapidjson::Value::ConstMemberIterator memberIt = value.FindMember(pName);
    		if (memberIt == value.MemberEnd())
    		{
    			if (expectedToExist)
    				FAIL() << AssertMsg::Format(line, L"Exists", L"%S is missing", pName);
    		}
    		else if (!expectedToExist)
    		{
    			FAIL() << AssertMsg::Format(line, L"Bool", L"%S exists", pName);
    		}
    	}
}
