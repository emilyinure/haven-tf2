#pragma once
class player_t;

class player_record_t
{
public:
    player_record_t()
    {
    }
    ~player_record_t();
    bool cache();
    void restore();
    bool on_ground = false;
    player_t* player = nullptr;
    float sim_time = 0;
    bool valid() const;
    int m_lag = 0;
    vector vel = vector();
    float dir = 0;
    float dir_decay = 0;
    float ground_dir = 0;
    int flags = 0;
    c_base_entity* last_ground = nullptr;
    vector origin = vector();
    vector eye_angle = vector();
    vector move_data = vector();

    vector mins = vector();
    vector maxs = vector();

    matrix_3x4 bones[128];
    bool built = false;
};
class player_t
{
public:
    std::deque<std::shared_ptr<player_record_t>> m_records = {};
    float m_sim_time = -1.f;

    vector pred_origin = vector(0, 0, 0);
    c_base_player* player;
    c_base_entity* m_ground = nullptr;
    vector m_base_velocity = vector();
    float m_last_time_jumped = -1.f;
    void SetGroundEntity(trace_t* pm);
};
class c_player_manager
{
public:
    player_t players[64];
    void update_players();
} inline g_player_manager;
