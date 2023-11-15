#ifndef _STDAFX_H
#define _STDAFX_H


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
#define TSTRINGIZE_LITERAL(x) #x

#define STRINGIZE_VALUE(x) STRINGIZE_LITERAL(x)
#define TSTRINGIZE_VALUE(x) TSTRINGIZE_LITERAL(x)

#ifdef _USE_CE_TYPES
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned char uchar;

#endif

// C/C++ standard headers.
#include <algorithm>
#include <arpa/inet.h>
#include <array>
#include <atomic>
#include <chrono>
#include <codecvt>
#include <condition_variable>
#include <cstdint>
#include <ctime>
#include <dirent.h>
#include <execinfo.h>
#include <exception>
#include <fstream>
#include <functional>
#include <ifaddrs.h>
#include <inttypes.h>
#include <iomanip>   
#include <iostream>
#include <limits.h>
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netpacket/packet.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rc4.h>
#include <ostream>
#include <queue>
#include <regex>
#include <set>
#include <signal.h>
#include <sstream>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sstream>
#include <stdexcept>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <thread>
#include <time.h>
#include <type_traits>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <assert.h> 

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

// use NOMINMAX so windef doesn't create "min" and "max" macros which conflict with the C++ stdlib -- std::min and std::max are good
#define NOMINMAX
#include <algorithm>
using std::min;
using std::max;

// Include windef.h after size_t, etc. are defined.
#include "windef.h"

// Include Aristocrat extras (beyond what's provided by s7lite's windef.h)
#include "windef-extra.h"

#ifdef _USE_S5DLL
extern "C" {
#include "s7lite.h" // Needs typedefs from windef.h.

// TODO: This is here until this structure is put back into s7lite.h
typedef struct
{
  ULONG   PllFreq;
  USHORT  XActive;
  USHORT  YActive;
  USHORT  XFront;
  USHORT  XSync;
  USHORT  XBlank;
  USHORT  YFront;
  USHORT  YSync;
  USHORT  YBlank;
  USHORT  HRefreshFreq;   // Is horiz. refresh freq. divided by HREFRESH_DIVISOR (10).
  // 0=+ve, 1=-ve
  USHORT  HSyncPolarity;
  USHORT  VSyncPolarity;
} VID_TABLE_ENTRY;

// TODO: This is here until this structure is put back into s7lite.h
typedef struct
{
  USHORT  Enabled;
  USHORT  Status;

  // currently active mode
  USHORT  ConfiguredFormat;
  USHORT  ActiveFormat;
  USHORT  ConfiguredMode;
  USHORT  ActiveMode;

  // from CVI/DE generator
  USHORT  XTotal;
  USHORT  YTotal;
  USHORT  XFront;
  USHORT  XActive;
  USHORT  XBack;
  USHORT  XSync;
  USHORT  YFront;
  USHORT  YActive;
  USHORT  YBack;
  USHORT  YSync;

  // from inputs
  ULONG   DotClock;
  USHORT  HSyncPolarity;
  USHORT  VSyncPolarity;

  // Not returned from NIOS.  Calculated in DLL.
  USHORT  HRefresh;   // Is horiz. refresh freq. divided by HREFRESH_DIVISOR (10).

} VID_PARAMS;

// TODO: This is here until this enum is put back into s7lite.h
  typedef enum {
    EE_VI_FORMAT_AUTO      = 0,
    EE_VI_FORMAT_ANALOGUE  = 1,
    EE_VI_FORMAT_DIGITAL   = 2,
  } eVI_FORMAT;
  
// TODO: This is here until this enum is put back into s7lite.h
  typedef enum 
  {
    // S6 names
    EE_VO_FORMAT_AUTO       = EE_VI_FORMAT_AUTO,
    EE_VO_FORMAT_ANALOGUE   = EE_VI_FORMAT_ANALOGUE,
    EE_VO_FORMAT_DIGITAL    = EE_VI_FORMAT_DIGITAL,
    EE_VO_FORMAT_BOTH       = EE_VO_FORMAT_DIGITAL+1
  } eVO_FORMAT;

// TODO: This is here until these defines are put back into s7lite.h
  // These macros for use with VID_PARAMS.
  #define VIDEO_SOURCE_ENABLED(x) (x!=0)
  #define VIDEO_SOURCE_OK(x)      ((x&((1<<10)|(1<<8))) == ((1<<10)|(1<<8)))
  #define VIDEO_OUTPUT_OK(x)      ((x&(1<<6))!=0)


}
#endif

#ifdef _USE_RAPIDJSON
#define UNUSED_EXPRESSION(X) ((void)(X))
#define RAPIDJSON_ASSERT(X) UNUSED_EXPRESSION(X)
#define RAPIDJSON_HAS_STDSTRING 1
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
#endif

#ifdef _USE_PUGIXML
#include "pugixml/pugiconfig.hpp"
#include "pugixml/pugixml.hpp"
#endif

#include <gtest/gtest_prod.h>

#endif

