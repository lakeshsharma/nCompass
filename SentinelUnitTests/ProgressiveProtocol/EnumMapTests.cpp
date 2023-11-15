#include "stdafx.h"
#include "UnitTestingUtils.h"
#include "EnumMap.h"
#include "gtest/gtest.h"

using namespace std;

namespace SentinelUnitTests
{
	class EnumMapTests : public ::testing::Test
	{
	public:
	};

	TEST_F(EnumMapTests, EnumMap_Str)
	{
		enum Numbers
		{
			Zero,
			One,
			Two
		};
		
		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Zero,	"zero" },
			{ One,	"one"  },
			{ Two,	"two"  }
		};
		
		EnumMap<Numbers> nummap(numbers);

		const char* pStr = nummap.At(Zero, nullptr, false);
		ASSERT_TRUE(NULL != pStr) << "zero";
		ASSERT_TRUE(strcmp(pStr,"zero") == 0) << "zero";

		pStr = nummap.At(One, nullptr, false);
		ASSERT_TRUE(NULL != pStr) << "one";
		ASSERT_TRUE(strcmp(pStr,"one") == 0) << "one";

		pStr = nummap.At(Two, nullptr, false);
		ASSERT_TRUE(NULL != pStr) << "two";
		ASSERT_TRUE(strcmp(pStr,"two") == 0) << "two";

		pStr = nummap.At((Numbers) 3, nullptr, false);
		ASSERT_TRUE(NULL == pStr) << "three";
	}

	TEST_F(EnumMapTests, EnumMap_Str2)
	{
		enum Numbers
		{
			One = 1,
			Two,
			Three
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ One,	"one"	},
			{ Two,	"two"	},
			{ Three,"three" }
		};

		EnumMap<Numbers> nummap(numbers, One);

		const char* pStr = nummap.At((Numbers) 0, nullptr, false);
		ASSERT_TRUE(NULL == pStr) << "zero";

		pStr = nummap.At(One, nullptr, false);
		ASSERT_TRUE(NULL != pStr) << "one";
		ASSERT_TRUE(strcmp(pStr,"one") == 0) << "one";

		pStr = nummap.At(Two, nullptr, false);
		ASSERT_TRUE(NULL != pStr) << "two";
		ASSERT_TRUE(strcmp(pStr,"two") == 0) << "two";

		pStr = nummap.At((Numbers) 3, nullptr, false);
		ASSERT_TRUE(NULL != pStr) << "three";
		ASSERT_TRUE(strcmp(pStr,"three") == 0) << "three";
	}

	TEST_F(EnumMapTests, EnumMap_Str_Default)
	{
		enum Numbers
		{
			Zero,
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Zero,	"zero" },
		};

		EnumMap<Numbers> nummap(numbers);

		const char* pStr = nummap.At((Numbers) 10, "ten", false);
		ASSERT_TRUE(NULL != pStr) << "ten";
		ASSERT_TRUE(strcmp(pStr,"ten") == 0) << "ten";
	}

	TEST_F(EnumMapTests, EnumMap_Str_Exception)
	{
		enum Numbers
		{
			Zero,
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Zero,	"zero" },
		};

		EnumMap<Numbers> nummap(numbers);

		try
		{
			const char* pStr = nummap.At((Numbers) 10, "ten", true);
			pStr = pStr;
			FAIL()  << "Expected exception";
		}
		catch (EnumMap<Numbers>::Exception& e)
		{
			ASSERT_TRUE((intptr_t) 10 == e.code());;
		}
	}

	TEST_F(EnumMapTests, EnumMap_EnumIndex)
	{
		enum Numbers
		{
			Zero,
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Zero,	"zero" },
		};

		EnumMap<Numbers> nummap(numbers);

		try
		{
			const char* pStr = nummap[Zero];
			ASSERT_TRUE(NULL != pStr) << "zero";
			ASSERT_TRUE(strcmp(pStr,"zero") == 0) << "zero";
		}
		catch (EnumMap<Numbers>::Exception&)
		{
			FAIL()  << "Did not expected exception";
		}
	}

	TEST_F(EnumMapTests, EnumMap_EnumIndex_Exception)
	{
		enum Numbers
		{
			Zero,
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Zero,	"zero" },
		};

		EnumMap<Numbers> nummap(numbers);

		try
		{
			const char* pStr = nummap[(Numbers) 10];
			pStr = pStr;
			FAIL()  << "Expected exception";
		}
		catch (EnumMap<Numbers>::Exception& e)
		{
			ASSERT_TRUE((intptr_t) 10 == e.code());;
		}
	}

	TEST_F(EnumMapTests, EnumMap_Enum)
	{
		enum Numbers
		{
			Zero,
			One,
			Two
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Zero,	"zero" },
			{ One,	"one"  },
			{ Two,	"two"  }
		};

		EnumMap<Numbers> nummap(numbers);

		Numbers val = nummap.At("zero", (Numbers) -1, false);
		ASSERT_TRUE((int) val == (int) Zero) << "zero";

		val = nummap.At("one", (Numbers) -1, false);
		ASSERT_TRUE((int) val == (int) One) << "one";

		val = nummap.At("two", (Numbers) -1, false);
		ASSERT_TRUE((int) val == (int) Two) << "two";

		val = nummap.At("three", (Numbers) -1, false);
		ASSERT_TRUE((int) val == -1) << "three";
	}

	TEST_F(EnumMapTests, EnumMap_Enum2)
	{
		enum Numbers
		{
			One = 1,
			Two,
			Three
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ One,	 "one"   },
			{ Two,	 "two"   },
			{ Three, "three" }
		};

		EnumMap<Numbers> nummap(numbers);

		Numbers val = nummap.At("zero", (Numbers) -1, false);
		ASSERT_TRUE((int) val == -1) << "zero";

		val = nummap.At("one", (Numbers) -1, false);
		ASSERT_TRUE((int) val == (int) One) << "one";

		val = nummap.At("two", (Numbers) -1, false);
		ASSERT_TRUE((int) val == (int) Two) << "two";

		val = nummap.At("three", (Numbers) -1, false);
		ASSERT_TRUE((int) val == (int) Three) << "three";
	}

	TEST_F(EnumMapTests, EnumMap_Enum_Default)
	{
		enum Numbers
		{
			Ten = 10,
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Ten,	"ten" },
		};

		EnumMap<Numbers> nummap(numbers, Ten);

		Numbers val = nummap.At("five", (Numbers) 5, false);
		ASSERT_TRUE((int) val == 5) << "five";
	}

	TEST_F(EnumMapTests, EnumMap_Enum_Exception)
	{
		enum Numbers
		{
			Zero,
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Zero,	"zero" },
		};

		EnumMap<Numbers> nummap(numbers);

		try
		{
			Numbers val = nummap.At("ten", (Numbers) 10, true);
			val = val;
			FAIL()  << "Expected exception";
		}
		catch (EnumMap<Numbers>::Exception& e)
		{
			ASSERT_TRUE(strcmp("ten",e.what()) == 0);
		}
	}

	TEST_F(EnumMapTests, EnumMap_StrIndex)
	{
		enum Numbers
		{
			Ten = 10,
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Ten,	"ten" },
		};

		EnumMap<Numbers> nummap(numbers, Ten);

		try
		{
			Numbers val = nummap["ten"];
			ASSERT_TRUE((int) val == 10) << "ten";
		}
		catch (EnumMap<Numbers>::Exception&)
		{
			FAIL()  << "Did not expected exception";
		}
	}

	TEST_F(EnumMapTests, EnumMap_StrIndex_Exception)
	{
		enum Numbers
		{
			Zero,
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Zero,	"zero" },
		};

		EnumMap<Numbers> nummap(numbers);

		try
		{
			Numbers val = nummap["ten"];
			val = val;
			FAIL()  << "Expected exception";
		}
		catch (EnumMap<Numbers>::Exception& e)
		{
			ASSERT_TRUE(strcmp("ten", e.what()) == 0);
		}
	}

	TEST_F(EnumMapTests, EnumSparseMap_At)
	{
		enum Numbers
		{
			Two = 2,
			Four = 4,
			Six = 6
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Two,	"two"  },
			{ Four,	"four" },
			{ Six,	"six"  }
		};

		EnumSparseMap<Numbers> nummap(numbers);

		const char* pStr = nummap.At((Numbers) 0, nullptr, false);
		ASSERT_TRUE(NULL == pStr) << "zero";

		pStr = nummap.At(Two, nullptr, false);
		ASSERT_TRUE(NULL != pStr) << "two";
		ASSERT_TRUE(strcmp(pStr,"two") == 0) << "two";

		pStr = nummap.At(Four, nullptr, false);
		ASSERT_TRUE(NULL != pStr) << "four";
		ASSERT_TRUE(strcmp(pStr,"four") == 0) << "four";

		pStr = nummap.At(Six, nullptr, false);
		ASSERT_TRUE(NULL != pStr) << "six";
		ASSERT_TRUE(strcmp(pStr,"six") == 0) << "six";

		Numbers num = nummap.At(nullptr, (Numbers) -1, false);
		ASSERT_TRUE(-1 == (int) num) << "-1";

		num = nummap.At("two", (Numbers) -1, false);
		ASSERT_TRUE(2 == (int) Two) << "2";

		num = nummap.At("four", (Numbers) -1, false);
		ASSERT_TRUE(4 == (int) Four) << "4";

		num = nummap.At("six", (Numbers) -1, false);
		ASSERT_TRUE(6 == (int) Six) << "6";
	}

	TEST_F(EnumMapTests, EnumSparseMap_GetStr_Default)
	{
		enum Numbers
		{
			Ten,
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Ten,	"ten" },
		};

		EnumSparseMap<Numbers> nummap(numbers);

		const char* pStr = nummap.At((Numbers) 6, "six", false);
		ASSERT_TRUE(NULL != pStr) << "six";
		ASSERT_TRUE(strcmp(pStr,"six") == 0) << "six";
	}

	TEST_F(EnumMapTests, EnumSparseMap_GetStr_Exception)
	{
		enum Numbers
		{
			Ten,
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Ten,	"ten" },
		};

		EnumSparseMap<Numbers> nummap(numbers);

		try
		{
		const char* pStr = nummap.At((Numbers) 6, "six", true);
			pStr = pStr;
			FAIL()  << "Expected exception";
		}
		catch (EnumMap<Numbers>::Exception& e)
		{
			ASSERT_TRUE((intptr_t) 6 == e.code());;
		}
	}

	TEST_F(EnumMapTests, EnumSparseMap_EnumIndex)
	{
		enum Numbers
		{
			Ten,
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Ten,	"ten" },
		};

		EnumSparseMap<Numbers> nummap(numbers);

		try
		{
			const char* pStr = nummap[Ten];
			ASSERT_TRUE(NULL != pStr) << "ten";
			ASSERT_TRUE(strcmp(pStr,"ten") == 0) << "ten";
		}
		catch (EnumMap<Numbers>::Exception&)
		{
			FAIL() << "Did not expected exception";
		}
	}

	TEST_F(EnumMapTests, EnumSparseMap_EnumIndex_Exception)
	{
		enum Numbers
		{
			Ten,
		};

		static EnumMap<Numbers>::EnumPair numbers[] =
		{
			{ Ten,	"ten" },
		};

		EnumSparseMap<Numbers> nummap(numbers);

		try
		{
			const char* pStr = nummap[(Numbers) 5];
			pStr = pStr;
			FAIL() << "Expected exception";
		}
		catch (EnumMap<Numbers>::Exception& e)
		{
			ASSERT_TRUE((intptr_t) 5 == e.code());
		}
	}
}
