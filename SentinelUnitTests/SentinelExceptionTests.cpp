#include "stdafx.h"
#include "SentinelException.h"
#include "TestUtil.h"
#include "Util/Convert.h"
#include "gtest/gtest.h"

namespace SentinelNativeUnitTests
{
		TEST(SentinelExceptionTests, SentinelException_Constructor)
		{
			const char* pMsg = "One";
			const intptr_t code = 1;

			SentinelException e(pMsg, code);

			ASSERT_STREQ(pMsg, e.what()) << "what()";
			ASSERT_EQ(code, e.m_code) << "m_code";
		}

		TEST(SentinelExceptionTests, SentinelException_Constructor1A)
		{
			const char* pMsg = "One";
			const intptr_t code = 1;

			SentinelException e(code, pMsg);

			ASSERT_STREQ(pMsg, e.what()) << "what()";
			ASSERT_EQ(code, e.m_code) << "m_code";
		}

		TEST(SentinelExceptionTests, SentinelException_Constructor1W)
		{
			const intptr_t code = 2;


			SentinelException e(code, "One");

			ASSERT_STREQ("One", e.what()) << "what()";
			ASSERT_EQ(code, e.m_code) << "m_code";
		}

		TEST(SentinelExceptionTests, SentinelException_Constructor2)
		{
			const char* pMsg = nullptr;
			const intptr_t code = 1;

			SentinelException e(pMsg, code);

			ASSERT_STREQ("", e.what()) << "what()";
			ASSERT_EQ(code, e.m_code) << "m_code";
		}

		TEST(SentinelExceptionTests, SentinelException_Constructor2a)
		{
			const char* pMsg = nullptr;
			const intptr_t code = 1;

			SentinelException e(pMsg, code);

			ASSERT_STREQ("", e.what()) << "what()";
			ASSERT_EQ(code, e.m_code) << "m_code";
		}

		TEST(SentinelExceptionTests, SentinelException_Constructor3)
		{
			const char* pMsg = "";
			const intptr_t code = 1;

			SentinelException e(pMsg, code);

			ASSERT_STREQ("", e.what()) << "what()";
			ASSERT_EQ(code, e.m_code) << "m_code";
		}

		TEST(SentinelExceptionTests, SentinelException_Constructor3a)
		{
			const char* pMsg = "";
			const intptr_t code = 1;

			SentinelException e(pMsg, code);

			ASSERT_STREQ("", e.what()) << "what()";
			ASSERT_EQ(code, e.m_code) << "m_code";
		}

		TEST(SentinelExceptionTests, SentinelException_CopyConstructor)
		{
			const char* pMsg = "One";
			const intptr_t code = 1;

			SentinelException e2(pMsg, code);
			SentinelException e(e2);

			ASSERT_STREQ(pMsg, e.what()) << "what()";
			ASSERT_EQ(code, e.m_code) << "m_code";
		}

		TEST(SentinelExceptionTests, SentinelException_CopyConstructor2)
		{
			const char* pMsg = nullptr;
			const intptr_t code = 1;

			SentinelException e2(pMsg, code);
			SentinelException e(e2);

			ASSERT_STREQ("", e.what()) << "what()";
			ASSERT_EQ(code, e.m_code) << "m_code";
		}

		TEST(SentinelExceptionTests, SentinelException_CopyConstructor3)
		{
			const char* pMsg = "";
			const intptr_t code = 1;

			SentinelException e2(pMsg, code);
			SentinelException e(e2);

			ASSERT_STREQ("", e.what()) << "what()";
			ASSERT_EQ(code, e.m_code) << "m_code";
		}

		TEST(SentinelExceptionTests, SentinelException_AssignmentOperator)
		{
			const char* pMsg = "One";
			const intptr_t code = 1;

			SentinelException e2(pMsg, code);
			SentinelException e("Two", 2);
			e = e2;

			ASSERT_EQ(code, e.m_code) << "m_code";
		}

		TEST(SentinelExceptionTests, SentinelException_AssignmentOperator2)
		{
			const char* pMsg = nullptr;
			const intptr_t code = 1;

			SentinelException e2(pMsg, code);
			SentinelException e("Two", 2);
			e = e2;

			ASSERT_EQ(code, e.m_code) << "m_code";
		}

		TEST(SentinelExceptionTests, SentinelException_AssignmentOperator3)
		{
			const char* pMsg = "";
			const intptr_t code = 1;

			SentinelException e2(pMsg, code);
			SentinelException e("Two", 2);
			e = e2;

			ASSERT_EQ(code, e.m_code) << "m_code";
		}

		TEST(SentinelExceptionTests, SentinelException_HasNonZeroData1)
		{
			struct
			{
				int line;
				intptr_t code;
				const char* pMsg;
				bool checkAll;
				bool expectedResult;
			}
			static data[] =
			{
				{ __LINE__, 0, nullptr, true, false },
				{ __LINE__, 0, nullptr, false, false },

				{ __LINE__, 1, nullptr, true, false },
				{ __LINE__, 1, nullptr, false, true },

				{ __LINE__, 0, "s", true, false },
				{ __LINE__, 0, "s", false, true },

				{ __LINE__, 1, "s", true, true },
				{ __LINE__, 1, "s", false, true },
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				SentinelException e(data[i].code, data[i].pMsg);
				bool result = e.HasNonZeroData(data[i].checkAll);
				ASSERT_EQ(data[i].expectedResult, result) << FormatString("%d: %s", data[i].line, "result");
			}
		}

		TEST(SentinelExceptionTests, SentinelExceptionT1)
		{
			enum Test
			{
				Ninety = 90,
				Minus2 = -2
			};

			SentinelExceptionT<Test> e1(Ninety);
			ASSERT_EQ((int) Ninety, (int) e1.code()) << "ctor";

			SentinelExceptionT<Test> e2(e1);
			ASSERT_EQ((int) Ninety, (int) e2.code()) << "copy ctor";

			SentinelExceptionT<Test> e3(Minus2);
			ASSERT_EQ((int) Minus2, (int) e3.code()) << "ctor 2";
			e1 = e3;
			ASSERT_EQ((int) Minus2, (int) e1.code()) << "assignment";
		}

		TEST(SentinelExceptionTests, SentinelExceptionT2)
		{
			enum Test
			{
				Ninety = 90,
				Minus2 = -2
			};

			SentinelExceptionT<Test> e1("90", Ninety);
			ASSERT_STREQ("90", e1.what()) << "ctor what";
			ASSERT_EQ((int) Ninety, (int) e1.code()) << "ctor code";

			SentinelExceptionT<Test> e2(e1);
			ASSERT_STREQ("90", e1.what()) << "copy ctor what";
			ASSERT_EQ((int) Ninety, (int) e2.code()) << "copy ctor code";

			SentinelExceptionT<Test> e3("-1", Minus2);
			ASSERT_STREQ("-1", e3.what()) << "ctor 2 what";
			ASSERT_EQ((int) Minus2, (int) e3.code()) << "ctor 2 code";
			e1 = e3;
			ASSERT_EQ((int) Minus2, (int) e3.code()) << "assignment code";
		}

		TEST(SentinelExceptionTests, SentinelFileException1)
		{
			struct
			{
				int line;
				const char* pFilename;

				void Verify(const SentinelFileException& e, LPCSTR pCtr)
				{
					if (pFilename == nullptr)
					{
						ASSERT_EQ(nullptr, e.rawfilename()) << FormatString("%d: %s %s", line, "rawfilename()", pCtr);
						ASSERT_STREQ("", e.filename()) << FormatString("%d: %s %s", line, "filename()", pCtr);
					}
					else
					{
						ASSERT_STREQ(pFilename, e.filename()) << FormatString("%d: %s %s", line, "filename()", pCtr);
					}

					ASSERT_EQ(line, e.code()) << FormatString("%d: %s %s", line, "code()", pCtr);
					ASSERT_STREQ("", e.what()) << FormatString("%d: %s %s", line, "what()", pCtr);
				}
			}
			static data[] =
			{
				{ __LINE__, "1"	},

				{ __LINE__, nullptr	},
				{ __LINE__, ""		},
				{ __LINE__, "1"	},

				{ __LINE__, nullptr,},
				{ __LINE__, ""		},
				{ __LINE__, "1"	},

				{ __LINE__, nullptr	},
				{ __LINE__, ""		},
				{ __LINE__, "1",	},

				{ __LINE__, nullptr },
				{ __LINE__, "",	},
				{ __LINE__, "1",	}
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				SentinelFileException e1(data[i].pFilename, data[i].line);
				data[i].Verify(e1, "1 arg");

				SentinelFileException e2(e1);
				data[i].Verify(e1, "copy");

				SentinelFileException e3("123", 456, "789");
				e3 = e2;
				data[i].Verify(e1, "assign");
			}
		}

		TEST(SentinelExceptionTests, SentinelFileException2a)
		{
			struct
			{
				int line;
				const char* pFilename;
				const char* pMsg;

				void Verify(const SentinelFileException& e, LPCSTR pCtr)
				{
					if (pFilename == nullptr)
					{
						ASSERT_EQ(nullptr, e.rawfilename()) << FormatString("%d: %s %s", line, "rawfilename()", pCtr);
						ASSERT_STREQ("", e.filename()) << FormatString("%d: %s %s", line, "filename()", pCtr);
					}
					else
					{
						ASSERT_STREQ(pFilename, e.filename()) << FormatString("%d: %s %s", line, "filename()", pCtr);
					}

					ASSERT_EQ(line, e.code()) << FormatString("%d: %s %s", line, "code()", pCtr);

					if (pMsg == nullptr)
					{
						ASSERT_STREQ("", e.what()) << FormatString("%d: %s %s", line, "what()", pCtr);
					}
					else
					{
						ASSERT_STREQ(pMsg, e.what()) << FormatString("%d: %s %s", line, "what()", pCtr);
					}
				}
			}
			static data[] =
			{
				{ __LINE__, nullptr,	nullptr },
				{ __LINE__, "",		nullptr },
				{ __LINE__, "1",		nullptr },

				{ __LINE__, nullptr,	nullptr },
				{ __LINE__, "",		nullptr },
				{ __LINE__, "1",		nullptr },

				{ __LINE__, nullptr,	nullptr },
				{ __LINE__, "",		""      },
				{ __LINE__, "1",		"5"     },

				{ __LINE__, nullptr,	nullptr },
				{ __LINE__, "",		""      },
				{ __LINE__, "1",		"5"     }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				SentinelFileException e1(data[i].pFilename, data[i].line, (const char*) data[i].pMsg);
				data[i].Verify(e1, "2 args");

				SentinelFileException e2(e1);
				data[i].Verify(e1, "copy");

				SentinelFileException e3("123", 456, "789");
				e3 = e2;
				data[i].Verify(e1, "assign");
			}
		}

		TEST(SentinelExceptionTests, SentinelFileException2b)
		{
			struct
			{
				int line;
				const char* pFilename;
				const char* pMsg;

				void Verify(const SentinelFileException& e, LPCSTR pCtr)
				{
					if (pFilename == nullptr)
					{
						ASSERT_EQ(nullptr, e.rawfilename()) << FormatString("%d: %s %s", line, "rawfilename()", pCtr);
						ASSERT_STREQ("", e.filename()) << FormatString("%d: %s %s", line, "filename()", pCtr);
					}
					else
					{
						ASSERT_STREQ(pFilename, e.filename()) << FormatString("%d: %s %s", line, "filename()", pCtr);
					}

					ASSERT_EQ(line, e.code()) << FormatString("%d: %s %s", line, "code()", pCtr);

					if (pMsg == nullptr)
					{
						ASSERT_STREQ("", e.what()) << FormatString("%d: %s %s", line, "what()", pCtr);
					}
					else
					{
						ASSERT_STREQ( pMsg, e.what()) << FormatString("%d: %s %s", line, "what()", pCtr);
					}
				}
			}
			static data[] =
			{
				{ __LINE__, nullptr,	nullptr },
				{ __LINE__, "",		nullptr },
				{ __LINE__, "1",		nullptr },

				{ __LINE__, nullptr,	nullptr },
				{ __LINE__, "",		nullptr },
				{ __LINE__, "1",		nullptr },

				{ __LINE__, nullptr,	nullptr },
				{ __LINE__, "",		""     },
				{ __LINE__, "1",		"5"    },

				{ __LINE__, nullptr,	nullptr },
				{ __LINE__, "",		""     },
				{ __LINE__, "1",		"5"    }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				SentinelFileException e1(data[i].pFilename, data[i].line, (const char*) data[i].pMsg);
				data[i].Verify(e1, "2 args");

				SentinelFileException e2(e1);
				data[i].Verify(e1, "copy");

				SentinelFileException e3("123", 456, "789");
				e3 = e2;
				data[i].Verify(e1, "assign");
			}
		}

		TEST(SentinelExceptionTests, SentinelFileException2c)
		{
			struct
			{
				int line;
				const char* pFilename;
				intptr_t code;

				void Verify(const SentinelFileException& e, LPCSTR pCtr)
				{
					if (pFilename == nullptr)
					{
						ASSERT_EQ(nullptr, e.rawfilename()) << FormatString("%d: %s %s", line, "rawfilename()", pCtr);
						ASSERT_STREQ("", e.filename()) << FormatString("%d: %s %s", line, "filename()", pCtr);
					}
					else
					{
						ASSERT_STREQ(pFilename, e.filename()) << FormatString("%d: %s %s", line, "filename()", pCtr);
					}

					ASSERT_EQ(code, e.code()) << FormatString("%d: %s %s", line, "code()", pCtr);
					ASSERT_STREQ("", e.what()) << FormatString("%d: %s %s", line, "what()", pCtr);
				}
			}
			static data[] =
			{
				{ __LINE__, nullptr,	0 },
				{ __LINE__, "",		0 },
				{ __LINE__, "1",		0 },
										  
				{ __LINE__, nullptr,	1 },
				{ __LINE__, "",		1 },
				{ __LINE__, "1",		1 }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				SentinelFileException e1(data[i].pFilename, data[i].code);
				data[i].Verify(e1, "2 args");

				SentinelFileException e2(e1);
				data[i].Verify(e1, "copy");

				SentinelFileException e3("123", 456, "789");
				e3 = e2;
				data[i].Verify(e1, "assign");
			}
		}

		TEST(SentinelExceptionTests, SentinelFileException3a)
		{
			struct
			{
				int line;
				const char* pFilename;
				intptr_t code;
				const char* pMsg;

				void Verify(const SentinelFileException& e, LPCSTR pCtr)
				{
					if (pFilename == nullptr)
					{
						ASSERT_EQ(nullptr, e.rawfilename()) << FormatString("%d: %s %s", line, "rawfilename()", pCtr);
						ASSERT_STREQ("", e.filename()) << FormatString("%d: %s %s", line, "filename()", pCtr);
					}
					else
					{
						ASSERT_STREQ(pFilename, e.filename()) << FormatString("%d: %s %s", line, "filename()", pCtr);
					}

					ASSERT_EQ(code, e.code()) << FormatString("%d: %s %s", line, "code()", pCtr);

					if (pMsg == nullptr)
					{
						ASSERT_STREQ("", e.what()) << FormatString("%d: %s %s", line, "what()", pCtr);
					}
					else
					{
						ASSERT_STREQ(pMsg, e.what()) << FormatString("%d: %s %s", line, "what()", pCtr);
					}
				}
			}
			static data[] =
			{
				{ __LINE__, nullptr,	0, nullptr },
				{ __LINE__, "",		0, nullptr },
				{ __LINE__, "1",		0, nullptr },

				{ __LINE__, nullptr,	1, nullptr },
				{ __LINE__, "",		1, nullptr },
				{ __LINE__, "1",		1, nullptr },

				{ __LINE__, nullptr,	0, nullptr },
				{ __LINE__, "",		0, ""      },
				{ __LINE__, "1",		0, "5"     },

				{ __LINE__, nullptr,	1, nullptr },
				{ __LINE__, "",		1, ""      },
				{ __LINE__, "1",		1, "5"     }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				SentinelFileException e1(data[i].pFilename, data[i].code, data[i].pMsg);
				data[i].Verify(e1, "3 args");

				SentinelFileException e2(e1);
				data[i].Verify(e1, "copy");

				SentinelFileException e3("123", 456, "789");
				e3 = e2;
				data[i].Verify(e1, "assign");
			}
		}

		TEST(SentinelExceptionTests, SentinelFileException3b)
		{
			struct
			{
				int line;
				const char* pFilename;
				intptr_t code;
				const char* pMsg;

				void Verify(const SentinelFileException& e, LPCSTR pCtr)
				{
					if (pFilename == nullptr)
					{
						ASSERT_EQ(nullptr, e.rawfilename()) << FormatString("%d: %s %s", line, "rawfilename()", pCtr);
						ASSERT_STREQ("", e.filename()) << FormatString("%d: %s %s", line, "filename()", pCtr);
					}
					else
					{
						ASSERT_STREQ(pFilename, e.filename()) << FormatString("%d: %s %s", line, "filename()", pCtr);
					}

					ASSERT_EQ(code, e.code()) << FormatString("%d: %s %s", line, "code()", pCtr);

					if (pMsg == nullptr)
					{
						ASSERT_STREQ("", e.what()) << FormatString("%d: %s %s", line, "what()", pCtr);
					}
					else
					{
						ASSERT_STREQ(pMsg, e.what()) << FormatString("%d: %s %s", line, "what()", pCtr);
					}
				}
			}
			static data[] =
			{
				{ __LINE__, nullptr,	0, nullptr },
				{ __LINE__, "",		0, nullptr },
				{ __LINE__, "1",		0, nullptr },

				{ __LINE__, nullptr,	1, nullptr },
				{ __LINE__, "",		1, nullptr },
				{ __LINE__, "1",		1, nullptr },

				{ __LINE__, nullptr,	0, nullptr },
				{ __LINE__, "",		0, ""      },
				{ __LINE__, "1",		0, "5"     },

				{ __LINE__, nullptr,	1, nullptr },
				{ __LINE__, "",		1, ""      },
				{ __LINE__, "1",		1, "5"     }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				SentinelFileException e1(data[i].pFilename, data[i].code, data[i].pMsg);
				data[i].Verify(e1, "3 args");

				SentinelFileException e2(e1);
				data[i].Verify(e1, "copy");

				SentinelFileException e3("123", 456, "789");
				e3 = e2;
				data[i].Verify(e1, "assign");
			}
		}

		TEST(SentinelExceptionTests, SentinelCancelException_ctorA)
		{
			struct
			{
				int line;
				intptr_t code;
				const char* pMsg;

				void Verify(SentinelCancelException& e)
				{
					ASSERT_EQ(code, e.code()) << FormatString("%d: %s", line, "code");

					if (pMsg == nullptr)
					{
						ASSERT_STREQ("", e.what()) << FormatString("%d: %s", line, "what");
					}
					else
					{
						ASSERT_STREQ(pMsg, e.what()) << FormatString("%d: %s", line, "what");
					}
				}
			}
			static data[] =
			{
				{ __LINE__, 0, nullptr },
				{ __LINE__, 1, nullptr },

				{ __LINE__, 0, "" },
				{ __LINE__, 1, "" },

				{ __LINE__, 0, "s"     },
				{ __LINE__, 1, "s"     }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				try
				{
					throw SentinelCancelException(data[i].code, data[i].pMsg);
				}
				catch (SentinelCancelException& e)
				{
					data[i].Verify(e);

					try
					{
						throw SentinelCancelException(e);
					}
					catch (SentinelCancelException& e1)
					{
						data[i].Verify(e1);

						SentinelCancelException e2(9000, data[i].pMsg);
						e2 = e1;
						data[i].Verify(e2);
					}
				}

				try
				{
					throw SentinelCancelException(data[i].pMsg, data[i].code);
				}
				catch (SentinelCancelException& e)
				{
					data[i].Verify(e);
				}
			}
		}

		TEST(SentinelExceptionTests, SentinelCancelException_ctorW)
		{
			struct
			{
				int line;
				intptr_t code;
				const char* pMsg;

				void Verify(SentinelCancelException& e)
				{
					ASSERT_EQ(code, e.code()) << FormatString("%d: %s", line, "code");

					if (pMsg == nullptr)
					{
						ASSERT_STREQ("", e.what()) << FormatString("%d: %s", line, "what");
					}
					else
					{
						std::string msg = Convert::Utf8ToString(e.what());
						ASSERT_STREQ(pMsg, msg.c_str()) << FormatString("%d: %s", line, "what");
					}
				}
			}
			static data[] =
			{
				{ __LINE__, 0, nullptr },
				{ __LINE__, 1, nullptr },

				{ __LINE__, 0, "" },
				{ __LINE__, 1, "" }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				try
				{
					throw SentinelCancelException(data[i].pMsg, data[i].code);
				}
				catch (SentinelCancelException& e)
				{
					data[i].Verify(e);

					try
					{
						throw SentinelCancelException(e);
					}
					catch (SentinelCancelException& e1)
					{
						data[i].Verify(e1);

						SentinelCancelException e2(9000, data[i].pMsg);
						e2 = e1;
						data[i].Verify(e2);
					}
				}
			}
		}

		TEST(SentinelExceptionTests, SentinelCreateThreadException_tests)
		{
			struct
			{
				int line;
				intptr_t code;
				intptr_t expectedCode;
				const char* pMsg;

				void Verify(SentinelCreateThreadException& e)
				{
					ASSERT_EQ(expectedCode, e.code()) << FormatString("%d: %s", line, "code");

					if (pMsg == nullptr)
					{
						ASSERT_STREQ("", e.what()) << FormatString("%d: %s", line, "what");
					}
					else
					{
						ASSERT_STREQ(pMsg, e.what()) << FormatString("%d: %s", line, "what");
					}
				}
			}
			static data[] =
			{
				{ __LINE__, 0, 1, nullptr },
				{ __LINE__, 0, 1, "s"     },
				{ __LINE__, 1, 1, nullptr },
				{ __LINE__, 1, 1, "s"     }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				try
				{
					if (data[i].code == 0)
					{
						throw SentinelCreateThreadException(data[i].pMsg, data[i].expectedCode);
					}
					else
					{
						throw SentinelCreateThreadException(data[i].pMsg, data[i].code);
					}
				}
				catch (SentinelCreateThreadException& e)
				{
					data[i].Verify(e);

					try
					{
						throw SentinelCreateThreadException(e);
					}
					catch (SentinelCreateThreadException& e1)
					{
						data[i].Verify(e1);

						SentinelCreateThreadException e2(data[i].pMsg, 9000);
						e2 = e1;
						data[i].Verify(e2);
					}
				}
			}
		}
}
