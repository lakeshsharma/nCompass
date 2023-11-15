#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "../serhex/serhex_api.h"
#include "rbcom_serial_api.h"
#include "rbcom_api.h"
#include "rbcom_dump.h"
#include "../rblog/rblog.h"


//used for debugging
#define TERM_NRM "\x1b[0m"
#define TERM_RED "\x1b[31m"
#define TERM_GRN "\x1b[32m"

//const int RB_DEBUG = 0;

const int RB_CMD_LEN = 1;
const int MIN_RESP_LEN = 3;
const int MAX_STR_LEN = 254;
const int UUID_LEN = 16;
const int IBCN_LEN = 27;
const int ALTB_LEN = 28;
const int EDDY_UID_LEN = 22;
const int EDDY_URL_MAX_LEN = 24;
const int EDDY_EID_LEN = 14;
const int FRAME_LEN = 31;
const int MAJOR_LEN = 2;
const int MINOR_LEN = 2;
const int REFPWR_LEN = 1;
const int NAME_LEN = 24;
const int RB_API_SUCCESS = 0;
const int RB_API_FAIL = 1;

const char *resp_type_to_str(enum RespType resp_type) {
	if (resp_type == RESP_SUCCESS) {
		return "RESP_SUCCESS";
	}
	if (resp_type == RESP_ERROR) {
		return "RESP_ERROR";
	}
	if (resp_type == RESP_UNDEF) {
		return "RESP_UNDEF";
	}
	else {
		return NULL;
	}
}

// ===================================
// Debug
// -----------------------------------
//int rbcom_included_test(int a){
//  return a + a;
//}

// ====================================
//  Helpers
// ------------------------------------
void array_copy(uint8_t dest[], uint8_t src[], int len) {
	int i;
	for (i = 0; i < len; i++) {
		dest[i] = src[i];
	}
}

// caller is responsible for initialize api_cmd to the right size
// Note, all array types are adjusted to be pointers when passed to a function (see C99 6.7.5.3p7)
int serial_api_cmd(
	uint8_t *cmd_def,
	uint8_t *pin,
	uint8_t *req_data,
	uint8_t *api_cmd,
	int      api_cmd_len
) {
	uint8_t wip_pinlen = 0;

	if (pin) {
		wip_pinlen = pin[0];
	}
	char logmsg[255];
	snprintf(logmsg, 255, "Pin length: %d", wip_pinlen);
	rblog(RBLOG_DEBUG, logmsg);

	// copy cmd to api packet
	array_copy(api_cmd, cmd_def, RB_CMD_LEN);

	// No pin or data, just command
	if (api_cmd_len == RB_CMD_LEN) {
		return 0;
	}
	if (!pin && req_data) {
		size_t i;
		for (i = 0; i < api_cmd_len - RB_CMD_LEN; i++) {
			api_cmd[i + RB_CMD_LEN] = req_data[i];
		}
		return 0;
	}
	if (pin && !req_data) {
		size_t i;
		for (i = 0; i < wip_pinlen; i++) {
			api_cmd[i + RB_CMD_LEN] = pin[i + 1];
		}
		return 0;
	}
	if (pin && req_data) {
		size_t i;
		// We start at 1 because the first element is the size of the array
		for (i = 0; i < wip_pinlen; i++) {
			api_cmd[i + RB_CMD_LEN] = pin[i + 1];
		}
		size_t j;
		// We start at 1 because the first element is the size of the array
		for (j = 0; j < api_cmd_len - RB_CMD_LEN - wip_pinlen; j++) {
			api_cmd[j + RB_CMD_LEN + wip_pinlen] = req_data[j + 1];
		}
		return 0;
	}
	return -1;
}

// struct of len and data *Rbcom_serial_cmd_maker(uint8_t cmd[], char *data, uint8_t *cmddata)
//{
//TODO: Abstract converting user commands and data
// to serial commands and data here
// Need the individual use cases identified first
//}

// ====================================helpers

// ====================================
// Config Declaration and Definitions
// (See header file too)

// Config factory constructor
// ------------------------------------
struct RbcomConfig *RbcomConfig_new(char *port) {
	struct RbcomConfig *cfg = (RbcomConfig *)malloc(sizeof(struct RbcomConfig));
	assert(cfg != NULL);
	cfg->port = strdup(port);

	return cfg;
}

// Config debug helper
// ------------------------------------
void RbcomConfig_debug(struct RbcomConfig *cfg) {
	printf(" - RbcomConfig debug - \n");
	//printf("    Mem: %p\n", cfg);
	printf("    port: %s\n", cfg->port);
}

// Config Destructor
// ------------------------------------
void RbcomConfig_destroy(struct RbcomConfig *cfg) {
	assert(cfg != NULL);
	char logmsg[255];

	snprintf(logmsg, 255, "Destroying config obj connected to port: %s\n", cfg->port);
	rblog(RBLOG_DEBUG, logmsg);

	if (cfg->port != NULL) {
		free(cfg->port);
		cfg->port = NULL;
	}
	free(cfg);
	cfg = NULL;

	snprintf(logmsg, 255, "Destroyed config object, value should be 1: %d\n", (cfg == NULL));
	rblog(RBLOG_DEBUG, logmsg);
}

// ====================================


// ====================================
// Request Definition and Declarations

// Request Struct
// ------------------------------------
struct RbcomReq
{
	struct RbcomConfig *config;
	uint8_t *serial_data;
	int serial_data_len;
	int status;
	char *status_msg;

	// method declarations
	int(*setSerialData) (struct RbcomReq *, uint8_t *data, int len);
	int(*setStatus)     (struct RbcomReq *, int status, char *msg);
};

// Declaration for functions that will be assigned as methods
// ------------------------------------
int RbcomReq_setSerialData(struct RbcomReq *, uint8_t *, int);
int RbcomReq_setStatus(struct RbcomReq *, int, char *);

// Request factory constructor
// ------------------------------------
struct RbcomReq *RbcomReq_new(struct RbcomConfig *cfg) {
	int max_req_size = 255;  //extra byte for null termination

	assert(cfg != NULL);
	struct RbcomReq *req = (RbcomReq *)malloc(sizeof(struct RbcomReq));
	assert(req != NULL);

	req->config = cfg;

	// initialize serial_data
	req->serial_data = (uint8_t *)calloc(max_req_size + 1, sizeof(uint8_t));
	assert(req->serial_data != NULL);
	//memset(req->serial_data, 0, sizeof(uint8_t));

	req->serial_data_len = 0;
	req->status = -1;
	req->status_msg = strdup("-- Not set --");

	// set methods
	req->setSerialData = RbcomReq_setSerialData;
	req->setStatus = RbcomReq_setStatus;

	return req;
}

// Request debug helper
// ------------------------------------
void RbcomReq_debug(struct RbcomReq *req) {
	printf(" - RbcomReq debug - \n");
	//printf("    Mem: %p\n", req);
	printf("    serial_data_len: %d\n", req->serial_data_len);
	printf("    serial_data: ");
	if (req->serial_data_len > 0 && req->serial_data != NULL) {
		int i = 0;
		for (i = 0; i < req->serial_data_len; i++) {
			printf("%02x:", req->serial_data[i]);
		}
		printf("\n");
	}
	else {
		printf("-- undefined --\n");
	}
	printf("    status: %d\n", req->status);
	printf("    status_msg: %s\n", req->status_msg);

	RbcomConfig_debug(req->config);
}

// Request reset values
// ------------------------------------
void RbcomReq_reset(struct RbcomReq *req) {
	// initialize serial_data
	memset(req->serial_data, 0, sizeof(uint8_t));

	req->serial_data_len = 0;
	req->status = -1;

	if (req->status_msg != NULL) {
		free(req->status_msg);
	}
	req->status_msg = strdup("-- reset --");
}

// Request destructor
// ------------------------------------
void RbcomReq_destroy(struct RbcomReq *req) {
	char logmsg[255];
	snprintf(logmsg, 255, "Destroying request obj for port: %s\n", req->config->port);
	rblog(RBLOG_DEBUG, logmsg);

	if (req->config != NULL) {
		RbcomConfig_destroy(req->config);
	}
	free(req->serial_data);
	free(req->status_msg);
	free(req);
	req = NULL;

	snprintf(logmsg, 255, "Destroyed request object, value should be 1: %d\n", (req == NULL));
	rblog(RBLOG_DEBUG, logmsg);
}

// Definitions for functions that will be assigned as methods
// ------------------------------------

// Request - setSerialData
int RbcomReq_setSerialData(struct RbcomReq *self, uint8_t *serial_data, int len) {
	if (self->serial_data != NULL) {
		free(self->serial_data);
	}
	// nul terminate the serial data. Although this isn't strictly
	// necessary it ensures that there is at least a single nul byte
	// after the end of the data.
	// NOTE: The data itself might include nul bytes,
	// so this is useful only as a safety/debug measure

	int memlen = len + 1; // to allow for nul terminated strings
	self->serial_data = (uint8_t *)calloc(memlen, sizeof(uint8_t));
	assert(self->serial_data != NULL);
	//self->serial_data[len]='\0';
	memcpy(self->serial_data, serial_data, memlen);
	self->serial_data_len = len;
	// TODO: better error checking
	return 0;
}

// Request - setStatus
int RbcomReq_setStatus(struct RbcomReq *self, int status, char *msg) {
	if (self->status_msg != NULL) {
		free(self->status_msg);
	}
	self->status = status;

	self->status_msg = strdup(msg);

	// TODO: better error checking
	return 0;
}

// ====================================request


// ====================================
// Response Declarations and Definitions
// (see header file too)

// function declarations of logic containers for methods
// ------------------------------------
int RbcomResp_setStatus(struct RbcomResp *, int, const char *);
int RbcomResp_setOut(struct RbcomResp *);
int RbcomResp_setSerialResp(struct RbcomResp *, uint8_t, uint8_t[], uint8_t *);


// Response factory constructor
// ------------------------------------
struct RbcomResp *RbcomResp_new(struct RbcomReq *req) {
	struct RbcomResp *resp = (RbcomResp *)malloc(sizeof(struct RbcomResp));
	assert(resp != NULL);

	int max_bytes = 255;
	int max_out = 255;

	resp->req = req;

	// Initialize
	resp->status = -1;
	resp->status_msg = strdup("-- Not set --");
	resp->out = strdup("-- Not set --");
	resp->len = 0;
	resp->cmd_code[0] = 0x00;
	resp->cmd_code[1] = 0x00;

	// initialize serial_data
	resp->bytes = (uint8_t *)calloc(max_bytes, sizeof(uint8_t));
	assert(resp->bytes != NULL);
	//memset(resp->bytes, 0, sizeof(uint8_t));

	//initialize output
	resp->out = (char *)calloc(max_out, sizeof(char));
	assert(resp->out != NULL);
	// set methods
	resp->setStatus = RbcomResp_setStatus;
	resp->setOut = RbcomResp_setOut;
	resp->setSerialResp = RbcomResp_setSerialResp;

	return resp;
}

// Response debug helper
// ------------------------------------
void RbcomResp_debug(struct RbcomResp *resp) {
	printf(" --- #RbcomResp debug \n");
	//printf("    Mem: %p\n", resp);
	printf("    ret_type: %s\n", resp_type_to_str(resp->ret_type));
	printf("    code: 0x%02x\n", resp->code);
	printf("    status: %d\n", resp->status);
	printf("    status_msg: %s\n", resp->status_msg);
	printf("    output: %s\n", resp->out);
	printf("    len: %d\n", resp->len);
	printf(TERM_GRN);
	printf("    cmd_code: %02x:%02x\n", resp->cmd_code[0], resp->cmd_code[1]);

	printf("    bytes: ");
	if (resp->len > 0 && resp->bytes != NULL) {
		int i = 0;
		for (i = 0; i < resp->len - 2; i++) {
			printf("%02x:", resp->bytes[i]);
		}
		printf("\n");
	}
	else {
		printf(TERM_RED);
		printf("-- undefined --\n");
	}
	printf(TERM_NRM);
	RbcomReq_debug(resp->req);
}

// Reset response;
// ------------------------------------
void RbcomResp_reset(struct RbcomResp *resp) {
	RbcomReq_reset(resp->req);

	// Initialize
	resp->status = -1;

	if (resp->status_msg != NULL) {
		free(resp->status_msg);
	}
	//  if(resp->out != NULL){
	//      free(resp->out);
	//  }

	resp->status_msg = strdup("-- reset --");
	resp->out = strdup("-- reset --");
	resp->len = 0;
	resp->cmd_code[0] = 0x00;
	resp->cmd_code[1] = 0x00;

	// initialize serial_data
	memset(resp->bytes, 0, sizeof(uint8_t));
}

// Response Destructor
// ------------------------------------
void RbcomResp_destroy(struct RbcomResp *resp) {
	assert(resp != NULL);

	char logmsg[255];
	snprintf(logmsg, 255, "Destroying response obj\n");
	rblog(RBLOG_DEBUG, logmsg);


	if (resp->req != NULL) {
		RbcomReq_destroy(resp->req);
		resp->req = NULL;
	}
	free(resp->status_msg);
	resp->status_msg = NULL;

	free(resp->out);
	resp->out = NULL;

	free(resp->bytes);
	resp->bytes = NULL;
	free(resp);
	resp = NULL;

	snprintf(logmsg, 255, "Destroyed response object, value should be 1: %d\n", (resp == NULL));
	rblog(RBLOG_DEBUG, logmsg);
}

// Definitions for functions that will be assigned as methods
// ------------------------------------

// Response - setStatus
int RbcomResp_setStatus(struct RbcomResp *self, int status, const char *msg) {
	if (self->status_msg != NULL) {
		free(self->status_msg);
		self->status_msg = NULL;
	}
	self->status = status;


	self->status_msg = strdup(msg);

	// TODO: better error checking
	return 0;
}

// Response - setStatus
int RbcomResp_setOut(struct RbcomResp *self) {
	if (self->out != NULL) {
		free(self->out);
		self->out = NULL;
	}
	// TODO - constantify maximum of output data
	//memset(self->out, 0, 255);

	if (self->ret_type == RESP_SUCCESS) {
		if (self->len == 2) {
			self->out = strdup("OK");
		}
		else if (self->code == RB_STAT_RET_STR[0]) {
			// bytes represent string
			self->out = strdup((char *)self->bytes);
			//self->out = "string";
		}
		else if (self->code == RB_STAT_RET_HEX[0]) {
			char outmsg[512];
			char hexprefix[3];
			snprintf(hexprefix, 3, "0x");
			// first two bytes are command data
			int ret_len = self->len - 2;
			int resp_start = 1; //first byte is hex command
			int i = 0;
			strcpy(outmsg, hexprefix);
			for (i = resp_start; i < ret_len; i++) {
				char bytestr[8];
				snprintf(bytestr, 8, "%02X", self->bytes[i]);
				strcat(outmsg, bytestr);
			}
			self->out = strdup(&outmsg[0]);
		}
	}
	else if (self->ret_type == RESP_ERROR) {
		char outmsg[512];

		snprintf(outmsg, 512, "Error Returned: %s", self->status_msg);
		self->out = strdup(&outmsg[0]);
	}
	else {
		self->out = strdup("Error - Missing return type (Neither SUCCESS nor ERROR)");
	}
	// TODO: better error checking
	return 0;
}

// Response - setSerialResp
int RbcomResp_setSerialResp(struct RbcomResp *self, uint8_t len, uint8_t cmd_code[], uint8_t *bytes) {

	if (self->bytes != NULL) {
		free(self->bytes);
	}
	// len is the len of cmd_code + bytes;
	int cmd_code_len = 2;
	self->len = len;
	self->cmd_code[0] = cmd_code[0];
	self->cmd_code[1] = cmd_code[1];

	// nul terminate the serial data. Although this isn't strictly
	// necessary it ensures that there is at least a single nul byte
	// after the end of the data.
	// NOTE: The data itself might include nul bytes,
	// so this is useful only as a safety/debug measure
	int byteslen = len - cmd_code_len;
	int bytesmemlen = byteslen + 1; // to allow for nul terminated strings
	self->bytes = (uint8_t *)calloc(bytesmemlen, sizeof(uint8_t));
	assert(self->bytes != NULL);
	self->bytes[byteslen] = '\0';
	memcpy(self->bytes, bytes, byteslen);
	// TODO: better error checking
	return 0;
}

// ====================================response

// ====================================
// Connection Declaration and Definitions
// (see header file too)

// function declarations of logic containers for methods
// ------------------------------------
// void   readValue(struct RbcomConnect *self);
// void   Rbcom_set_port(struct RbcomConnect *self, char *_port_);
// int    Rbcom_test(struct RbcomConnect *self);
struct RbcomResp Rbcom_sys_ping(struct RbcomConnect *self);
struct RbcomResp Rbcom_sys_echo(struct RbcomConnect *self, char echo[], int echo_len);
struct RbcomResp Rbcom_sys_raw(struct RbcomConnect *self, char raw[], int raw_len);
struct RbcomResp Rbcom_sys_dfu(struct RbcomConnect *self, char pin[], uint8_t delay);
struct RbcomResp Rbcom_sys_reboot(struct RbcomConnect *self, char pin[], uint32_t delay);
struct RbcomResp Rbcom_sys_facrst(struct RbcomConnect *self, char pin[]);
struct RbcomResp Rbcom_adv_uuid(struct RbcomConnect *self, char pin[], uint8_t uuid[]);
struct RbcomResp Rbcom_adv_major(struct RbcomConnect *self, char pin[], uint8_t major[]);
struct RbcomResp Rbcom_adv_minor(struct RbcomConnect *self, char pin[], uint8_t minor[]);
struct RbcomResp Rbcom_adv_refpwr(struct RbcomConnect *self, char pin[], uint8_t refpwr);
struct RbcomResp Rbcom_ops_advint(struct RbcomConnect *self, char pin[], uint16_t advint);
struct RbcomResp Rbcom_ops_advrate(struct RbcomConnect *self, char pin[], uint8_t advrate);
struct RbcomResp Rbcom_ops_txpwr(struct RbcomConnect *self, char pin[], uint8_t txpwr);
struct RbcomResp Rbcom_ops_bctype(struct RbcomConnect *self, char pin[], uint8_t bctype);
struct RbcomResp Rbcom_ops_tlmtype(struct RbcomConnect *self, char pin[], uint8_t tlmtype);
struct RbcomResp Rbcom_ops_tlmratio(struct RbcomConnect *self, char pin[], uint8_t tlmratio);
struct RbcomResp Rbcom_ops_connto(struct RbcomConnect *self, char pin[], uint32_t connto);
struct RbcomResp Rbcom_ops_advoff(struct RbcomConnect *self, char pin[]);
struct RbcomResp Rbcom_info_name(struct RbcomConnect *self, char pin[], char name[]);
struct RbcomResp Rbcom_info_get(struct RbcomConnect *self, uint8_t type);

struct RbcomResp Rbcom_sec_chgpin4(struct RbcomConnect *self, char pin[], char newpin[]);
struct RbcomResp Rbcom_sec_chgpin8(struct RbcomConnect *self, char pin[], char newpin[]);
struct RbcomResp Rbcom_sec_lock(struct RbcomConnect *self, char pin[]);
struct RbcomResp Rbcom_ops_privacy(struct RbcomConnect *self, char pin[], uint8_t privacy);
struct RbcomResp Rbcom_ibcn(struct RbcomConnect *self, char pin[], uint8_t ibcn[]);
struct RbcomResp Rbcom_altb(struct RbcomConnect *self, char pin[], uint8_t altb[]);
struct RbcomResp Rbcom_eddy_uid(struct RbcomConnect *self, char pin[], uint8_t uid[]);
struct RbcomResp Rbcom_eddy_url(struct RbcomConnect *self, char pin[], uint8_t url[]);
struct RbcomResp Rbcom_eddy_eid(struct RbcomConnect *self, char pin[], uint8_t eid[]);
struct RbcomResp Rbcom_frame(struct RbcomConnect *self, char pin[], uint8_t frame[], uint8_t frame_slot);
struct RbcomResp Rbcom_conn_period(struct RbcomConnect *self, char pin[], uint32_t period);
struct RbcomResp Rbcom_conn_window(struct RbcomConnect *self, char pin[], uint32_t window);

struct RbcomResp Rbcom_dump_allow(struct RbcomConnect *self, char pin[], uint8_t flag);

// Connector factory constructor
// ------------------------------------
struct RbcomConnect *RbcomConnect_new(struct RbcomConfig *cfg) {
	struct RbcomConnect *conn = (struct RbcomConnect *)malloc(sizeof(struct RbcomConnect));
	assert(conn != NULL);

	// configuration container
	conn->config = cfg;

	// request container
	conn->req = RbcomReq_new(cfg);
	conn->resp = RbcomResp_new(conn->req);

	// API functions
	conn->sys_ping = Rbcom_sys_ping;
	conn->sys_echo = Rbcom_sys_echo;
	conn->sys_raw = Rbcom_sys_raw;
	conn->sys_dfu = Rbcom_sys_dfu;
	conn->sys_reboot = Rbcom_sys_reboot;
	conn->sys_facrst = Rbcom_sys_facrst;
	conn->adv_uuid = Rbcom_adv_uuid;
	conn->adv_major = Rbcom_adv_major;
	conn->adv_minor = Rbcom_adv_minor;
	conn->adv_refpwr = Rbcom_adv_refpwr;
	conn->ops_advint = Rbcom_ops_advint;
	conn->ops_advrate = Rbcom_ops_advrate;
	conn->ops_txpwr = Rbcom_ops_txpwr;
	conn->ops_bctype = Rbcom_ops_bctype;
	conn->ops_tlmtype = Rbcom_ops_tlmtype;
	conn->ops_tlmratio = Rbcom_ops_tlmratio;
	conn->ops_connto = Rbcom_ops_connto;
	conn->ops_advoff = Rbcom_ops_advoff;
	conn->info_name = Rbcom_info_name;
	conn->info_get = Rbcom_info_get;

	conn->sec_chgpin4 = Rbcom_sec_chgpin4; // Deprecated
	conn->sec_chgpin8 = Rbcom_sec_chgpin8;
	conn->sec_lock = Rbcom_sec_lock;
	conn->ops_privacy = Rbcom_ops_privacy;
	conn->ibcn = Rbcom_ibcn;
	conn->altb = Rbcom_altb;
	conn->eddy_uid = Rbcom_eddy_uid;
	conn->eddy_url = Rbcom_eddy_url;
	conn->eddy_eid = Rbcom_eddy_eid;
	conn->frame = Rbcom_frame;
	conn->conn_period = Rbcom_conn_period;
	conn->conn_window = Rbcom_conn_window;
	conn->dump_allow = Rbcom_dump_allow;
	return conn;
}

// Connector debug helper
// ------------------------------------
void RbcomConnect_debug(struct RbcomConnect *conn) {
	printf("- RbcomConnect debug - \n");
	//printf("  Mem: %p\n", conn);
	RbcomResp_debug(conn->resp);
}

// Connector Destructor
// ------------------------------------
void RbcomConnect_destroy(struct RbcomConnect *obj) {
	assert(obj != NULL);
	char logmsg[255];

	snprintf(logmsg, 255, "Destroying conn to port: %s\n", obj->config->port);
	rblog(RBLOG_DEBUG, logmsg);

	RbcomResp_destroy((obj)->resp);
	free((obj));
	obj = NULL;

	snprintf(logmsg, 255, "Destroyed connection object, value should be 1: %d\n", (obj == NULL));
	rblog(RBLOG_DEBUG, logmsg);
}

// ====================================connect


// ====================================
// Main API Logic


// Response was Success
struct RbcomResp *Rbcom_success_resp(struct RbcomResp *resp, uint8_t *resp_bytes) {
	resp->ret_type = RESP_SUCCESS;

	resp->setStatus(resp, RESP_SUCCESS, serial_success_to_str(resp->cmd_code[1]));
	resp->setOut(resp);

	return resp;
}

// TODO: just a stub right now
// Parse serial port error responses
struct RbcomResp *Rbcom_error_resp(struct RbcomResp *resp, uint8_t *resp_bytes) {
	resp->ret_type = RESP_ERROR;

	resp->setStatus(resp, RESP_ERROR, serial_error_to_str(resp->cmd_code[1]));

	resp->setOut(resp);
	//  char errmsg[255];
	//  snprintf(errmsg, 255, "rbcom failed, err code: %d", resp->cmd_code[1]);
	//  rblog(ERROR, errmsg);
	return resp;
}

/*
* Parse serial API Response
*/


struct RbcomResp *Rbcom_parse(struct RbcomResp *resp, uint8_t func_status, uint8_t serial_api_resp[]) {
	// hold log data
	char errmsg[255];
	snprintf(errmsg, 255, "parsing...");
	rblog(RBLOG_DEBUG, errmsg);
	//char bytecheck[8];
	resp->ret_type = RESP_UNDEF;
	resp->len = serial_api_resp[0];
	resp->cmd_code[0] = serial_api_resp[1];
	resp->cmd_code[1] = serial_api_resp[2];
	resp->code = serial_api_resp[2];

	int i = 0;
	for (i = 0; i < resp->len - 2; i++) {
		resp->bytes[i] = serial_api_resp[i + 3];
	}
	resp->bytes[resp->len - 2] = '\0';

	if (resp->cmd_code[0] == RB_SERIAL_SUCCESS[0]) {
		snprintf(errmsg, 255, "serial port success");
		rblog(RBLOG_INFO, errmsg);
		return Rbcom_success_resp(resp, resp->bytes);
	}
	else if (resp->cmd_code[0] == RB_SERIAL_ERROR[0]) {
		snprintf(errmsg, 255, "serial port returned error");
		rblog(RBLOG_WARN, errmsg);
		return Rbcom_error_resp(resp, resp->bytes);
	}
	else {
		snprintf(errmsg, 255, "unknown response code from rbcom: %d", resp->cmd_code[0]);
		rblog(RBLOG_WARN, errmsg);
	}
	return resp;
}

struct RbcomResp *Rbcom_serial_cmd_delay(struct RbcomResp *resp, int delay) {
	// hold log data
	char errmsg[255];
	//char bytecheck[8];

	uint8_t serial_response[255] = {
		0
	};

	snprintf(errmsg, 255, "rbcom api sending to serial port");
	rblog(RBLOG_DEBUG, errmsg);
	//  extern uint8_t RB_OPS_BCTYPE[1];

	uint8_t func_status;
	if (delay > 0) {
		func_status = send_cmd_with_delay(
			resp->req->config->port,
			resp->req->serial_data, resp->req->serial_data_len,
			serial_response,
			delay
		);
	}
	else {
		// default delay
		func_status = send_cmd(
			resp->req->config->port,
			resp->req->serial_data, resp->req->serial_data_len,
			serial_response
		);
	}
	if (func_status == 0) {
		snprintf(errmsg, 255, "sending serial port response to parser");
		rblog(RBLOG_DEBUG, errmsg);
		return Rbcom_parse(resp, func_status, serial_response);
	}
	else {
		snprintf(errmsg, 255, "unable to parse");
		rblog(RBLOG_WARN, errmsg);

		//TODO: check exit code of setStatus for success

		resp->ret_type = RESP_ERROR;
		if (func_status == 250) {
			// File descriptor became unset
			resp->setStatus(resp, -2, "Device not responding\npossible causes:\n  device is busy\n  wrong device or version");
		}
		else {
			resp->setStatus(resp, -1, "Error writing to serial port, is it open/configured?");
		}
		resp->setOut(resp);
		return resp;
	}
}

// Interface to Rbcom serial port API
struct RbcomResp *Rbcom_serial_cmd(struct RbcomResp *resp) {
	return Rbcom_serial_cmd_delay(resp, 0);
}

struct RbcomResp Rbcom_sys_ping(struct RbcomConnect *self) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_SYS_PING,
		NULL,
		NULL,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -2, "Unable to Set Command");
		return *(self->resp);
	}
	//int req_status = req->setSerialData(req, RB_SYS_PING, sizeof(RB_SYS_PING));
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));
	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "PING request data set");
	}
	else {
		set_req_status = req->setStatus(req, -3, "PING request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	//printf("- PING Request Debug -\n");
	//RbcomReq_debug(req);
	//printf("  .... sending to request generator -\n");
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_sys_echo(struct RbcomConnect *self, char echo[], int echo_len) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;

	// Don't use strlen else the size will be wrong
	// Problem: echo can contain byte values of 0x00, but these
	// should not be counted as string terminators.
	//int echo_len = sizeof(echo);
	uint8_t echo_bytes[echo_len];
	int i;
	for (i = 0; i < echo_len; i++) {
		echo_bytes[i] = (uint8_t)echo[i];
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + echo_len;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_SYS_ECHO,
		NULL,
		echo_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	//TODO: Abstract Command Parsing

	// shorter handle for self->resp->req
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));
	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "ECHO request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "ECHO request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_sys_raw(struct RbcomConnect *self, char raw[], int raw_len) {

	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;

	// Don't use strlen else the size will be wrong
	// Problem: echo can contain byte values of 0x00, but these
	// should not be counted as string terminators.
	//int echo_len = sizeof(echo);
	uint8_t raw_bytes[raw_len];
	int i;
	for (i = 0; i < raw_len; i++) {
		raw_bytes[i] = (uint8_t)raw[i];
	}
	// initialize serial api packet array
	// First byte(s) contain command byte(s)
	int packet_len = raw_len;
	uint8_t serial_api_packet[packet_len];

	uint8_t cmd_bytes[RB_CMD_LEN];
	uint8_t arg_bytes[raw_len - RB_CMD_LEN];

	// Copy command bytes from raw bytes
	memcpy(cmd_bytes, raw_bytes, RB_CMD_LEN * sizeof(uint8_t));

	// Copy arg bytes from raw bytes
	memcpy(arg_bytes, &raw_bytes[RB_CMD_LEN], (raw_len - RB_CMD_LEN) * sizeof(uint8_t));

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		cmd_bytes,
		NULL,
		arg_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	//TODO: Abstract Command Parsing

	// shorter handle for self->resp->req
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));
	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "RAW request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "RAW request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_sys_dfu(struct RbcomConnect *self, char *pin, uint8_t delay) {
	//TODO: test pin length?
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	// First byte is length
	int delay_len = 1;
	uint8_t delay_bytes[2];
	delay_bytes[0] = delay_len;
	delay_bytes[1] = delay;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + delay_len;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_SYS_DFU,
		pin_bytes,
		delay_bytes,
		serial_api_packet,
		packet_len);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));
	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "DFU request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "DFU request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_sys_reboot(struct RbcomConnect *self, char *pin, uint32_t delay) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	// First byte is length
	int delay_len = 4;
	uint8_t delay_bytes[5];
	delay_bytes[0] = delay_len;

	delay_bytes[4] = (delay >> 24) & 0xff;
	delay_bytes[3] = (delay >> 16) & 0xff;
	delay_bytes[2] = (delay >> 8) & 0xff;
	delay_bytes[1] = delay & 0xff;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + delay_len;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_OPS_REBOOT,
		pin_bytes,
		delay_bytes,
		serial_api_packet,
		packet_len);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));
	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "Reboot request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "Reboot request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_sys_facrst(struct RbcomConnect *self, char *pin) {
	//TODO: test pin length?
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_SYS_FACRST,
		pin_bytes,
		NULL,
		serial_api_packet,
		packet_len);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));
	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "Factory reset request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "Factory reset request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	// Factory Reset is slower to process through serial port than most commands
	return *Rbcom_serial_cmd_delay(self->resp, 100000);
}

struct RbcomResp Rbcom_adv_uuid(
	struct RbcomConnect *self,
	char *               pin,
	uint8_t              uuid[16]
) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);


	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	uint8_t uuid_bytes[UUID_LEN + 1];
	uuid_bytes[0] = UUID_LEN;

	for (i = 0; i < UUID_LEN; i++) {
		uuid_bytes[i + 1] = uuid[i];
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + UUID_LEN;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_ADV_UUID,
		pin_bytes,
		uuid_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	//int req_status = req->setSerialData(req, cmd, UUID_LEN+PIN_LEN+1);
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));
	//RbcomResp_debug(self->resp);
	int set_req_status;

	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "UUID request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "UUID request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_adv_major(struct RbcomConnect *self, char *pin, uint8_t major[2]) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	uint8_t major_bytes[MAJOR_LEN + 1];
	major_bytes[0] = MAJOR_LEN;
	for (i = 0; i < MAJOR_LEN; i++) {
		major_bytes[i + 1] = (uint8_t)major[i];
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + MAJOR_LEN;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_ADV_MAJOR,
		pin_bytes,
		major_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "MAJOR request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "MAJOR request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_adv_minor(
	struct RbcomConnect *self,
	char *               pin,
	uint8_t              minor[2]) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	uint8_t minor_bytes[MINOR_LEN + 1];
	minor_bytes[0] = MINOR_LEN;
	for (i = 0; i < MINOR_LEN; i++) {
		minor_bytes[i + 1] = (uint8_t)minor[i];
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + MINOR_LEN;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_ADV_MINOR,
		pin_bytes,
		minor_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "MINOR request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "MINOR request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_adv_refpwr(
	struct RbcomConnect *self,
	char *               pin,
	uint8_t              refpwr) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	uint8_t refpwr_bytes[REFPWR_LEN + 1];
	refpwr_bytes[0] = REFPWR_LEN;
	refpwr_bytes[1] = refpwr;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + REFPWR_LEN;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_ADV_REFPWR,
		pin_bytes,
		refpwr_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "REFPWR request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "REFPWR request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_ops_advint(struct RbcomConnect *self, char *pin, uint16_t adv_int) {
	RbcomResp_reset(self->resp);

	//valid range of adv_int is 81-799
	// enforced at dongle

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	int adv_int_len = 2;
	uint8_t adv_int8[adv_int_len + 1];
	adv_int8[0] = adv_int_len;
	//convert to little endian
	adv_int8[2] = (adv_int >> 8);
	adv_int8[1] = adv_int & 0xff;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + adv_int_len;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_OPS_ADVINT,
		pin_bytes,
		adv_int8,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "Adv interval request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "Adv interval request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_ops_advrate(struct RbcomConnect *self, char *pin, uint8_t advrate) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	int advrate_len = 1;
	uint8_t advrate_bytes[advrate_len + 1];
	advrate_bytes[0] = advrate_len;
	advrate_bytes[1] = advrate;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + advrate_len;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_OPS_ADVRATE,
		pin_bytes,
		advrate_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "Adv rate request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "Adv rate request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_ops_txpwr(struct RbcomConnect *self, char *pin, uint8_t txpwr) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	size_t txpwr_len = 1;
	uint8_t txpwr_bytes[txpwr_len + 1];
	txpwr_bytes[0] = txpwr_len;
	txpwr_bytes[1] = txpwr;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + txpwr_len;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_OPS_TXPWR,
		pin_bytes,
		txpwr_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "TXPWR request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "TXPWR request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_ops_bctype(struct RbcomConnect *self, char *pin, uint8_t bctype) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	size_t bctype_bytes_len = 1;
	uint8_t bctype_bytes[bctype_bytes_len + 1];
	bctype_bytes[0] = bctype_bytes_len;
	bctype_bytes[1] = bctype;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + bctype_bytes_len;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_OPS_BCTYPE,
		pin_bytes,
		bctype_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "BCTYPE request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "BCTYPE request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_ops_tlmtype(struct RbcomConnect *self, char *pin, uint8_t tlmtype) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	size_t tlmtype_bytes_len = 1;
	uint8_t tlmtype_bytes[tlmtype_bytes_len + 1];
	tlmtype_bytes[0] = tlmtype_bytes_len;
	tlmtype_bytes[1] = tlmtype;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + tlmtype_bytes_len;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_OPS_TLMTYPE,
		pin_bytes,
		tlmtype_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "TLMTYPE request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "TLMTYPE request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_ops_tlmratio(struct RbcomConnect *self, char *pin, uint8_t tlmratio) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	size_t tlmratio_bytes_len = 1;
	uint8_t tlmratio_bytes[tlmratio_bytes_len + 1];
	tlmratio_bytes[0] = tlmratio_bytes_len;
	tlmratio_bytes[1] = tlmratio;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + tlmratio_bytes_len;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_OPS_TLMRATIO,
		pin_bytes,
		tlmratio_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "TLMRATIO request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "TLMRATIO request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}


struct RbcomResp Rbcom_ops_connto(struct RbcomConnect *self, char *pin, uint32_t connto) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	// convert to little endian
	size_t connto_bytes_len = 4;
	uint8_t connto_bytes[connto_bytes_len + 1];
	connto_bytes[0] = connto_bytes_len;
	connto_bytes[4] = (connto >> 24) & 0xff;
	connto_bytes[3] = (connto >> 16) & 0xff;
	connto_bytes[2] = (connto >> 8) & 0xff;
	connto_bytes[1] = connto & 0xff;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + connto_bytes_len;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_OPS_CONNTO,
		pin_bytes,
		connto_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "CONNTO (connection timeout) request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "CONNTO (connection timeout) request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_ops_advoff(struct RbcomConnect *self, char *pin) {
	//TODO: test pin length?
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_OPS_ADVOFF,
		pin_bytes,
		NULL,
		serial_api_packet,
		packet_len);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));
	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "ADVOFF request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "ADVOFF request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_info_name(struct RbcomConnect *self, char *pin, char *name) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	//ensure nul terminated
	name[NAME_LEN] = '\0';
	int name_size = strlen(name);

	uint8_t name_bytes[NAME_LEN + 1];
	name_bytes[0] = NAME_LEN;
	for (i = 0; i < name_size; i++) {
		name_bytes[i + 1] = (uint8_t)name[i];
	}
	//zero out the rest of the string
	for (i = 0; i < NAME_LEN - name_size; i++) {
		name_bytes[i + name_size + 1] = '\0';
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + NAME_LEN;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_INFO_NAME,
		pin_bytes,
		name_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "NAME request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "NAME request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_info_get(struct RbcomConnect *self, uint8_t type) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;

	//  uint8_t pin_bytes[PIN_LEN];
	//  size_t i;
	//  for(i=0;i<PIN_LEN;i++){
	//    pin_bytes[i] = (uint8_t) pin[i];
	//  }

	uint8_t type_bytes[1];
	type_bytes[0] = type;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + sizeof(type_bytes);
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_INFO_GET,
		NULL,
		type_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "INFO request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "INFO request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}


struct RbcomResp Rbcom_sec_chgpin(uint8_t *cmdbytes, struct RbcomConnect *self, char *pin, char *newpin) {
	RbcomResp_reset(self->resp);
	int pinlen = strlen(pin);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	uint8_t new_pin_bytes[(pinlen * 2) + 1];
	new_pin_bytes[0] = pinlen * 2;
	for (i = 0; i < pinlen; i++) {
		new_pin_bytes[i + 1] = (uint8_t)newpin[i];
	}
	for (i = 0; i < pinlen; i++) {
		new_pin_bytes[i + pinlen + 1] = (uint8_t)newpin[i];
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen * 3;
	uint8_t serial_api_packet[packet_len];

	char logmsg[255];
	snprintf(logmsg, 255, "Change Pin from: '%s' to '%s'", pin, newpin);
	rblog(RBLOG_DEBUG, logmsg);
	snprintf(logmsg, 255, "Pin length: %d, Packet Len: %d", pinlen, packet_len);
	rblog(RBLOG_DEBUG, logmsg);
	//snprintf(logmsg, 255, "Change Pin from: '%s' to '%s'", pin_bytes, new_pin_bytes);
	//rblog(RBLOG_INFO, logmsg);
	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		cmdbytes,
		pin_bytes,
		new_pin_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "PIN request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "PIN request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_sec_chgpin4(struct RbcomConnect *self, char *pin, char *newpin) {
	return Rbcom_sec_chgpin(RB_SEC_CHGPIN4, self, pin, newpin);
}

struct RbcomResp Rbcom_sec_chgpin8(struct RbcomConnect *self, char *pin, char *newpin) {
	return Rbcom_sec_chgpin(RB_SEC_CHGPIN8, self, pin, newpin);
}

struct RbcomResp Rbcom_sec_lock(struct RbcomConnect *self, char *pin) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_SEC_LOCK,
		pin_bytes,
		NULL,
		serial_api_packet,
		packet_len);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "LOCK request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "LOCK request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_ops_privacy(struct RbcomConnect *self, char *pin, uint8_t privacy) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	size_t privacy_bytes_len = 1;
	uint8_t privacy_bytes[privacy_bytes_len + 1];
	privacy_bytes[0] = privacy_bytes_len;
	privacy_bytes[1] = privacy;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + privacy_bytes_len;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_OPS_PRIVACY,
		pin_bytes,
		privacy_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "PRIVACY request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "PRIVACY request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_ibcn(
	struct RbcomConnect *self,
	char *               pin,
	uint8_t              ibcn[27]
) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);


	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	uint8_t ibcn_bytes[IBCN_LEN + 1];
	ibcn_bytes[0] = IBCN_LEN;

	for (i = 0; i < IBCN_LEN; i++) {
		ibcn_bytes[i + 1] = ibcn[i];
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + IBCN_LEN;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_IBCN,
		pin_bytes,
		ibcn_bytes,
		serial_api_packet,
		packet_len
	);


	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	//int req_status = req->setSerialData(req, cmd, UUID_LEN+PIN_LEN+1);
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));
	//RbcomResp_debug(self->resp);
	int set_req_status;

	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "IBCN request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "IBCN request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_altb(
	struct RbcomConnect *self,
	char *               pin,
	uint8_t              altb[28]
) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);


	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	uint8_t altb_bytes[ALTB_LEN + 1];
	altb_bytes[0] = ALTB_LEN;

	for (i = 0; i < ALTB_LEN; i++) {
		altb_bytes[i + 1] = altb[i];
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + ALTB_LEN;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_ALTB,
		pin_bytes,
		altb_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	//int req_status = req->setSerialData(req, cmd, UUID_LEN+PIN_LEN+1);
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));
	//RbcomResp_debug(self->resp);
	int set_req_status;

	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "ALTB request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "ALTB request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_eddy_uid(
	struct RbcomConnect *self,
	char *               pin,
	uint8_t              uid[22]
) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);


	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	uint8_t uid_bytes[EDDY_UID_LEN + 1];
	uid_bytes[0] = EDDY_UID_LEN;

	for (i = 0; i < EDDY_UID_LEN; i++) {
		uid_bytes[i + 1] = uid[i];
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + EDDY_UID_LEN;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_EDDY_UID,
		pin_bytes,
		uid_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	//int req_status = req->setSerialData(req, cmd, UUID_LEN+PIN_LEN+1);
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));
	//RbcomResp_debug(self->resp);
	int set_req_status;

	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "EDDY UID request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "EDDY UID request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_eddy_url(
	struct RbcomConnect *self,
	char *               pin,
	uint8_t              url[24]
) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);


	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	uint8_t url_bytes[EDDY_URL_MAX_LEN + 1];
	url_bytes[0] = EDDY_URL_MAX_LEN;

	for (i = 0; i < EDDY_URL_MAX_LEN; i++) {
		url_bytes[i + 1] = url[i];
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + EDDY_URL_MAX_LEN;
	uint8_t serial_api_packet[packet_len];


	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_EDDY_URL,
		pin_bytes,
		url_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	//int req_status = req->setSerialData(req, cmd, UUID_LEN+PIN_LEN+1);
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));
	//RbcomResp_debug(self->resp);
	int set_req_status;

	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "EDDY URL request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "EDDY URL request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_eddy_eid(
	struct RbcomConnect *self,
	char *               pin,
	uint8_t              eid[14]
) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);


	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	uint8_t eid_bytes[EDDY_EID_LEN + 1];
	eid_bytes[0] = EDDY_EID_LEN;

	for (i = 0; i < EDDY_EID_LEN; i++) {
		eid_bytes[i + 1] = eid[i];
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + EDDY_EID_LEN;
	uint8_t serial_api_packet[packet_len];


	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_EDDY_EID,
		pin_bytes,
		eid_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	//int req_status = req->setSerialData(req, cmd, UUID_LEN+PIN_LEN+1);
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));
	//RbcomResp_debug(self->resp);
	int set_req_status;

	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "EDDY EID request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "EDDY EID request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_frame(
	struct RbcomConnect *self,
	char *               pin,
	uint8_t              frame[31],
	uint8_t              frame_slot
) {

	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	uint8_t frame_bytes[FRAME_LEN + 1];
	frame_bytes[0] = FRAME_LEN;

	for (i = 0; i < FRAME_LEN; i++) {
		frame_bytes[i + 1] = frame[i];
	}
	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + FRAME_LEN;
	uint8_t serial_api_packet[packet_len];


	//insert data into packet
	uint8_t cmd_bytes[1];
	cmd_bytes[0] = RB_FRAME_BASE[0] + frame_slot;
	int set_cmd_status = serial_api_cmd(
		cmd_bytes,
		pin_bytes,
		frame_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	//int req_status = req->setSerialData(req, cmd, UUID_LEN+PIN_LEN+1);
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));
	//RbcomResp_debug(self->resp);
	int set_req_status;

	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "Frame request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "Frame request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_conn_period(struct RbcomConnect *self, char *pin, uint32_t period) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	// convert to little endian
	size_t period_bytes_len = 4;
	uint8_t period_bytes[period_bytes_len + 1];
	period_bytes[0] = period_bytes_len;
	period_bytes[4] = (period >> 24) & 0xff;
	period_bytes[3] = (period >> 16) & 0xff;
	period_bytes[2] = (period >> 8) & 0xff;
	period_bytes[1] = period & 0xff;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + period_bytes_len;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_CONN_PERIOD,
		pin_bytes,
		period_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "CONN_PERIOD (connection period) request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "CONN_PERIOD (connection period) request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_conn_window(struct RbcomConnect *self, char *pin, uint32_t window) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}
	// convert to little endian
	size_t window_bytes_len = 4;
	uint8_t window_bytes[window_bytes_len + 1];
	window_bytes[0] = window_bytes_len;
	window_bytes[4] = (window >> 24) & 0xff;
	window_bytes[3] = (window >> 16) & 0xff;
	window_bytes[2] = (window >> 8) & 0xff;
	window_bytes[1] = window & 0xff;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + window_bytes_len;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_CONN_WINDOW,
		pin_bytes,
		window_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "CONN_WINDOW (connection window) request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "CONN_WINDOW (connection window) request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

struct RbcomResp Rbcom_dump_allow(struct RbcomConnect *self, char *pin, uint8_t flag) {
	RbcomResp_reset(self->resp);

	// shorter handle for self->resp->req
	struct RbcomReq *req = self->resp->req;
	int pinlen = strlen(pin);

	uint8_t pin_bytes[pinlen + 1];
	size_t i;
	pin_bytes[0] = pinlen;

	for (i = 0; i < pinlen; i++) {
		pin_bytes[i + 1] = (uint8_t)pin[i];
	}

	size_t flag_bytes_len = 1;

	uint8_t flag_bytes[flag_bytes_len + 1];
	flag_bytes[0] = flag_bytes_len;
	flag_bytes[1] = flag;

	// initialize serial api packet array
	int packet_len = RB_CMD_LEN + pinlen + flag_bytes_len;
	uint8_t serial_api_packet[packet_len];

	//insert data into packet
	int set_cmd_status = serial_api_cmd(
		RB_DUMP_FLAG,
		pin_bytes,
		flag_bytes,
		serial_api_packet,
		packet_len
	);

	if (set_cmd_status != 0) {
		req->setStatus(req, -1, "Unable to Set Command");
		return *(self->resp);
	}
	int req_status = req->setSerialData(req, serial_api_packet, sizeof(serial_api_packet));

	int set_req_status;
	if (req_status == 0) {
		set_req_status = req->setStatus(req, req_status, "INFO request data set");
	}
	else {
		set_req_status = req->setStatus(req, -1, "INFO request failed before sending");
	}
	if (set_req_status != 0) {
		// TODO: Unable to set status, what to do?
	}
	return *Rbcom_serial_cmd(self->resp);
}

// ====================================api logic