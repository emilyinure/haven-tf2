#include "sdk.h"

void c_netvars::iterate_props(const unsigned int name, recv_table* table, int offset)
{
    // bad table.
    if (!table)
        return;
    recv_table* child;

    // go through all our props.
    for (int i = 0; i < table->m_num_of_props; i++)
    {
        const auto& prop = table->m_props[i];
        child = prop.m_data_table;
        if (prop.m_data_table && prop.m_num_of_elements)
            iterate_props(name, prop.m_data_table, prop.m_offset + offset);
        this->m_netvars.emplace_back(name, g_utils.fnv_hash(prop.m_var_name), prop.m_offset + offset);
    }
}

void c_netvars::init()
{
    auto cc = g_interfaces.m_client->get_all_classes();

    while (cc)
    {
        const auto table = cc->m_recv_table;


        // bad table.
        if (!table)
            continue;
        const auto hashed_table = g_utils.fnv_hash(cc->m_network_name);
        if (hashed_table == g_utils.fnv_hash("CFuncConveyor"))
            printf_s("[>] found %i\n", cc->m_class_id);

        // go through all our props.
        for (int i = 0; i < table->m_num_of_props; i++)
        {
            auto& prop = table->m_props[i];

            this->m_netvars.emplace_back(g_utils.fnv_hash(table->m_net_table_name), g_utils.fnv_hash(prop.m_var_name),
                                         prop.m_offset);
        }

        iterate_props(g_utils.fnv_hash(table->m_net_table_name), table);

        cc = cc->m_next;
    }

    g_utils.m_debug.set_console_color(e_console_colors::console_color_cyan);
    printf_s("[>] populated %i netvars\n", this->m_netvars.size());

    // get our offsets.
    this->get_offsets();
}

netvar_t c_netvars::get(const char* table, const char* var)
{
    const auto hashed_table = g_utils.fnv_hash(table);
    const auto hashed_var = g_utils.fnv_hash(var);

    for (const auto& netvar : this->m_netvars)
    {
        // not the table we want.
        if (netvar.m_hashed_table != hashed_table)
            continue;

        // not the var we want.
        if (netvar.m_hashed_var != hashed_var)
            continue;

        return netvar;
    }

    return {};
}

void c_netvars::get_offsets()
{
    this->m_offsets.dt_base_entity.m_vec_origin = this->get("DT_BaseEntity", "m_vecOrigin").m_offset;
    this->m_offsets.dt_base_entity.m_vec_view_offset = this->get("DT_TFPlayer", "m_vecViewOffset[0]").m_offset;
    this->m_offsets.dt_base_entity.m_max_speed = this->get("DT_TFPlayer", "m_flMaxspeed").m_offset;
    this->m_offsets.dt_base_entity.m_i_team_num = this->get("DT_BaseEntity", "m_iTeamNum").m_offset;
    this->m_offsets.dt_base_entity.m_owner_handle = this->get("DT_BaseEntity", "m_hOwnerEntity").m_offset;
    this->m_offsets.dt_base_entity.m_simulation_time = this->get("DT_BaseEntity", "m_flSimulationTime").m_offset;
    this->m_offsets.dt_base_entity.m_simulation_time = this->get("DT_BaseEntity", "m_flSimulationTime").m_offset;
    this->m_offsets.dt_base_entity.m_move_type = this->get("DT_BaseEntity", "movetype").m_offset;
    this->m_offsets.dt_base_entity.m_ang_rot = this->get("DT_BaseEntity", "m_angRotation").m_offset;

    this->m_offsets.dt_base_entity.m_mins = this->get("DT_BaseEntity", "m_vecMins").m_offset;
    this->m_offsets.dt_base_entity.m_maxs = this->get("DT_BaseEntity", "m_vecMaxs").m_offset;

    this->m_offsets.dt_base_weapon.m_charge_begin_time =
        this->get("DT_WeaponCompoundBow", "m_flChargeBeginTime").m_offset;
    this->m_offsets.dt_base_weapon.m_pipe_charge_begin_time =
        this->get("DT_WeaponPipebombLauncher", "m_flChargeBeginTime").m_offset;
    this->m_offsets.dt_base_weapon.m_next_primary_attack =
        this->get("DT_BaseCombatWeapon", "m_flNextPrimaryAttack").m_offset;
    this->m_offsets.dt_base_weapon.m_next_secondary_attack =
        this->get("DT_BaseCombatWeapon", "m_flNextSecondaryAttack").m_offset;
    this->m_offsets.dt_base_weapon.m_item_definition_index =
        this->get("DT_EconEntity", "m_iItemDefinitionIndex").m_offset;
    this->m_offsets.dt_base_weapon.m_charge_damage = this->get("DT_BaseCombatWeapon", "m_flChargedDamage").m_offset;
    this->m_offsets.dt_base_weapon.m_rot_speed = this->get("DT_BaseCombatWeapon", "m_flRotationSpeed").m_offset;

    this->m_offsets.dt_base_weapon.m_clip1 = this->get("DT_BaseCombatWeapon", "m_iClip1").m_offset;
    this->m_offsets.dt_base_weapon.m_clip2 = this->get("DT_BaseCombatWeapon", "m_iClip2").m_offset;

    this->m_offsets.dt_base_animating.m_hitbox_set = this->get("DT_BaseAnimating", "m_nHitboxSet").m_offset;

    this->m_offsets.dt_player_resource.m_max_health = this->get("DT_TFPlayerResource", "m_iMaxBuffedHealth").m_offset;
    this->m_offsets.dt_base_player.m_shared = this->get("DT_TFPlayer", "m_Shared").m_offset;
    this->m_offsets.dt_base_player.m_active_weapon = this->get("DT_BaseCombatCharacter", "m_hActiveWeapon").m_offset;
    this->m_offsets.dt_base_player.m_eye_angles = this->get("DT_TFPlayer", "m_angEyeAngles[0]").m_offset;
    this->m_offsets.dt_base_player.m_class = this->get("DT_TFPlayer", "m_iClass").m_offset;
    this->m_offsets.dt_base_player.m_taunt = this->get("DT_TFPlayer", "m_nForceTauntCam").m_offset;
    this->m_offsets.dt_base_player.m_tick_base = this->get("DT_BasePlayer", "m_nTickBase").m_offset;
    this->m_offsets.dt_base_player.m_next_attack = this->get("DT_BaseCombatCharacter", "m_flNextAttack").m_offset;
    this->m_offsets.dt_base_player.m_max_speed = this->get("DT_BasePlayer", "m_flMaxspeed").m_offset;
    this->m_offsets.dt_base_player.m_velocity = this->get("DT_TFPlayer", "m_vecVelocity[0]").m_offset;
    this->m_offsets.dt_base_player.m_i_health = this->get("DT_BasePlayer", "m_iHealth").m_offset;
    this->m_offsets.dt_base_player.m_life_state = this->get("DT_BasePlayer", "m_lifeState").m_offset;
    this->m_offsets.dt_base_player.m_ground_handle = this->get("DT_TFPlayer", "m_hGroundEntity").m_offset;
    this->m_offsets.dt_base_player.m_flags = this->get("DT_BasePlayer", "m_fFlags").m_offset;
    this->m_offsets.m_fl_conveyor_speed = this->get("DT_FuncConveyor", "m_flConveyorSpeed").m_offset;
}
