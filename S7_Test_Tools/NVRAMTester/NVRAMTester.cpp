
#include "NVRAMTester.h"
#include "NVRAMTesterParts.h"

using namespace std;
#define PST (-8);

FILE *log_file;

int main (int argc, char *argv[])
{
	string result;
	int loops = -1;
	char buffer[256];
	bool manualInput = false;
	NVRAMTesterParts ntp;

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
			exit(-1);
		} 
		else if (strcmp("-m", argv[i]) == 0) 
		{ // support manual input
			manualInput = true;
		} 
		else if (strcmp("-n", argv[i]) == 0)
		{
			try
			{
				if (i+1 < argc)
				{
					loops = atoi(argv[++i]);
				} else {
					throw("invalid Value.");
				}
			}
			catch (exception &ex)
			{
				sprintf(buffer, "Error: unexpect reason %s [%s]", ex.what(), argv[i-1]);
				displayUsage(string (buffer));
				exit(-1);
			}
		}
		else 
		{
			displayUsage("Error: unknown argument: " + string(argv[i]));
			exit(-1);
		}
	}

	// Start the Test
	//
	try
	{
		log_file = fopen("/opt/ncompass/mediadisk/nvram_log.txt", "a");
		// Run the Test
		while ((loops < 0) || (loops > 0))
		{
			// Initialize the NVRAMTester Test
			ntp.initialize();

			if (loops > 0) 
			{
				--loops;
			}
			result = ntp.RunTest(manualInput);

			// End of the NVRAM Test; clean up any open ports or log files
			ntp.finalize();
		}
	}
	catch (const std::exception& e) 
	{
		prtf((char *)"ERROR: Exiting reason: %s\n", (char *)"\r", e.what());
		return (-1);
	}

	fclose(log_file);
	return (0);
}

void displayUsage(string errorMessage)
{
	// Display the usage error message if it available
	if (!errorMessage.empty())
	{
		printf("%s\n", errorMessage.c_str());
	}
	//
	// Display the general usage message
	printf("\n");
	printf("Usage: NVRAMTester [-h][-n nn][-v]\n");
	printf("\t-h) displays this usage display.\n");
	printf("\t-m) support manual input on clear error (default no)\n");
	printf("\t-n nn) specified the number of loops to run.  (default is infinite.)\n");
	printf("\t-v) displays the version of this test utility.\n");
	printf("\n");
}

void dispayVersion()
{
	printf("Version: NVRAMTester V1.0\n");

}

void prtf(char *fmt, char *chk, ...)
{
	va_list ap;
	va_start(ap, chk);
	char buff[20];
	char prtFmt[1024];
	char buffer[1024];

	int hour;
	time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = gmtime(&rawtime);
	// set the time zone to Pacific time
	hour = timeinfo->tm_hour + PST;
    timeinfo->tm_hour = (hour > 0)? hour : hour + 24;

	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", timeinfo);
	memset(prtFmt, 0, sizeof(prtFmt));
	strcat(prtFmt, buff);
	strcat(prtFmt, ": ");
	strcat(prtFmt, fmt);
	vsprintf(buffer, prtFmt,  ap);
	fprintf(log_file, buffer);
	fflush(log_file);
	printf(buffer);
	fflush(stdout);
	va_end(ap);
}



