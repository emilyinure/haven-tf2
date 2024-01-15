#include "player_manager.h"
#include "sdk.h"
#include "movement_simulate.h"
#include "c_user_cmd.h"

#define DIST_EPSILON (0.03125f)
#define TIME_TO_TICKS(dt) ((int)(0.5f + (float)(dt) / g_interfaces.m_global_vars->m_interval_per_tick))
#define TICKS_TO_TIME(dt) (g_interfaces.m_global_vars->m_interval_per_tick * (float)(dt))

bool player_record_t::valid() const
{
    // use prediction curtime for this.
    const float curtime = TICKS_TO_TIME(g_cl.m_local->m_tick_base());

    // correct is the amount of time we have to correct game time,
    static auto* cl_interp = g_interfaces.m_cvar->find_var("cl_interp");
    static auto* cl_interp_ratio = g_interfaces.m_cvar->find_var("cl_interp_ratio");
    static auto* cl_updaterate = g_interfaces.m_cvar->find_var("cl_updaterate");
    const float lerp = std::fmaxf(cl_interp->m_value.m_float_value,
                                  cl_interp_ratio->m_value.m_float_value / cl_updaterate->m_value.m_float_value);
    float correct = lerp;

    // stupid fake latency goes into the incoming latency.
    const auto* nci = g_interfaces.m_engine->get_net_channel_info();
    if (nci)
        correct += nci->GetLatency(1);
    // check bounds [ 0, sv_maxunlag ]
    static auto sv_maxunlag = g_interfaces.m_cvar->find_var("sv_maxunlag");
    correct = std::clamp<float>(correct, 0.f, sv_maxunlag->m_value.m_float_value);

    // calculate difference between tick sent by player and our latency based tick.
    // ensure this record isn't too old.
    return std::abs(correct - (curtime - sim_time)) < 0.19f;
}

vector direction_pressed(player_record_t* record, vector last_vel)
{
    const vector vel = record->vel.normalized();
    last_vel.normalize_in_place();
    if ((vel - last_vel).length_2d() > 0.1f)
    {
        vector vel_difference = vel - last_vel;
        vel_difference.m_z = 0;
        vector dir = vel_difference.angle_to();
        dir.m_y -= record->eye_angle.m_y;
        return dir.angle_vector() * 450.f;
    }
    return vector();
}

inline float anglemod(float a)
{
    a = (360.f / 65536) * ((int)(a * (65536.f / 360.0f)) & 65535);
    return a;
}

float ApproachAngle(float target, float value, float speed)
{
    float delta = target - value;

    // Speed is assumed to be positive
    if (speed < 0)
        speed = -speed;

    if (delta < -180)
        delta += 360;
    else if (delta > 180)
        delta -= 360;

    if (delta > speed)
        value += speed;
    else if (delta < -speed)
        value -= speed;
    else
        value = target;

    return value;
}

float turn_epsilon(vector orig_vel)
{
    float max = 0.f;
    for (auto i = -1; i < 3; i++)
    {
        for (auto k = -1; k < 3; k++)
        {
            vector vel = orig_vel;
            vel.m_x += i * DIST_EPSILON;
            vel.m_y += k * DIST_EPSILON;
            float turn = rad_to_deg(std::atan2(vel.m_y, vel.m_x) - std::atan2(orig_vel.m_y, orig_vel.m_x));
            if (turn > max)
                max = turn;
        }
    }
    return max;
}

float dir_turning(vector vel, vector last_vel)
{
    if (vel.length_sqr_2d() < 1.f)
        return 0;
    if (last_vel.length_sqr_2d() < 1.f)
        return 0;

    float dir = rad_to_deg(std::atan2(vel.m_y, vel.m_x) - std::atan2(last_vel.m_y, last_vel.m_x));
    while (dir > 180.f)
        dir -= 360.f;
    while (dir < -180.f)
        dir += 360.f;
    if (fabsf(dir) > 175.f || fabsf(dir) < 1.f)
        return 0;
    if(fabs(dir) < 0.5f)
        return 0;
    return dir;
}

void player_t::SetGroundEntity(trace_t* pm)
{
    auto* oldGround = m_ground;
    auto* newGround = pm ? pm->m_entity : NULL;
    if (oldGround && newGround != oldGround)
    {
        if (oldGround->get_client_class()->m_class_id == CFuncConveyor)
        {
            vector right;
            oldGround->m_ang_rot().angle_vectors(nullptr, &right, nullptr);
            right *= ((c_func_conveyor*)oldGround)->conveyor_speed();
            m_base_velocity -= right;
            m_base_velocity.m_z = right.m_z;
        }
    }
    if (newGround && oldGround != newGround)
    {
        if (newGround->get_client_class()->m_class_id == CFuncConveyor)
        {
             vector right;
            newGround->m_ang_rot().angle_vectors(nullptr, &right, nullptr);
            right *= ((c_func_conveyor*)newGround)->conveyor_speed();
            m_base_velocity += right;
            m_base_velocity.m_z = right.m_z;
        }
    }
    m_ground = newGround;
}

#define GAMEMOVEMENT_JUMP_TIME 510.f
 
vector get_velocity(vector origin_diff, int lag, int flags, int last_flags, float jump_time_delta) // TODO: implement the rest of these cases
{
    if (!(flags & FL_ONGROUND))
    {
        vector hullSizeNormal = vector(16, 16, 72) - vector(-16, -16, 0);
        vector hullSizeCrouch = vector(16, 16, 36) - vector(-16, -16, 0);
        vector viewDelta = (hullSizeNormal - hullSizeCrouch);
        if ((flags & FL_DUCKING) && !(last_flags & FL_DUCKING))
        {
            origin_diff -= viewDelta;
        }
        else if (!(flags & FL_DUCKING) && (last_flags & FL_DUCKING))
        {
            origin_diff += viewDelta;
        }
    }
    return (origin_diff) * (1.f / TICKS_TO_TIME(lag));
}

void c_player_manager::update_players()
{
    for (auto i = 1; i <= g_interfaces.m_engine->get_max_clients(); i++)
    {
        const auto target = g_interfaces.m_entity_list->get_entity<c_base_player>(i);
        auto* player = &players[i - 1];
        if (player->player != target)
        {
            player->player = target;
            player->m_sim_time = -1.f;
        }

        if (!player->player)
            continue;

        if (!target->is_valid(g_cl.m_local, false))
        {
            player->m_records.clear();
            continue;
        }

        const float new_time = target->sim_time();
        if (player->m_sim_time < target->sim_time())
        {
            player_record_t new_record;
            new_record.flags = target->flags();
            new_record.eye_angle = target->m_eye_angles();
            new_record.m_lag =
                (player->m_sim_time == -1.f)
                    ? 1
                    : static_cast<int>(
                          0.5f + (float)(new_time - player->m_sim_time) /
                                     g_interfaces.m_global_vars
                                         ->m_interval_per_tick); // TIME_TO_TICKS( ( new_time - player->sim_time ) );
            player->m_sim_time = new_time;
            new_record.origin = target->m_vec_origin();
            const vector dif = (new_record.origin - player->pred_origin);
            if (fabs(dif.m_x) <= 0.03125f)
                new_record.origin.m_x = player->pred_origin.m_x;
            if (fabs(dif.m_y) <= 0.03125f)
                new_record.origin.m_y = player->pred_origin.m_y;
            if (fabs(dif.m_z) <= 0.03125f)
                new_record.origin.m_z = player->pred_origin.m_z;
            new_record.vel = target->m_velocity();
            // new_record.dir = 0;
            new_record.player = player;
            new_record.sim_time = new_time;

            if (new_record.flags & FL_ONGROUND)
            {
                trace_t pm;

                vector vecStartPos = new_record.origin;
                vector vecEndPos(new_record.origin.m_x, new_record.origin.m_y, (new_record.origin.m_z - 2.0f));
                bool bMoveToEndPos = false;
                vecEndPos.m_z -= 16.f + DIST_EPSILON;
                bMoveToEndPos = true;
                c_movement_simulate::try_touch_ground(target, vecStartPos, vecEndPos, target->mins(), target->maxs(),
                                                      MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, pm);
                if (pm.m_plane.normal[2] < 0.7f)
                {
                    // Test four sub-boxes, to see if any of them would have found shallower slope we could actually
                    // stand on
                    c_movement_simulate::try_touch_ground_in_quadrants(target, vecStartPos, vecEndPos, target->mins(),
                                                                       target->maxs(), MASK_PLAYERSOLID,
                                                                       COLLISION_GROUP_PLAYER_MOVEMENT, pm);
                    player->SetGroundEntity(&pm);
                }
                else
                {
                    player->SetGroundEntity(&pm);
                }
            }
            else
                player->SetGroundEntity(nullptr);

            new_record.on_ground = player->m_ground != nullptr;
            new_record.mins_prescaled = target->get_collideable()->obb_mins_pre_scaled();
            new_record.maxs_prescaled = target->get_collideable()->obb_maxs_pre_scaled();
            new_record.mins = target->mins();
            new_record.maxs = target->maxs();
            if (new_record.on_ground)
                new_record.vel.m_z = 0;

            const auto backup_abs_origin = target->get_abs_origin();

            target->set_abs_origin(target->m_vec_origin());

            const auto bones_ac = player->player->bone_cache();
            if (!bones_ac)
                return;

            bones_ac->UpdateBones(new_record.bones, 128, -1);

            new_record.built = target->setup_bones(new_record.bones, 128, 0x100, target->sim_time());


            target->set_abs_origin(backup_abs_origin);
            player_info_t info;
            if (g_interfaces.m_engine->get_player_info(target->entindex(), &info))
                if (!player->m_records.empty())
                {
                    new_record.vel = get_velocity(new_record.origin - player->m_records[0]->origin, new_record.m_lag,
                                     new_record.flags, player->m_records[0]->flags, new_record.sim_time - player->m_last_time_jumped);
                    g_movement.mv.m_player = target;
                    new_record.move_data = g_movement.estimate_walking_dir(
                        new_record.vel, player->m_records[0]->vel, new_record.eye_angle, new_record.origin);
                    new_record.dir_change = (new_record.eye_angle.m_y - player->m_records[0]->eye_angle.m_y);
                    if (!new_record.on_ground)
                        new_record.dir_change = dir_turning(new_record.vel, player->m_records[0]->vel);
                    while (new_record.dir_change > 180.f)
                        new_record.dir_change -= 360.f;
                    while (new_record.dir_change < -180.f)
                        new_record.dir_change += 360.f;
                    new_record.dir_change /= new_record.m_lag;

                    float accumulated_change = new_record.dir_change;
                    int count = 1;
                    for (auto i = 0; i < 2; i++)
                    {
                        if (i >= player->m_records.size())
                            break;
                        accumulated_change += player->m_records[i]->dir_change;
                        count++;
                    }

                    float max_turn = 10.f * g_interfaces.m_global_vars->m_interval_per_tick;
                    new_record.dir =
                        accumulated_change / count; // ApproachAngle(turn, player->m_records[0]->dir, max_turn);
                    new_record.ground_dir = 0.f; 
                    if (new_record.move_data.length_sqr_2d() > 1.f)
                    {

                        const int count = fmin(player->m_records.size(), TIME_TO_TICKS(0.4));
                        vector last_move_dir;
                        for (auto i = 0; i < count; i++)
                        {
                            if (i >= player->m_records.size())
                                break;
                            if (player->m_records[i]->move_data.length_sqr_2d() > 1.f)
                            {
                                last_move_dir = player->m_records[i]->move_data;
                                break;
                            }
                        }

                        float turn = dir_turning(new_record.move_data, last_move_dir);
                        while (turn > 180.f)
                            turn -= 360.f;
                        while (turn < -180.f)
                            turn += 360.f;
                        max_turn = 315 * g_interfaces.m_global_vars->m_interval_per_tick;
                        new_record.ground_dir = turn;
                    }

                    vector predicted_origin;
                    if(g_movement.setup_mv(new_record.vel, player->player) &&
                                             g_movement.run(player->pred_origin)){
                        player->pred_origin = predicted_origin;
                    }
                    
                    if (!(new_record.flags & FL_ONGROUND) && (player->m_records[0]->flags & FL_ONGROUND) &&
                        new_record.vel.m_z > 100.f)
                        player->m_last_time_jumped = new_record.sim_time;
                }

            player->m_records.push_front(std::make_shared<player_record_t>(new_record));
        }
        while (player->m_records.size() > 256)
            player->m_records.pop_back();
    }
}

player_record_t::~player_record_t()
{
    // g_interfaces.m_mem_alloc->free( bones );
}

bool player_record_t::cache()
{
    if (!built)
        return false;
    const auto bones_ac = player->player->bone_cache();
    if (!bones_ac)
        return false;

    memcpy(player->player->GetCachedBoneData()->m_elements, bones,
           sizeof(matrix_3x4) * player->player->GetCachedBoneData()->m_size);
}

void player_record_t::restore()
{
    const auto bones_ac = player->player->bone_cache();
    if (!bones_ac)
        return;
    bones_ac->UpdateBones(bones, 128, -1);
}
