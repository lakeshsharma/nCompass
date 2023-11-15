#pragma once

//TODO #include "targetver.h"

#ifdef _SENTINEL
//TODO #define _USE_MSXML3
#define _USE_S5DLL
#define _USE_CE_TYPES
#define _USE_RAPIDJSON
#define _USE_PUGIXML
//TODO #define _USE_DCOM
//TODO #define _USE_ASIO
#endif

#if 0//TODO
#pragma warning(push)
#pragma warning(disable:4548)	// expression before comma has no effect; expected expression with side-effect
#pragma warning(disable:4091)	// typedef enum: ignored when no variable is declared

#define RPC_USE_NATIVE_WCHAR

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

// Define WINNT to get access to the EndTask() function in winuser.h via windows.h.
#define WINNT

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxmt.h>			// AFXMT - MFC Multithreaded Extensions (Synchronization Objects)
#include <afxsock.h>
#include <afxinet.h>
#include <WinCrypt.h>
#include <WinIoCtl.h>
#include <Ws2tcpip.h>
#include <Shlobj.h>
#include <Shlwapi.h>

#ifdef _USE_MSXML3
#include "msxml3.tlh"
#endif
#endif//TODO

#ifdef uint8_t
#undef uint8_t
#endif
#ifdef uint16_t
#undef uint16_t
#endif
#ifdef int16_t
#undef int16_t
#endif
#ifdef uint32_t
#undef uint32_t
#endif

#ifdef _SENTINEL
#define LOGGING
#endif

#define __TFUNCTION__ __FUNCTION__

#define STRINGIZE_LITERAL(x) #x
#if 0//TODO
#define WSTRINGIZE_LITERAL(x) L ## #x
#ifdef  UNICODE
	#define TSTRINGIZE_LITERAL(x) L ## #x
#else
	#define TSTRINGIZE_LITERAL(x) #x
#endif
#endif//TODO

#define STRINGIZE_VALUE(x) STRINGIZE_LITERAL(x)
#if 0//TODO
#define WSTRINGIZE_VALUE(x) WSTRINGIZE_LITERAL(x)
#define TSTRINGIZE_VALUE(x) TSTRINGIZE_LITERAL(x)

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
#endif//TODO

#ifdef _USE_CE_TYPES
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned char uchar;
#endif

//TODO #include <atlbase.h>

// C/C++ standard headers.

#include <algorithm>
#include <arpa/inet.h>
#include <array>
#include <codecvt>
#include <cstdint>
#include <ctime>
#include <exception>
#include <fstream>
#include <functional>
#include <ifaddrs.h>
#include <iomanip>   
#include <iostream>
#include <limits.h>
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <mutex>
#include <netpacket/packet.h>
#include <queue>
#include <ostream>
#include <regex>
#include <set>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <thread>
#include <time.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <list>
#include <memory>

#ifndef BYTE_MAX
#define BYTE_MAX UINT8_MAX
#endif

#ifndef WORD_MAX
#define WORD_MAX UINT16_MAX
#endif

#ifndef DWORD_MAX
#define DWORD_MAX UINT32_MAX
#endif

#ifdef _USE_CPPUNITTEST_H
//TODO #include "CppUnitTest.h"
#endif

// Include windef.h after size_t, etc. are defined.
#include "windef.h"

#ifdef _USE_S5DLL
extern "C" {
#include "s7lite.h" // Needs typedefs from windef.h.
}
#endif

#ifdef _USE_RAPIDJSON
#define RAPIDJSON_ASSERT
#define RAPIDJSON_HAS_STDSTRING 1
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/encodedstream.h"
#include "rapidjson/encodings.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/memorybuffer.h"
#include "rapidjson/memorystream.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/error.h"
#include "rapidjson/error/en.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#pragma GCC diagnostic pop
#endif

#ifdef _USE_PUGIXML
#include "pugixml/pugiconfig.hpp"
#include "pugixml/pugixml.hpp"
#endif

#if 0//TODO
#ifdef _USE_DCOM
// COM-related headers.
#define _WIN32_DCOM
#include <atlsafe.h>
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
#endif
#endif//TODO

//#include "gtest/gtest_prod.h"
#ifdef _UNIT_TEST
//TODO #include <xmllite.h>//TODO Alan: Microsoft-specific for data-driven tests.
#endif

#if 0//TODO
// Device headers.
#include <dbt.h>
#include <devguid.h>

#pragma warning (pop)
#endif//TODO
