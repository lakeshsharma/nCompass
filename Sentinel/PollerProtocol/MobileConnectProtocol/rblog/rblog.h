#pragma once

enum rblog_levels
{
	RBLOG_UNDEF = 0,
	RBLOG_ERROR = 1,
	RBLOG_WARN = 2,
	RBLOG_INFO = 3,
	RBLOG_DEBUG = 4
};

void rblog(enum rblog_levels, char *);
enum rblog_levels set_rblog_level(enum rblog_levels);
enum rblog_levels str_to_loglevel(char *);
const char * loglevel_to_str(enum rblog_levels);
int rbloggable(enum rblog_levels);