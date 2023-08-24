#include "projectile aim.h"
#include "movement_simulate.h"
#include "player_manager.h"
#include <iostream>
#include <vector>
#include <algorithm>

class trace_filter_no_players : public i_trace_filter
{
public:
    virtual bool should_hit_entity(c_base_entity* pEntityHandle, int contentsMask)
    {
        return (pEntityHandle->entindex() == 0 || pEntityHandle->entindex() > g_interfaces.m_engine->get_max_clients());
    }

    [[nodiscard]] virtual TraceType_t get_type() const
    {
        return TraceType_t::TRACE_EVERYTHING;
    }
};

class CTraceFilterCollisionArrows : public i_trace_filter
{
public:
    CTraceFilterCollisionArrows(c_base_entity* passentity) : m_pPassEnt(passentity)
    {
    }

    virtual bool should_hit_entity(c_base_entity* pHandleEntity, int contentsMask)
    {
        if (pHandleEntity)
        {
            if (pHandleEntity == m_pPassEnt)
                return false;
            if (pHandleEntity->collision_group() == TF_COLLISIONGROUP_GRENADES)
                return false;
            if (pHandleEntity->collision_group() == TFCOLLISION_GROUP_ROCKETS)
                return false;
            if (pHandleEntity->collision_group() == TFCOLLISION_GROUP_ROCKET_BUT_NOT_WITH_OTHER_ROCKETS)
                return false;
            if (pHandleEntity->collision_group() == COLLISION_GROUP_DEBRIS)
                return false;
            if (pHandleEntity->collision_group() == TFCOLLISION_GROUP_RESPAWNROOMS)
                return false;
            if (pHandleEntity->collision_group() == COLLISION_GROUP_NONE)
                return false;

            return (pHandleEntity->entindex() == 0 ||
                    pHandleEntity->entindex() > g_interfaces.m_engine->get_max_clients());
        }

        return true;
    }

    virtual TraceType_t get_type() const
    {
        return TraceType_t::TRACE_EVERYTHING;
    }

protected:
    c_base_entity* m_pPassEnt;
};

class trace_filter_no_teamates : public i_trace_filter
{
public:
    virtual bool should_hit_entity(c_base_entity* pEntityHandle, int contentsMask)
    {
        return (pEntityHandle->entindex() == 0 ||
                pEntityHandle->entindex() > g_interfaces.m_engine->get_max_clients()) ||
               (pEntityHandle->m_i_team_num() != team);
    }

    virtual TraceType_t get_type() const
    {
        return TraceType_t::TRACE_EVERYTHING;
    }
    int team;
};

inline float RemapValClamped(float val, float A, float B, float C, float D)
{
    if (A == B)
        return val >= B ? D : C;
    float cVal = (val - A) / (B - A);
    cVal = std::clamp<float>(cVal, 0.0f, 1.0f);

    return C + (D - C) * cVal;
}

float proj_aim::travel_time(c_base_player* target, vector weapon_pos, vector pos, vector angle)
{
    vector eye_pos = g_cl.m_local->m_vec_origin() + g_cl.m_local->m_view_offset();
    if (!is_pipe())
    {
        trace_filter traceFilter;
        traceFilter.skip = g_cl.m_local;
        trace_t trace;

        vector forward, right, up;

        angle.angle_vectors(&forward, &right, &up);

        vector endPos = eye_pos + forward * 2000.f;

        ray_t ray;
        ray.initialize(eye_pos, endPos);
        g_interfaces.m_engine_trace->trace_ray(ray, MASK_SHOT_HULL, &traceFilter, &trace);

        if (trace.m_fraction > 0.1f)
            angle = weapon_pos.look(trace.m_end);
        else
            angle = weapon_pos.look(endPos);
    }
    const vector new_dir = angle.angle_vector() * this->m_weapon_speed;
    const float desired_dist = (pos - weapon_pos).length_2d();
    return desired_dist / new_dir.length_2d();
}

#define TIME_TO_TICKS(dt) ((int)(0.5f + (float)(dt) / g_interfaces.m_global_vars->m_interval_per_tick))
#define TICKS_TO_TIME(dt) (g_interfaces.m_global_vars->m_interval_per_tick * (float)(dt))

double get_speed()
{
    float begin_charge = 0;
    float charge = 0;
    auto weapon_speed = 0.0f;
    switch (g_cl.m_weapon->item_index())
    {
        case WPN_DirectHit:
            weapon_speed = 1980.;
            break;
        case WPN_BotRocketlauncherB:
        case WPN_BotRocketlauncherC:
        case WPN_BotRocketlauncherD:
        case WPN_BotRocketlauncherEG:
        case WPN_BotRocketlauncherES:
        case WPN_BotRocketlauncherG:
        case WPN_BotRocketlauncherR:
        case WPN_BotRocketlauncherS:
        case WPN_FestiveRocketLauncher:
        case WPN_NewRocketLauncher:
        case WPN_RocketLauncher:
        case WPN_Original:
        case WPN_Airstrike:
        case WPN_BlackBox:
            weapon_speed = 1100.;
            break;
        case WPN_FestiveFlaregun:
        case WPN_Flaregun:
        case WPN_ScorchShot:
            weapon_speed = 2000.;
            break;
        case WPN_SyringeGun:
        case WPN_NewSyringeGun:
        case WPN_Blutsauger:
        case WPN_Overdose:
            weapon_speed = 1000.;
            break;
        case WPN_RescueRanger:
        case WPN_Crossbow:
        case WPN_FestiveCrossbow:
            weapon_speed = 2400.;
            break;
        case WPN_Huntsman:
        case WPN_CompoundBow:
            begin_charge = g_cl.m_weapon->charge_time();

            charge = (begin_charge == 0.0f) ? 0.0f : TICKS_TO_TIME(g_cl.m_local->m_tick_base()) - begin_charge;

            weapon_speed = RemapValClamped(charge, 0.0f, 1.f, 1800.f, 2600.f);
            break;
        case WPN_GrenadeLauncher:
        case WPN_NewGrenadeLauncher:
        case WPN_FestiveGrenadeLauncher:
            weapon_speed = 1200.;
            break;
        case WPN_LochNLoad:
            weapon_speed = 1575.;
            break;
        case WPN_LoooseCannon:
            weapon_speed = 1440.;
            break;
        case WPN_StickyLauncher:
            begin_charge = g_cl.m_weapon->pipe_charge_time();
            charge = (begin_charge == 0.0f) ? 0.f : TICKS_TO_TIME(g_cl.m_local->m_tick_base()) - begin_charge;
            weapon_speed = RemapValClamped((charge), 0.0f, 4.f, 900.f, 2400.f);

            break;
        case WPN_IronBomber:
            // BeginCharge = weapon->charge_time( );
            // Charge = ( BeginCharge == 0.0f ) ? 0.0f : TICKS_TO_TIME( g_cl.m_local->m_tick_base( ) + 1 ) -
            // BeginCharge; weapon_speed = RemapValClamped( Charge, 	0.0f, 	4.f, 	900, 	2400 );
            //
            // break;
            weapon_speed = 1200.;
            break;
        case WPN_Sandman:
            weapon_speed = 3000.;
            break;
        default:
            weapon_speed = 0.;
            break;
    }
    return weapon_speed;
};

float get_gravity()
{
    float begin_charge;
    float charge;
    auto weapon_gravity = 0.0f;
    switch (g_cl.m_weapon->item_index())
    {
        case WPN_RescueRanger:
        case WPN_Crossbow:
        case WPN_FestiveCrossbow:
            weapon_gravity = 0.2f;
            break;
        case WPN_SyringeGun:
        case WPN_NewSyringeGun:
        case WPN_Blutsauger:
        case WPN_Overdose:
        case WPN_FestiveFlaregun:
        case WPN_Flaregun:
        case WPN_ScorchShot:
            weapon_gravity = 0.3f;
            break;
        case WPN_Huntsman:
        case WPN_CompoundBow:
            begin_charge = g_cl.m_weapon->charge_time();

            charge = begin_charge == 0.0f ? 0.0f : TICKS_TO_TIME(g_cl.m_local->m_tick_base()) - begin_charge;

            weapon_gravity = RemapValClamped(charge, 0.0f, 1.f, 0.5, 0.1);
            break;
        case WPN_GrenadeLauncher:
        case WPN_NewGrenadeLauncher:
        case WPN_FestiveGrenadeLauncher:
        case WPN_LoooseCannon:
        case WPN_LochNLoad:
        case WPN_IronBomber:
        case WPN_StickyLauncher:
            weapon_gravity = 1.5f;
            break;
        case WPN_Sandman:
            weapon_gravity = 0.5f;
            break;
        default:
            weapon_gravity = 0.0f;
            break;
    }
    return weapon_gravity;
};

vector iter_arrow_hitspot(player_t* player, vector hitbox)
{
    const auto target = player->player;
    const vector dir = vector(0, g_movement.mv.m_ground_dir, 0).angle_vector();
    const vector mins = target->mins();
    const vector maxs = target->maxs();
    vector cur = (target->maxs() + target->mins()) * 0.5f;
    cur.m_z = target->maxs().m_z;
    const vector start = cur;
    float last_last_dist = FLT_MAX;
    while (true)
    {
        if (vector next = cur + dir; (hitbox - cur).length_sqr() > (hitbox - next).length_sqr())
        {
            cur = next;
        }
        else
            return cur;
    }
}

vector aim_offset(player_t* player)
{
    const auto target = player->player;
    vector offset;
    auto dir = g_cl.m_shoot_pos.look(g_movement.mv.m_position +
                                     (player->m_records[0]->mins + player->m_records[0]->maxs) * 0.5f);
    vector forward, up;
    dir.angle_vectors(&forward, nullptr, &up);
    vector temp;

    switch (g_cl.m_weapon->item_index())
    {
        case WPN_RescueRanger:
        case WPN_Crossbow:
        case WPN_FestiveCrossbow:
        case WPN_Huntsman:
        case WPN_CompoundBow:
            offset = target->get_hitbox_pos(0) -
                     (target->get_abs_origin()); // -vector( 0, 0, target->get_collideable( )->obb_mins( ).m_z ));
            //offset = iter_arrow_hitspot(player, offset);
            // offset.m_x = temp.m_x;
            // offset.m_y = temp.m_y;
            // offset.m_z = fmaxf( offset.m_z + ( ( temp.m_z - offset.m_z ) * 0.5f ), offset.m_z );

            // if ( !g_movement.mv.on_ground )
            //	offset.m_z += 15.f;
            break;
        case WPN_IronBomber:
        case WPN_LoooseCannon:
        case WPN_LochNLoad:
        case WPN_GrenadeLauncher:
        case WPN_NewGrenadeLauncher:
        case WPN_FestiveGrenadeLauncher:
        case WPN_FestiveFlaregun:
        case WPN_Flaregun:
        case WPN_SyringeGun:
        case WPN_NewSyringeGun:
        case WPN_Blutsauger:
        case WPN_Overdose:
        case WPN_Sandman:
            offset = target->get_hitbox_pos(HITBOX_BODY) - (target->get_abs_origin());
            break;
        case WPN_DirectHit:
        case WPN_BotRocketlauncherB:
        case WPN_BotRocketlauncherC:
        case WPN_BotRocketlauncherD:
        case WPN_BotRocketlauncherEG:
        case WPN_BotRocketlauncherES:
        case WPN_BotRocketlauncherG:
        case WPN_BotRocketlauncherR:
        case WPN_BotRocketlauncherS:
        case WPN_FestiveRocketLauncher:
        case WPN_NewRocketLauncher:
        case WPN_RocketLauncher:
        case WPN_Original:
        case WPN_Airstrike:
        case WPN_BlackBox:
            // if( g_movement.mv.on_ground )
            offset.init(0, 0, 8 * fabsf(up.m_z) + fabsf(forward.m_z) * 8.f);
            // else
            //	offset = target->get_hitbox_pos( HITBOX_BODY ) - ( target->get_abs_origin( ) );
            break;
        case WPN_StickyLauncher:
            if (g_movement.mv.on_ground)
                offset.init(0, 0, 10);
            else
                offset.init(0, 0, 0);
            break;
        default:
            break;
    }
    return offset;
}

vector proj_aim::get_weapon_delta(vector eye_pos, vector weapon_pos, vector view)
{
    vector current_delta;
    const vector dir = view.angle_vector();
    vector min;
    vector max = (dir * 4000.f);
    const int max_iter = 1000;
    for (auto i = 0; i < max_iter; i++)
    {
        const vector test_pos = ((max + min) * 0.5f);
        const float delta = ((weapon_pos + test_pos) - eye_pos).length() - 2000.f;
        if (fabsf(delta) < 0.0005f)
            return test_pos;
        if (delta < 0.f)
            min = test_pos;
        else
            max = test_pos;
    }
    return vector();
}

float proj_aim::get_pipe_aim(float speed, float pitch)
{
    float min = -89.f;
    float max = 89.f;
    constexpr int max_iter = 1000;
    for (auto i = 0; i < max_iter; i++)
    {
        const float test_pos = ((max + min) * 0.5f);

        vector test_dir{test_pos, 0, 0};
        vector forward, up;
        test_dir.angle_vectors(&forward, nullptr, &up);
        test_dir = (forward * speed) + (up * 200.f);
        test_dir = test_dir.angle_to();

        const float delta = (test_dir.m_x - pitch);
        if (fabsf(delta) < 0.0005f)
            return test_pos;
        if (delta < 0.f)
            min = test_pos;
        else
            max = test_pos;
    }
    return 0.f;
}

vector proj_aim::get_aim_offset()
{
    vector vec_offset(23.5f, 12.0f, -3.0f);
    switch (g_cl.m_weapon->item_index())
    {
        case WPN_RescueRanger:
        case WPN_Crossbow:
        case WPN_FestiveCrossbow:
        case WPN_Huntsman:
        case WPN_CompoundBow:
            vec_offset.init(23.5f, 8.0f, -3.0f);
            break;
        case WPN_IronBomber:
        case WPN_StickyLauncher:
        case WPN_LoooseCannon:
        case WPN_LochNLoad:
        case WPN_GrenadeLauncher:
        case WPN_NewGrenadeLauncher:
        case WPN_FestiveGrenadeLauncher:
            vec_offset.init(16.0f, 8.0f, -6.0f);
            break;
        case WPN_SyringeGun:
        case WPN_NewSyringeGun:
        case WPN_Blutsauger:
        case WPN_Overdose:
            vec_offset.init(16, 6, -8);
            break;
        default:
            break;
    }
    return vec_offset;
}

bool proj_aim::is_pipe()
{
    int item_id = g_cl.m_weapon->item_index();
    if (item_id == WPN_IronBomber || item_id == WPN_StickyLauncher || item_id == WPN_LoooseCannon ||
        item_id == WPN_LochNLoad || item_id == WPN_GrenadeLauncher || item_id == WPN_NewGrenadeLauncher ||
        item_id == WPN_FestiveGrenadeLauncher)
    {
        return true;
    }
    return false;
}

void proj_aim::select_target()
{
    this->m_target = nullptr;
    bool found_vis = false;
    float distance = FLT_MAX;
    for (auto i = 1; i <= g_interfaces.m_engine->get_max_clients(); i++)
    {
        auto player = g_interfaces.m_entity_list->get_entity<c_base_player>(i);

        if (!player->is_valid(g_cl.m_local))
            continue;

        auto& target = g_player_manager.players[player->entindex() - 1];
        if (target.player != player || target.m_records.empty())
            continue;

        auto vis_pos = player->get_abs_origin();
        vis_pos.m_z += ((player->get_collideable()->obb_maxs().m_z + player->get_collideable()->obb_mins().m_z) * 0.5f);

        auto cur_distance = fabsf((vis_pos - g_cl.m_shoot_pos).delta(g_cl.m_cmd->m_viewangles));
        if (!found_vis)
        {
            if (cur_distance > 30.f)
                continue;
            if (cur_distance < distance)
            {
                this->m_target = player;
                distance = cur_distance;
            }
        }
        else if (cur_distance > distance || cur_distance > 30.f)
        {
            continue;
        }

        if (this->m_weapon_gravity < 1.f)
        {

            ray_t ray;
            ray.initialize(g_cl.m_shoot_pos, vis_pos);
            CTraceFilterIgnorePlayers traceFilter(g_cl.m_local, TFCOLLISION_GROUP_ROCKETS);
            trace_t trace;
            g_interfaces.m_engine_trace->trace_ray(ray, MASK_SHOT_HULL, &traceFilter, &trace);
            if (trace.m_entity == player || !trace.did_hit())
            {
                this->m_target = player;
                distance = cur_distance;
                found_vis = true;
            }
        }
        else
        {
            auto pos = player->m_vec_origin() + aim_offset(&target);
            vector view = g_cl.m_shoot_pos.look(pos);

            if (get_gravity_aim(pos - g_cl.m_shoot_pos, &view.m_x, false))
            {
                bool should_continue = false;

                vector new_eyepos = g_cl.m_shoot_pos;

                should_continue = setup_projectile(view, pos, new_eyepos);

                if (should_continue)
                    continue;

                vector new_dir = view.angle_vector() * this->m_weapon_speed;
                // rockettime = ( ( pos - new_eyepos ).length_2d( ) / ( new_dir.length_2d( ) ) );
                if (float rocket_time = travel_time(player, new_eyepos, pos, view);
                    proj_can_hit(player, view, rocket_time, new_eyepos))
                {
                    this->m_target = player;
                    distance = cur_distance;
                    found_vis = true;
                }
            }
        }
    }
}

double pipe_vel_change[2] = {1200., 200.};

void proj_aim::run()
{
    int attack = IN_ATTACK;
    if (g_cl.m_weapon->item_index() == WPN_Sandman)
        attack = IN_ATTACK2;

    static int ticks_since_shot = 0;

    auto distance = FLT_MAX;
    static bool was_shoot = false;
    bool shoud_shoot = (ticks_since_shot < 3) || (g_cl.m_cmd->buttons_ & attack) ||
                       g_ui.m_controls.aim.players.fire_mode->m_selected_index == 2;

    // g_cl.m_cmd->buttons_ |= IN_ATTACK;
    // if ( TICKS_TO_TIME( g_cl.m_local->m_tick_base( ) ) < g_cl.m_local->m_next_attack( ) ) {
    //	ticks_since_shot = 0;
    //	was_shoot = false;
    //	return;
    // }
    if (attack == IN_ATTACK2)
    {
        if (g_cl.m_weapon->m_clip2() <= 0)
        {
            if (g_cl.m_cmd->buttons_ & attack)
                ticks_since_shot = 0;
            else
                ticks_since_shot = fmin(5, ticks_since_shot + 1);
            was_shoot = false;
            return;
        }
    }
    else if (g_cl.m_weapon->m_clip1() <= 0)
    {
        if (g_cl.m_cmd->buttons_ & attack)
            ticks_since_shot = 0;
        else
            ticks_since_shot = fmin(5, ticks_since_shot + 1);
        was_shoot = false;
        return;
    }

    if (!g_ui.m_controls.aim.players.key->value_->enabled)
    {
        if (g_cl.m_cmd->buttons_ & attack)
            ticks_since_shot = 0;
        else
            ticks_since_shot = fmin(5, ticks_since_shot + 1);
        was_shoot = g_cl.m_cmd->buttons_ & attack;
        return;
    }

    if (!shoud_shoot)
    {
        if (g_cl.m_cmd->buttons_ & attack)
            ticks_since_shot = 0;
        else
            ticks_since_shot = fmin(5, ticks_since_shot + 1);
        was_shoot = g_cl.m_cmd->buttons_ & attack;
        return;
    }

    this->m_weapon_speed = get_speed();
    pipe_vel_change[0] = this->m_weapon_speed;
    if (this->m_weapon_speed == 0.f)
    {
        was_shoot = g_cl.m_cmd->buttons_ & IN_ATTACK;
        return;
    }
    if (!sv_gravity)
        sv_gravity = g_interfaces.m_cvar->find_var("sv_gravity");
    this->m_weapon_gravity = get_gravity();
    if (!is_pipe())
    {
        this->m_weapon_gravity *= sv_gravity->m_value.m_float_value;
    }
    else
    {
        this->m_weapon_speed = sqrt(pipe_vel_change[0] * pipe_vel_change[0] + pipe_vel_change[1] * pipe_vel_change[1]);
        this->m_weapon_gravity *= 800.f;
    }

    select_target();

    if (this->m_target)
    {
        find_shot(was_shoot, attack);
    }

    if (g_cl.m_cmd->buttons_ & attack)
        ticks_since_shot = 0;
    else
        ticks_since_shot = fmin(5, ticks_since_shot + 1);
    was_shoot = g_cl.m_cmd->buttons_ & attack;
}
void get_hull_size(vector& size)
{

    size = {3.8f, 3.8f, 3.8f};

    switch (g_cl.m_weapon->item_index())
    {
        case CTFParticleCannon:
        {
            size = {1.f, 1.f, 1.f};
            break;
        }
        case CTFCrossbow:
        {
            size = {3.f, 3.f, 3.f};
            break;
        }
        case CTFRocketLauncher:
        case CTFRocketLauncher_DirectHit:
        case CTFFlareGun:
        {
            size = {0.f, 3.7f, 3.7f};
            break;
        }
        case CTFSyringeGun:
        {
            size = {0.f, 1.f, 1.f};
            break;
        }
        case CTFCompoundBow:
        {
            size = {1.f, 1.f, 1.f};
            break;
        }
        case CTFRaygun:
        {
            size = {0.1f, 0.1f, 0.1f};
            break;
        }
        case CTFGrenadeLauncher:
        case CTFPipebombLauncher:
        case CTFStickBomb:
        {
            size = {2.f, 2.f, 2.f};
            break;
        }
        default:
            break;
    }
}
void proj_aim::find_shot(bool& was_shoot, int attack)
{
    auto target = g_player_manager.players[this->m_target->entindex() - 1];

    float last_time = -FLT_MAX;
    if (!g_movement.setup_mv(this->m_target->m_velocity(), this->m_target, g_cl.m_local->entindex()))
        return;

    auto* nci = g_interfaces.m_engine->get_net_channel_info();
    static auto* cl_interp = g_interfaces.m_cvar->find_var("cl_interp");
    static auto* cl_interp_ratio = g_interfaces.m_cvar->find_var("cl_interp_ratio");
    static auto* cl_updaterate = g_interfaces.m_cvar->find_var("cl_updaterate");
    if (!(nci && cl_interp && cl_interp_ratio && cl_updaterate))
        return;

    float lerp = std::fmaxf(cl_interp->m_value.m_float_value,
                            cl_interp_ratio->m_value.m_float_value / cl_updaterate->m_value.m_float_value);
    auto mindelta = FLT_MAX;
    auto maxsteps = 5000;
    float cur_time = -nci->GetLatency(2);
    vector last;
    struct target_holder
    {
        bool valid = false;
        vector pos;
        float time;
        vector eye_pos;
        vector view;
        int step = 0;
        bool ground;
    } found_holder;
    for (int steps = 0; steps < maxsteps; steps++)
    {
        cur_time += g_interfaces.m_global_vars->m_interval_per_tick;
        auto pos =
            g_movement.run() + aim_offset(&target); // +vector( 0, 0, closest->get_collideable( )->obb_mins( ).m_z );;
        auto delta_pos = pos - g_cl.m_shoot_pos;
        pos -= delta_pos.normalized() * 4.f;
        bool valid = true;

        vector view = g_cl.m_shoot_pos.look(pos);
        vector new_eye_pos = g_cl.m_shoot_pos;
        {
            if (this->m_weapon_gravity > 1 && !get_gravity_aim(pos - new_eye_pos, &view.m_x, false))
                break;

            bool should_continue = setup_projectile(view, pos, new_eye_pos);

            if (should_continue)
                continue;

            if (is_pipe())
            {
                vector forward, up;
                view.angle_vectors(&forward, nullptr, &up);
                vector velocity = ((forward * get_speed()) + (up * -200.));
                view.m_x = vector().look(velocity).m_x;
                // view.m_x -= static_cast< float >( atan2( -pipe_vel_change[ 1 ], pipe_vel_change[ 0 ] ) * 180. / pi );
                view.m_x = std::clamp<float>(view.m_x, -89.f, 89.f);
            }
        }
        float rocket_time = 0;
        if (this->m_weapon_gravity > 1)
        {

            vector new_dir = view.angle_vector() * this->m_weapon_speed;
            // rockettime = ( ( pos - new_eyepos ).length_2d( ) / ( new_dir.length_2d( ) ) );
            rocket_time = travel_time(this->m_target, new_eye_pos, pos, view);
        }
        else
        {
            vector new_dir = view.angle_vector() * this->m_weapon_speed;
            float desired_dist = (pos - new_eye_pos).length_2d();
            float step = new_dir.length_2d() * g_interfaces.m_global_vars->m_interval_per_tick;
            float current_dist = 0;
            rocket_time = 0.f;
            while (desired_dist > current_dist)
            {
                current_dist += step;
                rocket_time += g_interfaces.m_global_vars->m_interval_per_tick;
            }
        }
        if (rocket_time == -1)
            continue;

        if (g_cl.m_weapon->item_index() == WPN_StickyLauncher &&
            ((steps * g_interfaces.m_global_vars->m_interval_per_tick) < 0.8f))
            continue;

        auto delta = rocket_time - cur_time;
        auto vis_pos = pos;

        if (fabsf(delta) < mindelta && fabsf(delta) < g_interfaces.m_global_vars->m_interval_per_tick * 5)
        {
            found_holder.valid = true;
            found_holder.pos = pos;
            mindelta = fabsf(delta);
            found_holder.eye_pos = new_eye_pos;
            found_holder.view = view;
            found_holder.time = rocket_time;
            found_holder.step = steps;
            found_holder.ground = g_movement.mv.on_ground;
        }

        if (rocket_time <= (steps * g_interfaces.m_global_vars->m_interval_per_tick))
        {
            break;
        }
        last = pos;
    }
    bool shot = false;
    if (found_holder.valid)
    {
        /*float needed_hit_chance = g_ui.m_controls.aim.players.hitchance->m_value;
        if (needed_hit_chance > 0.f)
        {
            float cur_hitchance = (1.f - (fminf(TICKS_TO_TIME(found_holder.step), 1.f))) * 100.f;
            if (!found_holder.ground)
                cur_hitchance *= 0.5f;

            if (needed_hit_chance > cur_hitchance)
                goto skip;
        }*/

        m_path.clear();
        vector view = found_holder.view;
        if (this->m_weapon_gravity < 1)
        {
            ray_t ray;
            vector hullsize = {};
            get_hull_size(hullsize);
            ray.initialize(found_holder.eye_pos, found_holder.pos, hullsize * -1.f, hullsize);
            CTraceFilterIgnorePlayers traceFilter(g_cl.m_local, TFCOLLISION_GROUP_ROCKETS);
            trace_t trace;
            g_interfaces.m_engine_trace->trace_ray(ray, MASK_SHOT_HULL, &traceFilter, &trace);
            if (!trace.did_hit())
            {
                if (g_ui.m_controls.aim.players.fire_mode->m_selected_index != 0)
                    g_cl.m_cmd->buttons_ |= attack;
                g_cl.m_cmd->m_viewangles = view;
                shot = true;
            }
        }
        else
        {
            // if ( get_gravity_aim( speed, gravity, found_holder.pos - found_holder.eye_pos, &view.m_x, false ) ) {
            vector forward, right, up;
            // if ( is_pipe( ) ) {
            //	view.m_x -= static_cast< float >( atan2( -pipe_vel_change[ 1 ], pipe_vel_change[ 0 ] ) * 180. / pi );
            //	view.m_x = std::clamp<float>( view.m_x, -89.f, 89.f );
            // }
            vector new_dir = view.angle_vector();
            if (proj_can_hit(this->m_target, view, found_holder.time, found_holder.eye_pos, true))
            {
                //}
                if (g_ui.m_controls.aim.players.fire_mode->m_selected_index != 0)
                    if (was_shoot)
                        g_cl.m_cmd->buttons_ &= ~attack;
                    else
                    {
                        g_cl.m_cmd->buttons_ |= attack;
                    }
                g_cl.m_cmd->m_viewangles = view;
                shot = true;
                g_cl.m_cmd->tick_count_ = TIME_TO_TICKS(target.m_sim_time + lerp);
            }
        }
    }
skip:
    if (shot)
    {
        auto& log = g_movement.m_logs[target.player->entindex()];
        log.m_path = g_movement.path;
        log.end_time = g_movement.end_time;
        auto& log2 = m_logs[target.player->entindex()];
        log2.m_path = m_path;
        log2.end_time = end_time;
    }
}

void GetProjectileFireSetup(vector vecOffset, vector* vecSrc, vector* angForward, float flEndDist = 2000)
{
    // @todo third person code!!
    static convar* cl_flipviewmodels = g_interfaces.m_cvar->find_var("cl_flipviewmodels");
    if (cl_flipviewmodels->m_value.m_int_value)
    {
        vecOffset.m_y *= -1;
    }

    vector forward, right, up;
    angForward->angle_vectors(&forward, &right, &up);

    vector vecShootPos = *vecSrc;
    vector endPos = vecShootPos + forward * flEndDist;

    trace_t tr;
    ray_t ray;
    ray.initialize(vecShootPos, endPos);

    // Trace forward and find what's in front of us, and aim at that
    CTraceFilterIgnoreTeammates traceFilter(g_cl.m_local, COLLISION_GROUP_NONE, g_cl.m_local->m_i_team_num());
    g_interfaces.m_engine_trace->trace_ray(ray, MASK_SOLID, &traceFilter, &tr);

    // Offset actual start point
    *vecSrc += vecShootPos + (forward * vecOffset.m_x) + (right * vecOffset.m_y) + (up * vecOffset.m_z);

    // Find angles that will get us to our desired end point
    // Only use the trace end if it wasn't too close, which results
    // in visually bizarre forward angles
    if (tr.m_fraction > 0.1)
    {
        angForward->look(tr.m_end - *vecSrc);
    }
    else
    {
        angForward->look(endPos - *vecSrc);
    }
}

bool proj_aim::setup_projectile(vector& view, vector& pos, vector& new_eyepos)
{
    bool should_continue = false;
    vector vecVelocity;
    vector forward, right, up;

    if (is_pipe())
    {
        view.m_x -= static_cast<float>(atan2(-pipe_vel_change[1], pipe_vel_change[0]) * 180. / pi);
        view.m_x = std::clamp<float>(view.m_x, -89.f, 89.f);
    }

    vector vecOffset = get_aim_offset();
    CTraceFilterIgnoreTeammates traceFilter(g_cl.m_local, COLLISION_GROUP_NONE, g_cl.m_local->m_i_team_num());
    trace_t trace;

    vector last_eye{};

    for (auto i = 0; i < 5; i++)
    {
        vector new_view = view;

        new_view.angle_vectors(&forward, &right, &up);

        vector new_eye_pos = g_cl.m_shoot_pos + forward * vecOffset.m_x + right * vecOffset.m_y + up * vecOffset.m_z;
        // if ((last_eye - new_eye_pos).length_sqr() <= 0.001)
        //     break;
        //
        // last_eye = new_eye_pos;
        new_view = new_eye_pos.look(pos);

        if (this->m_weapon_gravity > 1)
        {
            if (!get_gravity_aim(pos - new_eye_pos, &new_view.m_x, false))
            {
                should_continue = true;
                break;
            }
        }

        if (is_pipe())
        {
            vector forward, up;
            new_view.angle_vectors(&forward, nullptr, &up);
            vector velocity = ((forward * get_speed()) + (up * -200.));
            view.m_x = vector().look(velocity).m_x;
            // view.m_x -= static_cast< float >( atan2( -pipe_vel_change[ 1 ], pipe_vel_change[ 0 ] ) * 180. / pi );
            view.m_x = std::clamp<float>(view.m_x, -89.f, 89.f);
            // new_view.m_x -= static_cast< float >( atan2( -pipe_vel_change[ 1 ], pipe_vel_change[ 0 ] ) * 180. / pi );
            // new_view.m_x = std::clamp<float>( new_view.m_x, -89.f, 89.f );
            // view = new_view;
        }
        else
        {
            vector delta = get_weapon_delta(g_cl.m_shoot_pos, new_eye_pos, new_view);
            ray_t ray;
            ray.initialize(new_eye_pos, (delta + new_eye_pos));
            // ray.initialize( new_eyepos, ( delta + new_eyepos ), vector( -8, -8, -8 ), vector( 8, 8, 8 ) );
            g_interfaces.m_engine_trace->trace_ray(ray, MASK_SOLID, &traceFilter, &trace);
            if (trace.m_fraction > 0.1f)
                view = g_cl.m_shoot_pos.look(trace.m_end);
            else
                view = g_cl.m_shoot_pos.look((delta + new_eye_pos));
        }
    }

    view.angle_vectors(&forward, &right, &up);
    new_eyepos = g_cl.m_shoot_pos + forward * vecOffset.m_x + right * vecOffset.m_y + up * vecOffset.m_z;
    return should_continue;
}

bool proj_aim::get_gravity_aim(vector difference, float* ret, bool lob)
{
    // float travel_time = difference.length_2d( ) / speed;
    // difference.m_z += travel_time * 800.f * g_interfaces.m_global_vars->m_interval_per_tick;
    //*ret = vector( ).look( difference ).m_x;
    // return true;

    const float x = difference.length_2d();
    double root = this->m_weapon_speed * this->m_weapon_speed * this->m_weapon_speed * this->m_weapon_speed -
                  this->m_weapon_gravity * (this->m_weapon_gravity * x * x +
                                            2. * difference.m_z * this->m_weapon_speed * this->m_weapon_speed);
    if (root < 0.0f || isnan(root))
    {
        return false;
    }
    root = sqrt(root);
    *ret = -rad_to_deg((std::atan2(((this->m_weapon_speed * this->m_weapon_speed) - (lob ? -root : root)),
                                   (this->m_weapon_gravity * x))));
    return true;
}

std::vector<vector> proj_aim::path_pred(vector view, float goal_time, vector weapon_pos)
{
    if (!sv_gravity)
        sv_gravity = g_interfaces.m_cvar->find_var("sv_gravity");
    this->m_weapon_gravity = get_gravity();
    this->m_weapon_speed = get_speed();
    if (!is_pipe())
    {
        this->m_weapon_gravity *= sv_gravity->m_value.m_float_value;
    }
    else
    {
        this->m_weapon_speed = sqrt(pipe_vel_change[0] * pipe_vel_change[0] + pipe_vel_change[1] * pipe_vel_change[1]);
        this->m_weapon_gravity *= 800.f;
    }
    if (this->m_weapon_gravity < 1 || this->m_weapon_speed < 1)
        return {};
    std::vector<vector> path = {};
    vector eye_pos = g_cl.m_local->m_vec_origin() + g_cl.m_local->m_view_offset();

    vector hullsize = {};
    get_hull_size(hullsize);

    vector forward, right, up;
    view.angle_vectors(&forward, &right, &up);
    vector view_offset = get_aim_offset();
    weapon_pos += forward * view_offset.m_x + right * view_offset.m_y + up * view_offset.m_z;
    if (!is_pipe())
    {
        CTraceFilterIgnoreTeammates traceFilter(g_cl.m_local, COLLISION_GROUP_NONE, g_cl.m_local->m_i_team_num());
        trace_t trace;


        vector endPos = eye_pos + forward * 2000.f;

        ray_t ray;
        ray.initialize(eye_pos, endPos);
        g_interfaces.m_engine_trace->trace_ray(ray, MASK_SOLID, &traceFilter, &trace);

        if (trace.m_fraction > 0.1f)
            view = weapon_pos.look(trace.m_end);
        else
            view = weapon_pos.look(endPos);
    }

    vector pos = weapon_pos;
    vector dir = view.angle_vector();
    vector velocity = (dir * this->m_weapon_speed);
    if (is_pipe())
    {
        view.angle_vectors(&forward, nullptr, &up);
        velocity = ((forward * get_speed()) + (up * 200.));
    }
    float time = 0;

    path.push_back(pos);
    while (time < goal_time)
    {
        float frac = goal_time - time;
        float interval = fminf(frac, g_interfaces.m_global_vars->m_interval_per_tick);
        // float height = pos.m_z;
        // velocity.m_z -= ( gravity * g_interfaces.m_global_vars->m_interval_per_tick * 0.5f );

        velocity.m_z -= (this->m_weapon_gravity * g_interfaces.m_global_vars->m_interval_per_tick * 0.5f);

        ray_t ray;
        ray.initialize(pos, pos + (velocity * interval), hullsize * -1.f,
                       hullsize); //, bounds*-1, bounds );
        trace_t trace;
        CTraceFilterIgnorePlayers traceFilter(g_cl.m_local, TFCOLLISION_GROUP_COMBATOBJECT);
        g_interfaces.m_engine_trace->trace_ray(ray, MASK_SHOT_HULL, &traceFilter, &trace);

        if (trace.did_hit())
        {
            path.push_back(trace.m_end);
            break;
        }

        pos += velocity * interval;

        path.push_back(pos);

        velocity.m_z -= (this->m_weapon_gravity * g_interfaces.m_global_vars->m_interval_per_tick * 0.5f);

        time += g_interfaces.m_global_vars->m_interval_per_tick;
    }
    return path;
}

void proj_aim::draw()
{
    if (g_cl.m_weapon && g_cl.m_local && g_cl.m_local->is_alive())
    {
        vector view;
        g_interfaces.m_engine->get_view_angles(view);
        auto path = path_pred(view, 2000.f, g_cl.m_local->eye_pos());
        vector screen_1, screen_2;
        if (path.size() > 1u)
        {
            auto backup = g_ui.m_theme.m_a;
            g_ui.m_theme.m_a = 100.f;
            for (auto i = path.end() - 2; i != path.begin(); --i)
            {

                if (g_interfaces.m_debug_overlay->screen_position(*(i + 1), screen_1) ||
                    g_interfaces.m_debug_overlay->screen_position(*i, screen_2))
                {
                    continue;
                }

                c_render::line(screen_1, screen_2,
                               g_ui.m_theme); // 0xAE, 0xBA, 0xF8
            }
            vector end = path.back();
            vector hull_size;
            get_hull_size(hull_size);
            c_render::box(end - hull_size, hull_size + end, g_ui.m_theme);
            g_ui.m_theme.m_a = backup;
        }
    }
    for (auto& i : m_logs)
    {
        auto& log = i.second;
        if (log.end_time <= 0)
            log.m_path.clear();
        if (log.m_path.size() <= 1u)
            continue;

        float temp_time = log.end_time;
        int iter = 1;
        vector screen_1, screen_2, screen_3;
        float backup = g_ui.m_theme.m_a;
        for (auto i = log.m_path.end() - 2; i != log.m_path.begin(); --i)
        {

            if (g_interfaces.m_debug_overlay->screen_position(*(i + 1), screen_1) ||
                g_interfaces.m_debug_overlay->screen_position(*i, screen_2))
            {
                continue;
            }

            if (temp_time < 0.f)
                break;

            g_ui.m_theme.m_a = 255.f * fminf(temp_time, 1.f);

            c_render::line(screen_1, screen_2,
                           g_ui.m_theme); // 0xAE, 0xBA, 0xF8
            temp_time -= g_interfaces.m_global_vars->m_interval_per_tick;
            // if ( (i % 4) == 0 ) {
            //	vector dif = path[ i ] - last;
            //	const auto difference.init.look(dif).m_y;
            //
            //	vector new_dir = path[ i ] + vector( 0, difference + 90.f, 0).angle_vector() * fminf(dif.length(),
            // 15);
            //
            //	if ( !g_interfaces.m_debug_overlay->screen_position( new_dir, screen_1 ) ) {
            //		c_render::line( screen_1, screen_2, color( 0xAE, 0xBA, 0xF8, 100 ) );
            //	}
            //
            //	last = path[ i ];
            // }
        }
        if (g_cl.m_weapon)
        {
            g_ui.m_theme.m_a = 255.f * fminf(log.end_time, 1.f);
            vector end = log.m_path.back();
            vector hull_size;
            get_hull_size(hull_size);
            c_render::box(end - hull_size, hull_size + end, g_ui.m_theme);
            g_ui.m_theme.m_a = backup;
        }
        log.end_time -= g_interfaces.m_global_vars->m_frame_time;
    }
    g_ui.m_theme.m_a = 255;
}

bool proj_aim::proj_can_hit(c_base_player* target, vector view, float goal_time, vector weapon_pos, bool record)
{
    vector eye_pos = g_cl.m_local->m_vec_origin() + g_cl.m_local->m_view_offset();

    vector hullsize = {};
    get_hull_size(hullsize);

    vector forward, up;
    if (!is_pipe())
    {
        CTraceFilterIgnoreTeammates traceFilter(g_cl.m_local, COLLISION_GROUP_NONE, g_cl.m_local->m_i_team_num());
        trace_t trace;

        view.angle_vectors(&forward, nullptr, &up);

        vector endPos = eye_pos + forward * 2000.f;

        ray_t ray;
        ray.initialize(eye_pos, endPos);
        g_interfaces.m_engine_trace->trace_ray(ray, MASK_SHOT, &traceFilter, &trace);

        if (trace.m_fraction > 0.1f)
            view = weapon_pos.look(trace.m_end);
        else
            view = weapon_pos.look(endPos);
    }

    if (record)
        m_path.clear();
    vector pos = weapon_pos;
    vector dir = view.angle_vector();
    vector velocity = (dir * this->m_weapon_speed);
    if (is_pipe())
    {
        view.angle_vectors(&forward, nullptr, &up);
        velocity = ((forward * get_speed()) + (up * 200.));
    }
    float time = 0;
    if (record)
    {
        end_time = time + 10.f;
        m_path.push_back(pos);
    }
    while (time < goal_time)
    {
        float frac = goal_time - time;
        float interval = fminf(frac, g_interfaces.m_global_vars->m_interval_per_tick);
        // float height = pos.m_z;
        // velocity.m_z -= ( gravity * g_interfaces.m_global_vars->m_interval_per_tick * 0.5f );

        velocity.m_z -= (this->m_weapon_gravity * g_interfaces.m_global_vars->m_interval_per_tick * 0.5f);

        ray_t ray;
        ray.initialize(pos, pos + (velocity * interval), hullsize * -1.f,
                       hullsize); //, bounds*-1, bounds );
        trace_t trace;
        CTraceFilterIgnorePlayers traceFilter(g_cl.m_local, TFCOLLISION_GROUP_COMBATOBJECT);
        g_interfaces.m_engine_trace->trace_ray(ray, MASK_SHOT_HULL, &traceFilter, &trace);

        if (trace.did_hit())
        {
            return false;
        }

        pos += velocity * interval;
        if (record)
        {
            end_time = time + 10.f;
            m_path.push_back(pos);
        }

        // velocity.m_x *= 0.8f;
        // velocity.m_y *= 0.8f;
        velocity.m_z -= (this->m_weapon_gravity * g_interfaces.m_global_vars->m_interval_per_tick * 0.5f);

        time += g_interfaces.m_global_vars->m_interval_per_tick;
    }
    return true;
}
