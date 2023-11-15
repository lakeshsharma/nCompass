#include "stdafx.h"
#include "RadBeacon.h"
#include <termios.h>    // POSIX terminal control definitions
#include <thread>
#include <iomanip>
#include "Utilities.h"
#include "Logging/LogString.h"
#include <glob.h>
#include <sstream>  
#include "../../DirectoryManager.h"
#include "rbports/rbports.h"
#include "rbcom/rbcom_api.h"

using namespace std;
const int BUFFERSIZE = 128;
const int UUIDLENGTH = 16;
const uint8_t RADSUCCESS = 0xfe;
const int MAXPORTS = 10;
const int WAIT = 200;
const int POS_STATUS = 0;

CRadBeacon::CRadBeacon()
{
	m_beaconReady = false;

	if (CUtilities::FileExists(CDirectoryManager::Instance().GetRADBeaconPath()))
	{
		LogString(BEACON_LOG, "Beacon found.");
		if (InitializeSerial(CDirectoryManager::Instance().GetRADBeaconPath()))
		{
			m_beaconReady = InitializeBeaconFrames(EDDYSTONE_FRAME);
		}
		else
		{
			LogString(BEACON_LOG, "Initializing beacon failed.");
			m_beaconReady = false;
		}
	}
	else
	{
		LogString(BEACON_LOG, "Beacon not found.");
	}

	// Set name for beacon.
	const int LEN_UUID = 5;
	array<uint8_t, LEN_UUID> uid = { 'R','B','U','S','B' };
	if (m_beaconReady)
		SetName(uid.data(), LEN_UUID);
}

CRadBeacon::~CRadBeacon()
{
}

bool CRadBeacon::TurnOn()
{
	return false;
}

bool CRadBeacon::TurnOff()
{
	return false;
}

bool CRadBeacon::SetEddystoneFrameData(const std::string& uuid, const std::string& checksum, uint8_t power)
{
	LogString(BEACON_LOG, _T("%s %d: Beacon UUID: [%s], Checksum: [%s], Txpower: [%d]."), __TFUNCTION__, __LINE__, uuid.c_str(), checksum.c_str(), -power);
	const int CMDLENGTH = 40;
	const int POS_CMDCODE = 0;
	const int CMD_CODE = 0x82;
	const int POS_PIN = 1;
	const int POS_PREAMBLE = 9;
	const int POS_LENGTH = 16;
	const int POS_SERVICE_PREAMBLE = 17;
	const int POS_TYPE_ID = 20;
	const int POS_TX_POWER = 21;
	const int POS_UUID = 22;
	const int POS_CHECKSUM = 32;
	const int LEN_PREAMBLE = 7;
	const int LEN_SERVICE_PREAMBLE = 3;
	const int LEN_UUID = 10;
	const int LEN_CHECKSUM = 6;
	const int LEN_PIN = 8;

	const ssize_t WriteError = -1;
	const ssize_t ReadEOF = 0;

	// DATA
	const uint64_t DAT_PREAMBLE = 0xFEAA0303060102;
	const int DAT_LENGTH = 23;
	const int DAT_SERVICE_PREAMBLE = 0xFEAA16;
	const int DAT_TYPE_ID = 0;

	// Did this for G2E 2019, however player device app doesn't use this value to calculate distance (currently).
	// const int DAT_TX_POWER = -(127 - power); // 0 to -128
	const int DAT_TX_POWER = -power; // 0 to -128
	const std::string DefaultInstance = "10000000000";
	const int RESP_LENGTH = 2;

	bool retVal = false;

	if (m_beaconReady)
	{
		//Convert namespace string to byte array to be put into the namespace
		array<uint8_t, LEN_UUID> uuid_array;
		memset(uuid_array.data(), 0, LEN_UUID);
		CUtilities::ConvertHexStringToBytes(uuid, uuid_array.data(), LEN_UUID);

		//Convert checksum string to byte array to be put into the checksum
		array<uint8_t, LEN_CHECKSUM> checksum_array;
		memset(checksum_array.data(), 0, LEN_CHECKSUM);
		if (checksum.empty())
		{
			CUtilities::ConvertHexStringToBytes(DefaultInstance, checksum_array.data(), LEN_CHECKSUM);
		}
		else
		{
			CUtilities::ConvertHexStringToBytes(checksum, checksum_array.data(), LEN_CHECKSUM);
		}

		//Setup frame command
		array<uint8_t, CMDLENGTH> buf;
		memset(buf.data(), 0, CMDLENGTH);
		buf[POS_CMDCODE] = CMD_CODE;
		memset(&buf[POS_PIN], '0', LEN_PIN);
		memcpy(&buf[POS_PREAMBLE], &DAT_PREAMBLE, LEN_PREAMBLE);
		buf[POS_LENGTH] = DAT_LENGTH;
		memcpy(&buf[POS_SERVICE_PREAMBLE], &DAT_SERVICE_PREAMBLE, LEN_SERVICE_PREAMBLE);
		buf[POS_TYPE_ID] = DAT_TYPE_ID;
		buf[POS_TX_POWER] = DAT_TX_POWER;
		memcpy(&buf[POS_UUID], uuid_array.data(), LEN_UUID);
		memcpy(&buf[POS_CHECKSUM], checksum_array.data(), LEN_CHECKSUM);
		tcflush(m_usb, TCIFLUSH);
		ssize_t bytesWritten = write(m_usb, buf.data(), CMDLENGTH);

		if (bytesWritten != WriteError)
		{
			coutData("SetEddystoneFrameData() write data:", buf.data(), bytesWritten);

			std::this_thread::sleep_for(chrono::milliseconds(WAIT));

			ssize_t spot = 0;
			ssize_t bytesRead = 0;

			// Read response
			do
			{
				bytesRead = read(m_usb, &buf[spot], RESP_LENGTH - spot);
				spot += bytesRead;
			} while (bytesRead > ReadEOF && spot < RESP_LENGTH);

			coutData("SetEddystoneFrameData() read data:", buf.data(), spot);

			if (buf[POS_STATUS] == RADSUCCESS)
			{
				retVal = true;
			}
		}
		else
		{
			LogString(BEACON_LOG, "Error in setting UUID");
		}
	}

	if (!retVal)
	{
		LogString(BEACON_LOG, "Beacon not found.");
		m_beaconReady = false;
	}

	m_beaconReady = retVal;

	return retVal;
}

bool CRadBeacon::SetIBeaconFrameData(const std::string& uuid, uint16_t major, uint16_t minor, uint8_t power)
{
	LogString(BEACON_LOG, _T("%s %d: Beacon UUID: [%s], Major: [%d], Minor: [%d], Txpower: [%d]."), __TFUNCTION__, __LINE__, uuid.c_str(), major, minor, -power);

	const int CMD_LENGTH = 40;
	const int CMD_CODE = 0x80;
	
	const int POS_CMDCODE = 0;
	const int POS_PIN = 1;
	const int POS_PREAMBLE = 9;
	const int POS_PREAMBLE_LAST = 17;
	const int POS_UUID = 18;
	const int POS_MAJOR = 34;
	const int POS_MINOR = 36;
	const int POS_MEASUREDPOWER = 38;

	const int LEN_PIN = 8;
	const int LEN_PREAMBLE = 8;
	const int LEN_PREAMBLE_LAST = 1;
	const int LEN_MAJOR = 2;
	const int LEN_MINOR = 2;
	const int LEN_UUID = 16;
	const int LEN_MEASUREDPOWER = 1;

	const ssize_t WriteError = -1;
	const ssize_t ReadEOF = 0;

	// DATA
	const uint8_t DAT_PREAMBLE_LAST = 0x15; // 1 bytes
	const uint64_t DAT_PREAMBLE = 0x02004CFF1A060102; // 9 bytes

	// Did this for G2E 2019, however player device app doesn't use this value to calculate distance (currently).
	//const int DAT_TX_POWER = -(127 - power); // 0 to -128
	const int DAT_TX_POWER = -power; // 0 to -128

	// Default Major
	const uint16_t default_major = 1000;
	// Default Minor
	const uint16_t default_minor = 1000;

	const int RESP_LENGTH = 2;

	bool retVal = false;

	if (m_beaconReady)
	{
		//Convert uuid string to byte array to be put into the namespace
		array<uint8_t, LEN_UUID> uid;
		memset(uid.data(), 0, LEN_UUID);
		CUtilities::ConvertHexStringToBytes(uuid, uid.data(), LEN_UUID);

		if (major == 0)
		{
			major = default_major;
		}

		if (minor == 0)
		{
			minor = default_minor;
		}

		//Convert major and minor to little endian
		major = (major >> 8) | (major << 8);
		minor = (minor >> 8) | (minor << 8);

		//Setup frame command
		array<uint8_t, CMD_LENGTH> buf;
		memset(buf.data(), 0, CMD_LENGTH);
		buf[POS_CMDCODE] = CMD_CODE;
		memset(&buf[POS_PIN], '0', LEN_PIN);
		memcpy(&buf[POS_PREAMBLE], &DAT_PREAMBLE, LEN_PREAMBLE);
		memcpy(&buf[POS_PREAMBLE_LAST], &DAT_PREAMBLE_LAST, LEN_PREAMBLE_LAST);
		buf[POS_MEASUREDPOWER] = DAT_TX_POWER;
		memcpy(&buf[POS_UUID], uid.data(), LEN_UUID);
		memcpy(&buf[POS_MAJOR], &major, LEN_MAJOR);
		memcpy(&buf[POS_MINOR], &minor, LEN_MINOR);

		tcflush(m_usb, TCIFLUSH);
		ssize_t bytesWritten = write(m_usb, buf.data(), CMD_LENGTH);

		if (bytesWritten != WriteError)
		{
			coutData("SetIBeaconFrameData() write data:", buf.data(), bytesWritten);

			std::this_thread::sleep_for(chrono::milliseconds(WAIT));

			ssize_t spot = 0;
			ssize_t bytesRead = 0;

			// Read response
			do
			{
				bytesRead = read(m_usb, &buf[spot], RESP_LENGTH - spot);
				spot += bytesRead;
			} while (bytesRead > ReadEOF && spot < RESP_LENGTH);

			coutData("SetIBeaconFrameData() read data:", buf.data(), spot);

			if (buf[POS_STATUS] == RADSUCCESS)
			{
				retVal = true;
			}
		}
		else
		{
			LogString(BEACON_LOG, "Error in setting Proximity UUID");
		}
	}

	if (!retVal)
	{
		LogString(BEACON_LOG, "Beacon not found.");
		m_beaconReady = false;
	}

	m_beaconReady = retVal;

	return retVal;
}

bool CRadBeacon::GetUUID(std::string uuid)
{
	LogString(BEACON_LOG, "%s %d : ", __FUNCTION__, __LINE__);
	const int CMDLENGTH = 3;
	const int RESPLENGTH = 40;
	const int POS_CMDCODE = 0;
	const int CMD_CODE = 0x61;
	const int POS_TYPE = 1;
	const int POS_DATA = 2;
	const int POS_UUID = 17;
	const int LEN_UUID = 10;

	// DATA
	const int DAT_TYPE = 0x15;
	const int DAT_DATA = 0x09;

	bool retVal = false;

	if (m_beaconReady)
	{
		//Setup frame command
		array<uint8_t, CMDLENGTH> buf;
		array<uint8_t, RESPLENGTH> resp;
		memset(buf.data(), 0, CMDLENGTH);
		buf[POS_CMDCODE] = CMD_CODE;
		buf[POS_TYPE] = DAT_TYPE;
		buf[POS_DATA] = DAT_DATA;
		tcflush(m_usb, TCIOFLUSH);

		int n = write(m_usb, buf.data(), CMDLENGTH);
		if (n != CMDLENGTH)
			LogString(BEACON_LOG, "Error: Writing to beacon");

		std::this_thread::sleep_for(chrono::milliseconds(WAIT));
		int spot = 0;
		// Read response
		do
		{
			n = read(m_usb, &resp[spot], RESPLENGTH - spot);
			spot += n;
		} while (n > 0 && spot < RESPLENGTH);

		if (resp[POS_STATUS] == RADSUCCESS)
		{
			std::stringstream ss;
			ss << std::hex;
			for (int i = 0; i < LEN_UUID; i++)
			{
				ss << resp[POS_UUID + i];
			}
			uuid = ss.str();
			retVal = true;
		}
	}

	m_beaconReady = retVal;

	return retVal;
}

bool CRadBeacon::SetMajor(WORD)
{
	return false;
}

bool CRadBeacon::GetMajor(WORD&)
{
	return false;
}

bool CRadBeacon::SetMinor(WORD)
{
	return false;
}

bool CRadBeacon::GetMinor(WORD&)
{
	return false;
}

bool CRadBeacon::SetTransmissionPower(int power)
{
	bool retVal = false;
	try
	{
		//This code uses Radius networks library to access device. It is doing something not documented in Radius documentation.
		if (!m_devicePath.empty())
		{
			// configuration struct
			struct RbcomConfig *cfg = RbcomConfig_new(const_cast<char *>(m_devicePath.c_str()));

			// connection struct
			struct RbcomConnect *conn = RbcomConnect_new(cfg);

			// response struct
			struct RbcomResp response;
			response = conn->ops_txpwr(conn, (char *)RB_DEFAULT_PIN, power);
			if (response.ret_type != RESP_SUCCESS)
			{
				LogString(BEACON_LOG, "rbcom error setting transmit power %s", response.out);
			}
			else
			{
				retVal = true;
				LogString(BEACON_LOG, "rbcom successful setting transmit power %d.", power);
			}

			//RbcomConfig_destroy(cfg); RB library crashes if this call is made.
			// If call is made after connection destroyed it doesn't crash but blocks on subsequent calls.
			// Leaving it this way which will be a small memory leak.
			RbcomConnect_destroy(conn);
		}
	}
	catch (...)
	{
		cout << "RadBeacon constructor exception caught" << endl;
	}

	return retVal;
}

bool CRadBeacon::GetTransmissionPower(int&)
{
	return false;
}

bool CRadBeacon::SetMeasuredPower(int)
{
	return false;
}

bool CRadBeacon::GetMeasuredPower(int&)
{
	return false;
}

bool CRadBeacon::SetAdvertisingInterval(int)
{
	return false;
}

bool CRadBeacon::GetAdvertisingInterval(int&)
{
	return false;
}

bool CRadBeacon::SetName(byte *name, size_t length)
{
	const int POS_CMDCODE = 0;
	const int CMDLENGTH = 32;
	const int LEN_NAME = 24;
	const int CMD_CODE = 0x30;
	const int POS_PIN = 1;
	const int PIN_LENGTH = 8;
	const int POS_NAME = 9;
	bool retVal = false;
	if (m_beaconReady && name != nullptr)
	{
		//Setup frame command
		array<uint8_t, CMDLENGTH> buf;
		buf[POS_CMDCODE] = CMD_CODE;
		memset(&buf[POS_PIN], '0', PIN_LENGTH); // using PIN 00000000
		memcpy(&buf[POS_NAME], name, length);
		memset(&buf[POS_NAME + length], 0, LEN_NAME - length);

		if (write(m_usb, buf.data(), CMDLENGTH) == CMDLENGTH)
		{
			retVal = true;
		}
	}

	m_beaconReady = retVal;

	return retVal;
}

bool CRadBeacon::IsConnected() const
{
	return m_beaconReady;
}

bool CRadBeacon::Reboot()
{
	const int POS_CMDCODE = 0;
	const int CMDLENGTH = 13;
	const int CMD_CODE = 0x25;
	const int POS_PIN = 1;
	const int PIN_LENGTH = 8;
	const int POS_TIME = 9;
	const int LEN_TIME = 4;
	const int DAT_TIME = 16000; // 1/2 second
	bool retVal = false;
	if (m_beaconReady)
	{
		//Setup frame command
		array<char, CMDLENGTH> buf;
		buf[POS_CMDCODE] = CMD_CODE;
		memset(&buf[POS_PIN], '0', PIN_LENGTH); // using PIN 00000000
		memcpy(&buf[POS_TIME], &DAT_TIME, LEN_TIME);

		if (write(m_usb, buf.data(), CMDLENGTH) == CMDLENGTH)
		{
			retVal = true;
		}
	}

	return retVal;
}

bool CRadBeacon::InitializeSerial(const std::string& deviceString)
{
	bool retVal = true;

	// Probe for RadBeacon with echo command to compare sent and returned values
	const string check_val("98765432");
	const string valid_val("0x3938373635343332");//check val in byte form
	LogString(BEACON_LOG, "InitializeSerial");

	// Run the echo command via API against the port to see if it's a RadBeacon 2.0+
	struct RbcomConfig *cfg = RbcomConfig_new(const_cast<char*>(deviceString.c_str()));
	struct RbcomConnect *conn = RbcomConnect_new(cfg);
	struct RbcomResp echo_resp;
	echo_resp = conn->sys_echo(conn, const_cast<char*>(check_val.c_str()), check_val.size());
	// Compare response to expected value
	if (strncmp(echo_resp.out, valid_val.c_str(), valid_val.size()) == 0)
	{
		LogString(BEACON_LOG, "%s is found, echoResponseString=%s", deviceString.c_str(), echo_resp.out);
	}
	LogString(BEACON_LOG, "InitializeSerial RbcomConnect_destroy");
	RbcomConnect_destroy(conn);

	LogString(BEACON_LOG, "Open File Descriptor");
	/* Open File Descriptor */
	m_usb = open(deviceString.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

	LogString(BEACON_LOG, "Initializing serial device %s m_usb=%d", deviceString.c_str(), m_usb);

	/* Error Handling */
	if (m_usb < 0)
	{
		LogString(BEACON_LOG, "Could not open serial device %s", deviceString.c_str());
		retVal = false;
	}

	/* *** Configure Port *** */
	struct termios tty;
	memset(&tty, 0, sizeof tty);
	if (retVal && tcgetattr(m_usb, &tty) != 0)
	{
		LogString(BEACON_LOG, "Could not configure serial device %s.", deviceString.c_str());
		retVal = false;
	}

	if (retVal)
	{
		/* Set Baud Rate */
		cfsetispeed(&tty, (speed_t)B38400);

		/* Setting other Port Stuff */
		tty.c_cflag &= ~PARENB;            // Make 8n1
		tty.c_cflag &= ~CSTOPB;
		tty.c_cflag &= ~CSIZE;
		tty.c_cflag |= CS8;

		tty.c_cflag &= ~CRTSCTS;           // no flow control
		tty.c_cc[VMIN] = 0;                  // read doesn't block
		tty.c_cc[VTIME] = 5;                  // 0.5 seconds read timeout
		tty.c_cflag |= CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

										   /* Flush Port, then applies attributes */
		tcflush(m_usb, TCIOFLUSH);
		if (tcsetattr(m_usb, TCSANOW, &tty) != 0)
		{
			LogString(BEACON_LOG, "Port %d flushing failed.", m_usb);
			retVal = false;
		}
		else
		{
			LogString(BEACON_LOG, "Port %d flushed successfully.", m_usb);
		}
	}

	if (retVal)
	{
		m_devicePath = deviceString;
		LogString(BEACON_LOG, "%s initialized successfully.", m_devicePath.c_str());
	}

	return retVal;
}

bool CRadBeacon::InitializeBeaconFrames(uint8_t frames)
{
	const uint8_t CMDLENGTH = 10;
	const uint8_t RESPLENGTH = 2;
	const uint8_t POS_CMDCODE = 0;
	const uint8_t CMD_CODE = 0x22;
	const uint8_t POS_PIN = 1;
	const uint8_t PIN_LENGTH = 8;
	const uint8_t POS_FRAME = 9;
	const uint8_t CMD_FRAME = frames;
	bool retVal = false;

	//Setup frame command
	uint8_t buf[CMDLENGTH];
	buf[POS_CMDCODE] = CMD_CODE;
	memset(&buf[POS_PIN], '0', PIN_LENGTH); // using PIN 00000000
	buf[POS_FRAME] = CMD_FRAME;

	tcflush(m_usb, TCIOFLUSH);

	int n = write(m_usb, buf, CMDLENGTH);
	coutData("InitializeBeaconFrames write data:", buf, n);
	std::this_thread::sleep_for(chrono::milliseconds(WAIT));
	int spot(0);
	n = 0;

	// Read response
	do
	{
		n = read(m_usb, &buf, RESPLENGTH - spot);
		spot += n;
	} while (n > 0 && spot < RESPLENGTH);

	coutData("InitializeBeaconFrames read data:", buf, spot);
	if (buf[POS_STATUS] == RADSUCCESS)
	{
		LogString(BEACON_LOG, "InitializeBeaconFrames successful (frames = %d).", frames);
		retVal = true;
	}
	else
	{
		LogString(BEACON_LOG, "InitializeBeaconFrames failed (frames = %d).", frames);
	}

	return retVal;
}

void CRadBeacon::coutData(std::string label, uint8_t *data, ssize_t len)
{
	std::stringstream logData;
	logData << std::hex << std::setw(2) << std::setfill('0') << label << std::endl;
	for (ssize_t i = 0; i < len; i++)
	{
		logData << " " << std::setw(2) << std::setfill('0') << +(uint8_t)data[i];
	}
	logData << std::endl;

	LogString(BEACONDATA, " %s", logData.str().c_str());

}

bool CRadBeacon::Echo()
{
	const int CMDLENGTH = 5;
	const int RESPLENGTH = 6;
	const int POS_CMDCODE = 0;
	const int CMD_CODE = 0x04;
	const int POS_VAL = 1;
	const int32_t ECHOVAL = 0x01020304;
	bool retVal = false;

	if (m_beaconReady)
	{
		//Setup frame command
		array<uint8_t, CMDLENGTH> buf;
		array<uint8_t, RESPLENGTH> resp;
		buf[POS_CMDCODE] = (char)CMD_CODE;
		memcpy(&buf[POS_VAL], &ECHOVAL, sizeof(int32_t));

		tcflush(m_usb, TCIOFLUSH);
		int n = write(m_usb, buf.data(), CMDLENGTH);
		coutData("Echo write data:", buf.data(), n);

		std::this_thread::sleep_for(chrono::milliseconds(WAIT));
		int spot = 0;
		n = 0;

		// Read response
		do
		{
			n = read(m_usb, &resp[spot], RESPLENGTH - spot);
			spot += n;
		} while (n > 0 && spot < RESPLENGTH);
		coutData("Echo read data:", buf.data(), spot);

		if (resp[POS_STATUS] == RADSUCCESS)
		{
			LogString(BEACON_LOG, "Echo: successful.");
			retVal = true;
		}
		else
		{
			LogString(BEACON_LOG, "Echo: failed.");
		}
	}

	return retVal;

}