#include "stdafx.h"
#include "Path.h"

// Sentinel includes.
#include "Utilities.h"

using namespace std;

namespace Path
{
	const bool useForwardSlash(true);

	bool IsSlash(TCHAR ch)
	{
		return ch == BackSlash || ch == Slash;
	}

	bool IsDelimeter(TCHAR ch)
	{
		return IsSlash(ch) || ch == Colon;
	}

	const std::string &AddSlashString(std::string& path)
	{
		if (!path.empty() && !IsSlash(path[path.length() - 1]))
			path += useForwardSlash ? Slash : BackSlash;

		return path;
	}

	const std::string &RemoveSlashString(std::string& path)
	{
		if (!path.empty() && IsSlash(path[path.length() - 1]))
			path.resize(path.length() - 1);

		return path;
	}

	std::string AddSlash(LPCTSTR pPath)
	{
		string newPath;
		if (pPath != nullptr && *pPath != 0)
		{
			newPath = pPath;
			AddSlashString(newPath);
		}
		return newPath;
	}

	std::string RemoveSlash(LPCTSTR pPath)
	{
		string newPath;
		if (pPath != nullptr && *pPath != 0)
		{
			newPath = pPath;
			RemoveSlashString(newPath);
		}
		return newPath;
	}

	std::string AppendFile(LPCTSTR pDir, LPCTSTR pFile)
	{
		string newPath = Path::AddSlash(pDir);

		if (newPath.empty())
		{
			CUtilities::SetLastError(ENOENT);
		}
		else if (pFile == nullptr)
		{
			newPath += Asterisk;
		}
		else
		{
			newPath += pFile;
		}

		return newPath;
	}
}
