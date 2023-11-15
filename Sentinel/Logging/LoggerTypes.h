#pragma once
#include <stdafx.h>

// LogString() Identifier names.
extern const std::string BADPOLLERDATA_LOG;
extern const std::string DISPLAY_LOG;
extern const std::string EEPROM_LOG;
extern const std::string EEPROMDATA_LOG;
extern const std::string ERROR_LOG;
extern const std::string FLASHING;
extern const std::string SOCKETSERVER_LOG;
extern const std::string FPGA_LOG;
extern const std::string GAMES;
extern const std::string GENERAL;
extern const std::string JACKPOT;
extern const std::string LEVELBROADCAST_LOG;
extern const std::string MCR_LOG;
extern const std::string MEMORY_LOG;
extern const std::string MESSAGES_LOG;
extern const std::string NETWORKING_LOG;
extern const std::string NOPTS_LOG;
extern const std::string NVRAM_LOG;
extern const std::string ONELINKCONTENT_LOG;
extern const std::string PBT_LOG;
extern const std::string POLLER_LOG;
extern const std::string POLLERDATA_LOG;
extern const std::string PROGRESSIVE;
extern const std::string PROGRESSIVEDATA;
extern const std::string PROGRESSIVEUDP;
extern const std::string PROXIMITY_LOG;
extern const std::string SASDATA;
extern const std::string SASGENERAL;
extern const std::string ANGULAR_LOG;
extern const std::string ANGULARDATA;
extern const std::string ANGULARUILOG;
extern const std::string THIRDPARTY;
extern const std::string TICKETING_LOG;
extern const std::string UDP_LOG;
extern const std::string UI_LOG;
extern const std::string VIDEOGRAPHICS;
extern const std::string WARNING_LOG;
extern const std::string SENTINEL_API_LOG;
extern const std::string SUBSCRIPTIONS_LOG;
extern const std::string MEDIAPACKAGEMANAGER_LOG;
extern const std::string SESSION_LOG;
extern const std::string BEACON_LOG;
extern const std::string BEACONDATA;

// LogString() message queue log identifier names.
extern const std::string INT2GCF_MessageQueueLogIdentifier; // Internals framework to SAS protocol.
extern const std::string INTPROG_MessageQueueLogIdentifier; // Internals framework to progressive protocol.
extern const std::string INT2SCF_MessageQueueLogIdentifier; // Internals framework to Poller protocol.

extern const std::string GCF2INT_MessageQueueLogIdentifier; // SAS protocol to internals framework.
extern const std::string PROGINT_MessageQueueLogIdentifier; // Progressive protocol to internals framework.
extern const std::string SCF2INT_MessageQueueLogIdentifier; // Poller protocol to internals framework.
extern const std::string SUI2INT_MessageQueueLogIdentifier; // Sentinel user interface to internals framework.

// Codes for LogData().
enum COMM_DATA_TYPE
{
	DATA_WRITE = 0x1,
	DATA_READ = 0x2,
	DATA_COM1 = 0x4,
	DATA_COM2 = 0x8,
	DATA_ETHERNET = 0x10,
	DATA_NDI = 0x20
};
