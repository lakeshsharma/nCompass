#include "stdafx.h"
#include "SASGeneralException.h"

using namespace std;

const string CSASGeneralException::UnknownGPE = "Unknown General Exception Code";

CSASGeneralException::CSASGeneralException()
{
	LoadGPEDescriptions();
}


CSASGeneralException::~CSASGeneralException()
{
}

string CSASGeneralException::GetDescriptionFor(GeneralPollException gpe) const
{
	if (m_eventDescMap.find(gpe) != m_eventDescMap.end())
	{
		return m_eventDescMap.find(gpe)->second;
	}
	return UnknownGPE; // not found so return default string.
}


void CSASGeneralException::LoadGPEDescriptions()
{
	// Could be loaded from file, or could use a resource file for globalization
	m_eventDescMap.insert(GPEKeyPair(GPE_NO_ACTIVITY, "No Activity"));
	m_eventDescMap.insert(GPEKeyPair(GPE_SLOTDOOR_OPENED, "Slot Door Opened"));
	m_eventDescMap.insert(GPEKeyPair(GPE_SLOTDOOR_CLOSED, "Slot Door Closed"));
	m_eventDescMap.insert(GPEKeyPair(GPE_DROPDOOR_OPENED, "Drop Door Opened"));
	m_eventDescMap.insert(GPEKeyPair(GPE_DROPDOOR_CLOSED, "Drop Door Closed"));
	m_eventDescMap.insert(GPEKeyPair(GPE_CARDCAGE_OPENED, "Card Cage Opened"));
	m_eventDescMap.insert(GPEKeyPair(GPE_CARDCAGE_CLOSED, "Card Cage Closed"));
	m_eventDescMap.insert(GPEKeyPair(GPE_POWER_ON, "AC Power Applied To Gaming Machine"));
	m_eventDescMap.insert(GPEKeyPair(GPE_POWER_OFF, "AC Power Lost From Gaming Machine"));
	m_eventDescMap.insert(GPEKeyPair(GPE_CASHBOX_OPENED, "Cashbox Door Opened"));
	m_eventDescMap.insert(GPEKeyPair(GPE_CASHBOX_CLOSED, "Cashbox Door Closed"));
	m_eventDescMap.insert(GPEKeyPair(GPE_CASHBOX_REMOVED, "Cashbox Removed"));
	m_eventDescMap.insert(GPEKeyPair(GPE_CASHBOX_INSTALLED, "Cashbox Installed"));
	m_eventDescMap.insert(GPEKeyPair(GPE_BELLYDOOR_OPENED, "Belly Door Opened"));
	m_eventDescMap.insert(GPEKeyPair(GPE_BELLYDOOR_CLOSED, "Belly Door Closed"));
	m_eventDescMap.insert(GPEKeyPair(GPE_NO_ACTIVITY_WAITING, "No Activity And Waiting for Player Input"));
	m_eventDescMap.insert(GPEKeyPair(GPE_GENERAL_TILT, "General Tilt"));
	m_eventDescMap.insert(GPEKeyPair(GPE_COIN_IN_TILT, "Coin In Tilt"));
	m_eventDescMap.insert(GPEKeyPair(GPE_COIN_OUT_TILT, "Coin Out Tilt"));
	m_eventDescMap.insert(GPEKeyPair(GPE_HOPPER_EMPTY, "Hopper Empty Detected"));
	m_eventDescMap.insert(GPEKeyPair(GPE_EXTRA_COIN_PAID, "Extra Coin Paid"));
	m_eventDescMap.insert(GPEKeyPair(GPE_DIVERTER_MALFUNC, "Diverter Malfunction"));
	m_eventDescMap.insert(GPEKeyPair(GPE_CASHBOX_FULL, "Cashbox Full Detected"));
	m_eventDescMap.insert(GPEKeyPair(GPE_BILL_JAM, "Bill Jam"));
	m_eventDescMap.insert(GPEKeyPair(GPE_BILL_ACCEPTOR_HW_FAIL, "Bill Acceptor Hardware Failure"));
	m_eventDescMap.insert(GPEKeyPair(GPE_REVERSE_BILL, "Reverse Bill Detected"));
	m_eventDescMap.insert(GPEKeyPair(GPE_BILL_REJECTED, "Bill Rejected"));
	m_eventDescMap.insert(GPEKeyPair(GPE_COUNTERFEIT_BILL, "Counterfeit Bill Detected"));
	m_eventDescMap.insert(GPEKeyPair(GPE_REVERSE_COIN, "Reverse Coin Detected"));

	m_eventDescMap.insert(GPEKeyPair(GPE_CASHBOX_NEAR_FULL, "Cashbox Near Full Detected"));

	m_eventDescMap.insert(GPEKeyPair(GPE_RAM_ERR_DATA_RECOV, "CMOS RAM Error (Data Recovered From EEPROM)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_RAM_ERR_NO_DATA_RECOV, "CMOS RAM Error (No Data Recovered From EEPROM)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_RAM_ERR_BAD_DEVICE, "CMOS RAM Error (Bad Device)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_EEPROM_ERR_DATA, "EEPROM Error (Data Error)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_EEPROM_ERR_BAD_DEVICE, "EEPROM Error (Bad Device)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_EPROM_ERR_DIFF_CHKSM, "EPROM Error (Different Checksum - Version Changed)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_EPROM_ERR_BAD_CHKSM, "EPROM Error (Bad Checksum Compare)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_PEPROM_ERR_DIFF_CHKSM, "Partitioned EPROM Error (Checksum - Version Changed)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_PEPROM_ERR_BAD_CHKSM, "Partitioned EPROM Error (Bad Checksum Compare)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_MEMORY_ERR_RESET, "Memory Error Reset (Operator Used Self Test Switch)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_LOW_BATTERY, "Low Backup Battery Detected"));
	m_eventDescMap.insert(GPEKeyPair(GPE_OPTIONS_CHANGED, "Operator Changed Options"));
	m_eventDescMap.insert(GPEKeyPair(GPE_TICKET_CASHOUT, "Cash Out Ticket Printed"));
	m_eventDescMap.insert(GPEKeyPair(GPE_HANDPAY_VALIDATED, "Handpay Validated"));
	m_eventDescMap.insert(GPEKeyPair(GPE_VALIDATION_NO_CONFIG, "Validation ID Not Configured"));
	m_eventDescMap.insert(GPEKeyPair(GPE_REEL_TILT, "Reel Tilt (Reel Not Specified)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_REEL_1_TILT, "Reel 1 Tilt"));
	m_eventDescMap.insert(GPEKeyPair(GPE_REEL_2_TILT, "Reel 2 Tilt"));
	m_eventDescMap.insert(GPEKeyPair(GPE_REEL_3_TILT, "Reel 3 Tilt"));
	m_eventDescMap.insert(GPEKeyPair(GPE_REEL_4_TILT, "Reel 4 Tilt"));
	m_eventDescMap.insert(GPEKeyPair(GPE_REEL_5_TILT, "Reel 5 Tilt"));
	m_eventDescMap.insert(GPEKeyPair(GPE_REEL_DISCONNECT, "Reel Mechanism Disconnected"));
	m_eventDescMap.insert(GPEKeyPair(GPE_1_DOLLAR_ACCEPTED, "$1 Bill Accepted (Non-RTE Only)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_5_DOLLAR_ACCEPTED, "$5 Bill Accepted (Non-RTE Only)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_10_DOLLAR_ACCEPTED, "$10 Bill Accepted (Non-RTE Only)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_20_DOLLAR_ACCEPTED, "$20 Bill Accepted (Non-RTE Only)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_50_DOLLAR_ACCEPTED, "$50 Bill Accepted (Non-RTE Only)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_100_DOLLAR_ACCEPTED, "$100 Bill Accepted (Non-RTE Only)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_2_DOLLAR_ACCEPTED, "$2 Bill Accepted (Non-RTE Only)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_500_DOLLAR_ACCEPTED, "$500.00 Bill Accepted (Non-RTE Only)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_BILL_ACCEPTED, "Bill Accepted"));
	m_eventDescMap.insert(GPEKeyPair(GPE_200_DOLLAR_ACCEPTED, "$200 Bill Accepted (Non-RTE Only)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_HANDPAY_PENDING, "Handpay Pending"));
	m_eventDescMap.insert(GPEKeyPair(GPE_HANDPAY_RESET, "Handpay Reset"));
	m_eventDescMap.insert(GPEKeyPair(GPE_NO_PROG_5_SECS, "No Progressive Information Received For 5 Seconds"));
	m_eventDescMap.insert(GPEKeyPair(GPE_PROG_WIN, "Progressive Win"));

	m_eventDescMap.insert(GPEKeyPair(GPE_HANDPAY_CANCEL, "Player Has Cancelled Handpay Request"));

	m_eventDescMap.insert(GPEKeyPair(GPE_SAS_PROG_LEVEL_HIT, "SAS Progressive Level Hit"));
	m_eventDescMap.insert(GPEKeyPair(GPE_SYSTEM_VALID_REQ, "System Validation Request"));
	m_eventDescMap.insert(GPEKeyPair(GPE_PRINTER_COMM_ERR, "Printer Communication Error"));
	m_eventDescMap.insert(GPEKeyPair(GPE_PRINTER_PAPER_OUT, "Printer Paper Out Error"));
	m_eventDescMap.insert(GPEKeyPair(GPE_CASHOUT_PRESSED, "Cash Out Button Pressed"));
	m_eventDescMap.insert(GPEKeyPair(GPE_TICKET_INSERTED, "Ticket Inserted"));
	m_eventDescMap.insert(GPEKeyPair(GPE_TICKET_XFER_COMPLETE, "Ticket Transfer Complete"));
	m_eventDescMap.insert(GPEKeyPair(GPE_AFT_XFER_COMPLETE, "AFT Transfer Complete"));
	m_eventDescMap.insert(GPEKeyPair(GPE_AFT_REQ_HOST_CASHOUT, "AFT Request For Host Cashout"));
	m_eventDescMap.insert(GPEKeyPair(GPE_AFT_REQ_HOST_CASHOUT_WIN, "AFT Request For Host To Cash Out Win"));
	m_eventDescMap.insert(GPEKeyPair(GPE_AFT_REQ_REGISTER, "AFT Request To Register"));
	m_eventDescMap.insert(GPEKeyPair(GPE_AFT_REGISTRATION_ACK, "AFT Registration Acknowledged"));
	m_eventDescMap.insert(GPEKeyPair(GPE_AFT_REGISTRATION_CAN, "AFT Registration Cancelled"));
	m_eventDescMap.insert(GPEKeyPair(GPE_GAME_LOCKED, "Game Locked"));
	m_eventDescMap.insert(GPEKeyPair(GPE_BUFFER_OVERFLOW, "Exception Buffer Overflow"));
	m_eventDescMap.insert(GPEKeyPair(GPE_CHANGE_LAMP_ON, "Change Lamp On"));
	m_eventDescMap.insert(GPEKeyPair(GPE_CHANGE_LAMP_OFF, "Change Lamp Off"));
	m_eventDescMap.insert(GPEKeyPair(GPE_PRINTER_PAPER_LOW, "Printer Paper Low"));
	m_eventDescMap.insert(GPEKeyPair(GPE_PRINTER_POWER_OFF, "Printer Power Off"));
	m_eventDescMap.insert(GPEKeyPair(GPE_PRINTER_POWER_ON, "Printer Power On"));
	m_eventDescMap.insert(GPEKeyPair(GPE_REPLACE_PRINTER_RIBBON, "Replace Printer Ribbon"));
	m_eventDescMap.insert(GPEKeyPair(GPE_PRINTER_CARRIAGE_JAM, "Printer Carriage Jammed"));

	m_eventDescMap.insert(GPEKeyPair(GPE_COIN_IN_LOCKOUT_ERROR, "Coin In Lockout Malfunction"));

	m_eventDescMap.insert(GPEKeyPair(GPE_SOFT_METERS_RESET_TO_ZERO, "Gaming Machine Soft Meters Reset"));

	m_eventDescMap.insert(GPEKeyPair(GPE_BILL_VALIDATOR_RESET, "Bill Validator Totals Reset By Operator"));
	m_eventDescMap.insert(GPEKeyPair(GPE_LEGACY_BONUS_JACKPOT, "Legacy Bonus Pay Awarded Or Multiplied Jackpot Occurred"));

	m_eventDescMap.insert(GPEKeyPair(GPE_GAME_STARTED, "Game Started"));
	m_eventDescMap.insert(GPEKeyPair(GPE_GAME_ENDED, "Game Ended"));
	m_eventDescMap.insert(GPEKeyPair(GPE_HOPPER_FULL, "Hopper Full Detected"));
	m_eventDescMap.insert(GPEKeyPair(GPE_HOPPER_LEVEL_LOW, "Hopper Level Low Detected"));
	m_eventDescMap.insert(GPEKeyPair(GPE_DISP_MENU_ENTER, "Display Meters Or Attendant Menu Entered"));
	m_eventDescMap.insert(GPEKeyPair(GPE_DISP_MENU_EXIT, "Display Meters Or Attendant Menu Exited"));
	m_eventDescMap.insert(GPEKeyPair(GPE_SELF_TEST_ENTER, "Self Test Or Operator Menu Entered"));
	m_eventDescMap.insert(GPEKeyPair(GPE_SELF_TEST_EXIT, "Self Test Or Operator Menu Exited"));

	m_eventDescMap.insert(GPEKeyPair(GPE_EGM_OUT_OF_SERVICE, "Gaming Machine Out Of Service (By Attendant)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_PLAYER_DRAW_REQUEST, "Player Requested Draw Cards (RTE Mode)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_REEL_STOPPED, "Reel N has Stopped (RTE Mode)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_COINCREDIT_WAGERED, "Coin/Credit Wagered (RTE Mode)"));
	m_eventDescMap.insert(GPEKeyPair(GPE_GAME_RECALL_DISPLAY, "Game Recall Entry Displayed"));
	m_eventDescMap.insert(GPEKeyPair(GPE_CARD_HELDNOTHELD, "Card Held/Not Held (RTE Mode)"));

	m_eventDescMap.insert(GPEKeyPair(GPE_GAME_SELECTED, "Game Selected"));
	m_eventDescMap.insert(GPEKeyPair(GPE_COMPONENTLIST_CHANGED, "Component List Changed"));
	m_eventDescMap.insert(GPEKeyPair(GPE_AUTHENTICATED, "Authentication Complete"));

	m_eventDescMap.insert(GPEKeyPair(GPE_POWER_OFF_CARDCAGE_ACCESS, "Power Off Card Cage Access"));
	m_eventDescMap.insert(GPEKeyPair(GPE_POWER_OFF_SLOTDOOR_ACCESS, "Power Off Slot Door Access"));
	m_eventDescMap.insert(GPEKeyPair(GPE_POWER_OFF_CASHBOX_ACCESS, "Power Off Cashbox Door Access"));
	m_eventDescMap.insert(GPEKeyPair(GPE_POWER_OFF_DROPDOOR_ACCESS, "Power Off Drop Door Access"));

	m_eventDescMap.insert(GPEKeyPair(GPE_METER_CHANGE_PENDING, "Meter change pending"));
	m_eventDescMap.insert(GPEKeyPair(GPE_METER_CHANGE_CANCELLED, "Meter change canceled"));
	m_eventDescMap.insert(GPEKeyPair(GPE_ENABLED_GAMES_DENOM_CHANGED, "Enabled games denomination changed"));
}

