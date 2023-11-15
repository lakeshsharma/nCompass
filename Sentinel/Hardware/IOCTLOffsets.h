
#include <stdafx.h>

#ifndef __S3DEVICE_DRIVER_DEFINES__
#define __S3DEVICE_DRIVER_DEFINES__

// Access security.
// --------------------------------------
#define CRG_IOCTL_GET_CHASSIS_STATE	0x00

#define CHASSIS_OPEN_EVENT			l"ChassisOpen"
#define CHASSIS_CLOSE_EVENT			l"ClassisClose"

	
// Battery
// --------------------------------------
#define BKB_IOCTL_GET_STATUS  0x00
#define BKB_FILENAME	L"BKB1:"


// Character LCD
// --------------------------------------
#define TXS_IOCTL_WRITE_INSTRUCTION  0x01
#define TXS_IOCTL_WRITE_DATA  0x02


// Non-isolated digital input
// --------------------------------------
#define NDI_IOCTL_SET_SAMPLE_PERIOD	0x00
#define NDI_IOCTL_SET_BUFFER_SIZE	0x01
#define NDI_IOCTL_READ_DATA		0x02
#define NDI_IOCTL_CLEAR_DATA	0x05


// Graphic LCD backlight.
// --------------------------------------
#define BKL_IOCTL_SET_VALUE		0x00
#define BKL_IOCTL_GET_VALUE		0x01


// Keypad interface
// --------------------------------------
#define KEYPAD_1_EVENT		L"KEY00"
#define KEYPAD_2_EVENT		L"KEY04"
#define KEYPAD_3_EVENT		L"KEY08"
#define KEYPAD_4_EVENT		L"KEY01"
#define KEYPAD_5_EVENT		L"KEY05"
#define KEYPAD_6_EVENT		L"KEY09"
#define KEYPAD_7_EVENT		L"KEY02"
#define KEYPAD_8_EVENT		L"KEY06"
#define KEYPAD_9_EVENT		L"KEY10"
#define KEYPAD_0_EVENT		L"KEY07"
#define KEYPAD_UP_EVENT		L"KEY12"
#define KEYPAD_DOWN_EVENT	L"KEY13"
#define KEYPAD_2ND_EVENT	L"KEY14"
#define KEYPAD_CLEAR_EVENT	L"KEY03"
#define KEYPAD_HELP_EVENT	L"KEY11"
#define KEYPAD_ENTER_EVENT	L"KEY15"


// Magnetic card reader
// --------------------------------------
#define MCR_FILENAME				L"MCR1:"

//#define MCR_IOCTL_READ_TRACK_1		0x0
//#define MCR_IOCTL_READ_TRACK_2		0x1
//#define MCR_IOCTL_GET_CARD_STATUS	0x2

#define MCR_INSERT_EVENT	L"MagneticCardInserted"
#define MCR_PARTIAL_EVENT	L"MagneticCardPartialInsert"
#define MCR_REMOVE_EVENT	L"MagneticCardRemove"

#define MCR_FLAG_CARD_INSERTED		0x01
#define MCR_FLAG_TRACK_1_DATA		0x02
#define MCR_FLAG_TRACK_2_DATA		0x04

// NVRAM
// --------------------------------------
//#define NVR_FILENAME				L"NVR1:"

//#define NVR_IOCTL_READ_LIST				0x00
//#define NVR_IOCTL_WRITE_LIST			0x01
//#define NVR_IOCTL_READ_PREVIOUS_CHASSIS	0x02
//#define NVR_IOCTL_WRITE_SUCCESS			0x03

typedef struct
{
	DWORD dwOffset; // offset from beginning of NVRAM
	BYTE *dataPtr;
	DWORD dataLen; // number of bytes
} TNVDataBlockDescriptor;

typedef struct
{
WORD wWriteSuccess1; // Set to 0x00ff when starting a write operation.
                     // Set to 0xff00 when completing a write operation.
WORD wWriteSuccess2; // Copy of byWriteSuccess1.
} TNVWriteSuccess;

typedef struct
{
	BYTE byChassisRegister1; // Value read from Chassis Register.
	BYTE byChassisRegisterInv1; // byChassisRegister1 xor 0xff.
	BYTE byChassisRegister2; // Copy of byChassisRegister1.
	BYTE byChassisRegisterInv2; // Copy of byChassisRegisterInv1.
} TNVChassisRegister;

#define NVRAM_OFFSET_START	0
#define NVRAM_OFFSET_END	2097152		// 2MB

#define NVRAM_MAX_READ			256
#define NVRAM_MAX_READ_BLOCKS	1

#define NVRAM_MAX_WRITE			256	// In bytes.
#define NVRAM_MAX_WORD_WRITE	(NVRAM_MAX_WRITE/2)
#define NVRAM_MAX_WRITE_BLOCKS	4


// Boot Loader
// --------------------------------------
#define BTL_IOCTL_SET_IMAGE_LOAD_SUCCESS	0x00



// EEPROM
// --------------------------------------
#define EEPROM_FILENAME				L"EPR1:"

#define EEPROM_OFFSET_START	0
// Originally 192, now saving last two bytes for checksum.  Last usable eeprom spot should be 191 not 192.
#define EEPROM_OFFSET_END	189
#define EEPROM_CHECKSUM_OFFSET 190
#define EEPROM_CHECKSUM_SIZE 2
#define EEPROM_TOTAL_SIZE 256
// EEPROM writes cannot cross pages.
#define EEPROM_PAGE_SIZE 128

#define EEPROM_MAX_READ			32
#define EEPROM_MAX_READ_BLOCKS	1

const static DWORD EEPROM_MAX_WRITE = 32;
#define EEPROM_MAX_WRITE_BLOCKS	4

typedef struct
{
	DWORD dwOffset; // offset from beginning of EEPROM address
	BYTE *dataPtr;
	DWORD dataLen; // number of bytes
} TEPRDataBlockDescriptor;

typedef struct
{
   WORD wWriteSuccess1; // Set to 0x00ff when starting a write operation.
                        // Set to 0xff00 when completing a write operation.
   WORD wWriteSuccess2; // Copy of byWriteSuccess1.
} TEPWriteSuccess;


// 9-bit serial
// --------------------------------------
#define CMN_IOCTL_READ  0x00
#define CMN_IOCTL_WRITE  0x01

typedef struct
{
	bool bParity;	// If true, send mark parity with first byte
	BYTE *pbyData;	// Pointer to array of bytes
	DWORD dwDataLen;	// Number of bytes
} TSerialWriteData;

typedef struct
{
	BYTE byData;
	bool bParity;	// True for mark parity.
} TSerialReadData;


// Watchdog
// --------------------------------------
#define WDG_IOCTL_STROBE 0x00

//Legacy Board
//---------------------------------------
//Relays

#define RLY_IOCTL_SET_RELAY    0x00
#define RLY_IOCTL_CLEAR_RELAY  0x01

#endif

