#pragma once
#define MAX_CLIP_PLANES 5
#include "player_manager.h"
#include "sdk.h"
class c_movement_simulate {
  convar* sv_gravity              = nullptr;
  convar* sv_airaccelerate        = nullptr;
  convar* sv_accelerate           = nullptr;
  convar* sv_friction             = nullptr;
  convar* sv_stopspeed            = nullptr;
  convar* tf_clamp_back_speed     = nullptr;
  convar* tf_clamp_back_speed_min = nullptr;
  convar* tf_max_charge_speed     = nullptr;
  convar* sv_maxvelocity          = nullptr;

  vector       get_player_mins();
  vector       get_player_maxs();
  unsigned int player_solid_mask();
  int          try_player_move();
  int          clip_velocity(vector& in, vector& normal, vector& out, float overbounce);
  void         trace_player_bbox(const vector& start, const vector& end, unsigned int fMask,
                                 int collisionGroup, trace_t& pm);

  float calc_wish_speed_threshold();
  void  accelerate(vector& wishdir, float wishspeed, float accel);
  void  step_move(vector target, trace_t& trace);
  void  stay_on_ground(void);

  static float max_speed(c_base_player* player);
  void         friction(void);
  void         categorize_position();
  void         check_velocity(void);
  void         start_gravity(void);
  void         air_move();
  void         walk_move();
  void         air_acceletate(vector& wishdir, float wishspeed, float accel);
  void         full_walk_move();
  void         SetGroundEntity(trace_t* pm);

  void player_move();

  bool charge_move();

  void process_movment();

  struct player_log {
    std::vector<vector> m_path   = {};
    float               end_time = 0.f;
  };

public:
  std::unordered_map<int, player_log> m_logs = {};
  static void try_touch_ground(c_base_entity* player, const vector& start, const vector& end,
                               const vector& mins, const vector& maxs, unsigned int fMask,
                               int collisionGroup, trace_t& pm);
  static void try_touch_ground_in_quadrants(c_base_entity* player, const vector& start,
                                            const vector& end, const vector& minsSrc,
                                            const vector& maxsSrc, unsigned int fMask,
                                            int collisionGroup, trace_t& pm);
  void        finish_gravity(void);

  struct info {
    vector         m_velocity;
    vector         m_base_velocity;
    vector         m_position;
    vector         m_walk_direction;
    vector         m_target_velocity;
    c_base_player* m_player;
    c_base_entity* m_ground;
    bool           on_ground;
    float          m_surface_friction;
    float          m_dir;
    float          m_ground_dir;
    float          target_ground_dir;
    float          total_changed = 0.f;
    float          m_eye_dir;
    float          m_air_eye_dir;
    float          m_max_speed;
    float          m_time               = 0;
    float          m_accumulated_ground = 0.f;
    float          m_decay              = 1;
    float          ground_circle_prediction;
    float          ground_rotating_right;
    float          ground_rotation_timing;
    vector         m_air_dir = vector();
  } mv;

  std::vector<vector> path;
  float               end_time = 0.f;

  void draw();

  void update_strafe();

  vector estimate_walking_dir(vector velocity, vector last_fric_vel, vector eye_ang,
                              vector origin);
  vector estimate_air_dir(vector velocity, vector last_fric_vel, vector eye_ang, vector origin);
  vector find_unstuck(vector origin);
  bool   grab_convars();
  bool   setup_mv(vector last_vel, c_base_player* player);

  void air_input_prediction(const std::shared_ptr<player_record_t>& record,
                            const player_t&                         player_info);

  void ground_input_prediction(const player_t&                         player_info,
                               const std::shared_ptr<player_record_t>& record);

  void do_angle_prediction(const std::shared_ptr<player_record_t>& record,
                           const player_t&                         player_info);

  bool run(vector& position);
} inline g_movement;
