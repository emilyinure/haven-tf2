#include "stab.h"
#include "player_manager.h"
#include "sdk.h"

#define TIME_TO_TICKS(dt) ((int)(0.5f + (float)(dt) / g_interfaces.m_global_vars->m_interval_per_tick))
bool CanPerformBackstabONTarget(vector& angle, vector& enemy_view, c_base_player* target)
{
    vector vecToTarget, target_center = ((target->maxs() - target->mins()) * 0.5f) + target->mins(),
                        local_center = ((g_cl.m_local->maxs() - g_cl.m_local->mins()) * 0.5f) + g_cl.m_local->mins();
    vecToTarget = (target_center + target->get_abs_origin()) - (local_center + g_cl.m_local->get_abs_origin());
    vecToTarget.m_z = 0.0f;
    vecToTarget.normalize_in_place();

    // Get owner forward view vector
    vector vecOwnerForward = angle.angle_vector();
    vecOwnerForward.m_z = 0.0f;
    vecOwnerForward.normalize_in_place();

    // Get target forward view vector
    vector vecTargetForward = enemy_view.angle_vector();
    vecTargetForward.m_z = 0.0f;
    vecTargetForward.normalize_in_place();

    // Make sure owner is behind, facing and aiming at target's back
    float flPosVsTargetViewDot = vecToTarget.dot(vecTargetForward); // Behind?
    float flPosVsOwnerViewDot = vecToTarget.dot(vecOwnerForward);   // Facing?
    float flViewAnglesDot = vecTargetForward.dot(vecOwnerForward);  // Facestab?

    return (flPosVsTargetViewDot > 0.f && flPosVsOwnerViewDot > 0.5f && flViewAnglesDot > -0.3f);
}
bool DoSwingTraceInternal(vector& angle, trace_t& trace)
{
    // Setup a volume for the melee weapon to be swung - approx size, so all melee behave the same.
    static vector vecSwingMinsBase(-18, -18, -18);
    static vector vecSwingMaxsBase(18, 18, 18);

    float fBoundsScale = c_attribute_manager::attribute_hook_float(1.0f, "melee_bounds_multiplier", g_cl.m_weapon);
    vector vecSwingMins = vecSwingMinsBase * fBoundsScale;
    vector vecSwingMaxs = vecSwingMaxsBase * fBoundsScale;

    // Setup the swing range.
    float fSwingRange = g_cl.m_weapon->get_swing_range();

    // Scale the range and bounds by the model scale if they're larger
    // Not scaling down the range for smaller models because midgets need all the help they can get
    float fModelScale = g_cl.m_local->model_scale();
    if (fModelScale > 1.0f)
    {
        fSwingRange *= fModelScale;
        vecSwingMins *= fModelScale;
        vecSwingMaxs *= fModelScale;
    }

    fSwingRange = c_attribute_manager::attribute_hook_float(fSwingRange, "melee_range_multiplier", g_cl.m_weapon);

    vector vecForward = angle.angle_vector();
    ray_t ray;
    CTraceFilterSimple ignoreTeammatesFilter(g_cl.m_local, COLLISION_GROUP_NONE);
    ray.initialize(g_cl.m_shoot_pos, g_cl.m_shoot_pos + vecForward * fSwingRange);

    g_interfaces.m_engine_trace->trace_ray(ray, MASK_SOLID, &ignoreTeammatesFilter, &trace);
    if (trace.m_fraction < 1.f)
        return true;
    ray = ray_t();
    trace = trace_t();
    ray.initialize(g_cl.m_shoot_pos, g_cl.m_shoot_pos + vecForward * fSwingRange, vecSwingMinsBase, vecSwingMaxsBase);
    g_interfaces.m_engine_trace->trace_ray(ray, MASK_SOLID, &ignoreTeammatesFilter, &trace);
    return trace.m_fraction < 1.f;
}
bool c_backstab::check_player(c_base_player* base_player)
{
    auto [m_records, sim_time, pred_origin, player, m_base_velocity, m_ground, m_last_jump_time] =
        g_player_manager.players[base_player->entindex() - 1];
    if (player != base_player || m_records.empty())
        return false;

    static auto* cl_interp = g_interfaces.m_cvar->find_var("cl_interp");
    static auto* cl_interp_ratio = g_interfaces.m_cvar->find_var("cl_interp_ratio");
    static auto* cl_updaterate = g_interfaces.m_cvar->find_var("cl_updaterate");
    const int lerp =
        TIME_TO_TICKS(std::fmaxf(cl_interp->m_value.m_float_value,
                                 cl_interp_ratio->m_value.m_float_value / cl_updaterate->m_value.m_float_value));

    bool found = false;
    auto record = m_records[0];

    const vector origin = base_player->get_abs_origin();
    const vector mins = base_player->get_collideable()->obb_mins_pre_scaled();
    const vector maxs = base_player->get_collideable()->obb_maxs_pre_scaled();

    bool ret_state = false;
    auto bones = base_player->bone_cache();

    float fSwingRange = g_cl.m_weapon->get_swing_range();
    float fModelScale = g_cl.m_local->model_scale();
    if (fModelScale > 1.0f)
        fSwingRange *= fModelScale;

    if (record && record->valid())
    {
        base_player->set_abs_origin(record->origin);
        base_player->set_collision_bounds(record->mins_prescaled, record->maxs_prescaled);
        record->cache();
        const vector point = record->origin + (record->maxs + record->mins) * 0.5f;
        vector look = g_cl.m_shoot_pos.look(point);

        trace_t trace;

        if (std::powf(fSwingRange, 2) * 2 <= (record->origin - g_cl.m_shoot_pos).length_sqr() && //optimization
            ((g_cl.m_local->m_class() != TF2_Spy || CanPerformBackstabONTarget(look, record->eye_angle, base_player)) &&
             DoSwingTraceInternal(look, trace)))
        {
            auto entity = trace.m_entity;
            if (entity == base_player)
            {
                g_cl.m_cmd->m_viewangles = look;
                g_cl.m_cmd->buttons_ |= IN_ATTACK;
                g_cl.m_cmd->tick_count_ = TIME_TO_TICKS(record->sim_time) + lerp;
                ret_state = true;
            }
        }
        record->restore();
    }

    record = nullptr;
    if (!ret_state)
    {
        for (const auto next : m_records)
        {
            if (!next->valid())
                continue;
            record = next;
        }

        if (record)
        {
            base_player->set_abs_origin(record->origin);
            base_player->set_collision_bounds(record->mins_prescaled, record->maxs_prescaled);

            record->cache();
            const vector point = record->origin + (record->maxs + record->mins) * 0.5f;
            vector look = g_cl.m_shoot_pos.look(point);

            trace_t trace;

            if (std::powf(fSwingRange, 2) * 2 <= (record->origin - g_cl.m_shoot_pos).length_sqr() && //optimization
                     ((g_cl.m_local->m_class() != TF2_Spy ||
                         CanPerformBackstabONTarget(look, record->eye_angle, base_player)) &&
                        DoSwingTraceInternal(look, trace)))
            {
                auto entity = trace.m_entity;
                if (entity == base_player)
                {
                    g_cl.m_cmd->m_viewangles = look;
                    g_cl.m_cmd->buttons_ |= IN_ATTACK;
                    g_cl.m_cmd->tick_count_ = TIME_TO_TICKS(record->sim_time) + lerp;
                    ret_state = true;
                }
            }
            record->restore();
        }
    }

    base_player->set_abs_origin(origin);
    base_player->set_collision_bounds(mins, maxs);

    return ret_state;
}

void c_backstab::run()
{
    int slot = g_cl.m_weapon->GetSlot();
    if (slot != SLOT_MELEE)
        return;

    for (auto i = 1; i < g_interfaces.m_engine->get_max_clients(); i++)
    {
        const auto base_player = g_interfaces.m_entity_list->get_entity<c_base_player>(i);
        if (!base_player->is_valid(g_cl.m_local, g_cl.m_weapon->item_index() != WPN_DisciplinaryAction))
            continue;
        if (check_player(base_player))
            break;
    }
}
