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

    struct player_log
    {
        std::vector<vector> m_path = {};
        float end_time = 0.f;
	};

public:

    std::unordered_map<int, player_log> m_logs = {};
    static void try_touch_ground(c_base_entity* player, const vector& start, const vector& end, const vector& mins,
                                 const vector& maxs, unsigned int fMask, int collisionGroup, trace_t& pm);
    static void try_touch_ground_in_quadrants(c_base_entity* player, const vector& start, const vector& end,
                                              const vector& minsSrc, const vector& maxsSrc, unsigned int fMask,
                                              int collisionGroup, trace_t& pm);
    void finish_gravity(void);
    struct info
    {
        vector m_velocity;
        vector m_base_velocity;
		vector m_position;
		vector m_walk_direction;
		vector m_target_velocity;
		c_base_player* m_player;
        c_base_entity* m_ground;
		bool on_ground;
		float m_surface_friction;
		float m_dir;
		float m_ground_dir;
		float target_ground_dir;
		float total_changed = 0.f;
		float m_eye_dir;
		float m_max_speed;
        float m_time = 0;
		float m_decay = 1;
		vector m_air_dir = vector( );
		std::deque< std::vector<double>> inputVals_;
	};
	info mv;
    std::vector<vector> path;
    float end_time = 0.f;
	void draw( );

	vector estimate_walking_dir( vector velocity, vector last_fric_vel, vector eye_ang, vector origin );
	vector estimate_air_dir ( vector velocity, vector last_fric_vel, vector eye_ang, vector origin );
	vector find_unstuck ( vector origin );
    bool setup_mv(vector last_vel, c_base_player* player, int index);

    void air_input_prediction(const std::shared_ptr<player_record_t>& record, const player_t& player_info);

    void ground_input_prediction(const player_t& player_info, const std::shared_ptr<player_record_t>& record);

    void do_angle_prediction(const std::shared_ptr<player_record_t>& record, const player_t& player_info);

	vector run( );
} inline g_movement;

