#pragma once

// todo: add this
class c_base_achievement;

class i_achievement {
public:
	virtual int get_achievement_id( ) = 0;
	virtual const char* get_name( ) = 0;
	virtual int get_flags( ) = 0;
	virtual int get_goal( ) = 0;
	virtual int get_count( ) = 0;
	virtual bool is_achieved( ) = 0;
	virtual int get_point_value( ) = 0;
	virtual bool should_save_with_game( ) = 0;
	virtual bool should_hide_until_achieved( ) = 0;
	virtual bool should_show_on_hud( ) = 0;
	virtual void set_show_on_hud( bool bshow ) = 0;
};

class i_achievement_mgr {
public:
	virtual i_achievement * get_achievement_by_index( int index ) = 0;
	virtual c_base_achievement* get_achievement_by_id( int id ) = 0;
	virtual int get_achievement_count( ) = 0;
	virtual void initialize_achievements( ) = 0;
	virtual void award_achievement( int i_achievement_id ) = 0;
	virtual void on_map_event( const char* pch_event_name ) = 0;
	virtual void download_user_data( ) = 0;
	virtual void ensure_global_state_loaded( ) = 0;
	virtual void save_global_state_if_dirty( bool b_async ) = 0;
	virtual bool has_achieved( const char* pch_name ) = 0;
	virtual bool were_cheats_ever_on( ) = 0;
};