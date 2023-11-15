//
// This file presents the common values needed when using the
// named pipe to communicate with the Userdll_rev2.dll stub
// dll.
//
// Joe Magee	September 11, 2012
//

#define	S5_DLLReadNamedPipeName		L"S5DLLReadPipe"
#define	S5_DLLWriteNamedPipeName	L"S5DLLWritePipe"

#define	S5NAMEDPIPEINBUFSIZE		2048
#define	S5NAMEDPIPEOUTBUFSIZE		2048

const int PIPE_BUF_SIZE = 0x4000;

// Various protocol items for this Client/Server exchange

// These are the command strings for the DLL and monitor window

#define	LITE_DLL_CMD_DISABLE_FILE_LOGGING		"DisableFileLogging:"
#define	LITE_DLL_CMD_ENABLE_FILE_LOGGING		"EnableFileLogging:"
#define	LITE_DLL_CMD_IS_FILE_LOGGING			"GetFileLogging:"
#define	LITE_DLL_CMD_LOG						"Log:"
#define	LITE_DLL_CMD_QUIT						"Quit:"	// gui app disconnecting

#define	LITE_DLL_CMD_DISABLE_FILE_LOGGING_LEN	(_countof(LITE_DLL_CMD_DISABLE_FILE_LOGGING) - 1)
#define	LITE_DLL_CMD_ENABLE_FILE_LOGGING_LEN	(_countof(LITE_DLL_CMD_ENABLE_FILE_LOGGING) - 1)
#define	LITE_DLL_CMD_IS_FILE_LOGGING_LEN		(_countof(LITE_DLL_CMD_IS_FILE_LOGGING) - 1)
#define	LITE_DLL_CMD_LOG_LEN					(_countof(LITE_DLL_CMD_LOG) - 1)
#define	LITE_DLL_CMD_QUIT_LEN					(_countof(LITE_DLL_CMD_QUIT) - 1)

// Flags for controlling opening pipes
typedef enum {
	PIPE_OPEN_BOTH,
	PIPE_OPEN_READ,
	PIPE_OPEN_WRITE,
} PIPEOPENMODE;

