#pragma once

// todo: add these.
class i_material;
struct surf_info;
struct model_t;
class c_sentence;
class i_material_system;
class c_phys_collide;
class c_gamestats_data;

typedef uint32_t crc_32_t;

typedef struct player_info_s {
	char m_name[ 32 ];
	int	m_user_id;
	char m_guid[ 33 ];
	uint32_t m_friends_id;
	char m_friends_name[ 32 ];
	bool m_fake_player;
	bool m_is_hltv;
	bool m_is_replay;
	crc_32_t m_custom_files[ 4 ];
	unsigned char m_files_downloaded;
} player_info_t;

struct audio_state_t {
	vector m_origin;
	vector m_angles;
	bool m_is_underwater;
};

struct occlusion_params_t {
	float m_max_occludee_area;
	float m_min_occluder_area;
};

enum skybox_visibility_t {
	SKYBOX_NOT_VISIBLE = 0,
	SKYBOX_3DSKYBOX_VISIBLE,
	SKYBOX_2DSKYBOX_VISIBLE,
};

class iv_engine_client {
public:
	virtual int get_intersecting_surfaces( const model_t * model, const vector & v_center, const float radius, const bool only_visible_surfaces, surf_info * infos, const int max_infos ) = 0;
	virtual vector get_light_for_point( const vector& pos, bool clamp ) = 0;
	virtual i_material* trace_line_material_and_lighting( const vector& start, const vector& end, vector& diffuse_light_color, vector& base_color ) = 0;
	virtual const char* parse_file( const char* data, char* token, int maxlen ) = 0;
	virtual bool copy_local_file( const char* source, const char* destination ) = 0;
	virtual void get_screen_size( int& width, int& height ) = 0;
	virtual void server_cmd( const char* cmd_string, bool reliable = true ) = 0;
	virtual void client_cmd( const char* cmd_string ) = 0;
	virtual bool get_player_info( int ent_num, player_info_t* info ) = 0;
	virtual int get_player_for_user_id( int user_id ) = 0;
	virtual client_text_message_t* text_message_get( const char* name ) = 0;
	virtual bool con_is_visible( void ) = 0;
	virtual int get_local_player( void ) = 0;
	virtual const model_t* load_model( const char* name, bool prop = false ) = 0;
	virtual float time( void ) = 0;
	virtual float get_last_time_stamp( void ) = 0;
	virtual c_sentence* get_sentence( c_audio_source* audio_source ) = 0;
	virtual float get_sentence_length( c_audio_source* audio_source ) = 0;
	virtual bool is_streaming( c_audio_source* audio_source ) const = 0;
	virtual void get_view_angles( vector& va ) = 0;
	virtual void set_view_angles( vector& va ) = 0;
	virtual int get_max_clients( void ) = 0;
	virtual const char* key_lookup_binding( const char* binding ) = 0;
	virtual const char* key_binding_for_key( button_code_t code ) = 0;
	virtual void start_key_trap_mode( void ) = 0;
	virtual bool check_done_key_trapping( button_code_t& code ) = 0;
	virtual bool is_in_game( void ) = 0;
	virtual bool is_connected( void ) = 0;
	virtual bool is_drawing_loading_image( void ) = 0;
	virtual void con_n_printf( int pos, const char* fmt, ... ) = 0;
	virtual void con_nx_printf( const struct con_nprint_s* info, const char* fmt, ... ) = 0;
	virtual int is_box_visible( const vector& mins, const vector& maxs ) = 0;
	virtual int is_box_in_view_cluster( const vector& mins, const vector& maxs ) = 0;
	virtual bool cull_box( const vector& mins, const vector& maxs ) = 0;
	virtual void sound_extra_update( void ) = 0;
	virtual const char* get_game_directory( void ) = 0;
	virtual const view_matrix& world_to_screen_matrix( ) = 0;
	virtual const view_matrix& world_to_view_matrix( ) = 0;
	virtual int game_lump_version( int lump_id ) const = 0;
	virtual int game_lump_size( int lump_id ) const = 0;
	virtual bool load_game_lump( int lump_id, void* buffer, int size ) = 0;
	virtual int level_leaf_count( ) const = 0;
	virtual i_spatial_query* get_bsp_tree_query( ) = 0;
	virtual void linear_to_gamma( float* linear, float* gamma ) = 0;
	virtual float light_style_value( int style ) = 0;
	virtual void compute_dynamic_lighting( const vector& pt, const vector* normal, vector& color ) = 0;
	virtual void get_ambient_light_color( vector& color ) = 0;
	virtual int get_dx_support_level( ) = 0;
	virtual bool supports_hdr( ) = 0;
	virtual void mat_stub( i_material_system* mat_sys ) = 0;
	virtual void get_chapter_name( char* pch_buff, int i_max_length ) = 0;
	virtual char const* get_level_name( void ) = 0;
	virtual int get_level_version( void ) = 0;
	virtual struct i_voice_tweak_s* get_voice_tweak_api( void ) = 0;
	virtual void engine_stats_begin_frame( void ) = 0;
	virtual void engine_stats_end_frame( void ) = 0;
	virtual void fire_events( ) = 0;
	virtual int get_leaves_area( int* leaves, int num_leaves ) = 0;
	virtual bool does_box_touch_area_frustum( const vector& mins, const vector& maxs, int i_area ) = 0;
	virtual void set_audio_state( const audio_state_t& state ) = 0;
	virtual int sentence_group_pick( int group_index, char* name, int name_buf_len ) = 0;
	virtual int sentence_group_pick_sequential( int group_index, char* name, int name_buf_len, int sentence_index, int reset ) = 0;
	virtual int sentence_index_from_name( const char* sentence_name ) = 0;
	virtual const char* sentence_name_from_index( int sentence_index ) = 0;
	virtual int sentence_group_index_from_name( const char* group_name ) = 0;
	virtual const char* sentence_group_name_from_index( int group_index ) = 0;
	virtual float sentence_length( int sentence_index ) = 0;
	virtual void compute_lighting( const vector& pt, const vector* normal, bool clamp, vector& color, vector* box_colors = NULL ) = 0;
	virtual void activate_occluder( int occluder_index, bool active ) = 0;
	virtual bool is_occluded( const vector& vec_abs_mins, const vector& vec_abs_maxs ) = 0;
	virtual void* save_alloc_memory( size_t num, size_t size ) = 0;
	virtual void save_free_memory( void* save_mem ) = 0;
	virtual i_net_channel_info* get_net_channel_info( void ) = 0;
	virtual void debug_draw_phys_collide( const c_phys_collide* collide, i_material* material, matrix_3x4& transform, const color32& color ) = 0;
	virtual void check_point( const char* name ) = 0;
	virtual void draw_portals( ) = 0;
	virtual bool is_playing_demo( void ) = 0;
	virtual bool is_recording_demo( void ) = 0;
	virtual bool is_playing_time_demo( void ) = 0;
	virtual int get_demo_recording_tick( void ) = 0;
	virtual int get_demo_playback_tick( void ) = 0;
	virtual int get_demo_playback_start_tick( void ) = 0;
	virtual float get_demo_playback_time_scale( void ) = 0;
	virtual int get_demo_playback_total_ticks( void ) = 0;
	virtual bool is_paused( void ) = 0;
	virtual bool is_taking_screenshot( void ) = 0;
	virtual bool is_hltv( void ) = 0;
	virtual bool is_level_main_menu_background( void ) = 0;
	virtual void get_main_menu_background_name( char* dest, int destlen ) = 0;
	virtual void get_video_modes( int& count, vmode_s*& modes ) = 0;
	virtual void set_occlusion_parameters( const occlusion_params_t& params ) = 0;
	virtual void get_ui_language( char* dest, int destlen ) = 0;
	virtual skybox_visibility_t is_skybox_visible_from_point( const vector& vec_point ) = 0;
	virtual const char* get_map_entities_string( ) = 0;
	virtual bool is_in_edit_mode( void ) = 0;
	virtual float get_screen_aspect_ratio( ) = 0;
	virtual bool removed_steam_refresh_login( const char* password, bool is_secure ) = 0;
	virtual bool removed_steam_process_call( bool& finished ) = 0;
	virtual unsigned int get_engine_build_number( ) = 0;
	virtual const char* get_product_version_string( ) = 0;
	virtual void grab_pre_color_corrected_frame( int x, int y, int width, int height ) = 0;
	virtual bool is_hammer_running( ) const = 0;
	virtual void execute_client_cmd( const char* cmd_string ) = 0;
	virtual bool map_has_hdr_lighting( void ) = 0;
	virtual int get_app_id( ) = 0;
	virtual vector get_light_for_point_fast( const vector& pos, bool clamp ) = 0;
	virtual void client_cmd_unrestricted( const char* cmd_string ) = 0;
	virtual void set_restrict_server_commands( bool restrict ) = 0;
	virtual void set_restrict_client_commands( bool restrict ) = 0;
	virtual void set_overlay_bind_proxy( int i_overlay_id, void* bind_proxy ) = 0;
	virtual bool copy_frame_buffer_to_material( const char* material_name ) = 0;
	virtual void change_team( const char* team_name ) = 0;
	virtual void read_configuration( const bool read_default = false ) = 0;
	virtual void set_achievement_mgr( i_achievement_mgr* achievement_mgr ) = 0;
	virtual i_achievement_mgr* get_achievement_mgr( ) = 0;
	virtual bool map_load_failed( void ) = 0;
	virtual void set_map_load_failed( bool state ) = 0;
	virtual bool is_low_violence( ) = 0;
	virtual const char* get_most_recent_save_game( void ) = 0;
	virtual void set_most_recent_save_game( const char* lpsz_filename ) = 0;
	virtual void start_xbox_exiting_process( ) = 0;
	virtual bool is_save_in_progress( ) = 0;
	virtual int on_storage_device_attached( void ) = 0;
	virtual void on_storage_device_detached( void ) = 0;
	virtual void reset_demo_interpolation( void ) = 0;
	virtual void set_gamestats_data( c_gamestats_data* gamestats_data ) = 0;
	virtual c_gamestats_data* get_gamestats_data( ) = 0;
	virtual void server_cmd_key_values( key_values* key_values ) = 0;
	virtual bool is_skipping_playback( void ) = 0;
	virtual bool is_loading_demo( void ) = 0;
	virtual bool is_playing_demo_a_locally_recorded_demo( ) = 0;
	virtual const char* key_lookup_binding_exact( const char* binding ) = 0;
	virtual void add_phoneme_file( const char* phoneme_file ) = 0;
	virtual float get_paused_expire_time( void ) = 0;
	virtual bool start_demo_recording( const char* filename, const char* folder = NULL ) = 0;
	virtual void stop_demo_recording( void ) = 0;
	virtual void take_screenshot( const char* filename, const char* folder = NULL ) = 0; 
	i_net_channel_info* GetNetChannelInfo( void ) {
		typedef i_net_channel_info* ( __thiscall* OriginalFn )( PVOID );
		return g_utils.get_virtual_function<OriginalFn>( this, 72 )( this );
	}
};
