#if 000//TODO-PORT
#include "stdafx.h"
#include <Util\PathStd.h>
#include "TestUtil\AssertMsg.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(Util_PathStdTests)
	{
	public:
		TEST_METHOD(Util_PathStd_IsSlash)
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
				{ __LINE__, L'/' , true  },
				{ __LINE__, L':' , false },
				{ __LINE__, L'.' , false },
				{ __LINE__, L'\0', false }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				bool result = PathStd::IsSlash(data[i].ch);
				Assert::AreEqual(data[i].result, result, AssertMsg::Format(data[i].line, nullptr));
			}
		}

		TEST_METHOD(Util_PathStd_IsDelimeter)
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
				{ __LINE__, L'/' , true  },
				{ __LINE__, L':' , true  },
				{ __LINE__, L'.' , false },
				{ __LINE__, L'\0', false }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				bool result = PathStd::IsDelimeter(data[i].ch);
				Assert::AreEqual(data[i].result, result, AssertMsg::Format(data[i].line, nullptr));
			}
		}

		TEST_METHOD(Util_PathStd_AddSlash)
		{
			struct
			{
				int line;
				const wchar_t* pPath;
				const wchar_t* pExpected;
			}
			static data[] =
			{
				{ __LINE__, L"C:\\Temp",   L"C:\\Temp/"  },
				{ __LINE__, L"C:\\Temp\\", L"C:\\Temp\\" },
				{ __LINE__, L"C:/Temp",    L"C:/Temp/"   },
				{ __LINE__, L"C:/Temp/",   L"C:/Temp/"   },
				{ __LINE__, L"C:/",		   L"C:/"        },
				{ __LINE__, L"C:",		   L"C:/"        },
				{ __LINE__, L"",           L""           },
				{ __LINE__, nullptr,       L""           }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				wstring path(data[i].pPath ? data[i].pPath : L"");
				wstring& result = PathStd::AddSlash(path);

				Assert::AreEqual(data[i].pExpected, path.c_str(), AssertMsg::Format(data[i].line, L"path"));
				Assert::AreSame(path, result, AssertMsg::Format(data[i].line, L"pResult"));
			}
		}

		TEST_METHOD(Util_PathStd_RemoveSlash)
		{
			struct
			{
				int line;
				const wchar_t* pPath;
				const wchar_t* pExpected;
			}
			static data[] =
			{
				{ __LINE__, L"C:\\Temp",   L"C:\\Temp" },
				{ __LINE__, L"C:\\Temp\\", L"C:\\Temp" },
				{ __LINE__, L"C:/Temp",    L"C:/Temp"  },
				{ __LINE__, L"C:/Temp/",   L"C:/Temp"  },
				{ __LINE__, L"C:/",		   L"C:"       },
				{ __LINE__, L"C:",		   L"C:"       },
				{ __LINE__, L"",           L""         },
				{ __LINE__, nullptr,       L""         }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				wstring path(data[i].pPath ? data[i].pPath : L"");
				wstring& result = PathStd::RemoveSlash(path);

				Assert::AreEqual(data[i].pExpected, path.c_str(), AssertMsg::Format(data[i].line, L"path"));
				Assert::AreEqual(path, result, AssertMsg::Format(data[i].line, L"pResult"));
			}
		}

		TEST_METHOD(Util_PathStd_AddSlash_str)
		{
			struct
			{
				int line;
				const wchar_t* pPath;
				const wchar_t* pExpected;
			}
			static data[] =
			{
				{ __LINE__, L"C:\\Temp",   L"C:\\Temp/"  },
				{ __LINE__, L"C:\\Temp\\", L"C:\\Temp\\" },
				{ __LINE__, L"C:/Temp",    L"C:/Temp/"   },
				{ __LINE__, L"C:/Temp/",   L"C:/Temp/"   },
				{ __LINE__, L"C:/",		   L"C:/"        },
				{ __LINE__, L"C:",		   L"C:/"        },
				{ __LINE__, L"",           L""           },
				{ __LINE__, nullptr,       L""           }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				wstring result = PathStd::AddSlash(data[i].pPath, MAX_PATH);
				Assert::AreEqual(data[i].pExpected, result.c_str(), AssertMsg::Format(data[i].line, L"result"));
				Assert::IsTrue((size_t) MAX_PATH <= result.capacity(), AssertMsg::Format(data[i].line, L"reserved"));
			}
		}

		TEST_METHOD(Util_PathStd_RemoveSlash_str)
		{
			struct
			{
				int line;
				const wchar_t* pPath;
				const wchar_t* pExpected;
			}
			static data[] =
			{
				{ __LINE__, L"C:\\Temp",   L"C:\\Temp" },
				{ __LINE__, L"C:\\Temp\\", L"C:\\Temp" },
				{ __LINE__, L"C:/Temp",    L"C:/Temp"  },
				{ __LINE__, L"C:/Temp/",   L"C:/Temp"  },
				{ __LINE__, L"C:/",		   L"C:"       },
				{ __LINE__, L"C:",		   L"C:"       },
				{ __LINE__, L"",           L""         },
				{ __LINE__, nullptr,       L""         }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				wstring result = PathStd::RemoveSlash(data[i].pPath, MAX_PATH);
				Assert::AreEqual(data[i].pExpected, result.c_str(), AssertMsg::Format(data[i].line, L"result"));
				Assert::IsTrue((size_t) MAX_PATH <= result.capacity(), AssertMsg::Format(data[i].line, L"reserved"));
			}
		}

		TEST_METHOD(Util_PathStd_AppendFile)
		{
			struct
			{
				int line;
				const wchar_t* pDir;
				const wchar_t* pFile;
				const wchar_t* pExpected;
			}
			static data[] =
			{
				{ __LINE__, L"C:/Dir"   , L"File" , L"C:/Dir/File" },
				{ __LINE__, L"C:/Dir/"  , L"File" , L"C:/Dir/File" },
				{ __LINE__, L"C:"       , L"File" , L"C:/File"     },
				{ __LINE__, L""         , L"File" , L""            },
				{ __LINE__, nullptr     , L""     , L""            },
				{ __LINE__, nullptr     , nullptr , L""            },
				{ __LINE__, L"C:/Dir"   , L""     , L"C:/Dir/"     },
				{ __LINE__, L"C:/Dir"   , nullptr , L"C:/Dir/*"    }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				wstring result = PathStd::AppendFile(data[i].pDir, data[i].pFile, MAX_PATH);
				Assert::AreEqual(data[i].pExpected, result.c_str(), AssertMsg::Format(data[i].line, L"result"));
				Assert::IsTrue((size_t) MAX_PATH <= result.capacity(), AssertMsg::Format(data[i].line, L"reserved"));
			}
		}

		TEST_METHOD(Util_PathStd_Fix)
		{
			struct
			{
				int line;
				const wchar_t* pPath;
				const wchar_t* pExpected;
			}
			static data[] =
			{
				{ __LINE__, L"C:/Dir/Dir2"   , L"C:/Dir/Dir2/" },
				{ __LINE__, L"C:/Dir\\Dir2"  , L"C:/Dir/Dir2/" },
				{ __LINE__, L"C:\\Dir/Dir2"  , L"C:/Dir/Dir2/" },
				{ __LINE__, L"C:\\Dir\\Dir2" , L"C:/Dir/Dir2/" },
				{ __LINE__, L"C:\\Dir"       , L"C:/Dir/"      },
				{ __LINE__, L"C:\\Dir\\"     , L"C:/Dir/"      },
				{ __LINE__, L"C:\\"          , L"C:/"          },
				{ __LINE__, L"C:"            , L"C:/"          },
				{ __LINE__, L""              , L""             },
				{ __LINE__, nullptr          , L""             }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				wstring result = PathStd::Fix(data[i].pPath);
				Assert::AreEqual(data[i].pExpected, result.c_str(), AssertMsg::Format(data[i].line, L"result"));
				Assert::IsTrue((size_t) MAX_PATH <= result.capacity(), AssertMsg::Format(data[i].line, L"reserved"));
			}
		}
	};
}
#endif //TODO-PORT