#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#define RB_DEFAULT_PIN                    "00000000"
#define RB_INTERVAL_TIME_UNIT             0.625
#define RB_INTERVAL_TIME_IN_MS(X)         (X) / RB_INTERVAL_TIME_UNIT
#define RB_TIME_IN_MS_FROM_INTERVAL(X)    (X) * RB_INTERVAL_TIME_UNIT

enum RespType
{
	RESP_SUCCESS = 0,
	RESP_ERROR = 1,
	RESP_UNDEF = 2
};

// frame slot ids
enum RB_FRAME_SLOT {
	RB_FRAME_SLOT_IBEACON = 0,
	RB_FRAME_SLOT_ALTBEACON = 1,
	RB_FRAME_SLOT_EDDYSTONE_UID = 2,
	RB_FRAME_SLOT_EDDYSTONE_URL = 3,
	RB_FRAME_SLOT_EDDYSTONE_EID = 4
};

// beacon types (frame slot flags)
enum RB_BCTYPE_FRAME {
	RB_BCTYPE_FRAME0_IBEACON = 1 << RB_FRAME_SLOT_IBEACON,
	RB_BCTYPE_FRAME1_ALTBEACON = 1 << RB_FRAME_SLOT_ALTBEACON,
	RB_BCTYPE_FRAME2_EDDYSTONE_UID = 1 << RB_FRAME_SLOT_EDDYSTONE_UID,
	RB_BCTYPE_FRAME3_EDDYSTONE_URL = 1 << RB_FRAME_SLOT_EDDYSTONE_URL,
	RB_BCTYPE_FRAME4_EDDYSTONE_EID = 1 << RB_FRAME_SLOT_EDDYSTONE_EID
};

// txpwers
enum RB_TXPWR {
	RB_TXPWR_NEG23DBM = 0,
	RB_TXPWR_NEG21DBM = 1,
	RB_TXPWR_NEG20DBM = 2,
	RB_TXPWR_NEG18DBM = 3,
	RB_TXPWR_NEG16DBM = 4,
	RB_TXPWR_NEG14DBM = 5,
	RB_TXPWR_NEG12DBM = 6,
	RB_TXPWR_NEG11DBM = 7,
	RB_TXPWR_NEG8DBM = 8,
	RB_TXPWR_NEG7DBM = 9,
	RB_TXPWR_NEG5DBM = 10,
	RB_TXPWR_NEG4DBM = 11,
	RB_TXPWR_NEG2DBM = 12,
	RB_TXPWR_NEG1DBM = 13,
	RB_TXPWR_0DBM = 14,
	RB_TXPWR_POS3DBM = 15
};

const char *resp_type_to_str(enum RespType);

// TODO: Can be made private?
// ====================================
// Config Declaration and Definitions

struct RbcomConfig
{
	char *port;
};

struct RbcomConfig *RbcomConfig_new(char *port);

void RbcomConfig_debug(struct RbcomConfig *);

// ====================================


// TODO: Can be made private?
// ====================================
// Response Declarations and Definitions

struct RbcomResp
{
	struct RbcomReq *req;
	enum RespType ret_type;
	uint8_t code;
	int status;
	char *status_msg;
	char *out;
	uint8_t len;
	uint8_t cmd_code[2];
	uint8_t *bytes;

	// method declarations
	int(*setStatus)     (struct RbcomResp *, int status, const char *msg);
	int(*setOut)        (struct RbcomResp *);
	int(*setSerialResp) (struct RbcomResp *, uint8_t len, uint8_t cmd_code[], uint8_t *bytes);
};
void RbcomResp_debug(struct RbcomResp *);

// ====================================

/*
* RbcomConnect
*
*    This struct holds all the connection data and methods
*    Connection Data:
*    - Configuration data (i.e., serial port name)
*    - Request container to hold request params
*    - Response container to hold response data
*
*    Note: The response container holds, the request
*    container, which in turn holds the config.
*
*    Methods:
*    ping (etc);
*/
struct RbcomConnect
{
	// TODO: RbcomResp has RbcomConfig and RbcomReq already
	//  so it might be ok to remove them from RbcomConnect
	struct RbcomConfig *config;
	struct RbcomReq *req;
	struct RbcomResp *resp;

	// "methods"
	//ToDo: Make argument types consistent, some are byte pointers (byte arrays),
	//some are byte-sized (heh!) integers.
	struct RbcomResp(*sys_ping)     (struct RbcomConnect *);
	struct RbcomResp(*sys_echo)     (struct RbcomConnect *, char[], int len);
	struct RbcomResp(*sys_raw)      (struct RbcomConnect *, char[], int len);
	struct RbcomResp(*sys_dfu)      (struct RbcomConnect *, char *, uint8_t);
	struct RbcomResp(*sys_reboot)   (struct RbcomConnect *, char *, uint32_t);
	struct RbcomResp(*sys_facrst)   (struct RbcomConnect *, char *);
	struct RbcomResp(*adv_uuid)     (struct RbcomConnect *, char *, uint8_t *);
	struct RbcomResp(*adv_major)    (struct RbcomConnect *, char *, uint8_t *);
	struct RbcomResp(*adv_minor)    (struct RbcomConnect *, char *, uint8_t *);
	struct RbcomResp(*adv_refpwr)   (struct RbcomConnect *, char *, uint8_t);
	struct RbcomResp(*ops_advint)   (struct RbcomConnect *, char *, uint16_t);
	struct RbcomResp(*ops_advrate)   (struct RbcomConnect *, char *, uint8_t);
	struct RbcomResp(*ops_txpwr)    (struct RbcomConnect *, char *, uint8_t);
	struct RbcomResp(*ops_bctype)   (struct RbcomConnect *, char *, uint8_t);
	struct RbcomResp(*ops_tlmtype)  (struct RbcomConnect *, char *, uint8_t);
	struct RbcomResp(*ops_tlmratio)  (struct RbcomConnect *, char *, uint8_t);
	struct RbcomResp(*ops_connto)   (struct RbcomConnect *, char *, uint32_t);
	struct RbcomResp(*ops_advoff)   (struct RbcomConnect *, char *);
	struct RbcomResp(*info_name)    (struct RbcomConnect *, char *, char *);
	// think about using enum for info get
	struct RbcomResp(*info_get)     (struct RbcomConnect *, uint8_t);

	struct RbcomResp(*sec_chgpin4)  (struct RbcomConnect *, char *, char *);
	struct RbcomResp(*sec_chgpin8)  (struct RbcomConnect *, char *, char *);
	struct RbcomResp(*sec_lock)     (struct RbcomConnect *, char *);
	struct RbcomResp(*ops_privacy)  (struct RbcomConnect *, char *, uint8_t);
	struct RbcomResp(*ibcn)         (struct RbcomConnect *, char *, uint8_t *);
	struct RbcomResp(*altb)         (struct RbcomConnect *, char *, uint8_t *);
	struct RbcomResp(*eddy_uid)     (struct RbcomConnect *, char *, uint8_t *);
	struct RbcomResp(*eddy_url)     (struct RbcomConnect *, char *, uint8_t *);
	struct RbcomResp(*eddy_eid)     (struct RbcomConnect *, char *, uint8_t *);
	struct RbcomResp(*frame)        (struct RbcomConnect *, char *, uint8_t *, uint8_t);
	struct RbcomResp(*conn_period)   (struct RbcomConnect *, char *, uint32_t);
	struct RbcomResp(*conn_window)   (struct RbcomConnect *, char *, uint32_t);

	struct RbcomResp(*dump_allow)   (struct RbcomConnect *, char *, uint8_t);

	// deprecated
	//void  (*read)     (struct RbcomConnect*);
	//void  (*set_port) (struct RbcomConnect*, char*);
	//int   (*test)     (struct RbcomConnect*);
};

struct RbcomConnect *RbcomConnect_new(struct RbcomConfig *);
void RbcomConnect_debug(struct RbcomConnect *);
void RbcomConnect_destroy(struct RbcomConnect *);