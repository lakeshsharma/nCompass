#pragma once

#include "InternalMsgContent.h"

class CFileList :
	public CInternalMsgContent<INT_MSG_CONTENT_FILELIST>
{
public:
	CFileList(void);
	~CFileList(void);

	void Add(std::string filename, int checksum);
	void SetPath(const std::string& pathname);
	void Remove(std::string filename);

	int GetCount();
	std::string GetPath();

    template<typename Func>
    void ForEach(Func func)
    { 
        for (std::map<std::string, int>::const_iterator itr = m_fileList.begin(), end = m_fileList.end(); itr != end; ++itr )
        {
            std::string filename = itr->first;
            int checksum = itr->second;
            func( filename, checksum);
        }
    }
private:
    std::map<std::string, int> m_fileList;
	std::string m_path;
	std::recursive_mutex m_criticalSection;
};
