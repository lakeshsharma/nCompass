#include "stdafx.h"
#include "LoggerTypes.h"

// LogString() Identifier names.
const std::string BADPOLLERDATA_LOG("BADPOLLERDATA");
const std::string DISPLAY_LOG("DISPLAY");
const std::string EEPROM_LOG("EEPROM");
const std::string EEPROMDATA_LOG("EEPROMDATA");
const std::string ERROR_LOG("ERROR");
const std::string FLASHING("FLASHING");
const std::string SOCKETSERVER_LOG("SOCKETSERVER");
const std::string FPGA_LOG("FPGA");
const std::string GAMES("GAMES");
const std::string GENERAL("GENERAL");
const std::string JACKPOT("JACKPOT");
const std::string LEVELBROADCAST_LOG("LEVELBROADCAST");
const std::string MCR_LOG("MCR");
const std::string MEMORY_LOG("MEMORY");
const std::string MESSAGES_LOG("MESSAGES");
const std::string NETWORKING_LOG("NETWORKING");
const std::string NOPTS_LOG("NOPTS");
const std::string NVRAM_LOG("NVRAM");
const std::string ONELINKCONTENT_LOG("ONELINKCONTENT");
const std::string PBT_LOG("PBT");
const std::string POLLER_LOG("POLLER");
const std::string POLLERDATA_LOG("POLLERDATA");
const std::string PROGRESSIVE("PROGRESSIVE");
const std::string PROGRESSIVEDATA("PROGRESSIVEDATA");
const std::string PROGRESSIVEUDP("PROGRESSIVEUDP");
const std::string PROXIMITY_LOG("PROXIMITY");
const std::string SASDATA("SASDATA");
const std::string SASGENERAL("SASGENERAL");
const std::string ANGULARWINDOW_LOG("ANGULARWINDOW");
const std::string ANGULARDATA("ANGULARDATA");
const std::string ANGULARUILOG("ANGULARUILOG");
const std::string THIRDPARTY("THIRDPARTY");
const std::string TICKETING_LOG("TICKETING");
const std::string UDP_LOG("UDP");
const std::string UI_LOG("UI");
const std::string VIDEOGRAPHICS("VIDEOGRAPHICS");
const std::string WARNING_LOG("WARNING");
const std::string SENTINEL_API_LOG("SENTINELAPI");
const std::string SUBSCRIPTIONS_LOG("SUBSCRIPTIONS");
const std::string MEDIAPACKAGEMANAGER_LOG("MEDIAPACKAGEMANAGER");
const std::string SESSION_LOG("SESSION");
const std::string BEACON_LOG("BEACON");
const std::string BEACONDATA("BEACONDATA");

// LogString() message queue log identifier names.
const std::string INT2GCF_MessageQueueLogIdentifier("InternalsToGCF");
const std::string INTPROG_MessageQueueLogIdentifier("InternalsToPROG");
const std::string INT2SCF_MessageQueueLogIdentifier("InternalsToSCF");
const std::string INT2SMA_MessageQueueLogIdentifier("InternalsToSMA");

const std::string GCF2INT_MessageQueueLogIdentifier("GCFToInternals");
const std::string PROGINT_MessageQueueLogIdentifier(PROGRESSIVE);
const std::string SCF2INT_MessageQueueLogIdentifier("SCFToInternals");
const std::string SMA2INT_MessageQueueLogIdentifier("SMAToInternals");
const std::string SUI2INT_MessageQueueLogIdentifier("SUIToInternals");
