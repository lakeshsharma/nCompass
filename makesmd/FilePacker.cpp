#include "FilePacker.h"
#include "sys/stat.h"
#include <iostream>
#include <iterator>
#include <algorithm>

using namespace std;

	FilePacker::FilePacker()
	{
	}
	
	FilePacker::~FilePacker()
	{
	}
	
	std::string FilePacker::GetFilenameFromPath(std::string fullPath)
	{
		int index = fullPath.find_last_of('/');
		return fullPath.substr(index + 1);
	}
	
	void FilePacker::GetIndex()
	{
		_file.seekg(_indexPtr, fstream::beg);
		__int32_t fileCount;
		_file >> fileCount;
		
		FileIndexInfo *info;
		for (int i = 0; i < fileCount; i++)
		{
			info = new FileIndexInfo();
			_file >> info->_targetFilename;
			_file >> info->_startPtr;
			_file >> info->_length;
			_fileList.push_back(info);
		}
		
		_indexImported = true;
	}
	
	void FilePacker::WriteIndex()
	{
		if (_file.is_open())
		{
			_file.seekg(0, fstream::beg);
			_file.write((const char *)&_indexPtr, 4);
			_file.seekg(_indexPtr, fstream::beg);
			int fileCount = _fileList.size();
			_file.write((const char *)&fileCount, 4);
			
			for_each(_fileList.cbegin(), _fileList.cend(), [&](FileIndexInfo *info)
			{
				uint8_t nameLength = info->_targetFilename.length();
				_file.write((const char *)&nameLength, 1);
				_file.write(info->_targetFilename.c_str(), nameLength);
				_file.write((const char *)&info->_startPtr, 4);
				_file.write((const char *)&info->_length, 4);
			}
			);
		}
	}
	
//	void FilePacker::AddDirectoryStubsToIndex();
//	void FilePacker::MakeDirectory(std::string fullPath);
		
	std::vector<FileIndexInfo *> FilePacker::GetFileList()
	{
		return _fileList;
	}
	
	void FilePacker::Changed()
	{
		_changeMade = true;
	}
	
	void FilePacker::SetFileListFilename(int index, std::string strValue)
	{
		_fileList[index]->_targetFilename = strValue;
		_changeMade = true;
	}
	
	void FilePacker::Close()
	{
		if (_file.is_open())
		{
			if (_changeMade && _created)
			{
				WriteIndex();
			}
			_file.close();
		}
		
		// empty file list
		
		_created = false;
		_indexPtr = 0;
		_changeMade = false;
	}
	
	void FilePacker::Cancel()
	{
		_changeMade = false;
		Close();
	}

	bool FilePacker::Open(std::string inFilename)
	{
		_file.open(inFilename, fstream::in | fstream::out | fstream::binary);
		_file.seekg(0, ios_base::beg);
		_file >> _indexPtr;
		GetIndex();
		
		return true;
	}
	
	bool FilePacker::Create(std::string inFilename)
	{
		bool retVal = true;
		if (_file.is_open())
		{
			Close();
		}
		
		_created = true;
		_indexPtr = 4; // the byte right after the indexptr
		
		try
		{
			_file.open(inFilename, fstream::in | fstream::out | fstream::binary | fstream::trunc);
			_file.seekg(0, fstream::beg);
		}
		catch (...)
		{
			retVal = false;
		}
		
		_indexImported = true;
		return retVal;
	}
	
	bool FilePacker::AddFile(std::string inFilename, std::string inTargetName)
	{
		struct stat fd;
		
		if (stat(inFilename.c_str(),&fd) != 0)
		{
			cout << "Could not open file " << inFilename << endl;
			return false;
		}
		
		if (!_indexImported)
		{
			return false;
		}
		
		FileIndexInfo *info = new FileIndexInfo();
		info->_length = fd.st_size;
		info->_startPtr = _indexPtr;
		info->_targetFilename = inTargetName;
		
		_file.seekg(_indexPtr, fstream::beg);
		ifstream inStream(inFilename.c_str(), std::ios::binary);
		std::copy(istreambuf_iterator<char>(inStream),
					 istreambuf_iterator<char>(),
					 ostreambuf_iterator<char>(_file));
		_indexPtr += info->_length;
		
		_fileList.push_back(info);
		_changeMade = true;
		
		return true;
		
		
	}
	
	bool FilePacker::ExtractAll(std::string destPath)
	{
		if (_fileList.size() == 0)
		{
			return false;
		}
		
		ofstream unpack;
		char buffer[_readSize];
		for_each(_fileList.cbegin(), _fileList.cend(), [&](FileIndexInfo *info)
		{
			if (info->_length > 0)
			{
				// set the package pointer
				_file.seekg(info->_startPtr, fstream::beg);
				
				// open the file to be extracted to:
				// assume current directory for now
				unpack.open(info->_targetFilename.c_str(), ios_base::out | ios_base::trunc | ios_base::binary);
				int bytesToWrite = info->_length;
				int bytesRead;
				int lastBytes;
				
				while (bytesToWrite > 0)
				{
					if (bytesToWrite >  _readSize)
					{
						bytesRead = _file.read(buffer, _readSize).gcount();
					}
					else
					{
						lastBytes = bytesToWrite;
						bytesRead = _file.read(buffer, lastBytes).gcount();
					}
					
					unpack.write(buffer, bytesRead);
					bytesToWrite -= bytesRead;
				}
				
				unpack.close();
			}
		}
		);
		

		return true;
		
	}

