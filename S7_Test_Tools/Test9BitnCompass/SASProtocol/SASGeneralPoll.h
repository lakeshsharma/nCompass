#pragma once
#include "SASPollMessage.h"

typedef enum
{
	GPE_NO_ACTIVITY			= 0x00,
	GPE_SLOTDOOR_OPENED		= 0x11,
	GPE_SLOTDOOR_CLOSED		= 0x12,
	GPE_DROPDOOR_OPENED		= 0x13,
	GPE_DROPDOOR_CLOSED		= 0x14,
	GPE_CARDCAGE_OPENED		= 0x15,
    GPE_CARDCAGE_CLOSED		= 0x16,
    GPE_POWER_ON			= 0x17,
    GPE_POWER_OFF			= 0x18,
    GPE_CASHBOX_OPENED		= 0x19,
    GPE_CASHBOX_CLOSED		= 0x1A,
    GPE_CASHBOX_REMOVED		= 0x1B,
    GPE_CASHBOX_INSTALLED	= 0x1C,
    GPE_BELLYDOOR_OPENED	= 0x1D,
    GPE_BELLYDOOR_CLOSED	= 0x1E,
	GPE_NO_ACTIVITY_WAITING	= 0x1F,
    GPE_GENERAL_TILT		= 0x20,
    GPE_COIN_IN_TILT		= 0x21,
    GPE_COIN_OUT_TILT		= 0x22,
    GPE_HOPPER_EMPTY		= 0x23,
    GPE_EXTRA_COIN_PAID		= 0x24,
    GPE_DIVERTER_MALFUNC	= 0x25,
    GPE_CASHBOX_FULL		= 0x27,
    GPE_BILL_JAM			= 0x28,
    GPE_BILL_ACCEPTOR_HW_FAIL= 0x29,
    GPE_REVERSE_BILL		= 0x2A,
    GPE_BILL_REJECTED		= 0x2B,
    GPE_COUNTERFEIT_BILL	= 0x2C,
    GPE_REVERSE_COIN		= 0x2D,
    GPE_RAM_ERR_DATA_RECOV	= 0x31,		//data recovered from EEPROM}
    GPE_RAM_ERR_NO_DATA_RECOV= 0x32,	// no data recovered from EEPROM}
    GPE_RAM_ERR_BAD_DEVICE	= 0x33,		// bad device}
    GPE_EEPROM_ERR_DATA		= 0x34,		//{data error}
    GPE_EEPROM_ERR_BAD_DEVICE	= 0x35,	//{bad device}
    GPE_EPROM_ERR_DIFF_CHKSM	= 0x36,	//{different checksum--version changed}
    GPE_EPROM_ERR_BAD_CHKSM	= 0x37,		//{bad checksum compare}
    GPE_PEPROM_ERR_DIFF_CHKSM	= 0x38,	//{partitioned EPROM error, different checksum--version changed}
    GPE_PEPROM_ERR_BAD_CHKSM= 0x39,		//{partitioned EPROM error, bad checksum compare}
    GPE_MEMORY_ERR_RESET	= 0x3A,		//{memory error reset--operator used selftest switch}
    GPE_LOW_BATTERY			= 0x3B,		//{low backup battery}
	GPE_OPTIONS_CHANGED		= 0x3C,
    GPE_TICKET_CHASHOUT		= 0x3D,		//{ticket cashout}
    GPE_HANDPAY_VALIDATED	= 0x3E,		//{Payout but no Ticket Printed-handpay or hopper}
    GPE_VALIDATION_NO_CONFIG= 0x3F,		//{Enhanced Ticket initialization required}
    GPE_REEL_TILT			= 0x40,
    GPE_REEL_1_TILT			= 0x41,
    GPE_REEL_2_TILT			= 0x42,
    GPE_REEL_3_TILT			= 0x43,
    GPE_REEL_4_TILT			= 0x44,
    GPE_REEL_5_TILT			= 0x45,
    GPE_REEL_DISCONNECT		= 0x46,		//{reel mechanism disconnected}
    GPE_1_DOLLAR_ACCEPTED	= 0x47,
    GPE_5_DOLLAR_ACCEPTED	= 0x48,
    GPE_10_DOLLAR_ACCEPTED	= 0x49,
    GPE_20_DOLLAR_ACCEPTED	= 0x4A,
    GPE_50_DOLLAR_ACCEPTED	= 0x4B,
    GPE_100_DOLLAR_ACCEPTED	= 0x4C,
    GPE_2_DOLLAR_ACCEPTED	= 0x4D,
    GPE_500_DOLLAR_ACCEPTED	= 0x4E,
    GPE_BILL_ACCEPTED		= 0x4F,
    GPE_200_DOLLAR_ACCEPTED	= 0x50,
    GPE_HANDPAY_PENDING		= 0x51,		//{hand pay JP is pending--progressive or nonprogressive}
    GPE_HANDPAY_RESET		= 0x52,		//{hand pay reset.  JP reset switch activated}
    GPE_NO_PROG_5_SECS		= 0x53,		//{link mode.  No prog info for 5 seconds}
    GPE_PROG_WIN			= 0x54,		//{progressive JP--hopper paid}
    GPE_SAS_PROG_LEVEL_HIT	= 0x56,		//{SAS progessive level hit -- use long poll $85 to clear.}
    GPE_SYSTEM_VALID_REQ	= 0x57,		//{Request for a Validation coupon Number}
    GPE_PRINTER_COMM_ERR	= 0x60,		//{printer communication error}
    GPE_PRINTER_PAPER_OUT	= 0x61,
    GPE_CASHOUT_PRESSED		= 0x66,
    GPE_TICKET_INSERTED		= 0x67,		//{Request for a Validation of a voucher Number}
    GPE_TICKET_XFER_COMPLETE= 0x68,		//{Voucher transaction complete}
    GPE_AFT_XFER_COMPLETE	= 0x69,		//{AFT trasfer complete}
    GPE_AFT_REQ_HOST_CASHOUT= 0x6A,		//{AFT request host cashout}
    GPE_AFT_REQ_HOST_CASHOUT_WIN= 0x6B, //{AFT request host to cashout win}
    GPE_AFT_REQ_REGISTER	= 0x6C,		//{request to register}
    GPE_AFT_REGISTRATION_ACK= 0x6D,		//{AFT registration acknowlege}
    GPE_AFT_REGISTRATION_CAN= 0x6E,		//{AFT registration Cancelled}
    GPE_GAME_LOCKED			= 0x6F,		//{game locked}
	GPE_BUFFER_OVERFLOW		= 0x70,
	GPE_CHANGE_LAMP_ON		= 0x71,
	GPE_CHANGE_LAMP_OFF		= 0x72,
    GPE_PRINTER_PAPER_LOW	= 0x74,
    GPE_PRINTER_POWER_OFF	= 0x75,
    GPE_PRINTER_POWER_ON	= 0x76,
    GPE_REPLACE_PRINTER_RIBBON	= 0x77,
    GPE_PRINTER_CARRIAGE_JAM	= 0x78,
	GPE_SOFT_METERS_RESET_TO_ZERO = 0x7a,
	GPE_GAME_STARTED		= 0x7e,
	GPE_GAME_ENDED			= 0x7f,
    GPE_HOPPER_FULL			= 0x80,
    GPE_HOPPER_LEVEL_LOW	= 0x81,
    GPE_DISP_MENU_ENTER		= 0x82,
    GPE_DISP_MENU_EXIT		= 0x83,
    GPE_SELF_TEST_ENTER		= 0x84,
    GPE_SELF_TEST_EXIT		= 0x85,
	GPE_GAME_SELECTED		= 0x8c,
    GPE_POWER_OFF_CARDCAGE_ACCESS	= 0x98,
    GPE_POWER_OFF_SLOTDOOR_ACCESS	= 0x99,
    GPE_POWER_OFF_CASHBOX_ACCESS	= 0x9A,
    GPE_POWER_OFF_DROPDOOR_ACCESS	= 0x9B,
	GPE_METER_CHANGE_PENDING = 0xA0,
	GPE_METER_CHANGE_CANCELLED = 0xA1,
	GPE_ENABLED_GAMES_DENOM_CHANGED = 0xA2,
} GeneralPollException;

class CSASGeneralPoll :
	public CSASPollMessage
{
public:
	CSASGeneralPoll(byte gameAddress);
public:
	~CSASGeneralPoll(void);
	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	virtual void Fill();
	void ResponseReceived();

	GeneralPollException GetExceptionValue( void )
	{
		return m_exceptionValue;
	}

	void ResetExceptionValue( void )
	{
		m_exceptionValue = GPE_NO_ACTIVITY;
	}

private:
	GeneralPollException m_exceptionValue;

};
