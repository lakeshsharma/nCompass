#include "stdafx.h"
#include "TestUtil.h"
#include "Util/Path.h"
#include "gtest/gtest.h"

using namespace std;
using namespace TestUtil;

namespace SentinelNativeUnitTests
{
   class Util_PathTests : public ::testing::Test
	{
	};

	TEST_F(Util_PathTests, Util_Path_IsSlash)
	{
		struct
		{
			int line;
			wchar_t ch;
			bool result;
		}
		static data[] =
		{
			{ __LINE__, L'\\', true  },
			{ __LINE__, L'/', true  },
			{ __LINE__, L':', false },
			{ __LINE__, L'.', false },
			{ __LINE__, L'\0', false }
		};

		for (size_t i = 0; i < _countof(data); i++)
		{
			bool result = Path::IsSlash(data[i].ch);
			ASSERT_EQ(data[i].result, result);
		}
	}

	TEST_F(Util_PathTests, Util_Path_IsDelimeter)
	{
		struct
		{
			int line;
			wchar_t ch;
			bool result;
		}
		static data[] =
		{
			{ __LINE__, L'\\', true  },
			{ __LINE__, L'/', true  },
			{ __LINE__, L':', true  },
			{ __LINE__, L'.', false },
			{ __LINE__, L'\0', false }
		};

		for (size_t i = 0; i < _countof(data); i++)
		{
			bool result = Path::IsDelimeter(data[i].ch);
			ASSERT_EQ(data[i].result, result);
		}
	}

	TEST_F(Util_PathTests, Util_Path_AddSlashstring)
	{
		struct
		{
			int line;
			LPCTSTR pPath;
			string pExpected;
		}
		static data[] =
		{
			{ __LINE__,"C:\\Temp",  "C:\\Temp/" },
			{ __LINE__,"C:\\Temp\\","C:\\Temp\\" },
			{ __LINE__,"C:/Temp",   "C:/Temp/"  },
			{ __LINE__,"C:/Temp/",  "C:/Temp/"   },
			{ __LINE__,"C:\\",	  "C:\\"       },
			{ __LINE__,"C:",		  "C:/"       },
			{ __LINE__,"",          ""           },
		};

		for (size_t i = 0; i < _countof(data); i++)
		{
			string result = Path::AddSlash(data[i].pPath);

			ASSERT_EQ(data[i].pExpected, result);
		}
	}

	TEST_F(Util_PathTests, Util_Path_RemoveSlashstring)
	{
		struct
		{
			int line;
			LPCTSTR pPath;
			string pExpected;
		}
		static data[] =
		{
			{ __LINE__,"C:\\Temp",  "C:\\Temp" },
			{ __LINE__,"C:\\Temp\\","C:\\Temp" },
			{ __LINE__,"C:/Temp",   "C:/Temp"  },
			{ __LINE__,"C:/Temp/",  "C:/Temp"  },
			{ __LINE__,"C:\\",	  "C:"       },
			{ __LINE__,"C:",		  "C:"       },
			{ __LINE__,"",          ""         },
		};

		for (size_t i = 0; i < _countof(data); i++)
		{
			string result = Path::RemoveSlash(data[i].pPath);

			ASSERT_EQ(data[i].pExpected, result);
		}
	}

	TEST_F(Util_PathTests, Util_Path_AppendFile)
	{
		struct
		{
			int line;
			const LPCTSTR pDir;
			const LPCTSTR pFile;
			const string pExpected;
		}
		static data[] =
		{
			{ __LINE__,"C:/Dir","File","C:/Dir/File" },
			{ __LINE__,"C:/Dir/","File","C:/Dir/File"  },
			{ __LINE__,"C:","File","C:/File"     },
			{ __LINE__,"","File",""             },
		};

		for (size_t i = 0; i < _countof(data); i++)
		{
			string result = Path::AppendFile(data[i].pDir, data[i].pFile);
			ASSERT_EQ(data[i].pExpected, result);
		}
	}
}; 
