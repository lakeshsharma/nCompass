#pragma once
#if 000//TODO-PORT
/// <summary>
/// CMemFile for testing generic output.
/// </summary>
class CMemFileTest : public CMemFile
{
public:
	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CMemFileTest"/> class.
	/// </summary>
	CMemFileTest(const BYTE *fileContents = NULL, size_t fileLength = 0);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CMemFileTest"/> class.
	/// </summary>
	virtual ~CMemFileTest();

	/// <summary>
	/// Opens the specified file name.
	/// </summary>
	/// <param name="lpszFileName">IN - Name of the file.</param>
	/// <param name="nOpenFlags">IN - The flags for opening the file.</param>
	/// <param name="pError">IN/OUT - A pointer to an exception.</param>
	/// <returns></returns>
	virtual BOOL Open(
		LPCTSTR lpszFileName,
		UINT nOpenFlags,
		CFileException* pError = NULL);

	/// <summary>
	/// Closes this instance.
	/// </summary>
	virtual void Close();

	CString m_fileName;
	UINT m_nOpenFlags;
	ULONGLONG m_fileLengthOnOpen;
	DWORD m_openCount;
	DWORD m_closeCount;
};
#endif //TODO-PORT