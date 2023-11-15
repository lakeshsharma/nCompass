#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include "../rbcom/rbcom_api.h"
#include "../rblog/rblog.h"

#define RB_MAX_PATH 1035
#define RB_MAX_NUM_SERIAL_PORTS 255

char* RB_DEFAULT_PORT("/dev/radbeacon");

int find_radbeacons(char port_list[])
{
	int valid_port_count = 0;

	// Probe for RadBeacon with echo command to compare sent and returned values
	char *check_val = "98765432";
	int check_val_len = 8;
	char *valid_val = "0x3938373635343332";     //check val in byte form
	int valid_val_len = 18;

	// Run the echo command via API against the port to see if it's a RadBeacon 2.0+
	struct RbcomConfig *cfg = RbcomConfig_new(RB_DEFAULT_PORT);
	struct RbcomConnect *conn = RbcomConnect_new(cfg);
	struct RbcomResp echo_resp;
	echo_resp = conn->sys_echo(conn, check_val, check_val_len);

	// Compare response to expected value
	if (strncmp(echo_resp.out, valid_val, valid_val_len) == 0)
	{
		strncpy(port_list, RB_DEFAULT_PORT, RB_MAX_PATH);
		valid_port_count++;
	}

	return valid_port_count;
}
