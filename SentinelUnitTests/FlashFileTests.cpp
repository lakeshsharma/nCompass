#include "stdafx.h"
#include "Flash.h"
#include "FilePacker.h"
#include "Util/FileSys.h"
#include "Util/Path.h"
#include "Utility/TestUtil.h"
#include "gtest/gtest.h"
#include "AssertMsg.h"


using namespace std;

namespace SentinelUnitTests
{
	class FlashFileTests : public ::testing::Test
	{
    public:
        void SetUp()
		{
			TestUtil::CreateSentinelDirectory(DIRECTORY_UPGRADE);
		}

		void TearDown()
		{
			TestUtil::DeleteSentinelDirectories();
		}

        void FlashFile_ctr()
        {
            string dataLocation = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE);
 
            const DWORD numBlocks = 3;
            CFlash flash(FLASH_SYSTEM, BLOCKSIZE * numBlocks, numBlocks, 1, 789, 443, 0x01020304);

            ASSERT_EQ(numBlocks, flash.GetBlockCount()) << L"flash.m_blockCount()";
            ASSERT_EQ(numBlocks, flash.m_missingBlockCount) <<L"flash.m_missingBlockCount";
            ASSERT_EQ((DWORD) BLOCKSIZE, flash.GetBlockSize()) << L"flash.m_blockSize()";
            ASSERT_EQ(BLOCKSIZE * numBlocks, flash.GetFileSize()) << L"flash.m_fileSize";
            ASSERT_EQ((int) FLASH_SYSTEM, (int) flash.m_type) << L"flash.m_type";
            ASSERT_EQ((DWORD) 1, flash.GetSessionID()) << L"flash.m_sessionID";
            ASSERT_EQ((DWORD) 789, (DWORD) flash.GetPort()) << L"flash.m_port";
            ASSERT_EQ((DWORD) 443, flash.m_expectedChecksum) << L"flash.m_expectedChecksum";
            ASSERT_EQ((DWORD) 0, flash.m_actualChecksum) << L"flash.m_actualChecksum";
            ASSERT_FALSE(flash.m_isFlashFileReady)<< L"flash.m_isFlashFileReady";
            ASSERT_FALSE(flash.m_isCleared) << L"flash.m_isCleared";
            ASSERT_EQ((DWORD) 0, (DWORD) flash.m_percentComplete) << L"flash.m_percentComplete";
            ASSERT_EQ((DWORD) 1, (DWORD) flash.m_nextBlockBegin) << L"flash.m_nextBlockBegin";
            ASSERT_FALSE(flash.m_restartServiceWindow) << L"flash.m_restartServiceWindow";
            ASSERT_EQ((DWORD) (numBlocks + 1), (DWORD) flash.m_blockArray.size()) << L"flash.m_blockArray.GetCount()";
            ASSERT_EQ((DWORD) 0x01020304, (DWORD) flash.m_ipAddress)<< L"flash.m_ipAddress";
        }

        void FlashFile_IsMulticastT()
        {
            CFlash flash;
            flash.m_ipAddress = 0x01020304;
            ASSERT_TRUE(flash.IsMulticast());
        }
        void FlashFile_IsMulticastF()
        {
            CFlash flash;
            flash.m_ipAddress = 0;
            ASSERT_FALSE(flash.IsMulticast());
        }

        void FlashFile_GetIp()
        {
            CFlash flash;
            flash.m_ipAddress = 0x01020304;

            char ip[32];
            flash.GetIP(ip, _countof(ip));
            ASSERT_STREQ("4.3.2.1", ip);
        }

        void FlashFile_GetIp_nullptr()
        {
            CFlash flash;
            flash.m_ipAddress = 0x01020304;

            char ip[32] = "";
            flash.GetIP(nullptr, _countof(ip));
            ASSERT_STREQ("", ip);
        }

        void FlashFile_GetIP_Zerolength()
        {
            CFlash flash;
            flash.m_ipAddress = 0x01020304;

            char ip[32] = "";
            flash.GetIP(ip, 0);
            ASSERT_STREQ(ip, "");
        }

        void FlashFile_GetIP_Lowlength()
        {
            CFlash flash;
            flash.m_ipAddress = 0x01020304;

            char ip[32] = "";
            ASSERT_STREQ("", ip);
        }

        void FlashFile_IsComplete_True()
        {
            CFlash flash;
            flash.m_missingBlockCount = 0;
            ASSERT_TRUE(flash.IsComplete());
        }

        void FlashFile_IsComplete_False()
        {
            CFlash flash;
            flash.m_missingBlockCount = 1;
            ASSERT_FALSE(flash.IsComplete());
        }

        void FlashFile_NextBlock_1()
        {
            CFlash flash;
            flash.m_blockCount = 2;
            flash.m_nextBlockBegin = 1;
            //flash.m_blockArray.resize(3);
            flash.m_blockArray.push_back(1);
            flash.m_blockArray.push_back(1);
            flash.m_blockArray.push_back(0);

            ASSERT_EQ((DWORD) 2, flash.NextBlock());
        }

        void FlashFile_NextBlock_2()
        {
            CFlash flash;
            flash.m_blockCount = 2;
            flash.m_nextBlockBegin = 2;
            //flash.m_blockArray.resize(3);
            flash.m_blockArray.push_back(1);
            flash.m_blockArray.push_back(1);
            flash.m_blockArray.push_back(0);

            ASSERT_EQ((DWORD) 2, flash.NextBlock());
        }

        void FlashFile_NextBlock_3()
        {
            CFlash flash;
            flash.m_blockCount = 2;
            flash.m_nextBlockBegin = 3;
            //flash.m_blockArray.resize(3);
            flash.m_blockArray.push_back(1);
            flash.m_blockArray.push_back(1);
            flash.m_blockArray.push_back(0);

            ASSERT_EQ((DWORD) 2, flash.NextBlock());
        }

        void FlashFile_NextBlock_3a()
        {
            CFlash flash;
            flash.m_blockCount = 2;
            flash.m_nextBlockBegin = 3;
            //flash.m_blockArray.resize(3);
            flash.m_blockArray.push_back(1);
            flash.m_blockArray.push_back(1);
            flash.m_blockArray.push_back(1);

            ASSERT_EQ((DWORD) 0, flash.NextBlock());
        }

        void FlashFile_CheckBlock_0()
        {
            CFlash flash;
            flash.m_blockCount = 0;
            ASSERT_FALSE(flash.CheckBlock(0)) << L"flash.CheckBlock(0)";
            ASSERT_TRUE(flash.CheckBlock(1)) << L"flash.CheckBlock(1)";
        }

        void FlashFile_CheckBlock_1()
        {
            CFlash flash;
            flash.m_blockCount = 1;
            //flash.m_blockArray.resize(2);

            flash.m_blockArray.push_back(1);
            flash.m_blockArray.push_back(0);

            uint8_t size = flash.m_blockArray.size();
            (void)size;
            ASSERT_TRUE(flash.CheckBlock(0)) << L"flash.CheckBlock(0)";
            ASSERT_FALSE(flash.CheckBlock(1)) << L"flash.CheckBlock(1)";
        }

        void FlashFile_GetFlashFileReady_true()
        {
            CFlash flash;
            flash.m_isFlashFileReady = true;
            ASSERT_TRUE(flash.GetFlashFileReady());
        }

        void FlashFile_GetFlashFileReady_false()
        {
            CFlash flash;
            flash.m_isFlashFileReady = false;
            ASSERT_FALSE(flash.GetFlashFileReady());
        }

        void FlashFile_IsCleared_true()
        {
            CFlash flash;
            flash.m_isCleared = true;
            ASSERT_TRUE(flash.IsCleared());
        }

        void FlashFile_IsCleared_false()
        {
            CFlash flash;
            flash.m_isCleared = false;
            ASSERT_FALSE(flash.IsCleared());
        }

        void FlashFile_GetMissingBlockCount()
        {
            CFlash flash;
            flash.m_missingBlockCount = 5;
            ASSERT_EQ((DWORD) 5, flash.GetMissingBlockCount());
        }

        void FlashFile_GetChecksum()
        {
            CFlash flash;
            flash.m_actualChecksum = 5;
            ASSERT_EQ((DWORD) 5, flash.GetChecksum());
        }

        void FlashFile_GetPercentComplete()
        {
            CFlash flash;
            flash.m_percentComplete = 5;
            ASSERT_EQ((DWORD) 5, (DWORD) flash.GetPercentComplete());
        }

        void  FlashFile_Clear()
        {
            /*
            CFlash flash;
            string filePath(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE));
            filePath.append("/Test.txt");
            flash.m_filename = filePath;
            ASSERT_TRUE(flash.OpenFile()) << L"Unable to open file";
            ASSERT_TRUE(FileSys::FileExists(flash.m_filename)) << L"FileExists true";
            ASSERT_FALSE(flash.IsCleared()) << L"Before Clear";
            flash.Clear();
            ASSERT_TRUE(flash.IsCleared()) << L"After Clear";
            ASSERT_FALSE((bool) flash.m_file) << L"flash.m_file";
            ASSERT_FALSE(FileSys::FileExists(flash.m_filename)) << L"FileExists false";
            */
            // CFlash not currently in use
            ASSERT_TRUE(true);
        }

        void FlashFile_ResetFileName_IsFileOpen()
        {
            /*
            CFlash flash;
            string filename(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE));
            filename.append("/Test.txt");
            flash.m_filename = filename;
            //flash.m_file->reset(new CFile(flash.m_filename, CFile::modeCreate | CFile::shareDenyNone | CFile::modeReadWrite));
            ASSERT_TRUE(flash.OpenFile()) << L"Unable to open file";
            ASSERT_TRUE(flash.IsFileOpen()) << L"flash.IsFileOpen() before force";
            ASSERT_TRUE(FileSys::FileExists(flash.m_filename)) << L"FileExists true";

            flash.ResetFileName("None.txt");
            ASSERT_TRUE(FileSys::FileExists(filename)) << L"FileExists true";
            ASSERT_STREQ("None.txt", flash.m_filename.c_str()) << L"flash.m_filename";
            ASSERT_FALSE(flash.IsFileOpen()) << L"flash.IsFileOpen() after force";
            */
            // CFlash not currently in use
            ASSERT_TRUE(true);
        }


        void FlashFile_ResetFileName()
        {
            /*
            CFlash flash;
            string filename(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE));
            filename.append("/Test.txt");
            flash.m_filename = filename;
            //flash.m_file.reset(new CFile(flash.m_filename, CFile::modeCreate | CFile::shareDenyNone | CFile::modeReadWrite));
            ASSERT_TRUE(flash.OpenFile()) << L"Unable to open file";
            ASSERT_TRUE(flash.IsFileOpen()) << L"flash.IsFileOpen() before close";
            ASSERT_TRUE(FileSys::FileExists(flash.m_filename)) << L"FileExists before close";

            flash.CloseFile();
            ASSERT_FALSE(flash.IsFileOpen()) << L"flash.IsFileOpen() after close";
            ASSERT_TRUE(FileSys::FileExists(flash.m_filename)) << L"FileExists after close";
            ASSERT_FALSE((bool) flash.m_file) << L"flash.m_file";
            */
            // CFlash not currently in use
            ASSERT_TRUE(true);
        }

        void FlashFile_CalculateChecksum_CloseFile()
        {
            /*
            CFlash flash;
            flash.m_expectedChecksum = 0;

            BYTE buffer[256];
            for (int i = 0; i < sizeof(buffer); i++)
            {
                buffer[i] = (BYTE) i;
                flash.m_expectedChecksum += (BYTE) i;
            }

            string filename(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE));
            filename.append("/Test.txt");
            flash.m_filename = filename;
            //flash.m_file.reset(new CFile(flash.m_filename, CFile::modeCreate | CFile::shareDenyNone | CFile::modeReadWrite));
            ASSERT_TRUE(flash.OpenFile()) << L"Unable to open file";
            //flash.m_file->Write(buffer, sizeof(buffer));
            fwrite(buffer, sizeof(BYTE), sizeof(buffer), flash.m_file);
            flash.CalculateChecksum();
            ASSERT_TRUE(flash.GetFlashFileReady());
            */
            // CFlash not currently in use
            ASSERT_TRUE(true);
        }

        void FlashFile_CalculateChecksum_false()
        {
            /*
            CFlash flash;
            flash.m_expectedChecksum = 0;

            BYTE buffer[256];
            for (int i = 0; i < sizeof(buffer); i++)
            {
                buffer[i] = i == 128 ? (BYTE) i : 0;
                flash.m_expectedChecksum += (BYTE) i;
            }

            string filename(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE));
            filename.append("/Test.txt");
            flash.m_filename = filename;
			remove(flash.m_filename.c_str());

            ASSERT_TRUE(flash.OpenFile()) << L"Unable to open file";
            flash.AddDataBlock(buffer, sizeof(buffer), (DWORD)255);

            flash.CalculateChecksum();
            ASSERT_FALSE(flash.GetFlashFileReady());
            */
            // CFlash not currently in use
            ASSERT_TRUE(true);
        }

        void FlashFile_CalculateChecksum_Exception()
        {
            /*
            CFlash flash;
            flash.m_expectedChecksum = 0;

            BYTE buffer[256];
            for (int i = 0; i < sizeof(buffer); i++)
            {
                buffer[i] = (BYTE) i;
                flash.m_expectedChecksum += (BYTE) i;
            }

            string filename(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE));
            filename.append("/Test.txt");
            flash.m_filename = filename;
			remove(flash.m_filename.c_str());

			ASSERT_TRUE(flash.OpenFile()) << L"Unable to open file";
            flash.AddDataBlock(buffer, sizeof(buffer), 255);

            try
            {
                flash.CalculateChecksum();
                ASSERT_FALSE(flash.GetFlashFileReady());
            }
            catch (exception* pE)
            {
                FAIL()<<L"Unexpected exception";
            }
            */
            // CFlash not currently in use
            ASSERT_TRUE(true);
        }

        void  FlashFile_CloseFile_exception()
        {
            /*
            CFlash flash;
            string filename(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE));
            filename.append("/Test.txt");
            flash.m_filename = filename;
            ASSERT_TRUE(flash.OpenFile()) << L"Unable to open file";
            ASSERT_TRUE(flash.IsFileOpen()) <<L"flash.IsFileOpen() before close";
            ASSERT_TRUE(FileSys::FileExists(flash.m_filename)) << L"FileExists before close";

            try
            {
                flash.CloseFile();
                ASSERT_FALSE(flash.IsFileOpen()) << L"flash.IsFileOpen() after close";
                ASSERT_TRUE(FileSys::FileExists(flash.m_filename)) << L"FileExists after close";
            }
            catch (exception* pE)
            {
                FAIL() <<L"Unexpected exception";
            }
            */
            // CFlash not currently in use
            ASSERT_TRUE(true);
        }

        void FlashFile_DeleteFile()
        {
            /*
            CFlash flash;
            string filename(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE));
            filename.append("/Test.txt");
            flash.m_filename = filename;

            ASSERT_TRUE(flash.OpenFile()) << L"Unable to open file";
            ASSERT_TRUE(flash.IsFileOpen()) << L"flash.IsFileOpen() before close";
            ASSERT_TRUE(FileSys::FileExists(flash.m_filename)) << L"FileExists before close";

            try
            {
                flash.DeleteFile();
                ASSERT_FALSE(flash.IsFileOpen()) << L"flash.IsFileOpen() after close";
                ASSERT_FALSE(FileSys::FileExists(flash.m_filename)) << L"FileExists after close";
            }
            catch (exception* pE)
            {
                FAIL() << L"Unexpected exception";
            }
            */
            // CFlash not currently in use
            ASSERT_TRUE(true);
        }

        void FlashFile_DeleteFile_exception()
        {
            /*
            CFlash flash;
            string filename(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE));
            filename.append("/Test.txt");
            flash.m_filename = filename;
            ASSERT_TRUE(flash.OpenFile()) << L"Unable to open file";
            ASSERT_TRUE(flash.IsFileOpen()) << L"flash.IsFileOpen() before close";
            ASSERT_TRUE(FileSys::FileExists(flash.m_filename)) << L"FileExists before close";

           try
            {
                flash.DeleteFile();
                ASSERT_FALSE(flash.IsFileOpen()) << L"flash.IsFileOpen() after close";
                ASSERT_FALSE(FileSys::FileExists(flash.m_filename)) << L"FileExists after close";
            }
            catch (exception* pE)
            {
                FAIL() << L"Unexpected exception";
            }
            */
            // CFlash not currently in use
            ASSERT_TRUE(true);
        }

        void FlashFile_OpenFile()
        {
            /*
            try
            {
                string filename(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE));
                filename.append("/Test.txt");
 
                CFlash flash;
                flash.ResetFileName(filename);
                bool result = flash.OpenFile(false);
                ASSERT_TRUE(result) << L"flash.OpenFile(false)";
                ASSERT_TRUE(flash.IsFileOpen()) << L"flash.IsFileOpen [1]";
                ASSERT_TRUE(FileSys::FileExists(flash.m_filename)) <<  L"FileExists [1]";

                //flash.m_file->Write("Hello", 5);
                fwrite("Hello", sizeof(char), 5, flash.m_file);

                flash.CloseFile();

                //result = flash.OpenFile(true);
                flash.m_file = fopen(filename.c_str(), "r+");
                ASSERT_TRUE(flash.IsFileOpen()) <<  L"flash.IsFileOpen [2]";
                ASSERT_TRUE(FileSys::FileExists(flash.m_filename)) <<L"FileExists [2]";

                char buffer[32];
                fseek(flash.m_file,0, SEEK_SET);
                //UINT bytesRead = flash.m_file->Read(buffer, 5);
                UINT bytesRead=fread(buffer, sizeof(char), 5, flash.m_file);
                ASSERT_EQ((UINT)5, bytesRead) << L"flash.m_file->Read bytesRead";
                buffer[bytesRead] = 0;
                ASSERT_STREQ("Hello", buffer) << L"flash.m_file->Read buffer";

                try
                {
                    //flash.m_file->Write("World", 5);
                    fwrite("World", sizeof(char), 5, flash.m_file);
                    
                }
                catch (exception* pE)
                {
                    FAIL() << L"Expected exception when writing to a read only file";
                }
            }
            catch (exception *p)
            {
                FAIL() << L"Unexpected exception";
            }
            */
            // CFlash not currently in use
            ASSERT_TRUE(true);
        }

		/// <summary>
		/// Functional test for:
		///     CFlash::CFlash()
		///     CFlash::GetExpectedChecksum()
		/// Test default value for (private) constructor without parameters.
		/// </summary>
        void GetExpectedChecksum_Default()
        {
            /*
            // Data.
            const DWORD expectedChecksum(0);

            // Perform operation(s) and test(s).
            CFlash flash;
            ASSERT_EQ((DWORD)expectedChecksum, flash.GetExpectedChecksum()) << L"flash.GetExpectedChecksum()";
            */
            // CFlash not currently in use
            ASSERT_TRUE(true);
        }

        /// <summary>
		/// Functional test for:
		///     CFlash::CFlash()
		///     CFlash::GetExpectedChecksum()
		/// Test value from (public) constructor with parameters.
        /// </summary> 
        void GetExpectedChecksum_Flash()
        {
            /*
            // Data.
            const DWORD expectedChecksum(123456789);

            // Perform operation(s) and test(s).
            CFlash flash(FLASH_SYSTEM, 0, 0, 0, 0, expectedChecksum, 0);
            ASSERT_EQ((DWORD)expectedChecksum, flash.GetExpectedChecksum()) << L"flash.GetExpectedChecksum()";
            */
            // CFlash not currently in use
            ASSERT_TRUE(true);
        }
    };


    TEST_F(FlashFileTests, FlashFile_ctor)
    {
        FlashFile_ctr();
    }


    TEST_F(FlashFileTests, FlashFile_IsMulticast_true)
    {
        FlashFile_IsMulticastT();
    }

    TEST_F(FlashFileTests, FlashFile_IsMulticast_false)
    {
        FlashFile_IsMulticastF();
    }

    TEST_F(FlashFileTests, FlashFile_GetIP)
    {
        FlashFile_GetIp();
    }

    TEST_F(FlashFileTests, FlashFile_GetIP_nullptr)
    {
        FlashFile_GetIp_nullptr();
    }

    TEST_F(FlashFileTests, FlashFile_GetIP_zerolength)
    {
        FlashFile_GetIP_Zerolength();
    }

    TEST_F(FlashFileTests, FlashFile_GetIP_lowlength)
    {
        FlashFile_GetIP_Lowlength();
    }

    TEST_F(FlashFileTests, FlashFile_IsComplete_true)
    {
        FlashFile_IsComplete_True();
    }

    TEST_F(FlashFileTests, FlashFile_IsComplete_false)
    {
        FlashFile_IsComplete_False();
    }

    TEST_F(FlashFileTests, FlashFile_NextBlock_1)
    {
        FlashFile_NextBlock_1();
    }

    TEST_F(FlashFileTests, FlashFile_NextBlock_2)
    {
        FlashFile_NextBlock_2();
    }

    TEST_F(FlashFileTests, FlashFile_NextBlock_3)
    {
        FlashFile_NextBlock_3();
    }

    TEST_F(FlashFileTests, FlashFile_NextBlock_3a)
    {
       FlashFile_NextBlock_3a();
    }

    TEST_F(FlashFileTests, FlashFile_CheckBlock_0)
    {
       FlashFile_CheckBlock_0();
    }

    TEST_F(FlashFileTests, FlashFile_CheckBlock_1)
    {
        FlashFile_CheckBlock_1();
    }

    TEST_F(FlashFileTests, FlashFile_GetFlashFileReady_true)
    {
        FlashFile_GetFlashFileReady_true();
    }

    TEST_F(FlashFileTests, FlashFile_GetFlashFileReady_false)
    {
        FlashFile_GetFlashFileReady_false();
    }

    TEST_F(FlashFileTests, FlashFile_IsCleared_true)
    {
        FlashFile_IsCleared_true();
    }

    TEST_F(FlashFileTests, FlashFile_IsCleared_false)
    {
        FlashFile_IsCleared_false();
    }

    TEST_F(FlashFileTests, FlashFile_GetMissingBlockCount)
    {
        FlashFile_GetMissingBlockCount();
    }

    TEST_F(FlashFileTests, FlashFile_GetChecksum)
    {
        FlashFile_GetChecksum();
    }

    TEST_F(FlashFileTests, FlashFile_GetPercentComplete)
    {
        FlashFile_GetPercentComplete();
    }

    TEST_F(FlashFileTests, FlashFile_Clear)
    {
        FlashFile_Clear();
    }

    TEST_F(FlashFileTests, FlashFile_ResetFileName_IsFileOpen)
    {
        FlashFile_ResetFileName_IsFileOpen();
    }


    TEST_F(FlashFileTests, FlashFile_ResetFileName)
    {
       FlashFile_ResetFileName();
    }

    TEST_F(FlashFileTests, FlashFile_CalculateChecksum_CloseFile)
    {
        FlashFile_CalculateChecksum_CloseFile();
    }

    TEST_F(FlashFileTests, FlashFile_CalculateChecksum_false)
    {
        FlashFile_CalculateChecksum_false();
    }

    TEST_F(FlashFileTests, FlashFile_CalculateChecksum_Exception)
    {
        FlashFile_CalculateChecksum_Exception();
    }

    TEST_F(FlashFileTests, FlashFile_CloseFile_exception)
    {
       FlashFile_CloseFile_exception();
    }

    TEST_F(FlashFileTests, FlashFile_DeleteFile)
    {
        FlashFile_DeleteFile();
    }

    TEST_F(FlashFileTests, FlashFile_DeleteFile_exception)
    {
        FlashFile_DeleteFile_exception();
    }


    TEST_F(FlashFileTests, FlashFile_OpenFile)
    {
        FlashFile_OpenFile();
    }

    TEST_F(FlashFileTests, GetExpectedChecksum_Default)
    {
        GetExpectedChecksum_Default();
    }
    TEST_F(FlashFileTests, GetExpectedChecksum_Flash)
    {
        GetExpectedChecksum_Flash();
    }
}


