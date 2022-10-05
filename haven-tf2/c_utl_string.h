#pragma once

class c_utl_string {
public:
	typedef enum {
		pattern_none = 0x00000000,
		pattern_directory = 0x00000001
	} t_utl_string_pattern;
public:
	char* m_string;
};