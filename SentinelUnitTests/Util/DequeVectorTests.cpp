#include "stdafx.h"
#include "gtest/gtest.h"
#include "Util/DequeVector.h"
//#include "TestUtil/AssertMsg.h"

using namespace std;

namespace SentinelUnitTests
{
	class Util_deque_vector_Tests : public ::testing::Test
	{
	};
		TEST_F(Util_deque_vector_Tests,  deque_vector_ctor)
		{
			deque_vector<int, 3> v;
			ASSERT_EQ((size_t) 3,  v.size()) << "size()";
		}

		TEST_F(Util_deque_vector_Tests,  deque_vector_ctor2)
		{
			deque_vector<int, 3> v(1);
			ASSERT_EQ((size_t) 3,  v.size()) << "size()";

			for (size_t i = 0; i < v.size(); i++)
			{
				ASSERT_EQ(1,  v[i]);
			}
		}

		TEST_F(Util_deque_vector_Tests,  deque_vector_cctor)
		{
			deque_vector<int, 3> v_org;
			v_org[0] = 1;
			v_org[1] = 2;
			v_org[2] = 3;

			deque_vector<int, 3> v(v_org);

			ASSERT_EQ((size_t) 3,  v.size()) << "size()";

			ASSERT_EQ(v_org[0],  v[0]) << "[0]";
			ASSERT_EQ(v_org[1],  v[1]) << "[1]";
			ASSERT_EQ(v_org[2],  v[2]) << "[2]";
		}

		TEST_F(Util_deque_vector_Tests,  deque_vector_pop_front)
		{
			struct
			{
				int line;
				int val;
				int expected;
			}
			data[] =
			{
				{ __LINE__, 1, 0 },
				{ __LINE__, 2, 1 },
				{ __LINE__, 3, 2 },
				{ __LINE__, 4, 3 },
				{ __LINE__, 5, 4 },
				{ __LINE__, 6, 5 },
				{ __LINE__, 7, 6 },
				{ __LINE__, 8, 7 }
			};

			deque_vector<int, 3> v;

			for (size_t i = 0; i < _countof(data); i++)
			{
				v.push_back(data[i].val);

				int val = v.pop_front();

				ASSERT_EQ(data[i].expected, val);
			}
		}

		TEST_F(Util_deque_vector_Tests,  deque_vector_push_front)
		{
			struct
			{
				int line;
				int val;
				int expected[3];
			}
			data[] =
			{
				{ __LINE__, 1, { 1, 0, 0 }},
				{ __LINE__, 2, { 2, 1, 0 }},
				{ __LINE__, 3, { 3, 2, 1 }},
				{ __LINE__, 4, { 4, 3, 2 }},
				{ __LINE__, 5, { 5, 4, 3 }},
				{ __LINE__, 6, { 6, 5, 4 }}
			};

			deque_vector<int, 3> v;

			for (size_t i = 0; i < _countof(data); i++)
			{
				v.push_front(data[i].val);

				ASSERT_EQ((size_t) 3,  v.size());

				ASSERT_EQ(data[i].expected[0],  v[0]);
				ASSERT_EQ(data[i].expected[1],  v[1]);
				ASSERT_EQ(data[i].expected[2],  v[2]);
			}
		}

		TEST_F(Util_deque_vector_Tests,  deque_vector_pop_back)
		{
			struct
			{
				int line;
				int val;
				int expected;
			}
			data[] =
			{
				{ __LINE__, 1, 0 },
				{ __LINE__, 2, 1 },
				{ __LINE__, 3, 2 },
				{ __LINE__, 4, 3 },
				{ __LINE__, 5, 4 },
				{ __LINE__, 6, 5 },
				{ __LINE__, 7, 6 },
				{ __LINE__, 8, 7 }
			};

			deque_vector<int, 3> v;

			for (size_t i = 0; i < _countof(data); i++)
			{
				v.push_front(data[i].val);

				int val = v.pop_back();

				ASSERT_EQ(data[i].expected, val);
			}
		}

		TEST_F(Util_deque_vector_Tests,  deque_vector_push_back)
		{
			struct
			{
				int line;
				int val;
				int expected[3];
			}
			data[] =
			{
				{ __LINE__, 1, { 0, 0, 1 } },
				{ __LINE__, 2, { 0, 1, 2 } },
				{ __LINE__, 3, { 1, 2, 3 } },
				{ __LINE__, 4, { 2, 3, 4 } },
				{ __LINE__, 5, { 3, 4, 5 } },
				{ __LINE__, 6, { 4, 5, 6 } }
			};

			deque_vector<int, 3> v;

			for (size_t i = 0; i < _countof(data); i++)
			{
				v.push_back(data[i].val);

				ASSERT_EQ((size_t) 3,  v.size());

				ASSERT_EQ(data[i].expected[0],  v[0]);
				ASSERT_EQ(data[i].expected[1],  v[1]);
				ASSERT_EQ(data[i].expected[2],  v[2]);
			}
		}

		TEST_F(Util_deque_vector_Tests,  deque_vector_pop_front2)
		{
			struct
			{
				int line;
				int val;
				int expected;
			}
			data[] =
			{
				{ __LINE__, 1, 0 },
				{ __LINE__, 2, 0 },
				{ __LINE__, 3, 0 },
				{ __LINE__, 4, 1 },
				{ __LINE__, 5, 2 },
				{ __LINE__, 6, 3 },
				{ __LINE__, 7, 4 },
				{ __LINE__, 8, 5 }
			};

			deque_vector<int, 3> v;

			for (size_t i = 0; i < _countof(data); i++)
			{
				int val = v.pop_front(data[i].val);
				ASSERT_EQ(data[i].expected, val);
			}
		}

		TEST_F(Util_deque_vector_Tests,  deque_vector_pop_back2)
		{
			struct
			{
				int line;
				int val;
				int expected;
			}
			data[] =
			{
				{ __LINE__, 1, 0 },
				{ __LINE__, 2, 0 },
				{ __LINE__, 3, 0 },
				{ __LINE__, 4, 1 },
				{ __LINE__, 5, 2 },
				{ __LINE__, 6, 3 },
				{ __LINE__, 7, 4 },
				{ __LINE__, 8, 5 }
			};

			deque_vector<int, 3> v;

			for (size_t i = 0; i < _countof(data); i++)
			{
				int val = v.pop_back(data[i].val);
				ASSERT_EQ(data[i].expected, val);
			}
		}
}

