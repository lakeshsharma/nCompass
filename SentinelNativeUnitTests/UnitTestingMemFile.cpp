#if 000//TODO-PORT
#include "stdafx.h"
#include "UnitTestingMemFile.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

CMemFileTest::CMemFileTest(const BYTE *fileContents, size_t fileLength) :
	m_nOpenFlags(0),
	m_fileLengthOnOpen(0),
	m_openCount(0),
	m_closeCount(0)
{
	if (fileContents == nullptr)
	{
		fileLength = 0;
	}

	Write(fileContents, fileLength);
	SeekToBegin();
}

CMemFileTest::~CMemFileTest()
{
}

BOOL CMemFileTest::Open(
	LPCTSTR lpszFileName,
	UINT nOpenFlags,
	CFileException* pError)
{
	m_fileName = lpszFileName;
	m_nOpenFlags = nOpenFlags;
	m_fileLengthOnOpen = GetLength();
	++m_openCount;
	return TRUE;
}

void CMemFileTest::Close()
{
	++m_closeCount;
}
#endif //TODO-PORT