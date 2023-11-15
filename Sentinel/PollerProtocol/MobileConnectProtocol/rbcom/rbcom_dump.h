#pragma once

#include <stdint.h>
#include "rbcom_api.h"

#define RBDUMP_MOD_CMD 0x61

#define RBDUMP_MAX_FIELD_LEN 63
#define RBDUMP_MAX_FIELDS 255
#define RBDUMP_MAX_UNKNOWN_FIELDS 20


// Each field in a module
typedef struct
{
    uint8_t id;
    uint8_t len;
    uint8_t val[RBDUMP_MAX_FIELD_LEN];
} rbdump_fetch_t;

// Complete module result
typedef struct
{
    uint8_t id;
    uint8_t total_fields;
    rbdump_fetch_t fields[RBDUMP_MAX_FIELDS];
    uint8_t current_field_count;
} rbdump_mod_result_t;


void rbdump_dump(struct RbcomConnect *, char *);
//const char *serial_success_to_str(uint8_t);
