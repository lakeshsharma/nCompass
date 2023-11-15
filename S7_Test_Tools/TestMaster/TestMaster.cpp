#include "TestMaster.h"
#include "TestMasterParts.h"

using namespace std;

#define DEFAULT_LOG_FILE "/opt/ncompass/mediadisk/testmaster_log.txt"
#define PST (-8)

FILE *log_file;
FILE *m_IniFile;
bool s9bit_serial = true;
bool IDC_LOOPBACK = false;
UINT IDC_RTC_PowerCycle;
UINT IDC_Battery_STATUS;
UINT IDC_ERROR_COUNT;


int flagAll = 1;
int flags[7] = {0, 0, 0, 0, 0, 0, 0}; 

int main (int argc, char *argv[])
{
	int c, i;
	string result;
	int loops = -1;
	char x_buffer[256];
	TestMasterParts tmp;

	// verify that a test option has been selected
	if (argc <= 0)
	{
		displayUsage("Error: Too few Arguments.");
		exit(-1);
	}
	// Process command line arguments
	while ((c = getopt (argc, argv, "9abdhln:rsuvw")) != -1)
	{
		switch (c)
		{
		case '9':
			s9bit_serial = false;
			break;
		case 'a':
			flagAll = 1;
			for (int i =0; i < 6; i++) flags[i] = 0;
			break;
		case 'b':
			flagAll = 0;
			flags[battery] = 1;
			break;
		case 'd':
			flagAll = 0;
			flags[dll] = 1;
			break;
		case 'h':
			displayUsage("");
			exit(0);
			break;
		case 'l':
			flagAll = 0;
			flags[backlight] = 1;
			break;
		case 'n':
			loops = atoi(optarg);
			try
			{
				loops = atoi(optarg);
			}
			catch (exception &ex)
			{
				sprintf(x_buffer, "Error: unexpect reason %s [%s]", ex.what(), optarg);
				displayUsage(string (x_buffer));
				exit(-1);
			}
			break;
		case 'r':
			flagAll = 0;
			flags[timer] = 1;
			break;
		case 's':
			flagAll = 0;
			flags[sram] = 1;
			break;
		case 'u':
			flagAll = 0;
			flags[uart] = 1;
			break;
		case 'v':
			dispayVersion();
			break;
		case 'w':
			flagAll = 0;
			flags[watchdog] = 1;
			break;
		case '?':
			if (optopt == 'n')
			{
				sprintf(x_buffer, "Error: Option -%c requires an argument.\n", optopt);
				displayUsage(string(x_buffer));
			}
			else if (isprint (optopt))
			{
				sprintf(x_buffer, "Error: Unknown option `-%c'.\n", optopt);
				displayUsage(string(x_buffer));
			}
			else
			{
				sprintf(x_buffer, "Error: Unknown option character `\\x%x'.\n", optopt);
				displayUsage(string(x_buffer));
			}
			exit (1);
		default:
			abort ();
		}
	}

	// Start the Test
	//
	try
	{
		// Open The Logfile
		log_file = fopen((char *)DEFAULT_LOG_FILE, "a");

		// Initialize the TestMaster Test
		tmp.Set9BitSerial(s9bit_serial);
		tmp.DetectPhidgetHardware();
		tmp.InitTestVars();	

		// Run the Test
		prtf((char *)"%s\n",  (char *)"b", (char *)"Start!!                      ");
		while ((loops < 0) || (loops > 0))
		{
			if (loops > 0) 
			{
				--loops;
			}
			// initialize the dll
			tmp.initialize();

			result = tmp.RunTest(flagAll, flags);

			// End of the NVRAM Test; clean up any open ports or log files
			tmp.finalize();
		}
	}
	catch (const std::exception& e) 
	{
		prtf((char *)"ERROR: %s\n", (char *)"b", e.what());
		return (-1);
	}

	fclose(log_file);
	return (0);
}

void displayUsage(string errorMessage)
{
	// Display the usage error message if it available
	if ((errorMessage != "") && (errorMessage.length() > 0))
	{
		printf("%s\n", errorMessage.c_str());
	}
	//
	// Display the general usage message 
	printf("\n");
	printf("Usage: TestMaster [-abdhlrtuv][-n nn]\n");
	printf("\t-a)    run all the tests (default)\n");
	printf("\t-b)    run battery test.\n");
	printf("\t-d)    run DLL test.\n");
	printf("\t-h)    displays this usage output.\n");
	printf("\t-l)    run Backlight test.\n");
	printf("\t-n nn) specified the number of loops to run.  (default is infinite.)");
	printf("\t-r)    run RTC Timer test.\n");
	printf("\t-s)    run SRAM test.\n");
	printf("\t-u)    run UART test.\n");
	printf("\t-v)    displays the versio	n of this test utility.\n");
	printf("\t-w)    run WatchDog test.\n");
	printf("\n");
}

void dispayVersion()
{
	printf("Version: TestMaster V1.0\n");
}

void prtf(char *fmt, char *chk, ...)
{
	va_list ap;
	va_start(ap, chk);
	char buff[20];
	char prtFmt[128];
	char buffer[1024];
	char buffer2[1024];

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
	if ((strcmp(chk, "f") == 0) || (strcmp(chk, "b") == 0)) 
	{ 
		fprintf(log_file, buffer);
		fflush(log_file);
	}
	if ((strcmp(chk, "s") == 0) || (strcmp(chk, "b") == 0)) 
	{ 
		char *ptr = &buffer[21];
		strcpy(buffer2, ptr);
		printf(buffer2);
		fflush(stdout);
	}
	// TODO Output lines to buffer pool
	va_end(ap);
}

std::string timeString()
{
	char buff[20];
	char prtFmt[128];

	int hour;
	time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = gmtime(&rawtime);

	// set the time zone to Pacific time
	hour = timeinfo->tm_hour + PST;
    timeinfo->tm_hour = (hour > 0)? hour : hour + 24;

	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", timeinfo);
	return string(buff);
}

