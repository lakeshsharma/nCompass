#include "stdafx.h"

#include "FileList.h"


CFileList::CFileList(void)
  : m_path("")
{
}

CFileList::~CFileList(void)
{
    m_fileList.erase(m_fileList.begin(), m_fileList.end());
}

void CFileList::Add(std::string filename, int checksum)
{
	m_criticalSection.lock();
	{
        m_fileList[filename] = checksum;
	}
	m_criticalSection.unlock();
}

void CFileList::Remove(std::string filename)
{
	m_criticalSection.lock();
	{
        m_fileList.erase(filename);
	}
	m_criticalSection.unlock();
}

int CFileList::GetCount()
{
    return m_fileList.size();
}

void CFileList::SetPath(const std::string& pathname)
{
	m_path = pathname;
}

std::string CFileList::GetPath()
{
	return m_path;
}

