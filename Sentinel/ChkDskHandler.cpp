#include "stdafx.h"
#include "ChkDskHandler.h"
#include "Utilities.h"

#include <dirent.h>


using namespace std;

ChkDskHandler::ChkDskHandler(const std::string* directories, unsigned dirCount)
	: m_directories(directories), m_dirCount(dirCount)
{
}

ChkDskHandler::~ChkDskHandler(void)
{
}

bool ChkDskHandler::IsChkdskRequired(void)
{
	bool chkdskRequired = false;
	
	DIR             *dirp;
    struct dirent   *dent;
	struct stat 	 stBuff;
	std::string		 filePath;
	
	for (unsigned dirIndex = 0; dirIndex < m_dirCount && !chkdskRequired; dirIndex++)
	{
		
		string files( m_directories[dirIndex] );
		
		if((dirp = opendir(files.c_str())) != NULL) 
		{				
			// Catch everything
			bool fileFound = true;
				
			while(fileFound && !chkdskRequired && ( (dent = readdir(dirp)) != NULL )) 
			{
                bool isDots = ((strcmp(dent->d_name,".") == 0) || (strcmp(dent->d_name,"..") == 0));
                if(isDots)   //if . or .. then continue
                    continue;

				filePath = files + string("/") + string(dent->d_name);

				if(stat(filePath.c_str(), &stBuff) < 0) {
					fileFound = false;
				}
				
				if ( (fileFound) || // Not . or .. or last file
					 ( 5 == errno ) ) // File not found because of I/O error (5)
				{
					// found something that shouldn't be there.
					chkdskRequired = true;
				}
			} 

			closedir(dirp);
		}
	}

	return chkdskRequired;
}

