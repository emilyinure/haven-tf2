#pragma once

struct netvar_t {
	unsigned int m_hashed_table;
	unsigned int m_hashed_var;
	uintptr_t m_offset;
};

class c_netvars {
	void get_offsets( );
public:
	std::deque< netvar_t > m_netvars;

	struct {
		struct {
			uintptr_t m_vec_origin, m_vec_view_offset, m_i_team_num, m_i_health, m_life_state, m_owner_handle, m_simulation_time, m_spawn_time, m_move_type, m_mins, m_maxs, m_max_speed;
		} dt_base_entity;

		struct {
			uintptr_t m_hitbox_set;
		} dt_base_animating;

		struct {
			uintptr_t m_max_health;
		} dt_player_resource;

		struct {
			uintptr_t m_item_definition_index, m_pipe_charge_begin_time, m_charge_begin_time, m_next_primary_attack, m_next_secondary_attack, m_charge_damage, m_rot_speed;
		} dt_base_weapon;

		struct {
			uintptr_t m_i_health, m_life_state, m_ground_handle, m_velocity, m_eye_angles, m_taunt, m_flags, m_active_weapon, m_next_attack, m_max_speed, m_tick_base, m_class, m_shared;
		} dt_base_player;

	} m_offsets;
	void iterate_props( const unsigned int name, recv_table* table, int offset = 0);

	void init( );
	netvar_t get( const char* table, const char* var );
}; inline c_netvars g_netvars;