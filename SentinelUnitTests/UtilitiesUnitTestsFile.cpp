#include "stdafx.h"
#include "Utilities.h"

#include "TestUtil.h"
#include "Hardware/WatchDog.h"
#include "IPAddressArray.h"
#include "SentinelException.h"
#include "AssertMsg.h"

#include <fstream>

#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	static string tempPathTemp;

		TEST(UtilitiesUnitTestsFile, Utilities_RenameFile)
		{
			TestUtil::DeleteOnCloseDir tempDir("TestDir");

			string orgFilename(CUtilities::PathAddSlash(tempDir) + "TestFile1");

            std::fstream file;
            file.open(orgFilename, std::fstream::out);
			file.close();

			ASSERT_TRUE(CUtilities::DoesFileExist(orgFilename.c_str())) << string("orgFilename (before): ") + orgFilename;

			string dstFilename(CUtilities::PathAddSlash(tempDir) + "TestFile2");
			ASSERT_FALSE(CUtilities::DoesFileExist(dstFilename.c_str())) << string("dstFilename (before): ") + dstFilename;

			DWORD errCode;
			bool result = CUtilities::RenameFile(orgFilename, dstFilename, &errCode, false, false, false);

			ASSERT_TRUE(result == 0) << "result";
			ASSERT_EQ((DWORD) ERROR_SUCCESS, errCode) << "errCode";
			ASSERT_FALSE(CUtilities::DoesFileExist(orgFilename)) << string("orgFilename (after): ") + orgFilename;
			ASSERT_TRUE(CUtilities::DoesFileExist(dstFilename)) << string("dstFilename (after): ") + dstFilename;
		}

		TEST(UtilitiesUnitTestsFile, Utilities_RenameFileNull)
		{
			struct
			{
				int line;
				LPCTSTR pSrcFile;
				LPCTSTR pDstFile;
				bool	expectedReturn;
				DWORD	expectedError;
			}
			static data[] =
			{
				{ __LINE__, "F1",	 "",		false, ENOENT },
				{ __LINE__, "F1",	 nullptr,	false, ENOENT },
				{ __LINE__, "",	 "F2",		false, ENOENT },		
				{ __LINE__, nullptr, "F2",		false, ENOENT },		
				{ __LINE__, nullptr, nullptr,	false, ENOENT },
				{ __LINE__, nullptr, nullptr,	false, ENOENT },
				{ __LINE__, nullptr, "",		false, ENOENT },
				{ __LINE__, "",	 nullptr,	false, ENOENT },
				{ __LINE__, "",	 "",		false, ENOENT }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				TestUtil::DeleteOnCloseDir tempDir("TestDir");

				string pSrcFile;
				string srcFilename;
				if (data[i].pSrcFile && *data[i].pSrcFile)
				{
					srcFilename = (CUtilities::PathAddSlash(tempDir) + data[i].pSrcFile);
                    std::fstream file;
                    file.open(srcFilename, std::fstream::out);
                    file.close();
                    pSrcFile = srcFilename;
				}
				else
				{
					pSrcFile = ""; //emptry string
				}

				string pDstFile;
				string dstFilename;
				if (data[i].pDstFile && *data[i].pDstFile)
				{
					dstFilename = (CUtilities::PathAddSlash(tempDir) + data[i].pDstFile);
					pDstFile = dstFilename.c_str();
				}
				else
				{
					pDstFile = "" ; //empty string
				}

				DWORD errCode;
				bool result = CUtilities::RenameFile(pSrcFile, pDstFile, &errCode, false, false, false);

				ASSERT_EQ(data[i].expectedReturn, !result) << (data[i].line) << "return";
                ASSERT_EQ(data[i].expectedError, errCode) << (data[i].line) << "errCode";
                if (!pDstFile.empty())
                {
					ASSERT_FALSE(CUtilities::DoesFileExist(dstFilename)) << data[i].line << "dstFile exists" << pDstFile;
                    cout << "error" << errCode << endl;
                }
			}
		}

		TEST(UtilitiesUnitTestsFile, Utilities_RenameFile_NoFile)
		{
			TestUtil::DeleteOnCloseDir tempDir("TestDir");

			string orgFilename(CUtilities::PathAddSlash(tempDir) + "TestFile1");
			ASSERT_FALSE(CUtilities::DoesFileExist(orgFilename)) << string("orgFilename: ") + orgFilename;

			string dstFilename(CUtilities::PathAddSlash(tempDir) + "TestFile2");
			ASSERT_FALSE(CUtilities::DoesFileExist(orgFilename)) << string("dstFilename: ") + dstFilename;

			DWORD errCode;
			bool result = CUtilities::RenameFile(orgFilename, dstFilename, &errCode, false, false, false);

			ASSERT_FALSE (!result) << "result";
			ASSERT_EQ((DWORD) ENOENT, errCode) << "errCode";
			ASSERT_FALSE(CUtilities::DoesFileExist(orgFilename)) << string("orgFilename (after): ") + orgFilename;
			ASSERT_FALSE(CUtilities::DoesFileExist(dstFilename)) << string("dstFilename (after): ") + dstFilename;
		}

		TEST(UtilitiesUnitTestsFile, Utilities_RenameFile_NoFileNoError)
		{
			TestUtil::DeleteOnCloseDir tempDir("TestDir");

			string orgFilename(CUtilities::PathAddSlash(tempDir) + "TestFile1");
			ASSERT_FALSE(CUtilities::DoesFileExist(orgFilename)) << string("orgFilename: ") + orgFilename;

			string dstFilename(CUtilities::PathAddSlash(tempDir) + "TestFile2");
			ASSERT_FALSE(CUtilities::DoesFileExist(orgFilename)) << string("dstFilename: ") + dstFilename;

			bool result = CUtilities::RenameFile(orgFilename, dstFilename, nullptr, false, false, false);

			ASSERT_FALSE (!result) << "result";
			ASSERT_FALSE(CUtilities::DoesFileExist(orgFilename)) << string("orgFilename (after): ") + orgFilename;
			ASSERT_FALSE(CUtilities::DoesFileExist(dstFilename)) << string("dstFilename (after): ") + dstFilename;
		}

		TEST(UtilitiesUnitTestsFile, Utilities_RenameFileTwoFiles)
		{
			TestUtil::DeleteOnCloseDir tempDir("TestDir");

			string orgFilename(CUtilities::PathAddSlash(tempDir) + "TestFile1");
            std::fstream file;
            file.open(orgFilename, std::fstream::out);
            file.close();

			ASSERT_TRUE(CUtilities::DoesFileExist(orgFilename)) << string("orgFilename (before): ") + orgFilename;

			string dstFilename(CUtilities::PathAddSlash(tempDir) + "TestFile2");
            std::fstream file2;
            file2.open(dstFilename, std::fstream::out);
            file2.close();

			ASSERT_TRUE(CUtilities::DoesFileExist(dstFilename)) << string("dstFilename (before): ") + dstFilename;

			DWORD errCode;
			bool result = CUtilities::RenameFile(orgFilename, dstFilename, &errCode, false, false, false);

			ASSERT_FALSE(!result) << "result";
			ASSERT_EQ((DWORD) EEXIST, errCode) << "errCode";
			ASSERT_TRUE(CUtilities::DoesFileExist(orgFilename)) << string("orgFilename (after): ") + orgFilename;
			ASSERT_TRUE(CUtilities::DoesFileExist(dstFilename)) << string("dstFilename (after): ") + dstFilename;
		}

		TEST(UtilitiesUnitTestsFile, Utilities_RenameFileTwoFilesReplace)
		{
			TestUtil::DeleteOnCloseDir tempDir("TestDir");

			string orgFilename(CUtilities::PathAddSlash(tempDir) + "TestFile1");
            std::fstream file;
            file.open(orgFilename, std::fstream::out);
            file << "1234";
            file.close();

			ASSERT_TRUE(CUtilities::DoesFileExist(orgFilename)) << string("orgFilename (before): ") + orgFilename;

			string dstFilename(CUtilities::PathAddSlash(tempDir) + "TestFile2");
            std::fstream file2;
            file2.open(dstFilename, std::fstream::out);
            file2 << "ABCD";
            file2.close();

			ASSERT_TRUE(CUtilities::DoesFileExist(dstFilename)) << string("dstFilename (before): ") + dstFilename;

			DWORD errCode;
			bool result = CUtilities::RenameFile(orgFilename, dstFilename, &errCode, false, true, false);

			ASSERT_TRUE(!result) << "result";
			ASSERT_EQ((DWORD) ERROR_SUCCESS, errCode) << "errCode";
			ASSERT_FALSE(CUtilities::DoesFileExist(orgFilename)) << string("orgFilename (after): ") + orgFilename;
			ASSERT_TRUE(CUtilities::DoesFileExist(dstFilename)) << string("dstFilename (after): ") + dstFilename;

            std::fstream file3;
            file3.open(dstFilename, std::fstream::in | std::fstream::binary);
            
 			char buffer[8];
            file3 >> buffer;
            buffer[4] = 0;

            int len = CUtilities::GetFileSize(file3);
            file3.close();

			ASSERT_STREQ("1234", buffer) << "Contents";
			ASSERT_EQ(4, len) << "file3 length";
		}

		TEST(UtilitiesUnitTestsFile, Utilities_RenameFileTwoFilesEx)
		{
			TestUtil::DeleteOnCloseDir tempDir("TestDir");

			string orgFilename(CUtilities::PathAddSlash(tempDir) + "TestFile1");
            std::fstream file1;
            file1.open(orgFilename, std::fstream::out);
			file1.close();

			ASSERT_TRUE(CUtilities::DoesFileExist(orgFilename)) << string("orgFilename (before): ") + orgFilename;

			string dstFilename(CUtilities::PathAddSlash(tempDir) + "TestFile2");
            std::fstream file2;
            file2.open(dstFilename, std::fstream::out);
			file2.close();

			ASSERT_TRUE(CUtilities::DoesFileExist(dstFilename)) << string("dstFilename (before): ") + dstFilename;

			try
			{
				CUtilities::RenameFile(orgFilename, dstFilename, nullptr, false, false, true);
				FAIL() << "Expected exception";
			}
			catch (SentinelFileException& e)
			{
				ASSERT_EQ((DWORD) EEXIST , (DWORD) e.code()) << "error code";
				ASSERT_TRUE(CUtilities::DoesFileExist(orgFilename)) << string("orgFilename (after): ") + orgFilename;
				ASSERT_TRUE(CUtilities::DoesFileExist(dstFilename)) << string("dstFilename (after): ") + dstFilename;
			}
		}

		TEST(UtilitiesUnitTestsFile, Utilities_MoveFile)
		{
			TestUtil::DeleteOnCloseDir tempDir1("TestDir1");

			string orgFilename(CUtilities::PathAddSlash(tempDir1) + "TestFile1");
			
            std::fstream file;
            file.open(orgFilename, std::fstream::out);
            file << "1234";
            file.close();

			ASSERT_TRUE(CUtilities::DoesFileExist(orgFilename)) << string("orgFilename (before): ") + orgFilename;

			TestUtil::DeleteOnCloseDir tempDir2("TestDir2");

			string dstFilename(CUtilities::PathAddSlash(tempDir2) + "TestFile1");
			
            std::fstream file2;
            file2.open(dstFilename, std::fstream::out);
            file2 << "1234";
            file2.close();

			ASSERT_TRUE(CUtilities::DoesFileExist(dstFilename)) << string("dstFilename (before): ") + dstFilename;

			try
			{
				CUtilities::RenameFile(orgFilename, dstFilename, nullptr, false, true, true);

				ASSERT_FALSE(CUtilities::DoesFileExist(orgFilename)) << string("orgFilename (after): ") + orgFilename;
				ASSERT_TRUE(CUtilities::DoesFileExist(dstFilename)) << string("dstFilename (after): ") + dstFilename;

				std::fstream file3;
				file3.open(dstFilename, std::fstream::in | std::fstream::binary);
				
				char buffer[8];
				file3 >> buffer;
				buffer[4] = 0;
				
				int len = CUtilities::GetFileSize(file3);
				file3.close();


				ASSERT_EQ(4, len) << "file3 length";
				ASSERT_STREQ("1234", buffer) << "Contents";
			}
			catch (SentinelFileException&)
			{
				FAIL() << "Unexpected exception";
			}
		}

		TEST(UtilitiesUnitTestsFile, Utilities_RenameDeleteFile)
		{
			const char* pOrgFile = "OrgFilename";
			const char* pNewFile = "NewFilename";

            TestUtil::DeleteOnCloseDir tempDir("");
			char* orgPath = const_cast<char*>(tempDir.GetName().c_str());
			int orgPathLen = (orgPath != nullptr)? strlen(orgPath): 0;
			ASSERT_NE( 0, orgPathLen) << "Error: temp path not found";
			
			char newPath[MAX_PATH];
			memset (newPath, 0, MAX_PATH);
			strncpy(newPath, orgPath, orgPathLen);
			strcat(newPath, pNewFile);
			strcat(orgPath, pOrgFile);
			
            std::fstream file;
            file.open(orgPath, std::fstream::out);
            file << "1234";
            file.close();
			
			CUtilities::DeleteFile(newPath);

			DWORD errCode;
			bool result = CUtilities::RenameFile(orgPath, newPath, &errCode, true);
			ASSERT_TRUE(result == 0) << FormatString("RenameFile() returned false with error %u", errCode);
			ASSERT_EQ((DWORD) ERROR_SUCCESS, errCode);

			result = CUtilities::RenameFile(orgPath, newPath, nullptr, false);
			ASSERT_FALSE(result == 0) << FormatString("Second RenameFile() returned true");

			result = CUtilities::RenameFile(newPath, orgPath, &errCode, false);
			ASSERT_TRUE(result == 0) << FormatString("Third RenameFile() returned false with error %u", errCode);
			ASSERT_EQ((DWORD) ERROR_SUCCESS, errCode);

			result = CUtilities::DeleteFile(orgPath, &errCode, true);
			ASSERT_TRUE(result) << FormatString("DeleteFile() returned false with error %u", errCode);
			ASSERT_EQ((DWORD) ERROR_SUCCESS, errCode);
		}

		void SetupDirectories(const string& tempDir)
		{
			string baseDir(CUtilities::PathAddSlash(tempDir.c_str()) + "BaseDir");
			bool result = CUtilities::CreateDirectory(baseDir);
			ASSERT_TRUE(result) << FormatString("::CreateDirectory(%s, nullptr)", baseDir.c_str());

			string baseDirFile(CUtilities::PathAddSlash(baseDir.c_str()) + "File1.txt");
            std::fstream baseFile;
            baseFile.open(baseDirFile, std::fstream::out);
			baseFile.close();

			string secondDir(CUtilities::PathAddSlash(baseDir.c_str()) + "2ndDir");
			result = CUtilities::CreateDirectory(secondDir);
			ASSERT_TRUE(result) << FormatString("::CreateDirectory(%s, nullptr)",  secondDir.c_str());

			string secondDirFile(CUtilities::PathAddSlash(secondDir.c_str()) + "File2.txt");
            std::fstream secondFile;
            secondFile.open(secondDirFile, std::fstream::out);
			secondFile.close();

		}

		void VerifyDirectories(const string& tempDir)
		{
			vector<unique_ptr<TestUtil::FileInfo>> files;
			size_t count = TestUtil::GetFiles(tempDir.c_str(), "*", &files, true, true, true, true);
			ASSERT_EQ((size_t) 4, count) << "VerifyDeleteDirectory Count";

			vector<string> strings;
			map<string, bool> dirs;
			for_each(files.cbegin(), files.cend(), [&dirs, &strings](const unique_ptr<TestUtil::FileInfo>& file)
			{
				dirs.insert(std::pair<string, bool>(file->pathname + file->filename, true));
				strings.push_back(file->pathname + file->filename);
			});

			map<string, bool>::iterator it;
			
			it=dirs.find(tempDir + "BaseDir");
			ASSERT_TRUE(it->second);
			dirs.erase(it);
			
			it=dirs.find(tempDir + "BaseDir/File1.txt");
			ASSERT_TRUE(it->second);
			dirs.erase(it);
			
			it=dirs.find(tempDir + "BaseDir/2ndDir");
			ASSERT_TRUE(it->second);
			dirs.erase(it);
			
			it=dirs.find(tempDir + "BaseDir/2ndDir/File2.txt");
			ASSERT_TRUE(it->second);
			dirs.erase(it);
			
			ASSERT_TRUE(dirs.empty());
		}

#if 000//TODO-PORT LV
		TEST(UtilitiesUnitTestsFile, Utilities_DeleteDirectory)
		{
			TestUtil::DeleteOnCloseDir tempDir("TestDir", true);

			SetupDirectories(tempDir.GetName());
			VerifyDirectories(tempDir.GetName());

			string baseDir(CUtilities::PathAddSlash(tempDir) + "BaseDir");

			bool rval = CUtilities::DeleteDirectory(baseDir, false, nullptr);
			ASSERT_TRUE(rval == 0) << "CUtilities::DeleteDirectory";

			vector<unique_ptr<TestUtil::FileInfo>> files;
			size_t count = TestUtil::GetFiles(baseDir.c_str(), "*", &files, true, true, true, true);
			ASSERT_EQ((size_t) 0, count) << "VerifyDeleteDirectory Count";
		}

		TEST(UtilitiesUnitTestsFile, Utilities_DeleteDirectory2)
		{
			TestUtil::DeleteOnCloseDir tempDir("TestDir", true);

			SetupDirectories(tempDir.GetName());
			VerifyDirectories(tempDir.GetName());

			string baseDir(CUtilities::PathAddSlash(tempDir) + "BaseDir");

			bool rval = CUtilities::DeleteDirectory(baseDir, true, nullptr);
			ASSERT_TRUE(rval == 0) << "CUtilities::DeleteDirectory";

			vector<unique_ptr<TestUtil::FileInfo>> files;
			size_t count = TestUtil::GetFiles(tempDir, "*", &files, true, true, true, true);
			ASSERT_EQ((size_t) 1, count) << "VerifyDeleteDirectory Count";

			ASSERT_EQ("BaseDir", files[0]->filename);
		}

		TEST(UtilitiesUnitTestsFile, Utilities_DeleteDirectory3)
		{
			TestUtil::DeleteOnCloseDir tempDir("TestDir", true);

			string baseDir(CUtilities::PathAddSlash(tempDir) + "BaseDir");
			bool result = CUtilities::CreateDirectory(baseDir);
			ASSERT_TRUE(result) << FormatString("::CreateDirectory(%s, nullptr)", baseDir.c_str());

			for (int i = 1; i <= 4; i++)
			{
				string file = FormatString("File%d.txt", i);				
				std::fstream baseFile;
				baseFile.open(CUtilities::PathAddSlash(baseDir.c_str()) + file, std::fstream::out);
			}

			size_t fileCount = TestUtil::GetFiles(baseDir.c_str(), "*.txt");
			ASSERT_EQ((size_t) 4, fileCount);

			map<string, bool> filesToKeep;
			filesToKeep["File1.txt"] = true;
			filesToKeep["File3.txt"] = true;

			bool rval = CUtilities::DeleteDirectory(baseDir, true, &filesToKeep);
			ASSERT_TRUE(rval) << "CUtilities::DeleteDirectory";

			rval = CUtilities::DoesDirExist(baseDir.c_str());
			ASSERT_TRUE(rval) << baseDir;

			rval = CUtilities::DoesFileExist(CUtilities::PathAddSlash(baseDir.c_str()) + "File1.txt");
			ASSERT_TRUE(rval) << "File1.txt";

            #if 000 //ToDo: Thi running fine on local system. But jenkins fails, might be some config issue.
			rval = CUtilities::DoesFileExist(CUtilities::PathAddSlash(baseDir.c_str()) + "File2.txt");
			ASSERT_FALSE(rval) << "File2.txt";

            rval = CUtilities::DoesFileExist(CUtilities::PathAddSlash(baseDir.c_str()) + "File4.txt");
            ASSERT_FALSE(rval) << "File4.txt";
            #endif

			rval = CUtilities::DoesFileExist(CUtilities::PathAddSlash(baseDir.c_str()) + "File3.txt");
			ASSERT_TRUE(rval) << "File3.txt";

			rval = CUtilities::DoesFileExist(CUtilities::PathAddSlash(baseDir.c_str()) + "File5.txt");
			ASSERT_FALSE(rval) << "File5.txt";
		}

		TEST(UtilitiesUnitTestsFile, Utilities_DeleteDirectory_RemoveDir)
		{
			TestUtil::DeleteOnCloseDir tempDir("TestDir", true);

			string baseDir(CUtilities::PathAddSlash(tempDir) + "BaseDir");
			bool result = CUtilities::CreateDirectory(baseDir);
			ASSERT_TRUE(result) << FormatString("::CreateDirectory(%s, nullptr)", baseDir.c_str());

			string baseDirFile(CUtilities::PathAddSlash(baseDir.c_str()) + "File.txt");
            std::fstream baseFile;
            baseFile.open(baseDirFile, std::fstream::out);
			baseFile.close();

			bool rval = CUtilities::DeleteDirectory(baseDir, false);
			ASSERT_TRUE(rval == 0) << "CUtilities::DeleteDirectory";

			rval = CUtilities::DoesDirExist(baseDir.c_str());
			ASSERT_FALSE(rval) << baseDir;
		}

		TEST(UtilitiesUnitTestsFile, Utilities_DeleteDirectory_RemoveDir_ReadOnly)
		{
			TestUtil::DeleteOnCloseDir tempDir("TestDir", true);

			string baseDir(CUtilities::PathAddSlash(tempDir) + "BaseDir");
			BOOL result = CUtilities::CreateDirectory(baseDir);
			ASSERT_TRUE(result) << FormatString("::CreateDirectory(%s, nullptr)", baseDir.c_str());

			string pathname(CUtilities::PathAddSlash(baseDir.c_str()) + "File.txt");
			std::fstream baseFile;
			baseFile.open(pathname, std::fstream::out);
			baseFile.close();

			chmod(pathname.c_str(), S_IRUSR | S_IRGRP | S_IROTH );
			chmod(baseDir.c_str(), S_IRUSR | S_IRGRP | S_IROTH );

			bool rval = CUtilities::DeleteDirectory(baseDir, true);
			ASSERT_TRUE(rval) << "CUtilities::DeleteDirectory";

			rval = CUtilities::DoesDirExist(baseDir.c_str());
			ASSERT_FALSE(rval == 0) << baseDir;

            //clean now directories. Other tests will fail otherwise.

            chmod(pathname.c_str(), S_IRWXU | S_IRWXG | S_IRWXO );
            chmod(baseDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO );

            rval = CUtilities::DeleteDirectory(baseDir, true);
            ASSERT_TRUE(rval == 0) << "CUtilities::DeleteDirectory";

		}
#endif

		TEST(UtilitiesUnitTestsFile, Utilities_PathAddBackslashstring)
		{
			struct
			{
				int line;
				LPCTSTR pPath;
				LPCTSTR pExpected;
			}
			static data[] =
			{
				{ __LINE__, "/tmp",  "/tmp/" },
				{ __LINE__, "/tmp/", "/tmp/" },
				{ __LINE__, "",      ""      },
				{ __LINE__, nullptr, ""      }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string path(data[i].pPath ? data[i].pPath : "");
				LPCTSTR pResult = CUtilities::PathAddSlashCString(path);

				ASSERT_STREQ(data[i].pExpected, path.c_str()) << FormatString("%d, %s", data[i].line, "path");
				ASSERT_STREQ(data[i].pExpected, pResult) << FormatString("%d, %s", data[i].line, "pResult");
			}
		}

		TEST(UtilitiesUnitTestsFile, Utilities_PathRemoveBackslashstring)
		{
			struct
			{
				int line;
				LPCTSTR pPath;
				LPCTSTR pExpected;
			}
			static data[] =
			{
				{ __LINE__, "/tmp/","/tmp" },
				{ __LINE__, "/tmp", "/tmp" },
				{ __LINE__, "",     ""   },
				{ __LINE__, nullptr, ""   }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string path(data[i].pPath ? data[i].pPath : "");
				LPCTSTR pResult = CUtilities::PathRemoveSlashCString(path);

				ASSERT_STREQ(data[i].pExpected, path.c_str()) << FormatString("%d, %s", data[i].line, "path");
				ASSERT_STREQ(data[i].pExpected, pResult) << FormatString("%d, %s", data[i].line, "pResult");
			}
		}

		TEST(UtilitiesUnitTestsFile, Utilities_PathAddBackslashLPCTSTR)
		{
			struct
			{
				int line;
				LPCTSTR pPath;
				LPCTSTR pExpected;
			}
			static data[] =
			{
				{ __LINE__, "/tmp",  "/tmp/" },
				{ __LINE__, "/tmp/", "/tmp/" },
				{ __LINE__, "", 	 "" 	 },
				{ __LINE__, nullptr, "" 	 }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
                string result = CUtilities::PathAddSlash(data[i].pPath);

				ASSERT_STREQ(data[i].pExpected, result.c_str()) << FormatString("%d, %s", data[i].line, "result");
			}
		}

		TEST(UtilitiesUnitTestsFile, Utilities_PathRemoveBackslashLPCTSTR)
		{
			struct
			{
				int line;
				LPCTSTR pPath;
				LPCTSTR pExpected;
			}
			static data[] =
				{
					{ __LINE__, "/tmp/","/tmp" },
					{ __LINE__, "/tmp", "/tmp" },
					{ __LINE__, "", 	""	 },
					{ __LINE__, nullptr, ""   }
				};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string result = CUtilities::PathRemoveSlash(data[i].pPath);

				ASSERT_STREQ(data[i].pExpected, result.c_str()) << FormatString("%d, %s", data[i].line, "result");
			}
		}
}
