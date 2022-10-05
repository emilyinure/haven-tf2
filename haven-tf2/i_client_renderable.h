#pragma once

typedef unsigned short client_shadow_handle_t;
typedef unsigned short client_render_handle_t;
struct model_t;

// todo: move this to model render once we have it.
typedef unsigned short model_instance_handle_t;

enum shadow_type_t {
	SHADOWS_NONE = 0,
	SHADOWS_SIMPLE,
	SHADOWS_RENDER_TO_TEXTURE,
	SHADOWS_RENDER_TO_TEXTURE_DYNAMIC,
	SHADOWS_RENDER_TO_DEPTH_TEXTURE,
};

class ipvs_notify {
public:
	virtual void on_pvs_status_changed( bool in_pvs ) = 0;
};

class i_client_renderable {
public:
	virtual i_client_unknown* get_i_client_unknown( ) = 0;
	virtual vector const& get_render_origin( void ) = 0;
	virtual vector const& get_render_angles( void ) = 0;
	virtual bool should_draw( void ) = 0;
	virtual bool is_transparent( void ) = 0;
	virtual bool uses_power_of_two_frame_buffer_texture( ) = 0;
	virtual bool uses_full_frame_buffer_texture( ) = 0;
	virtual client_shadow_handle_t get_shadow_handle( ) const = 0;
	virtual client_render_handle_t& render_handle( ) = 0;
	virtual const model_t* get_model( ) const = 0;
	virtual int draw_model( int flags ) = 0;
	virtual int get_body( ) = 0;
	virtual void compute_fx_blend( ) = 0;
	virtual int get_fx_blend( void ) = 0;
	virtual void get_color_modulation( float* color ) = 0;
	virtual bool lod_test( ) = 0;
	virtual bool setup_bones( matrix_3x4* bone_to_world_out, int max_bones, int bone_mask, float current_time ) = 0;
	virtual void setup_weights( const matrix_3x4* bone_to_world, int flex_weight_count, float* flex_weights, float* flex_delayed_weights ) = 0;
	virtual void do_animation_events( void ) = 0;
	virtual ipvs_notify* get_pvs_notify_interface( ) = 0;
	virtual void get_render_bounds( vector& mins, vector& maxs ) = 0;
	virtual void get_render_bounds_worldspace( vector& mins, vector& maxs ) = 0;
	virtual void get_shadow_render_bounds( vector& mins, vector& maxs, shadow_type_t shadow_type ) = 0;
	virtual bool should_receive_projected_textures( int flags ) = 0;
	virtual bool get_shadow_cast_distance( float* dist, shadow_type_t shadow_type ) const = 0;
	virtual bool get_shadow_cast_direction( vector* direction, shadow_type_t shadow_type ) const = 0;
	virtual bool is_shadow_dirty( ) = 0;
	virtual void mark_shadow_dirty( bool dirty ) = 0;
	virtual i_client_renderable* get_shadow_parent( ) = 0;
	virtual i_client_renderable* first_shadow_child( ) = 0;
	virtual i_client_renderable* next_shadow_peer( ) = 0;
	virtual shadow_type_t shadow_cast_type( ) = 0;
	virtual void create_model_instance( ) = 0;
	virtual model_instance_handle_t get_model_instance( ) = 0;
	virtual const matrix_3x4& renderable_to_world_transform( ) = 0;
	virtual int lookup_attachment( const char* attachment_name ) = 0;
	virtual bool get_attachment( int number, vector& origin, vector& angles ) = 0;
	virtual bool get_attachment( int number, matrix_3x4& matrix ) = 0;
	virtual float* get_render_clip_plane( void ) = 0;
	virtual int get_skin( ) = 0;
	virtual bool is_two_pass( void ) = 0;
	virtual void on_threaded_draw_setup( ) = 0;
	virtual bool uses_flex_delayed_weights( ) = 0;
	virtual void record_tool_message( ) = 0;
	virtual bool ignores_z_buffer( void ) const = 0;
};