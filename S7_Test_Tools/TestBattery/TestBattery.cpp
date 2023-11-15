
// Project includes
#include "TestBattery.h"
#include "BatteryTestParts.h"

using namespace std;

FILE *log_file;

int main (int argc, char *argv[])
{
	S7_Result s7Result;
	std::string result;
	BatteryTestParts btp;
	bool Testloop = true;

	// verify that a test option has been selected
	if (argc <= 0)
	{
		displayUsage("Error: Too few Arguments.");
		exit(-1);
	}

	// Process command line arguments
	for (int i = 1; i < argc; i++)
	{
		if (strcmp("-v", argv[i]) == 0) 
		{ // Version
			dispayVersion();
		}
		else if ((strcmp("-h", argv[i]) == 0) || (strcmp("-u", argv[i]) == 0))
		{ // Usage or help
			displayUsage("");
			exit(0);
		}
		else if (strcmp("-l", argv[i]) == 0) 
		{
			Testloop = false;
		} else {
			displayUsage("Error: unknown argument: " + string(argv[i]));
			exit(-1);
		}
	}
	
	// Start the Test
	//
	try
	{
		char cptr[1024];
		//log_file = fopen("/opt/ncompass/mediadisk/TestBattery_log.txt", "a");

		// Initialize the battery Test
		s7Result = btp.initialize();


		// Run the Test
		result = btp.RunTest();
		strcpy(cptr, result.c_str());
		printf((char *)"%s\n", cptr);
		
		// End of the Battery Test; clean up any open ports or log files
		s7Result = btp.finalize();

		//fclose(log_file);
	}
	catch (const std::exception& e) 
	{
		printf("Unexpected Exception encountered - %s\n", e.what());
		return (-1);
	}
	return (0);
}

void displayUsage(std::string errorMessage)
{
	// Display the usage error message if it available
	if ((errorMessage != "") && (errorMessage.length() > 0))
	{
		printf("%s\n", errorMessage.c_str());
	}
	//
	// Display the general usage message
	printf("\n");
	printf("Usage: BatteryTest [-h][-v]\n");
	printf("\t-h) displays this usage display.\n");
	printf("\t-v) displays the version of this test utility.\n");
	printf("\n");
}

void dispayVersion()
{
	printf("Version: BatteryTest V1.0\n");
}

void prtf(char *fmt, char *chk, ...)
{
	va_list ap;
	va_start(ap, chk);
	char buff[20];
	char prtFmt[128];
	char buffer[1024];

	time_t now = time(NULL);
	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
	memset(prtFmt, 0, sizeof(prtFmt));
	if ((chk == NULL) || (strlen(chk) <= 0)) 
	{
		strcat(prtFmt, (char *)"\r");
	} else {
		strcat(prtFmt, chk);
	}
	strcat(prtFmt, buff);
	strcat(prtFmt, ": ");
	strcat(prtFmt, fmt);
	vsprintf(buffer, prtFmt,  ap);
	printf(buffer);
	fprintf(log_file, buffer);
	fflush(stdout);
	fflush(log_file);
	va_end(ap);
}

