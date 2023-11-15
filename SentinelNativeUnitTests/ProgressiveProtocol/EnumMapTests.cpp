#if 000//TODO-PORT
#include "stdafx.h"
#include "UnitTestingUtils.h"
#include "EnumMap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(EnumMapTests)
	{
	public:
// #pragma region Str
		TEST_METHOD(EnumMap_Str)
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
			Assert::IsNotNull(pStr, L"zero");
			Assert::AreEqual(pStr, "zero", L"zero");

			pStr = nummap.At(One, nullptr, false);
			Assert::IsNotNull(pStr, L"one");
			Assert::AreEqual(pStr, "one", L"one");

			pStr = nummap.At(Two, nullptr, false);
			Assert::IsNotNull(pStr, L"two");
			Assert::AreEqual(pStr, "two", L"two");

			pStr = nummap.At((Numbers) 3, nullptr, false);
			Assert::IsNull(pStr, L"three");
		}

		TEST_METHOD(EnumMap_Str2)
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
			Assert::IsNull(pStr, L"zero");

			pStr = nummap.At(One, nullptr, false);
			Assert::IsNotNull(pStr, L"one");
			Assert::AreEqual(pStr, "one", L"one");

			pStr = nummap.At(Two, nullptr, false);
			Assert::IsNotNull(pStr, L"two");
			Assert::AreEqual(pStr, "two", L"two");

			pStr = nummap.At((Numbers) 3, nullptr, false);
			Assert::IsNotNull(pStr, L"three");
			Assert::AreEqual(pStr, "three", L"three");
		}

		TEST_METHOD(EnumMap_Str_Default)
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
			Assert::IsNotNull(pStr, L"ten");
			Assert::AreEqual(pStr, "ten", L"ten");
		}

		TEST_METHOD(EnumMap_Str_Exception)
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
				Assert::Fail(L"Expected exception");
			}
			catch (EnumMap<Numbers>::Exception& e)
			{
				Assert::AreEqual((intptr_t) 10, e.code());
			}
		}

		TEST_METHOD(EnumMap_EnumIndex)
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
				Assert::IsNotNull(pStr, L"zero");
				Assert::AreEqual(pStr, "zero", L"zero");
			}
			catch (EnumMap<Numbers>::Exception&)
			{
				Assert::Fail(L"Did not expected exception");
			}
		}

		TEST_METHOD(EnumMap_EnumIndex_Exception)
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
				Assert::Fail(L"Expected exception");
			}
			catch (EnumMap<Numbers>::Exception& e)
			{
				Assert::AreEqual((intptr_t) 10, e.code());
			}
		}
// #pragma endregion Str

// #pragma region Enum
		TEST_METHOD(EnumMap_Enum)
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
			Assert::AreEqual((int) val, (int) Zero, L"zero");

			val = nummap.At("one", (Numbers) -1, false);
			Assert::AreEqual((int) val, (int) One, L"one");

			val = nummap.At("two", (Numbers) -1, false);
			Assert::AreEqual((int) val, (int) Two, L"two");

			val = nummap.At("three", (Numbers) -1, false);
			Assert::AreEqual((int) val, -1, L"three");
		}

		TEST_METHOD(EnumMap_Enum2)
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
			Assert::AreEqual((int) val, -1, L"zero");

			val = nummap.At("one", (Numbers) -1, false);
			Assert::AreEqual((int) val, (int) One, L"one");

			val = nummap.At("two", (Numbers) -1, false);
			Assert::AreEqual((int) val, (int) Two, L"two");

			val = nummap.At("three", (Numbers) -1, false);
			Assert::AreEqual((int) val, (int) Three, L"three");
		}

		TEST_METHOD(EnumMap_Enum_Default)
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
			Assert::AreEqual((int) val, 5, L"five");
		}

		TEST_METHOD(EnumMap_Enum_Exception)
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
				Assert::Fail(L"Expected exception");
			}
			catch (EnumMap<Numbers>::Exception& e)
			{
				Assert::AreEqual("ten", e.what());
			}
		}

		TEST_METHOD(EnumMap_StrIndex)
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
				Assert::AreEqual((int) val, 10, L"ten");
			}
			catch (EnumMap<Numbers>::Exception&)
			{
				Assert::Fail(L"Did not expected exception");
			}
		}

		TEST_METHOD(EnumMap_StrIndex_Exception)
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
				Assert::Fail(L"Expected exception");
			}
			catch (EnumMap<Numbers>::Exception& e)
			{
				Assert::AreEqual("ten", e.what());
			}
		}
// #pragma endregion Enum

// #pragma region SparseGetStr
		TEST_METHOD(EnumSparseMap_At)
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
			Assert::IsNull(pStr, L"zero");

			pStr = nummap.At(Two, nullptr, false);
			Assert::IsNotNull(pStr, L"two");
			Assert::AreEqual(pStr, "two", L"two");

			pStr = nummap.At(Four, nullptr, false);
			Assert::IsNotNull(pStr, L"four");
			Assert::AreEqual(pStr, "four", L"four");

			pStr = nummap.At(Six, nullptr, false);
			Assert::IsNotNull(pStr, L"six");
			Assert::AreEqual(pStr, "six", L"six");

			Numbers num = nummap.At(nullptr, (Numbers) -1, false);
			Assert::AreEqual(-1, (int) num, L"-1");

			num = nummap.At("two", (Numbers) -1, false);
			Assert::AreEqual(2, (int) Two, L"2");

			num = nummap.At("four", (Numbers) -1, false);
			Assert::AreEqual(4, (int) Four, L"4");

			num = nummap.At("six", (Numbers) -1, false);
			Assert::AreEqual(6, (int) Six, L"6");
		}

		TEST_METHOD(EnumSparseMap_GetStr_Default)
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
			Assert::IsNotNull(pStr, L"six");
			Assert::AreEqual(pStr, "six", L"six");
		}

		TEST_METHOD(EnumSparseMap_GetStr_Exception)
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
				Assert::Fail(L"Expected exception");
			}
			catch (EnumMap<Numbers>::Exception& e)
			{
				Assert::AreEqual((intptr_t) 6, e.code());
			}
		}

		TEST_METHOD(EnumSparseMap_EnumIndex)
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
				Assert::IsNotNull(pStr, L"ten");
				Assert::AreEqual(pStr, "ten", L"ten");
			}
			catch (EnumMap<Numbers>::Exception&)
			{
				Assert::Fail(L"Did not expected exception");
			}
		}

		TEST_METHOD(EnumSparseMap_EnumIndex_Exception)
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
				Assert::Fail(L"Expected exception");
			}
			catch (EnumMap<Numbers>::Exception& e)
			{
				Assert::AreEqual((intptr_t) 5, e.code());
			}
		}
// #pragma endregion SparseGetStr
	};
}
#endif //TODO-PORT