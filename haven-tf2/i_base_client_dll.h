#pragma once

// not going down the rabbit hole of adding this.
// todo: maybe add later?
class c_engine_sprite;
class c_save_restore_data;
struct x_user_context;
struct x_user_property;

// todo: remove when i add convars
class i_convar;

// nowhere else to put this that makes sense.
// todo: figure that out.
struct screen_fade_t {
	unsigned short m_duration;
	unsigned short m_hold_time;
	short m_fade_flags;
	byte m_r, m_g, m_b, m_a;
};

enum client_frame_stage_t {
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END
};

class i_base_client_dll {
public:
	virtual int	init( create_interface_fn app_system_factory, create_interface_fn physics_factory, c_global_vars_base* globals ) = 0;
	virtual void post_init( ) = 0;
	virtual void shutdown( void ) = 0;
	virtual bool replay_init( create_interface_fn replay_factory ) = 0;
	virtual bool replay_post_init( ) = 0;
	virtual void level_init_pre_entity( char const* map_name ) = 0;
	virtual void level_init_post_entity( ) = 0;
	virtual void level_shutdown( void ) = 0;
	virtual client_class* get_all_classes( void ) = 0;
	virtual int	hud_vid_init( void ) = 0;
	virtual void hud_process_input( bool active ) = 0;
	virtual void hud_update( bool active ) = 0;
	virtual void hud_reset( void ) = 0;
	virtual void hud_text( const char* message ) = 0;
	virtual void in_activate_mouse( void ) = 0;
	virtual void in_deactive_mouse( void ) = 0;
	virtual void in_accumulate( void ) = 0;
	virtual void in_clear_states( void ) = 0;
	virtual bool in_is_key_down( const char* name, bool& is_down ) = 0;
	virtual void in_on_mouse_wheeld( int delta ) = 0;
	virtual int	in_key_event( int event_code, button_code_t key_num, const char* current_binding ) = 0;
	virtual void create_move( int sequence_number, float input_sample_frametime, bool active ) = 0;
	virtual void extra_mouse_sample( float frame_time, bool active ) = 0;
	virtual bool write_user_cmd_delta_to_buffer( bf_write* buf, int from, int to, bool is_new_command ) = 0;
	virtual void encode_user_cmd_to_buffer( bf_write& buf, int slot ) = 0;
	virtual void decode_user_cmd_from_buffer( bf_read& buf, int slot ) = 0;
	virtual void view_render( vrect_t* rect ) = 0;
	virtual void view_fade( screen_fade_t* sf ) = 0;
	virtual void set_crosshair_angle( const vector& angle ) = 0;
	virtual void init_sprite( c_engine_sprite* sprite, const char* load_name ) = 0;
	virtual void shutdown_sprite( c_engine_sprite* sprite ) = 0;
	virtual int	get_sprite_size( void ) const = 0;
	virtual void voice_status( int ent_index, int talking ) = 0;
	virtual void install_string_table_callback( char const* table_name ) = 0;
	virtual void frame_stage_notify( client_frame_stage_t stage ) = 0;
	virtual bool dispatch_user_message( int msg_type, bf_read& msg_data ) = 0;
	virtual c_save_restore_data* save_init( int size ) = 0;
	virtual void save_write_fields( c_save_restore_data*, const char*, void*, data_map_t*, type_description_t*, int ) = 0;
	virtual void save_read_fields( c_save_restore_data*, const char*, void*, data_map_t*, type_description_t*, int ) = 0;
	virtual void pre_save( c_save_restore_data* ) = 0;
	virtual void save( c_save_restore_data* ) = 0;
	virtual void write_save_headers( c_save_restore_data* ) = 0;
	virtual void read_restore_save_headers( c_save_restore_data* ) = 0;
	virtual void restore( c_save_restore_data*, bool ) = 0;
	virtual void dispatch_on_restore( ) = 0;
	virtual c_standard_recv_proxies* get_standard_recv_proxies( ) = 0;
	virtual void write_save_game_screenshot( const char* filename ) = 0;
	virtual void emit_sentence_close_caption( char const* tokenstream ) = 0;
	virtual void emit_close_caption( char const* caption_name, float duration ) = 0;
	virtual bool can_record_demo( char* error_msg, int length ) const = 0;
	virtual void on_demo_record_start( char const* demo_base_name ) = 0;
	virtual void on_demo_record_stop( ) = 0;
	virtual void on_demo_playback_start( char const* demo_base_name ) = 0;
	virtual void on_demo_playback_stop( ) = 0;
	virtual bool should_draw_dropdown_console( ) = 0;
	virtual int get_screen_width( ) = 0;
	virtual int get_screen_height( ) = 0;
	virtual void write_save_game_screenshot_of_size( const char* file_name, int width, int height, bool create_power_of_2_padded = false, bool write_vtf = false ) = 0;
	virtual bool get_player_view( c_view_setup& player_view ) = 0;
	virtual void setup_game_properties( c_utl_vector< x_user_context >& contexts, c_utl_vector< x_user_property >& properties ) = 0;
	virtual int get_presence_id( const char* id_name ) = 0;
	virtual const char* get_property_id_string( const int id ) = 0;
	virtual void get_property_display_string( int id, int value, char* output, int bytes ) = 0;
	virtual void start_stats_reporting( HANDLE handle, bool arbitrated ) = 0;
	virtual void invalidate_mdl_cache( ) = 0;
	virtual void in_set_sample_time( float frame_time ) = 0;
	virtual void reload_files_in_list( i_file_list* files_to_reload ) = 0;
	virtual bool handle_ui_toggle( ) = 0;
	virtual bool should_allow_console( ) = 0;
	virtual c_renamed_recv_table_info* get_renamed_recv_table_infos( ) = 0;
	virtual c_mouth_info* get_client_ui_mouth_info( ) = 0;
	virtual void file_received( const char* file_name, unsigned int transfer_id ) = 0;
	virtual const char* translate_effect_for_vision_filter( const char* pch_effect_type, const char* pch_effect_name ) = 0;
	virtual void client_adjust_start_sound_params( struct start_sound_params_t& params ) = 0;
	virtual bool disconnect_attempt( void ) = 0;
	virtual bool is_connected_user_info_change_allowed( i_convar* cvar ) = 0;
};