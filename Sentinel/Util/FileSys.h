#pragma once
// Do not put any additional headers in this file.

///	<summary>
///	File System functions.
///	</summary>
namespace FileSys
{
	/// <summary>
	/// Checks whether a file exists.
	/// </summary>
	/// <param name="pFile">A pointer to the possible file.</param>
	///	<returns><b>true</b> if the file exists, else <b>false</b>.</returns>
	bool FileExists(const std::string &pFile);

	/// <summary>
	/// Checks whether a directory exists.
	/// </summary>
	/// <param name="pDir">A pointer to the possible directory.</param>
	///	<returns><b>true</b> if the directory exists, else <b>false</b>.</returns>
	bool DirExists(const std::string &pDir);

	/// <summary>
	/// Tries to create a directory path if it does not exist.
	/// </summary>
	/// <param name="pDir">A pointer to the possible directory path.</param>
	///	<returns><b>true</b> if the directory exists upon return, else <b>false</b>.</returns>
	bool CreateDirectory(const std::string &pDir);

	void RemoveDirectory(const std::string &pDir);
}
