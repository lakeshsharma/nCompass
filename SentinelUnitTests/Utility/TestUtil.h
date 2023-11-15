#pragma once
#include "DirectoryManager.h"
#include "Utilities.h"

namespace TestUtil
{
	///	<summary>
	/// Checks if buffer contains only zero bytes.
	///	</summary>
	///	<param name="pBuffer">The buffer to check.</param>
	///	<param name="bufferLen">The size of the buffer in bytes.</param>
	///	<returns><b>true</b> if the buffer is nothing but zeroes, pBuffer is nullptr or
	///		bufferLen is zero (0), else <b>false</b>.</returns>
	bool IsZeroes(const void* pBuffer, size_t bufferLen);

	///	<summary>
	/// Checks if an object contains only zero bytes.
	///	</summary>
	///	<param name="item">The item to check.</param>
	///	<returns><b>true</b> if the buffer is nothing but zeroes, else <b>false</b>.</returns>
	template<class T>
	static bool IsZeroes(const T& item)
	{
		return IsZeroes(&item, sizeof(T));
	}

	///	<summary>
	/// Does a bitwise compare between two objects.
	///	</summary>
	///	<param name="item1">The first object to compare.</param>
	///	<param name="item1">The second object to compare.</param>
	///	<returns><b>true</b> if the objects are bitwise equivalent, else <b>false</b>.</returns>
	template<class T>
	static bool AreBitEqual(const T& item1, const T& item2)
	{
		return memcmp(&item1, &item2, sizeof(T)) == 0;
	}

	///	<summary>
	/// Returns the temp path for these tests.
	///	</summary>
	///	<returns>The temp path for these tests.</returns>
	std::string GetTempPath();

	///	<summary>
	/// Returns a temp file name and path.
	///	</summary>
	///	<param name="pPrefix">The prefix to use when creating the file name.</param>
	///	<param name="pDir">The optional path for the temporary file. If nullptr,
	///		GetTempPath() will be called.</param>
	///	<param name="startVal">The starting value to use when creating the file name.</param>
	///	<returns>The temp file name and path.</returns>
	///	<remarks>
	/// If pPrefix is valid and not a zero length string, the first part of the file
	///	name will consist of a tilde (~) followed by the first seven (7) characters
	///	of pPrefix then a period (.).
	///
	/// If startVal is non-zero (0), the next 4 characters will be a hexadecimal
	/// representation of the startVal. If a file exists with the resulting name,
	/// startVal will be incremented and checked until an unused filename is found.
	///
	/// If startVal is (0), the next 4 characters will be a hexadecimal
	/// representation of a random value.  If a file exists with the resulting name,
	/// a new random value will be generated and checked until an unused filename is
	/// found.
	///	</remarks>
	std::string GetTempName(LPCTSTR pPrefix, LPCTSTR pDir = nullptr, int startVal = 0);

	///	<summary>
	/// Creates a temp file with optional contents.
	///	</summary>
	///	<param name="pPrefix">The prefix to use when creating the file name.</param>
	///	<param name="pDir">The optional path for the temporary file. If nullptr,
	///		GetTempPath() will be called.</param>
	///	<param name="startVal">The starting value to use when creating the file name.</param>
	///	<param name="pContents">An optional pointer to the file contents.</param>
	///	<returns>The pathname of the created file.</returns>
	///	<remarks>
	/// The naming rules are those of GetTempName.
	///	</remarks>
	std::string CreateTempFile(LPCTSTR pPrefix, LPCTSTR pDir = nullptr, int startVal = 0, LPCSTR pContents = nullptr);
	std::string CreateTempDir(LPCTSTR pPrefix, LPCTSTR pDir = nullptr, int startVal = 0);

	struct TempFile
	{
	private:
		std::string filename;

	public:
		TempFile(LPCTSTR pPrefix, LPCTSTR pDir = nullptr, int startVal = 0, LPCSTR pContents = nullptr);
		~TempFile();

		const std::string& GetFilename() const;
		operator LPCTSTR() const;
	};

	struct DeleteOnCloseDir
	{
	private:
		std::string m_drive; // Intended for CDirectoryManager drive values.
		std::string m_path; // Intended for CDirectoryManager path values.
		std::string name; // m_drive + m_path.

		static void DeleteFolderTree (const char* directory_name);
		static int isDirectory (const char* path) ;

	public:
		DeleteOnCloseDir(LPCTSTR pName, bool create = true, bool _delete = true);
		~DeleteOnCloseDir();

		DWORD Create(bool deleteFirst = false) const;
		void Delete() const;
		static void Delete(const std::string &pDir);

		const std::string& GetDrive() const; // Intended for CDirectoryManager drive values.
		const std::string& GetPath() const; // Intended for CDirectoryManager path values.
		const std::string& GetName() const; // GetDrive() + GetPath().
		operator LPCTSTR() const;
	};

	struct TempDir : public DeleteOnCloseDir
	{
	public:
		TempDir(LPCTSTR pPrefix, bool create);
	};

	struct FileInfo
	{
		std::string  pathname;
		std::string  filename;
		DWORD    attributes;
		time_t creationTime;  //ToDo: Not supported on Linux. So decide whether to remove or any alternate
		time_t lastWriteTime;
		uint64_t fileSize;

		FileInfo(LPCTSTR pPath, LPCTSTR file, DWORD atts, time_t creationTime, time_t lastWriteTime, uint64_t sz );
		FileInfo(const FileInfo& other);

		FileInfo& operator=(const FileInfo& other);
	};

	size_t GetFiles(
		LPCTSTR pDir,
		LPCTSTR pMask = _T("*"),
		std::vector<std::unique_ptr<FileInfo>>* pFiles = nullptr,
		bool recurse = false,
		bool saveRelative = true,
		bool includeHidden = false,
		bool includeSystem = false);

	class CMemMapFile
	{
	protected:
#if 000//TODO-PORT
		HANDLE m_hFile;
		HANDLE m_hMap;
#endif//TODO-PORT
		const BYTE* m_pData;

	public:
		CMemMapFile();
		~CMemMapFile();

		const BYTE* Open(LPCTSTR pFile);
		void Close();

		DWORD GetFileLength() const;
		operator const BYTE*() const;
	};

	void WriteFileContents(const std::string &pPathname, const void* pContents, unsigned len);
	void WriteFileContents(const std::string &pPathname, const char* pContents, bool useBinary, unsigned len = UINT_MAX);

	class DeleteOnCloseFile
	{
	private:
		std::string m_pathname;

	public:
#ifdef DIRECTORY_MANAGER_H
		// Creates file and writes contents if not-zero length
		DeleteOnCloseFile(SentinelDirectoryType dirType, LPCTSTR pFilename, const char* pContents, bool useBinary = true);
#endif
		DeleteOnCloseFile(const DeleteOnCloseDir& dir, LPCTSTR pFilename, const char* pContents = nullptr, bool useBinary = true);
		virtual ~DeleteOnCloseFile();

		// Cannot use fatal gtest assertions (which return void) in a constructor, so use an Init() function.
		void Init(const DeleteOnCloseDir& dir, LPCTSTR pFilename, const char* pContents = nullptr, bool useBinary = true);
		void Delete() const;

		void WriteFileContents(const char* pContents, bool useBinary = true);

		const std::string& GetPathname() const;
		operator LPCTSTR() const;
	};

	///	<summary>
	/// Returns the path of the executing test DLL.
	///	</summary>
	///	<returns>A directory path of the executing test DLL.</returns>
	///	<remarks>
	/// This will call a unit testing Assert if a failure occurs.
	///	</remarks>
	const std::string& GetNativeUnitTestPath();

	///	<summary>
	/// Returns the path of the nCompass folder.
	///	</summary>
	///	<returns>A directory path of the nCompass base directory and an optional
	///		subdirectory.</returns>
	///	<remarks>
	/// This will call a unit testing Assert if a failure occurs.
	///	</remarks>
	const std::string& GetnCompassPath();

	///	<summary>
	/// Returns the path of the nCompass folder and appends a path to it.
	///	</summary>
	///	<param name="pRelativePath">An optional pointer to a path relative
	///		to the nCompass folder.</param>
	///	<returns>The path of the nCompass folder with an optional path.</returns>
	///	<remarks>
	/// This will call a unit testing Assert if a failure occurs.
	///	</remarks>
	std::string GetnCompassPath(LPCTSTR pRelativePath);

	///	<summary>
	/// Returns the path of the nCompass\TestFiles folder.
	///	</summary>
	///	<returns>A directory path of the nCompass\TestFiles folder and an optional
	///		subdirectory.</returns>
	///	<remarks>
	/// This will call a unit testing Assert if a failure occurs.
	///	</remarks>
	const std::string& GetTestFilesPath();

#ifdef DIRECTORY_MANAGER_H
	///	<summary>
	/// Creates a directory in the temp folder and sets it in CDirectoryManager.
	///	</summary>
	///	<param name="dirType">The SentinelDirectoryType directory to create in the
	///		temp folder and to set in CDirectoryManager. This must be
	///		DIRECTORY_SENTINELDATA or DIRECTORY_APPLICATIONS.</param>
	///	<remarks>
	/// This will call a unit testing Assert if the SentinelDirectoryType is incorrect.
	///	</remarks>
	void CreateSentinelDirectory(SentinelDirectoryType dirType);

	///	<summary>
	/// Deletes a directory in the temp folder
	///	</summary>
	///	<param name="dirType">The SentinelDirectoryType directory to create in the
	///		temp folder and to set in CDirectoryManager. This must be
	///		DIRECTORY_SENTINELDATA or DIRECTORY_APPLICATIONS.</param>
	void DeleteSentinelDirectory(SentinelDirectoryType dirType);
	void DeleteSentinelDirectories();

	///	<summary>
	/// Create a directory and write data to, or delete, a file.
	///	</summary>
	///	<param name="dirType">The SentinelDirectoryType in which to write the file.
	///		This must be DIRECTORY_SENTINELDATA, DIRECTORY_APPLICATIONS or DIRECTORY_DEFAULTMEDIA.</param>
	///	<param name="pFilename">The name of the file to write.</param>
	///	<param name="pContents">A pointer to the contents. If nullptr, the file will
	///		be deleted.</param>
	///	<param name="useBinary">If <b>true</b> use binary more rather than the default.</param>
	///	<param name="contentLen">The length of the content. If this is UINT_MAX, the 
	///		pContents will be written to a terminating zero, else it will written as
	///		a block.</param>
	/// <returns>The fully qualified pathname which was used</returns>
	///	<remarks>
	/// The file will be written with ofstream or deleted with DeleteFile.
	///
	/// This will call a unit testing Assert if a failure during creation occurs. No
	/// error will occur during a DeleteFile operation.
	///	</remarks>
	std::string WriteFileContents(SentinelDirectoryType dirType, LPCTSTR pFilename, const char* pContents, 
		bool useBinary = true, unsigned contentLen = UINT_MAX);

	///	<summary>
	/// Deletes all files in sentinel directories.
	///	</summary>
	void DeleteSentinelFiles();

	///	<summary>
	/// Deletes all files in a specific directory.
	///	</summary>
	///	<param name="dirType">The SentinelDirectoryType in which to delete the file.</param>
	void DeleteSentinelFiles(SentinelDirectoryType dirType);

	///	<summary>
	/// Instantiates a mock instance of DirectoryManager and sets a directory to
	/// a relative directory to the nCompass base folder.
	///	</summary>
	///	<param name="dirType">The SentinelDirectoryType to set.</param>
	///	<param name="pRelativeDirectory">An existing directory relative to the nCompass base folder.</param>
	///	<remarks>
	/// This will call a unit testing Assert if a failure occurs.
	///	</remarks>
	void SetSentinelDirectory(SentinelDirectoryType dirType, LPCTSTR pRelativeDirectory);
#endif
}