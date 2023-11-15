#pragma once
// Do not put any additional headers in this file.

///	<summary>
///	Path functions.
///	</summary>
namespace Path
{
	const TCHAR BackSlash = _T('\\');
	const TCHAR Slash = _T('/');
	const TCHAR Colon = _T(':');
	const TCHAR Asterisk = _T('*');

	///	<summary>
	///	Indicates whether a character is a forward (/) or backward (\) slash.
	///	</summary>
	///	<param name="ch">The character to check.</param>
	///	<returns><b>true</b> if the passed character is a forward (/) or backward (\) slash.</returns>
	bool IsSlash(TCHAR ch);

	///	<summary>
	///	Indicates whether a character is a colon (:) or slash (/\).
	///	</summary>
	///	<param name="ch">The character to check.</param>
	///	<returns><b>true</b> if the passed character is a colon (:) or slash (/\).</returns>
	bool IsDelimeter(TCHAR ch);

	///	<summary>
	/// Appends a slash to the passed path if one is not already present.
	///	</summary>
	///	<param name="path">The path to check and possibly change.</param>
	///	<returns>A pointer to the resulting string.</returns>
	const std::string &AddSlashString(std::string& path);

	///	<summary>
	///	Removes a slash from the passed path if one is present.
	///	</summary>
	///	<param name="path">The path to check and possibly change.</param>
	///	<returns>A pointer to the resulting string.</returns>
	const std::string &RemoveSlashString(std::string& path);

	///	<summary>
	/// Appends a slash to a path if one is not already present.
	///	</summary>
	///	<param name="pPath">The path to check.</param>
	///	<param name="reserve">Additional amount to reserve on return string.</param>
	///	<returns>A string instance containing the path.</returns>
	std::string AddSlash(LPCTSTR pPath);

	///	<summary>
	///	Removes a slash from a path if one is present.
	///	</summary>
	///	<param name="pPath">The path to check.</param>
	///	<returns>A string instance containing the path.</returns>
	std::string RemoveSlash(LPCTSTR pPath);

	///	<summary>
	///	Appends a file to a path.
	///	</summary>
	///	<param name="pDir">The directory.</param>
	///	<param name="pFile">The file to append to the path. If nullptr, a star will be
	///		appended.</param>
	///	<returns>The string of the path with the appended pFile. If pDir is nullptr
	///		the string will be empty.</returns>
	///	<remarks>
	///	</remarks>
	std::string AppendFile(LPCTSTR pDir, LPCTSTR pFile);
}
