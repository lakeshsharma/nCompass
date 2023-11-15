
#include "stdafx.h"
#include "../Sentinel/Hardware/CardReader.h"
#include "CardReaderTestHelpers.h"
#include "TestUtil.h"
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace pugi;
using namespace std;
using namespace TestUtil;

 

    class CardReaderTests : public ::testing::Test
	{
        public:  
        LPCTSTR pBadDir = "/tmp/~/";
  
        struct ExpectedReader
       	{
            int line;
        	LPCSTR pVendor;
        	WORD vendorId;
        	WORD productId;
        	CardReaderType::Type type;
        	bool found;
        };
	      
			virtual void SetUp()  
			{
			} 
			virtual void TearDown()
			{
                DeleteSentinelDirectories();
			}

            void Verify(CardReaderTests::ExpectedReader* pExpected, size_t numExpected, const vector<CardReader>& actual)
        	{
        		ASSERT_EQ(numExpected, actual.size()) << L"readers.size()";

                for (auto it = actual.cbegin(); it != actual.cend(); ++it)
        		{
        			for (size_t i = 0; i < numExpected; i++)
        			{
        				WORD vendorId = it->GetVendorId();
        				WORD productId = it->GetProductId();

        				if (vendorId == pExpected[i].vendorId && productId == pExpected[i].productId)
        				{
        					ASSERT_FALSE(pExpected[i].found) << AssertMsg::Format(pExpected[i].line, nullptr, L"pExpected[%d] was already found", i);
        					pExpected[i].found = true;

        					ASSERT_STREQ(pExpected[i].pVendor, it->GetVendor()) << AssertMsg::Format(pExpected[i].line, L"GetVendor", L"data[%d]", i);
        					ASSERT_EQ((int) pExpected[i].type, (int) it->GetType()) << AssertMsg::Format(pExpected[i].line, L"GetType", L"data[%d]", i);

        					break;
        				}
        			}
        		}

        		for (size_t i = 0; i < numExpected; i++)
        		{
        			ASSERT_TRUE(pExpected[i].found) << AssertMsg::Format(pExpected[i].line, nullptr, L"pExpected[%d] was not found", i);
        		}
        	}
    };

   

    TEST_F(CardReaderTests, CardReader_emap_pMCR)
	{
		CardReaderType::Type type = CardReaderType::emap[CardReaderType::pMCR];
		ASSERT_EQ((int) CardReaderType::MCR, (int) type);
	}


	TEST_F(CardReaderTests, CardReader_emap_pRFID)
	{
		CardReaderType::Type type = CardReaderType::emap[CardReaderType::pRFID];
		ASSERT_EQ((int) CardReaderType::RFID, (int) type);
	}

	TEST_F(CardReaderTests, CardReader_emap_MCR)
	{
		const char* pStr = CardReaderType::emap[CardReaderType::MCR];
		ASSERT_STREQ("MCR", pStr);
	}

	TEST_F(CardReaderTests, CardReader_emap_RFID)
	{
		const char* pStr = CardReaderType::emap[CardReaderType::RFID];
		ASSERT_STREQ("RFID", pStr);
	}



	TEST_F(CardReaderTests, CardReader_GetCardReaders_default)
	{
		CDirectoryManager::MockInstance();
		CDirectoryManager::MockSetPath(DIRECTORY_SENTINELDATA, "",pBadDir);
		CDirectoryManager::MockSetPath(DIRECTORY_APPLICATIONS,"",pBadDir);

		// this is deliberately NOT static
		ExpectedReader data[] =
		{
			{ __LINE__, "UIC", 25426,  9226, CardReaderType::MCR, false  },
			{ __LINE__, "XST", 10020, 24577, CardReaderType::MCR, false },
			{ __LINE__, "UIC", 25426,  9258, CardReaderType::RFID, false}
		};

		string pathname;
		vector<CardReader> readers = CardReader::GetCardReaders(pathname);
		ASSERT_EQ(pCardReaderMemoryFilepath, pathname) << L"Pathname";
		Verify(data, _countof(data), readers);
	}
	
	TEST_F(CardReaderTests, CardReader_GetCardReaders_applicationsDir)
	{
      	DeleteOnCloseFile cardReadersFile(DIRECTORY_APPLICATIONS, pCardReaderFilename.c_str(), CardReaderTestHelpers::pCardReadersXmlNoUicMcr);
		CDirectoryManager::MockSetPath(DIRECTORY_SENTINELDATA, "", pBadDir);

		// this is deliberately NOT static
		ExpectedReader data[] =
		{
			{ __LINE__, "XST", 10020, 24577, CardReaderType::MCR, false },
			{ __LINE__, "UIC", 25426,  9258, CardReaderType::RFID, false }
		};

		string pathname;
		vector<CardReader> readers = CardReader::GetCardReaders(pathname);
		ASSERT_EQ(cardReadersFile.GetPathname(), pathname) << L"Pathname";
		Verify(data, _countof(data), readers);
	}

	TEST_F(CardReaderTests, CardReader_GetCardReaders_dataDir)
	{
		DeleteOnCloseFile cardReadersFile(DIRECTORY_SENTINELDATA, pCardReaderFilename.c_str(), CardReaderTestHelpers::pCardReadersXmlNoUicMcr);
		CDirectoryManager::MockSetPath(DIRECTORY_APPLICATIONS, "", pBadDir);

		// this is deliberately NOT static
		ExpectedReader data[] =
		{
			{ __LINE__, "XST", 10020, 24577, CardReaderType::MCR, false  },
			{ __LINE__, "UIC", 25426,  9258, CardReaderType::RFID, false }
		};

		string pathname;
		vector<CardReader> readers = CardReader::GetCardReaders(pathname);
		ASSERT_EQ(cardReadersFile.GetPathname(), pathname) << L"Pathname";
		Verify(data, _countof(data), readers);
	}

	TEST_F(CardReaderTests, CardReader_GetCardReaders_applicationsDir_bad)
	{
		DeleteOnCloseFile cardReadersFile(DIRECTORY_APPLICATIONS, pCardReaderFilename.c_str(), CardReaderTestHelpers::pCardReadersXmlBad);
		CDirectoryManager::MockSetPath(DIRECTORY_SENTINELDATA, "", pBadDir);

		// this is deliberately NOT static
		ExpectedReader data[] =
		{
			{ __LINE__, "UIC", 25426,  9226, CardReaderType::MCR, false  },
			{ __LINE__, "XST", 10020, 24577, CardReaderType::MCR, false  },
			{ __LINE__, "UIC", 25426,  9258, CardReaderType::RFID, false }
		};

		string pathname;
		vector<CardReader> readers = CardReader::GetCardReaders(pathname);
		ASSERT_EQ(pCardReaderMemoryFilepath, pathname) << L"Pathname";
		Verify(data, _countof(data), readers);
	}

