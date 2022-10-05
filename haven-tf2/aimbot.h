#pragma once

class c_aimbot {
public:
	void other( c_base_player* local, usercmd_t* cmd );
	void run( c_base_player* local, usercmd_t* cmd );
} inline g_aimbot;