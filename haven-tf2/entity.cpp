#include "sdk.h"

vector& c_base_entity::m_vec_origin()
{
    return this->get<vector>(g_netvars.m_offsets.dt_base_entity.m_vec_origin);
}

vector& c_base_entity::m_ang_rot()
{
    return this->get<vector>(g_netvars.m_offsets.dt_base_entity.m_ang_rot);
}

vector c_base_entity::eye_pos()
{
    return m_vec_origin() + m_view_offset();
}

vector& c_base_entity::mins()
{
    return this->get<vector>(g_netvars.m_offsets.dt_base_entity.m_mins);
}
vector& c_base_entity::maxs()
{
    return this->get<vector>(g_netvars.m_offsets.dt_base_entity.m_maxs);
}

vector& c_base_entity::m_view_offset()
{
    return this->get<vector>(g_netvars.m_offsets.dt_base_entity.m_vec_view_offset);
}
float c_base_entity::sim_time()
{
    return this->get<float>(g_netvars.m_offsets.dt_base_entity.m_simulation_time);
}
float c_base_entity::spawn_time()
{
    return this->get<float>(g_netvars.m_offsets.dt_base_entity.m_simulation_time);
}
int& c_base_entity::m_i_team_num()
{
    return this->get<int>(g_netvars.m_offsets.dt_base_entity.m_i_team_num);
}

vector c_base_entity::get_abs_velocity()
{
    vector velocity;
    velocity[0] = this->get<float>(348);
    velocity[1] = this->get<float>(352);
    velocity[2] = this->get<float>(356);
    return velocity;
}

box_t c_base_entity::get_bounding_box()
{
    vector mins = {}, maxs = {}, mins_screen = {}, maxs_screen = {}, origin_screen = {};

    this->compute_hitbox_surrounding_box(&mins, &maxs);

    if (!math::world_to_screen(mins, mins_screen) ||
        !math::world_to_screen(maxs, maxs_screen) ||
        !math::world_to_screen(this->get_abs_origin(), origin_screen))
        return {};

    box_t return_box;

    return_box.m_h = mins_screen.m_y - maxs_screen.m_y;
    return_box.m_y = maxs_screen.m_y;
    return_box.m_w = return_box.m_h * 0.5;
    return_box.m_x = origin_screen.m_x - (return_box.m_w * 0.5);

    return return_box;
}

bool c_base_entity::compute_hitbox_surrounding_box(vector* world_mins, vector* world_maxs)
{
    static auto compute_hitbox_surrounding_box =
        reinterpret_cast<compute_hitbox_surrounding_box_fn>(g_offsets.m_sigs.compute_hitbox_surrounding_box);

    if (!compute_hitbox_surrounding_box)
        return false;

    return compute_hitbox_surrounding_box(this, world_mins, world_maxs);
}

c_base_handle c_base_player::active_weapon_handle()
{
    return this->get<c_base_handle>(g_netvars.m_offsets.dt_base_player.m_active_weapon);
}

c_base_weapon* c_base_player::get_active_weapon()
{
    return g_interfaces.m_entity_list->get_entity<c_base_weapon>(active_weapon_handle());
}

c_base_entity* c_base_player::calculate_ground()
{
    //Categorize position but without extra stuff
    auto try_touch_ground_in_quadrants =
        [](c_base_player* player, vector minsSrc, vector maxsSrc, const vector& start,
                                        const vector& end, unsigned int fMask, int collisionGroup, trace_t& pm)
    {
        vector mins, maxs;

        float fraction = pm.m_fraction;
        vector endpos = pm.m_end;

        // Check the -x, -y quadrant
        mins = minsSrc;
        maxs = vector{min(0.0f, maxsSrc[0]), min(0.0f, maxsSrc[1]), maxsSrc[2]};

        ray_t ray;
        ray.initialize(start, end, mins, maxs);
        CTraceFilterIgnorePlayers traceFilter(player, collisionGroup);
        g_interfaces.m_engine_trace->trace_ray(ray, fMask, &traceFilter, &pm);
        if (pm.m_entity && pm.m_plane.normal[2] >= 0.7)
        {
            pm.m_fraction = fraction;
            pm.m_end = endpos;
            return;
        }

        // Check the +x, +y quadrant
        mins = vector{max(0.0f, minsSrc[0]), max(0.0f, minsSrc[1]), minsSrc[2]};
        maxs = maxsSrc;
        ray.initialize(start, end, mins, maxs);
        g_interfaces.m_engine_trace->trace_ray(ray, fMask, &traceFilter, &pm);
        if (pm.m_entity && pm.m_plane.normal[2] >= 0.7)
        {
            pm.m_fraction = fraction;
            pm.m_end = endpos;
            return;
        }

        // Check the -x, +y quadrant
        mins = vector{minsSrc[0], max(0.0f, minsSrc[1]), minsSrc[2]};
        maxs = vector{min(0.0f, maxsSrc[0]), maxsSrc[1], maxsSrc[2]};
        ray.initialize(start, end, mins, maxs);
        g_interfaces.m_engine_trace->trace_ray(ray, fMask, &traceFilter, &pm);
        if (pm.m_entity && pm.m_plane.normal[2] >= 0.7)
        {
            pm.m_fraction = fraction;
            pm.m_end = endpos;
            return;
        }

        // Check the +x, -y quadrant
        mins = vector{max(0.0f, minsSrc[0]), minsSrc[1], minsSrc[2]};
        maxs = vector{maxsSrc[0], min(0.0f, maxsSrc[1]), maxsSrc[2]};
        ray.initialize(start, end, mins, maxs);
        g_interfaces.m_engine_trace->trace_ray(ray, fMask, &traceFilter, &pm);
        if (pm.m_entity && pm.m_plane.normal[2] >= 0.7)
        {
            pm.m_fraction = fraction;
            pm.m_end = endpos;
            return;
        }

        pm.m_fraction = fraction;
        pm.m_end = endpos;
    };

    trace_t pm;

    vector player_mins = this->mins();
    vector player_maxs = this->maxs();
    vector position = this->get_abs_origin();

    vector point = position;
    point[2] -= 2.0f;

    vector bump_origin = position;

    float zvel = this->m_velocity()[2];
    bool moving_up_rapidly = zvel > 140.0f;
    float ground_entity_vel_z = 0.0f;
    if (moving_up_rapidly)
    {
        auto ground = get_ground();
        if (ground)
        {
            ground_entity_vel_z = ground->get_abs_velocity()[2];
            moving_up_rapidly = (zvel - ground_entity_vel_z) > 140.0f;
        }
    }

    // Was on ground, but now suddenly am not
    if (moving_up_rapidly)
    {
        return nullptr;
    }

    // Try and move down.
    ray_t ray;
    ray.initialize(bump_origin, point, player_mins, player_maxs);
    CTraceFilterIgnorePlayers traceFilter(this, COLLISION_GROUP_PLAYER_MOVEMENT);
    g_interfaces.m_engine_trace->trace_ray(ray, MASK_PLAYERSOLID, &traceFilter, &pm);
    
    // Was on ground, but now suddenly am not.  If we hit a steep plane, we are not on ground
    if (!pm.m_entity || pm.m_plane.normal[2] < 0.7)
    {
        // Test four sub-boxes, to see if any of them would have found shallower slope we could actually stand on
        try_touch_ground_in_quadrants(this, player_mins, player_maxs, bump_origin, point, MASK_PLAYERSOLID,
                                  COLLISION_GROUP_PLAYER_MOVEMENT, pm);

        if (!pm.m_entity || pm.m_plane.normal[2] < 0.7)
        {
            return nullptr;
        }
        else
        {
            return pm.m_entity;
        }
    }
    else
    {
        return pm.m_entity;
    }
}

c_base_entity* c_base_player::get_ground()
{
    return g_interfaces.m_entity_list->get_entity<c_base_entity>(ground_handle());
}
c_base_handle c_base_player::ground_handle()
{
    return this->get<c_base_handle>(g_netvars.m_offsets.dt_base_player.m_ground_handle);
}

vector c_base_entity::world_space_center()
{
    return get_abs_origin() + (maxs() + mins()) * 0.5f;
}

c_base_entity* c_base_entity::get_owner()
{
    return g_interfaces.m_entity_list->get_entity<c_base_entity>(owner_handle());
}
c_base_handle c_base_entity::owner_handle()
{
    return this->get<c_base_handle>(g_netvars.m_offsets.dt_base_entity.m_owner_handle);
}

int& c_base_player::hitbox_set()
{
    return this->get<int>(g_netvars.m_offsets.dt_base_animating.m_hitbox_set);
}

vector c_base_player::get_hitbox_pos(int iHitbox, matrix_3x4* matrix, vector mins, vector max)
{
    const model_t* model = get_model();
    if (!model)
        return vector();

    const studiohdr_t* hdr = g_interfaces.m_model_info->GetStudiomodel(model);
    if (!hdr)
        return vector();
    const bool setup = !matrix;
    if (!matrix)
    {
        matrix = (matrix_3x4*)malloc(sizeof(matrix_3x4) * 128);
        if (!setup_bones(matrix, 128, 0x100, sim_time()))
            return vector();
    }

    const mstudiohitboxset_t* set = hdr->GetHitboxSet(hitbox_set());
    if (!set)
        return vector();

    const mstudiobbox_t* box = set->pHitbox(iHitbox);
    if (!box)
        return vector();

    vector center = ((box->bbmin * mins) + (box->bbmax * max));

    vector vHitbox;

    center.transform(matrix[box->bone], vHitbox);
    if (setup)
        free(matrix);
    return vHitbox;
}

c_tf_player_shared* c_base_player::m_shared()
{
    return reinterpret_cast<c_tf_player_shared*>(reinterpret_cast<uintptr_t>(this) +
                                                 g_netvars.m_offsets.dt_base_player.m_shared);
}

void c_base_player::force_taunt_cam(const int nTo)
{
    get<int>(g_netvars.m_offsets.dt_base_player.m_taunt) = nTo;
}

void c_base_player::SetCurrentCmd(usercmd_t* pCmd)
{
    static auto offset = g_netvars.get("DT_BasePlayer", "m_hConstraintEntity").m_offset - 0x4;
    get<usercmd_t*>(offset) = pCmd;
}

int& c_base_player::m_i_health()
{
    return this->get<int>(g_netvars.m_offsets.dt_base_player.m_i_health);
}

int& c_base_player::m_tick_base()
{
    return this->get<int>(g_netvars.m_offsets.dt_base_player.m_tick_base);
}

int& c_base_weapon::m_clip1()
{
    return this->get<int>(g_netvars.m_offsets.dt_base_weapon.m_clip1);
}

int& c_base_weapon::m_clip2()
{
    return this->get<int>(g_netvars.m_offsets.dt_base_weapon.m_clip2);
}

float& c_base_player::m_next_attack()
{
    return this->get<float>(g_netvars.m_offsets.dt_base_player.m_next_attack);
}

vector c_base_player::m_velocity()
{
    return this->get<vector>(g_netvars.m_offsets.dt_base_player.m_velocity);
}

float c_base_player::m_max_speed()
{
    return this->get<float>(g_netvars.m_offsets.dt_base_player.m_max_speed);
}

vector c_base_player::m_eye_angles()
{
    return this->get<vector>(g_netvars.m_offsets.dt_base_player.m_eye_angles);
}
void c_base_player::set_abs_origin(vector origin)
{
    vector* abs = &this->get<vector>(700);
    *abs = origin;
}
void c_base_player::set_abs_angles(vector angles)
{
    typedef void(__thiscall * oSetAbsOrigin)(void*, vector);
    static auto func =
        g_modules.get("client.dll").get_sig("55 8B EC 83 EC ? 56 57 8B F1 E8 ? ? ? ? 8B 7D ?").as<oSetAbsOrigin>();
    func(this, angles);
}

vector c_base_player::calculate_abs_velocity()
{
    typedef void(__thiscall * oCalcAbsoluteVelocity)(void*);
    static auto func = g_modules.get("client.dll")
                           .get_sig("55 8B EC 83 EC 3C 56 8B F1 F7")
                           .as<oCalcAbsoluteVelocity>();

    this->get<int>(416) |= (1 << 12);
    func(this);

    vector velocity = get_abs_velocity();
    const auto ground_entity = calculate_ground();
    if (ground_entity)
    {
        if (ground_entity->get_client_class()->m_class_id == CFuncConveyor)
        {
            vector right{};
            ground_entity->m_ang_rot().angle_vectors(nullptr, &right, nullptr);
            right *= ((c_func_conveyor*)ground_entity)->conveyor_speed();
            velocity -= right;
        }
    }
    return velocity;
}

CBoneCacheHandler* c_base_player::bone_cache()
{
    typedef CBoneCacheHandler*(__thiscall * oSetAbsOrigin)(void*, void*);
    static auto func =
        g_modules.get("client.dll").get_sig("55 8B EC 83 EC ? 56 8B F1 57 FF B6 ? ? ? ?").as<oSetAbsOrigin>();
    return func(this, NULL);
}

int& c_base_player::m_class()
{
    return this->get<int>(g_netvars.m_offsets.dt_base_player.m_class);
}
int& c_base_player::flags()
{
    return this->get<int>(g_netvars.m_offsets.dt_base_player.m_flags);
}

char& c_base_player::m_life_state()
{
    return this->get<char>(g_netvars.m_offsets.dt_base_player.m_life_state);
}

move_type_t c_base_player::move_type()
{
    return this->get<move_type_t>(g_netvars.m_offsets.dt_base_entity.m_move_type);
}

bool c_base_player::is_alive()
{
    return this->m_life_state() == e_life_state::life_alive;
}

bool c_base_player::is_valid(c_base_player* local, const bool team_check, const bool dormant_check,
                             const bool alive_check)
{
    if (!this)
        return false;

    if (dormant_check && this->is_dormant())
        return false;

    if (alive_check && !this->is_alive())
        return false;

    if (team_check && this->m_i_team_num() == local->m_i_team_num())
        return false;

    return true;
}

float c_base_weapon::charge_time()
{
    return this->get<float>(g_netvars.m_offsets.dt_base_weapon.m_charge_begin_time);
}

float c_base_weapon::pipe_charge_time()
{
    return this->get<float>(g_netvars.m_offsets.dt_base_weapon.m_pipe_charge_begin_time);
}

float& c_base_weapon::m_next_primary_attack()
{
    return this->get<float>(g_netvars.m_offsets.dt_base_weapon.m_next_primary_attack);
}

float& c_base_weapon::m_next_secondary_attack()
{
    return this->get<float>(g_netvars.m_offsets.dt_base_weapon.m_next_secondary_attack);
}

bool c_base_weapon::CanBackStab()
{
    static auto offset = g_netvars.get("DT_TFWeaponKnife", "m_bReadyToBackstab").m_offset;
    return *(bool*)(this + offset);
}

short& c_base_weapon::item_index()
{
    // g_netvars.m_offsets.dt_base_weapon.m_item_definition_index = g_netvars.get( "DT_BaseCombatWeapon",
    // "m_iItemDefinitionIndex" ).m_offset;
    return this->get<short>(g_netvars.m_offsets.dt_base_weapon.m_item_definition_index);
}

float& c_base_weapon::get_charge_damage()
{
    return this->get<float>(g_netvars.m_offsets.dt_base_weapon.m_charge_damage);
}

int c_base_weapon::GetSlot()
{
    typedef int(__thiscall * OriginalFn)(PVOID);
    return g_utils.get_virtual_function<OriginalFn>(this, 330)(this);
}

float c_base_weapon::rot_speed()
{
    return this->get<float>(g_netvars.m_offsets.dt_base_weapon.m_rot_speed);
}

float c_base_weapon::GetSwingRange()
{
    switch (this->item_index())
    {
        case WPN_Sword:
        case WPN_ScottsSkullctter:
        case WPN_Headless:
        case WPN_Claidheamhmor:
        case WPN_PersainPersuader:
        case WPN_Golfclub:
        case WPN_FestiveEyelander:
            return 70.0f;
        case WPN_DisciplinaryAction:
            return 90.0f;
    }

    return 48.0f;
}

bool c_base_weapon::do_swing_trace_internal(trace_t& trace)
{
    static auto do_swing_trace_internal_fn =
        g_modules.get("client.dll")
            .get_sig("53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC 38 05")
            .m_ptr;
    return reinterpret_cast<bool(__thiscall*)(void*, trace_t&, bool, c_utl_vector<trace_t>*)>(
        do_swing_trace_internal_fn)(this, trace, false, NULL);
}

bool c_base_weapon::can_perform_backstab_against_target(c_base_player* player)
{
    static auto can_perform_backstab_against_target_fn =
        g_modules.get("client.dll").get_sig("55 8B EC 51 53 56 8B 75 08 8B D9 85").m_ptr;
    return reinterpret_cast<bool(__thiscall*)(void*, c_base_player*)>(can_perform_backstab_against_target_fn)(this,
                                                                                                              player);
}

const char* c_base_weapon::get_print_name()
{
    return g_utils.get_virtual_function<const char*(__thiscall*)(void*)>(this, 333)(this);
}

const char* c_base_weapon::get_localized_name()
{
    return g_interfaces.m_localize->FindAsUTF8(this->get_print_name());
}

bool c_tf_player_shared::in_cond(e_tf_cond cond)
{
    static auto in_cond =
        g_modules.get("client.dll").get_sig("55 8B EC 83 EC 08 56 57 8B 7D 08 8B F1 83 FF 20").as<in_cond_fn>();

    if (!in_cond)
        return false;

    return in_cond(this, cond);
}

float c_func_conveyor::conveyor_speed()
{
    return this->get<float>(g_netvars.m_offsets.m_fl_conveyor_speed);
}
