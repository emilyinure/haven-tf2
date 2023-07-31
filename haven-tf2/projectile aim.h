#pragma once
#include "sdk.h"

class proj_aim
{
    struct player_log
    {
        std::vector<vector> m_path = {};
        float end_time = 0.f;
    };

public:
    std::unordered_map<int, player_log> m_logs = {};
    convar* sv_gravity = nullptr;
    float travel_time(c_base_player* target, vector weapon_pos, vector pos, vector angle);
    vector get_weapon_delta(vector eye_pos, vector weapon_pos, vector view);
    float get_pipe_aim(float speed, float pitch);
    vector get_aim_offset();
    bool is_pipe();
    void select_target();
    void run();
    void find_shot(bool& was_shoot, int attack);
    bool setup_projectile(vector& view, vector& pos, vector& new_eyepos);
    bool get_gravity_aim(vector difference, float* ret, bool lob);
    void draw();
    bool record(vector view, bool record);
    bool proj_can_hit(c_base_player* target, vector view, float goal_time, vector weapon_pos, bool record = false);
    std::vector<vector> m_path = {};
    float end_time = 0;
    float m_weapon_gravity;
    double m_weapon_speed;
    c_base_player* m_target;
} inline g_proj;
