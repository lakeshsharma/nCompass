#ifndef _RBCOM_SERIAL_API_H_
#define _RBCOM_SERIAL_API_H_

#include <stdint.h>

#include <stdint.h>
// Commands used by native library
// Defined in rbcom_serial_api.c

/*
uint8_t RB_SYS_PING[1];
uint8_t RB_SYS_ECHO[1];
uint8_t RB_SYS_DFU[1];
uint8_t RB_SYS_REBOOT[1];
uint8_t RB_SYS_FACRST[1];
uint8_t RB_ADV_UUID[1];
uint8_t RB_ADV_MAJOR[1];
uint8_t RB_ADV_MINOR[1];
uint8_t RB_ADV_REFPWR[1];
uint8_t RB_OPS_TXPWR[1];
uint8_t RB_OPS_ADVINT[1];
uint8_t RB_OPS_BCTYPE[1];
uint8_t RB_OPS_CONNTO[1];
uint8_t RB_OPS_ADVOFF[1];
uint8_t RB_OPS_TLMRATIO[1];
uint8_t RB_OPS_PRIVACY[1];
uint8_t RB_OPS_TLMTYPE[1];
uint8_t RB_OPS_ADVRATE[1];
uint8_t RB_INFO_NAME[1];
uint8_t RB_INFO_GET[1];
uint8_t RB_SEC_CHGPIN[1];
uint8_t RB_SEC_LOCK[1];
uint8_t RB_SEC_UNLOCK[1];
uint8_t RB_IBCN[1];
uint8_t RB_ALTB[1];
uint8_t RB_EDDY_UID[1];
uint8_t RB_EDDY_URL[1];
uint8_t RB_EDDY_EID[1];
uint8_t RB_FRAME_BASE[1];
uint8_t RB_DUMP_FLAG[1];
uint8_t RB_CONN_PERIOD[1];
uint8_t RB_CONN_WINDOW[1];
*/

// Status Codes

// Error
/*
uint8_t RB_SERIAL_ERROR[1];
uint8_t RB_SERR_RX_DATA_OVFLW[1];
uint8_t RB_SERR_CMD_UNKNOWN[1];
uint8_t RB_SERR_TX_DATA_OVFLW[1];
uint8_t RB_SERR_INVALID_PIN[1];
uint8_t RB_SERR_BAD_NEW_PIN[1];
uint8_t RB_SERR_INVALID_DATA[1];
uint8_t RB_SERR_DEPREC_PIN_CMD[1];
uint8_t RB_SERR_TXPWR_HI[1];
uint8_t RB_SERR_INTVL_LO[1];
uint8_t RB_SERR_INTVL_HI[1];
uint8_t RB_SERR_NAME_TOO_LONG[1];
*/

// Success

/*
uint8_t RB_SERIAL_SUCCESS[1];
uint8_t RB_STAT_DFU_REBOOT[1];
uint8_t RB_STAT_REBOOT[1];
//extern uint8_t RB_STAT_RET_VAL[1];
uint8_t RB_STAT_RET_HEX[1];
uint8_t RB_STAT_RET_STR[1];
uint8_t RB_STAT_ADV_CHG[1];
uint8_t RB_STAT_OPS_CHG[1];
uint8_t RB_STAT_INFO_CHG[1];
uint8_t RB_STAT_PIN_CHG[1];
uint8_t RB_STAT_LOCK[1];
uint8_t RB_STAT_UNLOCK[1];
*/

// Command Definitions
uint8_t RB_SYS_PING[] = { 0x03, };
uint8_t RB_SYS_ECHO[] = { 0x04 };
uint8_t RB_SYS_DFU[] = { 0x05 };
uint8_t RB_SYS_FACRST[] = { 0x06 };
uint8_t RB_ADV_UUID[] = { 0x10 };
uint8_t RB_ADV_MAJOR[] = { 0x11 };
uint8_t RB_ADV_MINOR[] = { 0x12 };
uint8_t RB_ADV_REFPWR[] = { 0x13 };
uint8_t RB_OPS_TXPWR[] = { 0x20 };
uint8_t RB_OPS_ADVINT[] = { 0x21 };
uint8_t RB_OPS_BCTYPE[] = { 0x22 };
uint8_t RB_OPS_CONNTO[] = { 0x23 };
uint8_t RB_OPS_ADVOFF[] = { 0x24 };
uint8_t RB_OPS_REBOOT[] = { 0x25 };
uint8_t RB_OPS_TLMRATIO[] = { 0x26 };
uint8_t RB_OPS_PRIVACY[] = { 0x27 };
uint8_t RB_OPS_TLMTYPE[] = { 0x28 };
uint8_t RB_OPS_ADVRATE[] = { 0x29 };
uint8_t RB_INFO_NAME[] = { 0x30 };
uint8_t RB_INFO_GET[] = { 0x31 };
uint8_t RB_SEC_CHGPIN4[] = { 0x40 }; // Deprecated
uint8_t RB_SEC_LOCK[] = { 0x41 };
uint8_t RB_SEC_UNLOCK[] = { 0x42 };
uint8_t RB_SEC_CHGPIN8[] = { 0x43 };
uint8_t RB_IBCN[] = { 0x70 };
uint8_t RB_ALTB[] = { 0x71 };
uint8_t RB_EDDY_UID[] = { 0x72 };
uint8_t RB_EDDY_URL[] = { 0x73 };
uint8_t RB_EDDY_EID[] = { 0x74 };
uint8_t RB_DUMP_FLAG[] = { 0x62 };

uint8_t RB_FRAME_BASE[] = { 0x80 };

uint8_t RB_CONN_PERIOD[] = { 0x90 };
uint8_t RB_CONN_WINDOW[] = { 0x91 };

// Status Code Definitions
// Error
uint8_t RB_SERIAL_ERROR[] = { 0xff };
uint8_t RB_SERR_RX_DATA_OVFLW[] = { 0x01 };
uint8_t RB_SERR_CMD_UNKNOWN[] = { 0x12 };
uint8_t RB_SERR_TX_DATA_OVFLW[] = { 0x20 };
uint8_t RB_SERR_INVALID_PIN[] = { 0x30 };
uint8_t RB_SERR_BAD_NEW_PIN[] = { 0x31 };
uint8_t RB_SERR_INVALID_DATA[] = { 0x32 };
uint8_t RB_SERR_DEPREC_PIN_CMD[] = { 0x33 };
uint8_t RB_SERR_TXPWR_HI[] = { 0x40 };
uint8_t RB_SERR_INTVL_LO[] = { 0x50 };
uint8_t RB_SERR_INTVL_HI[] = { 0x51 };
uint8_t RB_SERR_NAME_TOO_LONG[] = { 0x60 };


// Success
uint8_t RB_SERIAL_SUCCESS[] = { 0xfe };
uint8_t RB_STAT_DFU_REBOOT[] = { 0x01 };
uint8_t RB_STAT_RET_HEX[] = { 0x10 };
uint8_t RB_STAT_RET_STR[] = { 0x11 };
uint8_t RB_STAT_ADV_CHG[] = { 0x20 };
uint8_t RB_STAT_OPS_CHG[] = { 0x30 };
uint8_t RB_STAT_OPS_ADVOFF[] = { 0x31 };
uint8_t RB_STAT_REBOOT[] = { 0x32 };
uint8_t RB_STAT_INFO_CHG[] = { 0x40 };
uint8_t RB_STAT_PIN_CHG[] = { 0x50 };
uint8_t RB_STAT_LOCK[] = { 0x51 };
uint8_t RB_STAT_UNLOCK[] = { 0x52 };

const char *serial_success_to_str(uint8_t);
const char *serial_error_to_str(uint8_t);
#endif
