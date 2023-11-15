#include <iostream>
#include <sstream>
#include <memory>
#include <stdlib.h>
#include <string.h>
#include "FilePacker.h"

using namespace std;

string system_exec(const char* cmd)
{
	char buffer[128];
	string result;
	unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	if (!pipe)
	{
		throw std::runtime_error("popen() failed!");
	}

	while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr)
	{
		result += buffer;
	}
	return result;
}

int main(int argc, char* argv[])
{
	vector<string> fileList;
	string upgradeSmd;
	string packageSmd;

	if (argc < 4)
	{
		printf("\nInvalid number of arguments.");
		return 1;
	}

	if (!strcmp(argv[1], "-r"))
	{
		// Mode: [makesmd -r <UpgradeSmdName> <PackageSmdName>]
		// New mode: we need to pack contents of the current directory in recursive mode
		upgradeSmd = argv[2];
		packageSmd = argv[3];

		// Frame fileList
		string fileListFromShell = system_exec("find . -type f ");
		istringstream fileListStream(fileListFromShell);
		string file;
		while (getline(fileListStream, file))
		{
			fileList.push_back(file);
		}

		// Create manifest.sha1 file
		// File paths in manifest.sha1 added here begins with ./ but this
		// is compatible with the parsing and manifest validation logic
		if (0 != system("find . -type f -not -path ./manifest.sha1 -exec sha1sum '{}' + | sort > manifest.sha1"))
		{
			return 1;
		}
	}
	else
	{
		// Mode: [makesmd <one-or-more-file-names> <UpgradeSmdName> <PackageSmdName>]
		// Old mode: pack the files mentioned in the following arguments.
		upgradeSmd = argv[argc - 2];
		packageSmd = argv[argc - 1];
		stringstream shasumCmd;
		shasumCmd << "sha1sum";

		for (int i = 1; i < argc - 2; i++)
		{
			fileList.push_back(argv[i]);
			shasumCmd << " " << argv[i];
		}

		shasumCmd << " > manifest.sha1";

		// Create manifest.sha1 with input files specified
		if (0 != system(shasumCmd.str().c_str()))
		{
			return 1;
		}
	}

	FilePacker upgradeSmdPack;

	if (!upgradeSmdPack.Create(upgradeSmd))
	{
		return 1;
	}

	if (!upgradeSmdPack.AddFile("manifest.sha1", "manifest.sha1"))
	{
		return 1;
	}

	for (string file : fileList)
	{
		if (!upgradeSmdPack.AddFile(file, file))
		{
			return 1;
		}
	}

	upgradeSmdPack.Close();

	// Pack S5Upgrade SMD into target file
	FilePacker pakageSmdPack;

	if (!pakageSmdPack.Create(packageSmd))
	{
		return 1;
	}

	if (!pakageSmdPack.AddFile(upgradeSmd, upgradeSmd))
	{
		return 1;
	}

	pakageSmdPack.Close();

	return (0);
}

