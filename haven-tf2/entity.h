#pragma once

// would rather not use macros.
enum e_life_state
{
    life_alive,
    life_dying,
    life_dead,
    life_respawnable,
    life_discardbody
};

class CBoneCacheHandler
{
public:
    float m_timeValid;
    int m_boneMask;
    unsigned int m_size;
    unsigned short m_cachedBoneCount;
    unsigned short m_matrixOffset;
    unsigned short m_cachedToStudioOffset;
    unsigned short m_boneOutOffset;
    matrix_3x4* BoneArray()
    {
        return (matrix_3x4*)((char*)(this + 1) + m_matrixOffset);
    }
    short* CachedToStudio()
    {
        return (short*)((char*)(this + 1) + m_cachedToStudioOffset);
    }
    void UpdateBones(matrix_3x4* pBoneToWorld, int numbones, float curtime)
    {
        const matrix_3x4* pBones = BoneArray();
        const short* pCachedToStudio = CachedToStudio();

        for (int i = 0; i < m_cachedBoneCount; i++)
        {
            const int index = pCachedToStudio[i];
            memcpy(pBoneToWorld[index].m_mat, pBones[i].m_mat, sizeof(matrix_3x4));
        }
        m_timeValid = curtime;
    }
};

class bone_accessor
{
public:
    const c_base_entity* m_pAnimating;

    matrix_3x4* m_pBones;

    int m_ReadableBones; // Which bones can be read.
    int m_WritableBones; // Which bones can be written.
};

class c_base_entity : public i_client_entity
{
    typedef bool(__thiscall* compute_hitbox_surrounding_box_fn)(void*, vector*, vector*);

    enum e_indexes
    {
        GET_HEALTH = 106,
        GET_MAX_HEALTH = 107
    };

public:
    __VFUNC(get_health(), e_indexes::GET_HEALTH, int(__thiscall*)(void*));
    __VFUNC(get_max_health(), e_indexes::GET_MAX_HEALTH, int(__thiscall*)(void*));

    template <typename T> T* as()
    {
        return reinterpret_cast<T*>(this);
    }

    // todo: maybe macro this?
    // though some people might complain about macros.
    template <typename T> T& get(uintptr_t offset)
    {
        return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset);
    }

    vector& m_vec_origin();
    vector& m_ang_rot();
    vector eye_pos();
    vector& m_view_offset();
    float sim_time();
    float spawn_time();
    int collision_group()
    {
        return get<int>(896);
    }
    bool should_collide(int group, int mask)
    {
        return g_utils.get_virtual_function<bool(__thiscall*)(void*, int, int)>(this, 145)(this, group, mask);
    }
    vector world_space_center();
    c_base_entity* get_owner();
    c_base_handle owner_handle();
    int& m_i_team_num();
    vector get_abs_velocity();
    box_t get_bounding_box();
    bool compute_hitbox_surrounding_box(vector* world_mins, vector* world_maxs);
    vector& mins();
    vector& maxs();
};
class c_func_conveyor : c_base_entity     {
public:
    float conveyor_speed();
};

enum EWeaponSlots : int
{
    SLOT_PRIMARY,
    SLOT_SECONDARY,
    SLOT_MELEE
};

class c_base_player;

class c_base_weapon : public c_base_entity
{
public:
    int& m_clip1();
    int& m_clip2();
    float charge_time();
    float pipe_charge_time();
    float& m_next_primary_attack();
    float& m_next_secondary_attack();
    bool CanBackStab();
    short& item_index();
    float& get_charge_damage();
    int GetSlot();
    float rot_speed();
    float GetSwingRange();
    bool do_swing_trace_internal(trace_t& trace);
    bool can_perform_backstab_against_target(c_base_player* player);
    const char* get_print_name();
    const char* get_localized_name();
};

enum ETFHitboxes : int
{
    HITBOX_HEAD,
    HITBOX_NECK,
    HITBOX_LOWER_NECK,
    HITBOX_PELVIS,
    HITBOX_BODY,
    HITBOX_THORAX,
    HITBOX_CHEST,
    HITBOX_UPPER_CHEST,
    HITBOX_RIGHT_THIGH,
    HITBOX_LEFT_THIGH,
    HITBOX_RIGHT_CALF,
    HITBOX_LEFT_CALF,
    HITBOX_RIGHT_FOOT,
    HITBOX_LEFT_FOOT,
    HITBOX_RIGHT_HAND,
    HITBOX_LEFT_HAND,
    HITBOX_RIGHT_UPPER_ARM,
    HITBOX_RIGHT_FOREARM,
    HITBOX_MAX
};

enum e_tf_cond
{
    TF_COND_INVALID = -1,
    TF_COND_AIMING = 0, // Sniper aiming, Heavy minigun.
    TF_COND_ZOOMED,
    TF_COND_DISGUISING,
    TF_COND_DISGUISED,
    TF_COND_STEALTHED, // Spy specific
    TF_COND_INVULNERABLE,
    TF_COND_TELEPORTED,
    TF_COND_TAUNTING,
    TF_COND_INVULNERABLE_WEARINGOFF,
    TF_COND_STEALTHED_BLINK,
    TF_COND_SELECTED_TO_TELEPORT,
    TF_COND_CRITBOOSTED, // DO NOT RE-USE THIS -- THIS IS FOR KRITZKRIEG AND REVENGE CRITS ONLY
    TF_COND_TMPDAMAGEBONUS,
    TF_COND_FEIGN_DEATH,
    TF_COND_PHASE,
    TF_COND_STUNNED, // Any type of stun. Check iStunFlags for more info.
    TF_COND_OFFENSEBUFF,
    TF_COND_SHIELD_CHARGE,
    TF_COND_DEMO_BUFF,
    TF_COND_ENERGY_BUFF,
    TF_COND_RADIUSHEAL,
    TF_COND_HEALTH_BUFF,
    TF_COND_BURNING,
    TF_COND_HEALTH_OVERHEALED,
    TF_COND_URINE,
    TF_COND_BLEEDING,
    TF_COND_DEFENSEBUFF, // 35% defense! No crit damage.
    TF_COND_MAD_MILK,
    TF_COND_MEGAHEAL,
    TF_COND_REGENONDAMAGEBUFF,
    TF_COND_MARKEDFORDEATH,
    TF_COND_NOHEALINGDAMAGEBUFF,
    TF_COND_SPEED_BOOST,         // = 32
    TF_COND_CRITBOOSTED_PUMPKIN, // Brandon hates bits
    TF_COND_CRITBOOSTED_USER_BUFF,
    TF_COND_CRITBOOSTED_DEMO_CHARGE,
    TF_COND_SODAPOPPER_HYPE,
    TF_COND_CRITBOOSTED_FIRST_BLOOD, // arena mode first blood
    TF_COND_CRITBOOSTED_BONUS_TIME,
    TF_COND_CRITBOOSTED_CTF_CAPTURE,
    TF_COND_CRITBOOSTED_ON_KILL, // =40. KGB, etc.
    TF_COND_CANNOT_SWITCH_FROM_MELEE,
    TF_COND_DEFENSEBUFF_NO_CRIT_BLOCK, // 35% defense! Still damaged by crits.
    TF_COND_REPROGRAMMED,              // Bots only
    TF_COND_CRITBOOSTED_RAGE_BUFF,
    TF_COND_DEFENSEBUFF_HIGH,       // 75% defense! Still damaged by crits.
    TF_COND_SNIPERCHARGE_RAGE_BUFF, // Sniper Rage - Charge time speed up
    TF_COND_DISGUISE_WEARINGOFF,    // Applied for half-second post-disguise
    TF_COND_MARKEDFORDEATH_SILENT,  // Sans sound
    TF_COND_DISGUISED_AS_DISPENSER,
    TF_COND_SAPPED, // =50. Bots only
    TF_COND_INVULNERABLE_HIDE_UNLESS_DAMAGED,
    TF_COND_INVULNERABLE_USER_BUFF,
    TF_COND_HALLOWEEN_BOMB_HEAD,
    TF_COND_HALLOWEEN_THRILLER,
    TF_COND_RADIUSHEAL_ON_DAMAGE,
    TF_COND_CRITBOOSTED_CARD_EFFECT,
    TF_COND_INVULNERABLE_CARD_EFFECT,
    TF_COND_MEDIGUN_UBER_BULLET_RESIST,
    TF_COND_MEDIGUN_UBER_BLAST_RESIST,
    TF_COND_MEDIGUN_UBER_FIRE_RESIST, // =60
    TF_COND_MEDIGUN_SMALL_BULLET_RESIST,
    TF_COND_MEDIGUN_SMALL_BLAST_RESIST,
    TF_COND_MEDIGUN_SMALL_FIRE_RESIST,
    TF_COND_STEALTHED_USER_BUFF, // Any class can have this
    TF_COND_MEDIGUN_DEBUFF,
    TF_COND_STEALTHED_USER_BUFF_FADING,
    TF_COND_BULLET_IMMUNE,
    TF_COND_BLAST_IMMUNE,
    TF_COND_FIRE_IMMUNE,
    TF_COND_PREVENT_DEATH,          // =70
    TF_COND_MVM_BOT_STUN_RADIOWAVE, // Bots only
    TF_COND_HALLOWEEN_SPEED_BOOST,
    TF_COND_HALLOWEEN_QUICK_HEAL,
    TF_COND_HALLOWEEN_GIANT,
    TF_COND_HALLOWEEN_TINY,
    TF_COND_HALLOWEEN_IN_HELL,
    TF_COND_HALLOWEEN_GHOST_MODE, // =77
    TF_COND_MINICRITBOOSTED_ON_KILL,
    TF_COND_OBSCURED_SMOKE,
    TF_COND_PARACHUTE_DEPLOYED, // =80
    TF_COND_BLASTJUMPING,
    TF_COND_HALLOWEEN_KART,
    TF_COND_HALLOWEEN_KART_DASH,
    TF_COND_BALLOON_HEAD,        // =84 larger head, lower-gravity-feeling jumps
    TF_COND_MELEE_ONLY,          // =85 melee only
    TF_COND_SWIMMING_CURSE,      // player movement become swimming movement
    TF_COND_FREEZE_INPUT,        // freezes player input
    TF_COND_HALLOWEEN_KART_CAGE, // attach cage model to player while in kart
    TF_COND_DONOTUSE_0,
    TF_COND_RUNE_STRENGTH,
    TF_COND_RUNE_HASTE,
    TF_COND_RUNE_REGEN,
    TF_COND_RUNE_RESIST,
    TF_COND_RUNE_VAMPIRE,
    TF_COND_RUNE_REFLECT,
    TF_COND_RUNE_PRECISION,
    TF_COND_RUNE_AGILITY,
    TF_COND_GRAPPLINGHOOK,
    TF_COND_GRAPPLINGHOOK_SAFEFALL,
    TF_COND_GRAPPLINGHOOK_LATCHED,
    TF_COND_GRAPPLINGHOOK_BLEEDING,
    TF_COND_AFTERBURN_IMMUNE,
    TF_COND_RUNE_KNOCKOUT,
    TF_COND_RUNE_IMBALANCE,
    TF_COND_CRITBOOSTED_RUNE_TEMP,
    TF_COND_PASSTIME_INTERCEPTION,
    TF_COND_SWIMMING_NO_EFFECTS, // =107_DNOC_FT
    TF_COND_PURGATORY,
    TF_COND_RUNE_KING,
    TF_COND_RUNE_PLAGUE,
    TF_COND_RUNE_SUPERNOVA,
    TF_COND_PLAGUE,
    TF_COND_KING_BUFFED,
    TF_COND_TEAM_GLOWS, // used to show team glows to living players
    TF_COND_KNOCKED_INTO_AIR,
    TF_COND_COMPETITIVE_WINNER,
    TF_COND_COMPETITIVE_LOSER,
    TF_COND_HEALING_DEBUFF,
    TF_COND_PASSTIME_PENALTY_DEBUFF, // when carrying the ball without any teammates nearby
    TF_COND_GRAPPLED_TO_PLAYER,
    TF_COND_GRAPPLED_BY_PLAYER
};
class c_tf_player_shared
{
    typedef bool(__thiscall* in_cond_fn)(void*, int);

public:
    bool in_cond(e_tf_cond cond);
};

enum move_type_t
{
    MOVETYPE_NONE = 0,   // never moves
    MOVETYPE_ISOMETRIC,  // For players -- in TF2 commander view, etc.
    MOVETYPE_WALK,       // Player only - moving on the ground
    MOVETYPE_STEP,       // gravity, special edge handling -- monsters use this
    MOVETYPE_FLY,        // No gravity, but still collides with stuff
    MOVETYPE_FLYGRAVITY, // flies through the air + is affected by gravity
    MOVETYPE_VPHYSICS,   // uses VPHYSICS for simulation
    MOVETYPE_PUSH,       // no clip to world, push and crush
    MOVETYPE_NOCLIP,     // No gravity, no collisions, still do velocity/avelocity
    MOVETYPE_LADDER,     // Used by players only when going onto a ladder
    MOVETYPE_OBSERVER,   // Observer movement, depends on player's observer mode
    MOVETYPE_CUSTOM,     // Allows the entity to describe its own physics

    // should always be defined as the last item in the list
    MOVETYPE_LAST = MOVETYPE_CUSTOM,

    MOVETYPE_MAX_BITS = 4
};

class c_base_player : public c_base_entity
{
    typedef bool(__thiscall* in_cond_fn)(void*, int);

public:
    c_tf_player_shared* m_shared();
    bool in_cond(e_tf_cond cond)
    {
        return m_shared()->in_cond(cond);
    }
    void force_taunt_cam(const int nTo);
    void SetCurrentCmd(usercmd_t* pCmd);
    int& m_i_health();
    int& m_tick_base();
    float& m_next_attack();
    char& m_life_state();
    vector m_velocity();
    float m_max_speed();
    vector m_eye_angles();
    void set_abs_origin(vector origin);
    void set_abs_angles(vector angles);
    vector calculate_abs_velocity();
    CBoneCacheHandler* bone_cache();
    int& m_class();
    int& flags();

    c_base_handle active_weapon_handle();
    c_base_weapon* get_active_weapon();
    c_base_entity* calculate_ground();
    c_base_entity* get_ground();
    c_base_handle ground_handle();
    int& hitbox_set();
    vector get_hitbox_pos(int iHitbox, matrix_3x4* matrix = nullptr, vector mins = vector(0.5f, 0.5f, 0.5f),
                          vector max = vector(0.5f, 0.5f, 0.5f));

    move_type_t move_type();

    bool is_alive();
    bool is_valid(c_base_player* local, bool team_check = true, bool dormant_check = true, bool alive_check = true);
};
