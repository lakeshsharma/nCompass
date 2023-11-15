#include "stdafx.h"
#include "FileSys.h"

// Sentinel includes.
#include "Path.h"

using namespace std;

namespace FileSys
{
	bool FileExists(const std::string &pFile)
	{
		bool exists(false);

		struct stat statInfo;
		if (0 == stat(pFile.c_str(), &statInfo))
		{
			exists = S_ISREG(statInfo.st_mode);
		}

		return exists;
	}

	bool DirExists(const std::string &pPath)
	{
		bool exists(false);

		struct stat statInfo;
		if (0 == stat(pPath.c_str(), &statInfo))
		{
			exists = S_ISDIR(statInfo.st_mode);
		}

		return exists;
	}

	bool CreateDirectory(const std::string &pPath)
	{
		bool exists(DirExists(pPath));

		if (!exists)
		{
			bool parentExists = true; // Default to parent directory exists until determined otherwise.
			size_t index(pPath.rfind(Path::Slash));
			if (index != string::npos && index > 0)
			{
				// Recursive call to create parent directory.
				parentExists = CreateDirectory(pPath.substr(0, index));
			}

			// If parent directory exists then try to create the specified directory.
			if (parentExists)
			{
				mode_t mode(S_IRWXU | S_IRWXG | S_IRWXO);
				int mkdirRet = mkdir(pPath.c_str(), mode);
				exists = 0 == mkdirRet || EEXIST == errno;
			}
		}

		return exists;
	}

	void RemoveDirectory(const std::string &pPath)
	{
		bool isDir(DirExists(pPath));

		if(isDir)
		{			
			DIR 			*dirp;
			struct dirent	*dent;
			std::string		 filePath;
						

		    // if possible to read the directory 
			if((dirp = opendir(pPath.c_str())) != NULL) 
			{
				while( (dent = readdir(dirp)) != NULL ) 
				{
					bool isDots = ((strcmp(dent->d_name,".") == 0) || (strcmp(dent->d_name,"..") == 0));
					if(isDots)
						continue;
	
					filePath = pPath + string("/") + string(dent->d_name);
					if (dent->d_type == DT_DIR) {
						RemoveDirectory(filePath);
					} else {
						unlink(filePath.c_str());
					}
				}
				closedir(dirp);
			}
		}	
		rmdir(pPath.c_str());
	}

}
