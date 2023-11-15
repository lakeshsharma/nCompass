#include "stdafx.h"
#include "TestUtil.h"

// Sentinel includes.
#include "Util/Convert.h"
#include "Util/Path.h"
#include "Util/FileSys.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;

namespace TestUtil
{
	bool IsZeroes(const void* pBuffer, size_t bufferLen)
	{
		bool rval = true;
		if (pBuffer != nullptr)
		{
			const byte* p = (const byte*) pBuffer;
			size_t i = 0;
			while (i < bufferLen && p[i] == 0)
				i++;

			rval = i == bufferLen;
		}
		return rval;
	}

	const int MaxTempName = 7;
	static unique_ptr<DeleteOnCloseDir> testTempDir;

	const char* pTempDirPrefix = "nCompass.";

	std::string GetTempPath()
	{
		if (!testTempDir)
		{
			string pathBuffer;

			for (uint32_t i = 1; i < UINT32_MAX; i++)
			{
				pathBuffer = Path::AddSlash(P_tmpdir) + FormatString("%X", i);
				if (FileSys::CreateDirectory(pathBuffer.c_str()))
					break;
			}

			testTempDir.reset(new DeleteOnCloseDir(pathBuffer.c_str(), false, false));
		}
		return testTempDir->GetName();
	}

	std::string GetTempName(LPCTSTR pPrefix, LPCTSTR pDir, int startVal)
	{
		string pathBuffer;
		if (pDir == nullptr || *pDir == 0)
		{
			pathBuffer = P_tmpdir;
		}
		else
		{
			pathBuffer = pDir;
		}
		Path::AddSlashString(pathBuffer);

		pathBuffer += '~';
		if (pPrefix && *pPrefix)
		{
			if (*pPrefix == '~')
				pPrefix++;

			for (int i = 0; i < MaxTempName && pPrefix[i];)
				pathBuffer += pPrefix[i++];

			pathBuffer += '.';
		}

		bool doRand = startVal == 0;

		for (int i = 0; i < USHRT_MAX; i++)
		{
			string tempName(pathBuffer);
			tempName += FormatString("%04X", doRand ? (rand() & MICROSOFT_RAND_MAX) : startVal++);

			struct stat statInfo;
			if (0 != stat(tempName.c_str(), &statInfo))
				return string(tempName);
		}

		return string();
	}

	std::string CreateTempFile(LPCTSTR pPrefix, LPCTSTR pDir, int startVal, LPCSTR pContents)
	{
		string tempName = GetTempName(pPrefix, pDir, startVal);
		if (pContents)
		{
			ofstream file(tempName, ofstream::out | ofstream::trunc);
			file.write(pContents, strlen(pContents));
		}
		return tempName;
	}

	std::string CreateTempDir(LPCTSTR pPrefix, LPCTSTR pDir, int startVal)
	{
		string tempName = GetTempName(pPrefix, pDir, startVal);
		FileSys::CreateDirectory(tempName);
		return tempName;
	}

	TempFile::TempFile(LPCTSTR pPrefix, LPCTSTR pDir, int startVal, LPCSTR pContents)
	{
		filename = CreateTempFile(pPrefix, pDir, startVal, pContents);
	}

	TempFile::~TempFile()
	{
		::unlink(filename.c_str());
	}

	const std::string& TempFile::GetFilename() const
	{
		return filename;
	}

	TempFile::operator LPCTSTR() const
	{
		return filename.c_str();
	}

	DeleteOnCloseDir::DeleteOnCloseDir(LPCTSTR pName, bool create, bool _delete)
	{
		if (pName[0] == _T('/') || pName[1] == _T(':'))
		{
			name = Path::AddSlash(pName);
		}
		else
		{
			if (Path::IsSlash(*pName))
				pName++;

			name = GetTempPath() + Path::AddSlash(pName);
		}

		if (_delete)
			Delete();

		if (create)
			Create();
	}

	DeleteOnCloseDir::~DeleteOnCloseDir()
	{
		Delete();
	}

	DWORD DeleteOnCloseDir::Create(bool deleteFirst) const
	{
		if (deleteFirst)
			Delete();

		return FileSys::CreateDirectory(name) == 0 ? CUtilities::GetLastError() : 0;
	}

	void DeleteOnCloseDir::Delete(const std::string &pDir)
	{
#if 000//TODO-PORT - See CUtilities::DeleteDirectory().
		string dir(pDir);
		dir += char(0);

		SHFILEOPSTRUCT fileOp;
		ZeroMemory(&fileOp, sizeof(fileOp));

		fileOp.wFunc = FO_DELETE;
		fileOp.pFrom = dir;
		fileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;

		int result = SHFileOperation(&fileOp);
		result = result;
#else//TODO-PORT
		DeleteFolderTree(pDir.c_str());
		//CUtilities::DeleteDirectory(pDir, true, true, false, nullptr);
#endif//TODO-PORT
	}


	void DeleteOnCloseDir::DeleteFolderTree (const char* directory_name) {
	    DIR*            dp;
	    struct dirent*  ep;
	    char            p_buf[512] = {0};

	    dp = opendir(directory_name);
		if (dp == nullptr)
			return;

	    while ((ep = readdir(dp)) != NULL) {
			/* Skip . and .. */
			if (!strcmp(ep->d_name, ".") || !strcmp(ep->d_name, ".."))
			{
				continue;
			}
	        sprintf(p_buf, "%s/%s", directory_name, ep->d_name);
	        if (isDirectory(p_buf))
	            DeleteFolderTree(p_buf);
	        else
	            unlink(p_buf);
	    }

	    closedir(dp);
	    rmdir(directory_name);
	}

	int DeleteOnCloseDir::isDirectory (const char* path) {
	    struct stat s_buf;

	    if (stat(path, &s_buf))
	        return 0;

	    return S_ISDIR(s_buf.st_mode);
	}


	void DeleteOnCloseDir::Delete() const
	{
		Delete(name);
	}

	const std::string& DeleteOnCloseDir::GetName() const
	{
		return name;
	}

	DeleteOnCloseDir::operator LPCTSTR() const
	{
		return name.c_str();
	}

	TempDir::TempDir(LPCTSTR pPrefix, bool create)
		: DeleteOnCloseDir(GetTempName(pPrefix).c_str(), create)
	{
	}


	FileInfo::FileInfo(LPCTSTR pPath, LPCTSTR file, DWORD atts, time_t creationTime, time_t lastWriteTime, uint64_t sz )
		: pathname(pPath)
		, filename(file)
		, attributes(atts)
		, creationTime(creationTime)
		, lastWriteTime(lastWriteTime)
		, fileSize(sz)
	{
	}


	FileInfo::FileInfo(const FileInfo& other)
		: pathname(other.pathname)
		, filename(other.filename)
		, attributes(other.attributes)
		, creationTime(other.creationTime)
		, lastWriteTime(other.lastWriteTime)
		, fileSize(other.fileSize)
	{
	}

	FileInfo& FileInfo::operator=(const FileInfo& other)
	{
		if (this != &other)
		{
			pathname = other.pathname;
			filename = other.filename;
			attributes = other.attributes;
			creationTime = other.creationTime;
			lastWriteTime = other.lastWriteTime;
			fileSize = other.fileSize;
		}
		return *this;
	}

	size_t GetFiles(
		LPCTSTR pPath,
		LPCTSTR pMask,
		vector<unique_ptr<FileInfo>>* pFiles,
		bool recurse,
		bool saveRelative,
		bool includeHidden,
		bool includeSystem)
	{
	
		int len = 0, count = 0;
		struct dirent *ep;
		DIR *pDir;
	
		pDir = opendir(pPath);
		if (pDir != nullptr) {
			while ((ep = readdir(pDir)) != NULL) {
				if ( !strcmp(ep->d_name, ".") || !strcmp(ep->d_name, "..") )
				{
				     continue;
				}
				struct stat statBuf;
				if( stat(pPath, &statBuf) < 0 )
           			return 0;

                string newPath = CUtilities::PathAddSlash(pPath);

				if (DT_DIR == ep->d_type){
                    count++;
					
					string newFile = newPath + ep->d_name;
					
					if (pFiles != nullptr)
						pFiles->push_back(move(unique_ptr<FileInfo>(new FileInfo(newPath.c_str(), ep->d_name, ep->d_type, statBuf.st_ctime, statBuf.st_mtime, statBuf.st_size))));
					
                    int ret = GetFiles(newFile.c_str(), pMask, pFiles, recurse, saveRelative,includeHidden, includeSystem);
					count+=ret;
				}
				else{
					len = 0;
					len = strlen (ep->d_name);
					if (len >= 4) {
						std::string ext(ep->d_name + (len-4), 4); 
		
						if (!strcmp(pMask, "*") || ext.compare(pMask+1) == 0) {
							
							if (pFiles != nullptr)
								pFiles->push_back(move(unique_ptr<FileInfo>(new FileInfo(newPath.c_str(), ep->d_name, ep->d_type, statBuf.st_ctime, statBuf.st_mtime, statBuf.st_size))));
							
							count++;
						}
					}
				}	
			}	
			closedir (pDir);
		}
		return count;
	}
	
#if 000//TODO-PORT
	size_t GetFiles(
		LPCTSTR pDir,
		LPCTSTR pMask,
		vector<unique_ptr<FileInfo>>* pFiles,
		bool recurse,
		bool saveRelative,
		bool includeHidden,
		bool includeSystem)
	{
		size_t count = 0;

		if (pDir != nullptr && *pDir != 0)
		{
			string baseDir(Path::RemoveSlash(pDir));
			size_t baseDirLen = baseDir.length() + 1;

			vector<string> dirs;
			dirs.push_back(baseDir);

			while (dirs.size() > 0)
			{
				string dir = dirs.at(0);
				dirs.erase(dirs.begin());

				dir += Path::Slash;

				WIN32_FIND_DATA findData;
				HANDLE hFind = ::FindFirstFile(dir + pMask, &findData);
				if (hFind != INVALID_HANDLE_VALUE)
				{
					do
					{
						int i = 0;
						while (findData.cFileName[i] == _T('.'))
							i++;

						if (findData.cFileName[i] != 0 ||
							((findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) && !includeHidden) ||
							((findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) && !includeSystem))
						{
							count++;

							if (pFiles != nullptr)
								pFiles->push_back(move(unique_ptr<FileInfo>(new FileInfo(saveRelative ? dir.Mid(baseDirLen) : dir, findData))));

							if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && recurse)
								dirs.InsertAt(0, dir + findData.cFileName);
						}

					} while (::FindNextFile(hFind, &findData));

					::FindClose(hFind);
				}
			}
		}

		return count;
	}

	CMemMapFile::CMemMapFile()
		: m_hFile(INVALID_HANDLE_VALUE)
		, m_hMap(nullptr)
		, m_pData(nullptr)
	{
	}

	CMemMapFile::~CMemMapFile()
	{ 
		Close();
	}

	DWORD CMemMapFile::GetFileLength() const
	{ 
		return ::GetFileSize(m_hFile, nullptr);
	}

	CMemMapFile::operator const BYTE*() const
	{ 
		return m_pData;
	}

	const BYTE* CMemMapFile::Open(LPCTSTR pFile)
	{
		Close();

		m_hFile = CreateFile(pFile, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
		if (m_hFile == INVALID_HANDLE_VALUE)
			return nullptr;

		LARGE_INTEGER length;
		if (!GetFileSizeEx(m_hFile, &length))
			return nullptr;

		const LONGLONG ONE_MEGABYTE = 1024 * 1024;

		if (length.QuadPart > ONE_MEGABYTE * 512)
		{
			SetLastError(ERROR_FILE_INVALID);
		}
		else
		{
			m_hMap = ::CreateFileMapping(m_hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
			if (m_hMap != nullptr)
				m_pData = (const BYTE*) ::MapViewOfFile(m_hMap, FILE_MAP_READ, 0, 0, 0);
		}

		if (m_pData == nullptr)
		{
			DWORD err = GetLastError();
			Close();
			SetLastError(err);
		}

		return m_pData;
	}

	void CMemMapFile::Close()
	{
		::UnmapViewOfFile(m_pData);
		m_pData = nullptr;

		CloseHandle(m_hMap);
		m_hMap = nullptr;

		CloseHandle(m_hFile);
		m_hFile = nullptr;
	}
#endif//TODO-PORT

	void WriteFileContents(const std::string &pPathname, const void* pContents, unsigned len)
	{
		if (pContents == nullptr)
		{
			::unlink(pPathname.c_str());
		}
		else
		{
			ios_base::openmode mode = ios_base::out | ios_base::trunc | ios_base::binary;

			ofstream fs(pPathname, mode);
			DWORD err = CUtilities::GetLastError();
			ASSERT_TRUE((bool) fs) << AssertMsg::FormatOnly(L"Error (%u) creating %s", err, pPathname.c_str());

			fs << pContents;
			err = CUtilities::GetLastError();
			((bool) fs, AssertMsg::FormatOnly(L"Error (%u) writing %s", err, pPathname.c_str()));
		}
	}

	void WriteFileContents(const std::string &pPathname, const char* pContents, bool useBinary, unsigned len)
	{
		if (pContents == nullptr)
		{
			::unlink(pPathname.c_str());
		}
		else
		{
			ios_base::openmode mode = ios_base::out | ios_base::trunc;
			if (useBinary)
				mode |= ios_base::binary;

			ofstream fs(pPathname, mode);
			DWORD err = CUtilities::GetLastError();
			ASSERT_TRUE((bool) fs) << AssertMsg::FormatOnly(L"Error (%u) creating %s", err, pPathname.c_str());

			if (len < UINT_MAX)
			{
				fs.write((const char*) pContents, len);
			}
			else if (*pContents != 0)
			{
				fs.write((const char*) pContents, strlen(pContents));
				err = CUtilities::GetLastError();
				ASSERT_TRUE((bool) fs) << AssertMsg::FormatOnly(L"Error (%u) writing %s", err, pPathname.c_str());
			}
		}
	}

	DeleteOnCloseFile::DeleteOnCloseFile(const DeleteOnCloseDir& dir, LPCTSTR pFilename, const char* pContents, bool useBinary)
	{
		Init(dir, pFilename, pContents, useBinary);
	}
	
	void DeleteOnCloseFile::Init(const DeleteOnCloseDir& dir, LPCTSTR pFilename, const char* pContents, bool useBinary)
	{
		if (dir.GetName().empty())
		{
			FAIL() << L"DeleteOnCloseFile dir was empty";
		}

		ASSERT_NE((void *)nullptr, (void *)pFilename) << L"DeleteOnCloseFile pFilename was nullptr";

		m_pathname = dir.GetName() + pFilename;
		TestUtil::WriteFileContents(m_pathname, pContents, useBinary);
	}

	DeleteOnCloseFile::~DeleteOnCloseFile()
	{
		Delete();
	}

	void DeleteOnCloseFile::Delete() const
	{
		::unlink(m_pathname.c_str());
	}

	void DeleteOnCloseFile::WriteFileContents(const char* pContents, bool useBinary)
	{
		TestUtil::WriteFileContents(m_pathname, pContents, useBinary);
	}

	const std::string& DeleteOnCloseFile::GetPathname() const
	{
		return m_pathname;
	}

	DeleteOnCloseFile::operator LPCTSTR() const
	{
		return m_pathname.c_str();
	}

	static string nCompassPath;
	static string testFilesPath;
	static string nativeUnitTestPath;


	static void SetTestPaths()
	{
		if (nativeUnitTestPath.empty())
		{
			char buf[PATH_MAX + 1] = {};
			size_t len = readlink("/proc/self/exe", buf, sizeof(buf));
			if ((size_t)-1 == len)
			{
				int err = errno;
				FAIL() << AssertMsg::FormatOnly("SetTestPaths failed readlink() errno=%d", err);
			}
			string nativeUnitTestPath(buf, len);

			int offset = nativeUnitTestPath.rfind(Path::Slash);
			if (offset > 0)
			{
				nativeUnitTestPath.resize(offset + 1);
			}

#ifdef _DEBUG
			string subStr(_T("/SentinelUnitTests/Debug/"));
#else
			string subStr(_T("/SentinelUnitTests/Release/"));
#endif
			offset = nativeUnitTestPath.find(subStr);
			ASSERT_TRUE(offset > 0) << "GetSentinelPath could not find \'" + subStr + "\' in path " + nativeUnitTestPath;

			nCompassPath = nativeUnitTestPath.substr(0, offset + 1);
			testFilesPath = nCompassPath + _T("TestFiles/");

			struct stat statInfo;
			if (0 != stat(testFilesPath.c_str(), &statInfo) || !S_ISDIR(statInfo.st_mode))
			{
				FAIL() << "Could not locate TestFiles directory path=\"" << testFilesPath << "\"";
			}
		}
	}

	const std::string& GetNativeUnitTestPath()
	{
		SetTestPaths();
		return nativeUnitTestPath;
	}

	const std::string& GetnCompassPath()
	{
		SetTestPaths();
		return nCompassPath;
	}

	std::string GetnCompassPath(LPCTSTR pRelativePath)
	{
		string pathName(GetnCompassPath() + (pRelativePath != nullptr ? pRelativePath : _T("")));
		return Path::AddSlashString(pathName);
	}

	const std::string& GetTestFilesPath()
	{
		SetTestPaths();
		return testFilesPath;
	}

#ifdef DIRECTORY_MANAGER_H

	DeleteOnCloseFile::DeleteOnCloseFile(SentinelDirectoryType dirType, LPCTSTR pFilename, const char* pContents, bool useBinary)
		: m_pathname(TestUtil::WriteFileContents(dirType, pFilename, pContents, useBinary))
	{
	}

	struct
	{
		LPCTSTR pName;
		unique_ptr<DeleteOnCloseDir> dir;

		LPCTSTR Reset()
		{
			dir.reset(new DeleteOnCloseDir(pName));
			return dir->GetName().c_str();
		}

		void Delete()
		{
			dir.reset();
		}

		void DeleteFiles()
		{
			if (dir)
			{
				dir->Create(true);
			}
		}
	}
	static dirs[] =
	{
		{ nullptr			, nullptr },
		{ "App"			, nullptr },
		{ "DefMedia"		, nullptr },
		{ "DefMediaTmp"	, nullptr },
		{ "MWMedia"		, nullptr },
		{ "MWMediaTmp"		, nullptr },
		{ "SMContent"		, nullptr },
		{ "Upgrade"		, nullptr },
		{ "Sentinel"		, nullptr },
		{ "LcdMedia"		, nullptr },
		{ "LcdMediaTmp"	, nullptr },
		{ "Error"			, nullptr },
		{ "MediaWnd"		, nullptr },
		{ "MediaWndTmp"	, nullptr },
		{ "LcdWnd"			, nullptr },
		{ "LcdWndTmp"		, nullptr },
		{ "PL"				, nullptr },
		{ "PLTmp"			, nullptr }
	};

	// Fatal gtest assertions return void, so use a function that returns void.
	static void AssertTrue(bool condition, const string message)
	{
		ASSERT_TRUE(condition) << message;
	}

	static bool CheckDirType(SentinelDirectoryType dirType, LPCTSTR pCaller)
	{
		bool isValid = dirType >= DIRECTORY_APPLICATIONS && dirType < NUM_OF_DIRECTORY_TYPES;
		AssertTrue(isValid, AssertMsg::FormatOnly(_T("TestUtil::%s: dirType is out of range."), pCaller));
		return isValid;
	}

	void GetDriveAndPathFromName(const std::string &name, std::string &drive, std::string &path)
	{
		// Try to separate drive and path on the 2nd slash.
		size_t driveLength(0);
		{
			size_t pathIndex = name.find(Path::Slash);
			if (pathIndex != string::npos)
			{
				driveLength = pathIndex;
				pathIndex = name.find(Path::Slash, pathIndex + 1);
				if (pathIndex != string::npos)
				{
					driveLength = pathIndex;
				}
			}
		}

		drive = name.substr(0, driveLength);
		path = name.substr(driveLength);

	}

	void CreateSentinelDirectory(SentinelDirectoryType dirType)
	{
		if (CheckDirType(dirType, __FUNCTION__))
		{
			CDirectoryManager::MockInstance();

			if (!dirs[dirType].dir)
			{
				string name(dirs[dirType].Reset());
				string drive;
				string path;
				GetDriveAndPathFromName(name, drive, path);
				CDirectoryManager::MockSetPath(dirType, drive, path);
			}
		}
	}

	void DeleteSentinelDirectory(SentinelDirectoryType dirType)
	{
		if (CheckDirType(dirType, __FUNCTION__))
		{
			CDirectoryManager::MockResetPath(dirType);
			dirs[dirType].Delete();
		}
	}

	void DeleteSentinelDirectories()
	{
		CDirectoryManager::MockInstanceCleanup();
		for (size_t i = 0; i < _countof(dirs); i++)
		{
			dirs[i].Delete();
		}
	}

	void DeleteSentinelFiles(SentinelDirectoryType dirType)
	{
		if (CheckDirType(dirType, __FUNCTION__))
		{
			dirs[dirType].DeleteFiles();
		}
	}

	void DeleteSentinelFiles()
	{
		for (size_t i = 0; i < _countof(dirs); i++)
		{
			dirs[i].DeleteFiles();
		}
	}

	std::string WriteFileContents(SentinelDirectoryType dirType, LPCTSTR pFilename, const char* pContents, bool useBinary, unsigned contentLen)
	{
		CreateSentinelDirectory(dirType);

        string pathname = CDirectoryManager::Instance().GetWorkingPath(dirType);
		if (pFilename != nullptr && *pFilename != 0)
		{
			pathname += pFilename;
			WriteFileContents(pathname, pContents, useBinary, contentLen);
		}

		return pathname;
	}

	void SetSentinelDirectory(SentinelDirectoryType dirType, LPCTSTR pRelativeDirectory)
	{
		CDirectoryManager::MockInstance();

		string name(GetnCompassPath(pRelativeDirectory));
		string drive;
		string path;
		GetDriveAndPathFromName(name, drive, path);
		CDirectoryManager::MockSetPath(dirType, drive, path);
	}
#endif
}
