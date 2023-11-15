#pragma once
class ChkDskHandler
{
public:
	static const LPCTSTR CHKDSKCOMMAND;

	//Constructors/Deconstructors
	/// <summary>
	/// Creates ChkDskHandler
	/// </summary>
	/// <param name="directories">Array of directories that should be empty unless disk was corrupt.</param>
	/// <param name="dircount">Number of elements in the 'directories' array.</param>
	ChkDskHandler(const std::string* directories, unsigned dirCount);

	/// <summary>
	/// Obligatory virtual deconstructor
	/// </summary>
	virtual ~ChkDskHandler(void);

	/// <summary>
	/// Checks if given 'directories' are not empty or SMD files are still around, indicating that the disk may be 
	/// corrupted and need fsck to be run.
	/// </summary>
	/// <returns>Returns a bool to indicate if given 'directories' are empty.</returns>
	bool IsChkdskRequired(void);

private:
	const std::string* m_directories;
	unsigned m_dirCount;
};

