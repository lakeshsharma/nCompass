
#include "Test9BitnCompass.h"
#include "CTest9BitnCompassParts.h"


using namespace std;

//#define DEFAULT_LOG_PATH ""
#define DEFAULT_LOG_PATH "/opt/ncompass/mediadisk/"
#define DEFAULT_LOG_FILE  "test9bit_log.txt"
#define DEFAULT_LOG_PREFIX "test9bit_"
#define DEFAULT_ESG_FILE "/home/root/ESG_START"
#define DEFAULT_SENT_FILE "/home/root/SENTINEL_START"
#define PST (-8)

FILE *log_file;
FILE *m_IniFile;
bool s9bit_serial = true;
bool IDC_LOOPBACK = false;
UINT IDC_RTC_PowerCycle;
UINT IDC_Battery_STATUS;
UINT IDC_ERROR_COUNT;
CTest9BitnCompassParts *t9ns;
bool g_runAsSentinel;
bool g_runAsEsg;

int main (int argc, char *argv[])
{
	int c, i;
	string result;
	int loops = -1;
	int delay = 40000;
	char x_buffer[256];
	g_runAsEsg = false;
	g_runAsSentinel = true;
	t9ns = new CTest9BitnCompassParts();

	// verify that a test option has been selected
	if (argc <= 0)
	{
		displayUsage("Error: Too few Arguments.");
		exit(-1);
	}
	// Process command line arguments
	while ((c = getopt(argc, argv, "dehn:sv")) != -1)
	{
		switch (c)
		{
		case 'd':
			try
			{
				delay = atoi(optarg) * 1000;
			}
			catch (exception &ex)
			{
				sprintf(x_buffer, "Error: unexpect reason %s [%s]", ex.what(), optarg);
				displayUsage(string (x_buffer));
				exit(-1);
			}
			break;
		case 'e':
			g_runAsEsg = true;
			g_runAsSentinel = false;
			break;
		case 'h':
			displayUsage("");
			exit(0);
			break;
		case 'n':
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
		case 's':
			g_runAsEsg = false;
			g_runAsSentinel = true;
			break;
		case 'v':
			dispayVersion();
			break;
		case '?':
			if (optopt == 'd')
			{
				sprintf(x_buffer, "Error: Option -%c requires an argument.", optopt);
				displayUsage(string(x_buffer));
			}
			else if (optopt == 'n')
			{
				sprintf(x_buffer, "Error: Option -%c requires an argument.", optopt);
				displayUsage(string(x_buffer));
			}
			else if (isprint (optopt))
			{
				displayUsage("");
			}
			else
			{
				sprintf(x_buffer, "Error: Unknown option character `\\x%x'.", optopt);
				displayUsage(string(x_buffer));
			}
			exit (1);
		default:
			abort ();
		}
	}

	if ((!g_runAsEsg && !g_runAsSentinel) || (g_runAsEsg && g_runAsSentinel)) 
	{
		displayUsage("Error: Either the option '-e' or '-s' must be selected.");
		exit(-1);
	}

	// Check for file flag
	if (doesFileExist(DEFAULT_SENT_FILE))
	{
		g_runAsEsg = false;
		g_runAsSentinel = true;
	} else if (doesFileExist(DEFAULT_ESG_FILE)) 
	{
		g_runAsEsg = true;
		g_runAsSentinel = false;
	}
	
	// Start the Test
	//
	try
	{
		// Open The Logfile
		char LogFile[256];
		strcpy(LogFile, DEFAULT_LOG_PATH);
		strcat(LogFile, DEFAULT_LOG_FILE);
		log_file = fopen(LogFile, "a");

		// Run the Test
		prtf((char *)"Start!!\n",  (char *)"b");

		if (t9ns->initialize(delay) < 0)
		{
			prtf((char *)"ERROR: Initialize failed exiting.\n", (char *)"b");
			exit(-1);
		}

		while ((loops < 0) || (loops > 0))
		{
			if (loops > 0) 
			{
				--loops;
			}
			prtf((char *)"Starting RunTest!!\n",  (char *)"b");
			if (t9ns->RunTest(g_runAsEsg, g_runAsSentinel) < -1)
			{
				prtf((char *)"ERROR: RunTest failed exiting.\n", (char *)"b");
				exit(-1);
			}
		}

		if (t9ns->finalize() < 0)
		{
			prtf((char *)"ERROR: Finalize failed exiting.\n", (char *)"b");
			exit (-1);
		}
	}
	catch (const std::exception& e) 
	{
		prtf((char *)"ERROR: %s\n", (char *)"b", e.what());
		exit (-1);
	}

	fclose(log_file);
	exit (0);
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
	printf("Usage: Test9BitnCompass [-dehsv][-n nn]\n");
	printf("\t-d dd) Edit Delay in seconds.\n");
	printf("\t-e)    Run as an ESG process, if select -s can not be selected.\n");
	printf("\t-h)    displays this usage output.\n");
	printf("\t-n nn) specified the number of loops to run.  (default is infinite.)\n");
	printf("\t-s)    Run as an sentinel process, if select -e can not be selected.\n");
	printf("\t       This is the default action.\n");
	printf("\t-v)    displays the version of this test utility.\n");
	printf("\n");
}

void dispayVersion()
{
	printf("Version: Test9BitnCompass V1.0\n");
}


// Timer Handler.
void OnTimer(int sig, siginfo_t *si, void *uc)
{
	//prtf((char *)"NOTE: Timer fired correctly!\n", (char *)"s");
	if(t9ns->m_IsStartSentinel)
	{
		t9ns->DoGeneralPoll();
		t9ns->StartSentinel();
	}
	else
	{
		t9ns->StartEGM();
	}
	signal(sig, SIG_IGN);
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
	va_end(ap);
}

void LogString( string Identifier, LPCTSTR format, ... )
{
	char x_buffer[1024];
	va_list argList;
	va_start( argList, format );
	sprintf(x_buffer, format, argList);
	prtf((char *)"%s\n", (char *)"b", x_buffer);
}

void LogData(char *Identifier, BYTE code, void *data, long dataLength)
{
	UNREFERENCED_PARAMETER(code); 
	FILE *FileId;

	try
	{
		if (strlen(Identifier) > 0) 
		{
			char LogFile[256];
			dataLength = min(dataLength, (long)USHRT_MAX);
			strcpy(LogFile, DEFAULT_LOG_PATH);
			strcat(LogFile, DEFAULT_LOG_PREFIX);
			strcat(LogFile, Identifier);
			strcat(LogFile, ".data");
			FileId = fopen(LogFile, "ab");
			if( FileId != NULL)
			{
				DWORD dwWritten = fwrite(data, dataLength, 1, FileId);
				fflush(FileId);
				fclose(FileId);
			}
		}
	}
	catch (const std::exception& e) 
	{
		prtf((char *)"ERROR LogData: %s\n", (char *)"b", e.what());
	}
}

void LogS7DLLStatus(int status, LPCTSTR format, ... )
{
	if (S7DLL_STATUS_OK != status)
	{
		char x_buffer[1024];
		va_list argList;
		va_start( argList, format );
		sprintf(x_buffer, format, argList);
		if (S7DLL_STATUS_WARNING == status)
			prtf((char *)"%s  -> WARNING resulted\n", (char *)"b", x_buffer);
		else if (S7DLL_STATUS_ERROR == status)
			prtf((char *)"%s  -> ERROR resulted\n", (char *)"b", x_buffer);
	}
}

//Display Poll Command for EGM functionality
void CTest9BitnCompassParts::DisplayPollCommand(DWORD pollCommand)
{
	prtf((char *)"Poll Command = %d\n", (char *)"b", pollCommand);
}

DWORD GetTickCount()
{
	struct timeval tv;
	if (gettimeofday(&tv, NULL) != 0)
			return 0;

	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

bool doesFileExist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

void DumpBuffer(char note, byte lines, byte *buffer, int length, int max_length)
{
	byte *cptr = buffer;
	byte *nptr = buffer;
	char t_buff[10];
	char t_output[128];
	int num = 0;

	for (int i=0; i < length;) 
	{	
		// output characters
		if (lines == 0 || lines == 2) 
		{
			memset(t_output, 0, sizeof(t_output));
			sprintf(t_buff, (char *)"%c %03d: ", note, i);
			strcpy(t_output, t_buff);
			for (num = 0; num < max_length; num++) 
			{ 
				if ((i+num) < length) 
				{
					sprintf(t_buff, (char *)" %c ", (byte)*(cptr++));
					strcat(t_output, t_buff);
				}
			}
			prtf((char *)"%s\n", (char *)"b", t_output);
		}

		// Output the Hex
		if (lines == 1 || lines == 2) 
		{
			memset(t_output, 0, sizeof(t_output));
			sprintf(t_buff, (char *)"%c %03d: ", note, i);
			strcpy(t_output, t_buff);
			for (num = 0; num < max_length; num++) 
			{ 
				if ((i+num) < length) 
				{
					sprintf(t_buff, (char *)"%02x ", (byte)*(nptr++));
					strcat(t_output, t_buff);
				} 
			}
			prtf((char *)"%s\n", (char *)"b", t_output);
		}
		i += num;
	}
}

