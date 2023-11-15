#include "stdafx.h"
#include "UnitTestingUtils.h"
#include "OptionalVal.h"


#include "gtest/gtest.h"

using namespace std;

namespace SentinelUnitTests
{
		TEST(OptionalValTests, OptionalVal_ConstructorDefault_Int)
		{
			OptionalVal<int> optionalInt;
			ASSERT_FALSE(optionalInt.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorValue_Int)
		{
			OptionalVal<int> optionalInt(5);
			ASSERT_TRUE(optionalInt.isValid) << "IsValid";
			ASSERT_EQ(5, optionalInt.val) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorCopyInvalid_Int)
		{
			OptionalVal<int> other;
			OptionalVal<int> optionalInt(other);
			ASSERT_FALSE(optionalInt.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorCopyValid_Int)
		{
			OptionalVal<int> other(5);
			OptionalVal<int> optionalInt(other);
			ASSERT_TRUE(optionalInt.isValid) << "IsValid";
			ASSERT_EQ(5, optionalInt.val) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorMoveInvalid_Int)
		{
			OptionalVal<int> other;
			OptionalVal<int> optionalInt(move(other));
			ASSERT_FALSE(optionalInt.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorMoveValid_Int)
		{
			OptionalVal<int> other(5);
			OptionalVal<int> optionalInt(move(other));
			ASSERT_TRUE(optionalInt.isValid) << "IsValid";
			ASSERT_EQ(5, optionalInt.val) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_AssignmentInvalid_Int)
		{
			OptionalVal<int> optionalInt;

			OptionalVal<int> other;
			optionalInt = other;
			ASSERT_FALSE(optionalInt.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_AssignmentValid_Int)
		{
			OptionalVal<int> optionalInt;

			OptionalVal<int> other(5);
			optionalInt = other;
			ASSERT_TRUE(optionalInt.isValid) << "IsValid";
			ASSERT_EQ(5, optionalInt.val) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_MoveAssignmentInvalid_Int)
		{
			OptionalVal<int> optionalInt;

			OptionalVal<int> other;
			optionalInt = move(other);
			ASSERT_FALSE(optionalInt.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_MoveAssignmentValid_Int)
		{
			OptionalVal<int> optionalInt;

			OptionalVal<int> other(5);
			optionalInt = move(other);
			ASSERT_TRUE(optionalInt.isValid) << "IsValid";
			ASSERT_EQ(5, optionalInt.val) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorDefault_cstr)
		{
			OptionalVal<const char*> optionalCstr;
			ASSERT_FALSE(optionalCstr.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorValue_cstr)
		{
			OptionalVal<const char*> optionalCstr("5");
			ASSERT_TRUE(optionalCstr.isValid) << "IsValid";
			ASSERT_EQ("5", optionalCstr.val) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorCopyInvalid_cstr)
		{
			OptionalVal<const char*> other;
			OptionalVal<const char*> optionalCstr(other);
			ASSERT_FALSE(optionalCstr.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorCopyValid_cstr)
		{
			OptionalVal<const char*> other("5");
			OptionalVal<const char*> optionalCstr(other);
			ASSERT_TRUE(optionalCstr.isValid) << "IsValid";
			ASSERT_STREQ("5", optionalCstr.val) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorMoveInvalid_cstr)
		{
			OptionalVal<const char*> other;
			OptionalVal<const char*> optionalCstr(move(other));
			ASSERT_FALSE(optionalCstr.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorMoveValid_cstr)
		{
			OptionalVal<const char*> other("5");
			OptionalVal<const char*> optionalCstr(move(other));
			ASSERT_TRUE(optionalCstr.isValid) << "IsValid";
			ASSERT_STREQ("5", optionalCstr.val) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_AssignmentInvalid_cstr)
		{
			OptionalVal<const char*> optionalCstr;

			OptionalVal<const char*> other;
			optionalCstr = other;
			ASSERT_FALSE(optionalCstr.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_AssignmentValid_cstr)
		{
			OptionalVal<const char*> optionalCstr;

			OptionalVal<const char*> other("5");
			optionalCstr = other;
			ASSERT_TRUE(optionalCstr.isValid) << "IsValid";
			ASSERT_STREQ("5", optionalCstr.val) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_MoveAssignmentInvalid_cstr)
		{
			OptionalVal<const char*> optionalCstr;

			OptionalVal<const char*> other;
			optionalCstr = move(other);
			ASSERT_FALSE(optionalCstr.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_MoveAssignmentValid_cstr)
		{
			OptionalVal<const char*> optionalCstr;

			OptionalVal<const char*> other("5");
			optionalCstr = move(other);
			ASSERT_TRUE(optionalCstr.isValid) << "IsValid";
			ASSERT_STREQ("5", optionalCstr.val) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorDefault_string)
		{
			OptionalVal<string> optionalStr;
			ASSERT_FALSE(optionalStr.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorValue_string)
		{
			OptionalVal<string> optionalStr("5");
			ASSERT_TRUE(optionalStr.isValid) << "IsValid";
			ASSERT_STREQ("5", optionalStr.val.c_str()) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorCopyInvalid_string)
		{
			OptionalVal<string> other;
			OptionalVal<string> optionalStr(other);
			ASSERT_FALSE(optionalStr.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorCopyValid_string)
		{
			OptionalVal<string> other("5");
			OptionalVal<string> optionalStr(other);
			ASSERT_TRUE(optionalStr.isValid) << "IsValid";
			ASSERT_STREQ("5", optionalStr.val.c_str()) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorMoveInvalid_string)
		{
			OptionalVal<string> other;
			OptionalVal<string> optionalStr(move(other));
			ASSERT_FALSE(optionalStr.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_ConstructorMoveValid_string)
		{
			OptionalVal<string> other("5");
			OptionalVal<string> optionalStr(move(other));
			ASSERT_TRUE(optionalStr.isValid) << "IsValid";
			ASSERT_STREQ("5", optionalStr.val.c_str()) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_AssignmentInvalid_string)
		{
			OptionalVal<string> optionalStr;

			OptionalVal<string> other;
			optionalStr = other;
			ASSERT_FALSE(optionalStr.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_AssignmentValid_string)
		{
			OptionalVal<string> optionalStr;

			OptionalVal<string> other("5");
			optionalStr = other;
			ASSERT_TRUE(optionalStr.isValid) << "IsValid";
			ASSERT_STREQ("5", optionalStr.val.c_str()) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_MoveAssignmentInvalid_string)
		{
			OptionalVal<string> optionalStr;

			OptionalVal<string> other;
			optionalStr = move(other);
			ASSERT_FALSE(optionalStr.isValid) << "IsValid";
		}

		TEST(OptionalValTests, OptionalVal_MoveAssignmentValid_string)
		{
			OptionalVal<string> optionalStr;

			OptionalVal<string> other("5");
			optionalStr = move(other);
			ASSERT_TRUE(optionalStr.isValid) << "IsValid";
			ASSERT_STREQ("5", optionalStr.val.c_str()) << "Val";
		}

		TEST(OptionalValTests, OptionalVal_IsValidAndEqual_invalid)
		{
			OptionalVal<int> optionalInt;

			int val = 5;
			bool result = optionalInt.IsValidAndEqual(val);
			ASSERT_FALSE(result);
		}

		TEST(OptionalValTests, OptionalVal_IsValidAndEqual_notequal)
		{
			OptionalVal<int> optionalInt(6);

			int val = 5;
			bool result = optionalInt.IsValidAndEqual(val);
			ASSERT_FALSE(result);
		}
		TEST(OptionalValTests, OptionalVal_IsValidAndEqual_equal)
		{
			OptionalVal<int> optionalInt(5);

			int val = 5;
			bool result = optionalInt.IsValidAndEqual(val);
			ASSERT_TRUE(result);
		}

		TEST(OptionalValTests, OptionalVal_IsInvalidOrEqual_invalid)
		{
			OptionalVal<int> optionalInt;

			int val = 5;
			bool result = optionalInt.IsInvalidOrEqual(val);
			ASSERT_TRUE(result);
		}

		TEST(OptionalValTests, OptionalVal_IsInvalidOrEqual_notequal)
		{
			OptionalVal<int> optionalInt(6);

			int val = 5;
			bool result = optionalInt.IsInvalidOrEqual(val);
			ASSERT_FALSE(result);
		}

		TEST(OptionalValTests, OptionalVal_IsInvalidOrEqual_equal)
		{
			OptionalVal<int> optionalInt(5);

			int val = 5;
			bool result = optionalInt.IsInvalidOrEqual(val);
			ASSERT_TRUE(result);
		}
}

