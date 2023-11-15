#include <fstream>
#include <vector>

class FileIndexInfo
{
public:
	std::string _targetFilename;
	int _startPtr;
	int _length;
};

class FilePacker
{
private:
	int32_t	_indexPtr;
	std::vector<FileIndexInfo *> _fileList;
	std::fstream _file;
	bool _indexImported;
	int _readSize = 4096;
	bool _changeMade;
	bool _created;
	
	std::string GetFilenameFromPath(std::string fullPath);
	void GetIndex();
	void WriteIndex();
	void AddDirectoryStubsToIndex();
	void MakeDirectory(std::string fullPath);
		
public:
	FilePacker();
	~FilePacker();
	std::vector<FileIndexInfo *> GetFileList();
	void Changed();
	void SetFileListFilename(int index, std::string strValue);
	void Close();
	void Cancel();
	bool Open(std::string inFilename);
	bool Create(std::string inFilename);
	bool AddFile(std::string inFilename, std::string inTargetName);
	bool ExtractAll(std::string destPath);
};
