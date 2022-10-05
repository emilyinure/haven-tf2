#pragma once

class c_hooks {
	enum e_indexes {
		index_paint = 13,
		index_lock_cursor = 62,
	};
public:
	struct {
		typedef void( __fastcall* paint_fn )( uintptr_t, uintptr_t, paint_mode_t );
		paint_fn paint;

		typedef bool( __stdcall* create_move_fn )( float, usercmd_t* );
		create_move_fn create_move;

		typedef void( __stdcall* frame_stage_fn )( client_frame_stage_t );
		frame_stage_fn frame_stage;

		typedef void( __fastcall* lock_cursor_fn )( uintptr_t, uintptr_t );
		lock_cursor_fn lock_cursor;

		typedef void( __thiscall* run_command_fn )( c_prediction*, c_base_entity*, usercmd_t*, CMoveHelper* );
		run_command_fn run_command;

		WNDPROC wnd_proc;
	} m_original;

	void init( );
	void unload( );
}; inline c_hooks g_hooks;