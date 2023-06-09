#pragma once
#define	MAX_CLIP_PLANES	5
#include "player_manager.h"
#include "sdk.h"
class c_movement_simulate
{
	convar* sv_gravity = nullptr;

	vector get_player_mins();
	vector get_player_maxs();
	unsigned int player_solid_mask();
	int try_player_move( );
	int clip_velocity(vector& in, vector& normal, vector& out, float overbounce);
	void trace_player_bbox(const vector& start, const vector& end, unsigned int fMask, int collisionGroup, trace_t& pm);
	void try_touch_ground( const vector& start, const vector& end, const vector& mins, const vector& maxs, unsigned int fMask, int collisionGroup, trace_t& pm ) const;
	void try_touch_ground_in_quadrants( const vector& start, const vector& end, unsigned int fMask, int collisionGroup, trace_t& pm );

	void step_move( const vector& vecDestination, trace_t& trace );
	void accelerate( vector& wishdir, float wishspeed, float accel );
	void stay_on_ground( void );

	static float max_speed( c_base_player* player );
	void friction( void );
	void categorize_position( );
	void check_velocity( void );
	void start_gravity( void );
	void air_move( );
	void walk_move( );
	void air_acceletate( vector& wishdir, float wishspeed, float accel );
	void full_walk_move( );
	void SetGroundEntity ( trace_t *pm );

	void player_move( );

	class player_record_t {
	public:
		player_record_t( ) {}
		int m_lag = 0;
		vector vel = vector( );
		float dir = 0;
		bool on_ground;
		vector origin = vector( );
		vector eye_angle = vector( );
	};
	class player_t {
	public:
		std::deque<player_record_t> m_records = {};
		float sim_time = -1.f;
		c_base_player* player;
	};

public:
    void finish_gravity(void);
	struct info {
		vector m_velocity;
		vector m_position;
		vector m_walk_direction;
		vector m_target_velocity;
		c_base_player* m_player;
		bool on_ground;
		float m_surface_friction;
		float m_dir;
		float m_ground_dir;
		float target_ground_dir;
		float total_changed = 0.f;
		float m_eye_dir;
		float m_max_speed;
		float m_decay = 1;
		vector m_air_dir = vector( );
		std::deque< std::vector<double>> inputVals_;
	};
	info mv;
	std::deque<vector> path;
	void draw( );

	vector estimate_walking_dir( vector velocity, vector last_fric_vel, vector eye_ang, vector origin );
	vector estimate_air_dir ( vector velocity, vector last_fric_vel, vector eye_ang, vector origin );
	vector find_unstuck ( vector origin );
	bool setup_mv( vector last_vel, c_base_player* player, int index );

	vector run( );
} inline g_movement;

