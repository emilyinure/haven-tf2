#pragma once

// contents flags are seperate bits
// a given brush can contribute multiple content bits
// multiple brushes can be in a single leaf

// these definitions also need to be in q_shared.h!

// lower bits are stronger, and will eat weaker brushes completely
#define CONTENTS_EMPTY 0 // No contents

#define CONTENTS_SOLID 0x1  // an eye is never valid in a solid
#define CONTENTS_WINDOW 0x2 // translucent, but not watery (glass)
#define CONTENTS_AUX 0x4
#define CONTENTS_GRATE 0x8 // alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't
#define CONTENTS_SLIME 0x10
#define CONTENTS_WATER 0x20
#define CONTENTS_BLOCKLOS 0x40 // block AI line of sight
#define CONTENTS_OPAQUE 0x80   // things that cannot be seen through (may be non-solid though)
#define LAST_VISIBLE_CONTENTS 0x80

#define ALL_VISIBLE_CONTENTS (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS - 1))

#define CONTENTS_TESTFOGVOLUME 0x100
#define CONTENTS_UNUSED 0x200

// unused
// NOTE: If it's visible, grab from the top + update LAST_VISIBLE_CONTENTS
// if not visible, then grab from the bottom.
#define CONTENTS_UNUSED6 0x400

#define CONTENTS_TEAM1 0x800  // per team contents used to differentiate collisions
#define CONTENTS_TEAM2 0x1000 // between players and objects on different teams

// ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW
#define CONTENTS_IGNORE_NODRAW_OPAQUE 0x2000

// hits entities which are MOVETYPE_PUSH (doors, plats, etc.)
#define CONTENTS_MOVEABLE 0x4000

// remaining contents are non-visible, and don't eat brushes
#define CONTENTS_AREAPORTAL 0x8000

#define CONTENTS_PLAYERCLIP 0x10000
#define CONTENTS_MONSTERCLIP 0x20000

// currents can be added to any other contents, and may be mixed
#define CONTENTS_CURRENT_0 0x40000
#define CONTENTS_CURRENT_90 0x80000
#define CONTENTS_CURRENT_180 0x100000
#define CONTENTS_CURRENT_270 0x200000
#define CONTENTS_CURRENT_UP 0x400000
#define CONTENTS_CURRENT_DOWN 0x800000

#define CONTENTS_ORIGIN 0x1000000 // removed before bsping an entity

#define CONTENTS_MONSTER 0x2000000 // should never be on a brush, only in game
#define CONTENTS_DEBRIS 0x4000000
#define CONTENTS_DETAIL 0x8000000       // brushes to be added after vis leafs
#define CONTENTS_TRANSLUCENT 0x10000000 // auto set if any surface has trans
#define CONTENTS_LADDER 0x20000000
#define CONTENTS_HITBOX 0x40000000 // use accurate hitboxes on trace

// NOTE: These are stored in a short in the engine now.  Don't use more than 16 bits
#define SURF_LIGHT 0x0001 // value will hold the light strength
#define SURF_SKY2D 0x0002 // don't draw, indicates we should skylight + draw 2d sky but not draw the 3D skybox
#define SURF_SKY 0x0004   // don't draw, but add to skybox
#define SURF_WARP 0x0008  // turbulent water warp
#define SURF_TRANS 0x0010
#define SURF_NOPORTAL 0x0020 // the surface can not have a portal placed on it
#define SURF_TRIGGER                                                                                                   \
    0x0040 // FIXME: This is an xbox hack to work around elimination of trigger surfaces, which breaks occluders
#define SURF_NODRAW 0x0080 // don't bother referencing the texture

#define SURF_HINT 0x0100 // make a primary bsp splitter

#define SURF_SKIP 0x0200      // completely ignore, allowing non-closed brushes
#define SURF_NOLIGHT 0x0400   // Don't calculate light
#define SURF_BUMPLIGHT 0x0800 // calculate three lightmaps for the surface for bumpmapping
#define SURF_NOSHADOWS 0x1000 // Don't receive shadows
#define SURF_NODECALS 0x2000  // Don't receive decals
#define SURF_NOCHOP 0x4000    // Don't subdivide patches on this surface
#define SURF_HITBOX 0x8000    // surface is part of a hitbox

// -----------------------------------------------------
// spatial content masks - used for spatial queries (traceline,etc.)
// -----------------------------------------------------
#define MASK_ALL (0xFFFFFFFF)
// everything that is normally solid
#define MASK_SOLID (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE)
// everything that blocks player movement
#define MASK_PLAYERSOLID                                                                                               \
    (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE)
// blocks npc movement
#define MASK_NPCSOLID                                                                                                  \
    (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER | CONTENTS_GRATE)
// water physics in these contents
#define MASK_WATER (CONTENTS_WATER | CONTENTS_MOVEABLE | CONTENTS_SLIME)
// everything that blocks lighting
#define MASK_OPAQUE (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_OPAQUE)
// everything that blocks lighting, but with monsters added.
#define MASK_OPAQUE_AND_NPCS (MASK_OPAQUE | CONTENTS_MONSTER)
// everything that blocks line of sight for AI
#define MASK_BLOCKLOS (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_BLOCKLOS)
// everything that blocks line of sight for AI plus NPCs
#define MASK_BLOCKLOS_AND_NPCS (MASK_BLOCKLOS | CONTENTS_MONSTER)
// everything that blocks line of sight for players
#define MASK_VISIBLE (MASK_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE)
// everything that blocks line of sight for players, but with monsters added.
#define MASK_VISIBLE_AND_NPCS (MASK_OPAQUE_AND_NPCS | CONTENTS_IGNORE_NODRAW_OPAQUE)
// bullets see these as solid
#define MASK_SHOT                                                                                                      \
    (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_HITBOX)
// non-raycasted weapons see this as solid (includes grates)
#define MASK_SHOT_HULL                                                                                                 \
    (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_GRATE)
// hits solids (not grates) and passes through everything else
#define MASK_SHOT_PORTAL (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTER)
// everything normally solid, except monsters (world+brush only)
#define MASK_SOLID_BRUSHONLY (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_GRATE)
// everything normally solid for player movement, except monsters (world+brush only)
#define MASK_PLAYERSOLID_BRUSHONLY                                                                                     \
    (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_PLAYERCLIP | CONTENTS_GRATE)
// everything normally solid for npc movement, except monsters (world+brush only)
#define MASK_NPCSOLID_BRUSHONLY                                                                                        \
    (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE)
// just the world, used for route rebuilding
#define MASK_NPCWORLDSTATIC (CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MONSTERCLIP | CONTENTS_GRATE)
// These are things that can split areaportals
#define MASK_SPLITAREAPORTAL (CONTENTS_WATER | CONTENTS_SLIME)

// UNDONE: This is untested, any moving water
#define MASK_CURRENT                                                                                                   \
    (CONTENTS_CURRENT_0 | CONTENTS_CURRENT_90 | CONTENTS_CURRENT_180 | CONTENTS_CURRENT_270 | CONTENTS_CURRENT_UP |    \
     CONTENTS_CURRENT_DOWN)

// everything that blocks corpse movement
// UNDONE: Not used yet / may be deleted
#define MASK_DEADSOLID (CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_GRATE)

class c_base_entity;

struct csurface_t
{
    const char* name;
    short surfaceProps;
    unsigned short flags;
};

enum class TraceType_t
{
    TRACE_EVERYTHING = 0,
    TRACE_WORLD_ONLY,              // NOTE: This does *not* test static props!!!
    TRACE_ENTITIES_ONLY,           // NOTE: This version will *not* test static props
    TRACE_EVERYTHING_FILTER_PROPS, // NOTE: This version will pass the IHandleEntity for props through the filter,
                                   // unlike all other filters
};

class i_trace_filter
{
public:
    virtual bool should_hit_entity(c_base_entity* pEntity, int contentsMask) = 0;
    virtual TraceType_t get_type() const = 0;
};

class trace_filter : public i_trace_filter
{
public:
    virtual bool should_hit_entity(c_base_entity* pEntityHandle, int contentsMask)
    {
        return (pEntityHandle != skip);
    }

    virtual TraceType_t get_type() const
    {
        return TraceType_t::TRACE_EVERYTHING;
    }

    void* skip = nullptr;
};

class CTraceFilter : public i_trace_filter
{
public:
    virtual TraceType_t get_type() const
    {
        return TraceType_t::TRACE_EVERYTHING;
    }
};

class CTraceFilterSimple : public CTraceFilter
{
public:
    typedef bool (*ShouldHitFunc_t)(c_base_entity* pHandleEntity, int contentsMask);
    CTraceFilterSimple(c_base_entity* passentity, int collisionGroup, ShouldHitFunc_t pExtraShouldHitCheckFn = NULL);
    virtual bool should_hit_entity(c_base_entity* pHandleEntity, int contentsMask);
    virtual void SetPassEntity(c_base_entity* pPassEntity)
    {
        m_pPassEnt = pPassEntity;
    }
    virtual void SetCollisionGroup(int iCollisionGroup)
    {
        m_collisionGroup = iCollisionGroup;
    }

    const void* GetPassEntity(void)
    {
        return m_pPassEnt;
    }

private:
    c_base_entity* m_pPassEnt;
    int m_collisionGroup;
    ShouldHitFunc_t m_pExtraShouldHitCheckFunction;
};

//class CTraceFilterIgnoreFriendlyCombatItems : public CTraceFilterSimple
//{
//public:
//    DECLARE_CLASS(CTraceFilterIgnoreFriendlyCombatItems, CTraceFilterSimple);
//
//    CTraceFilterIgnoreFriendlyCombatItems(const c_base_entity* passentity, int collisionGroup, int iIgnoreTeam,
//                                          bool bIsProjectile = false)
//        : CTraceFilterSimple(passentity, collisionGroup), m_iIgnoreTeam(iIgnoreTeam)
//    {
//        m_bCallerIsProjectile = bIsProjectile;
//    }
//
//    virtual bool ShouldHitEntity(c_base_entity* pServerEntity, int contentsMask)
//    {
//        if (pServerEntity->())
//        {
//            if (pServerEntity->GetTeamNumber() == m_iIgnoreTeam)
//                return false;
//
//            // If source is a enemy projectile, be explicit, otherwise we fail a "IsTransparent" test downstream
//            if (m_bCallerIsProjectile)
//                return true;
//        }
//
//        return BaseClass::ShouldHitEntity(pServerEntity, contentsMask);
//    }
//
//    int m_iIgnoreTeam;
//    bool m_bCallerIsProjectile;
//};

class CTraceFilterIgnoreTeammates : public CTraceFilterSimple
{
public:
    CTraceFilterIgnoreTeammates(c_base_entity* passentity, int collisionGroup, int iIgnoreTeam)
        : CTraceFilterSimple(passentity, collisionGroup), m_iIgnoreTeam(iIgnoreTeam)
    {
    }

    virtual bool should_hit_entity(c_base_entity* pServerEntity, int contentsMask);

    int m_iIgnoreTeam;
};

class CTraceFilterIgnorePlayers : public CTraceFilterSimple
{
public:
    CTraceFilterIgnorePlayers(c_base_entity* passentity, int collisionGroup)
        : CTraceFilterSimple(passentity, collisionGroup)
    {
    }

    virtual bool should_hit_entity(c_base_entity* pServerEntity, int contentsMask);
};

class trace_filter_one_entity : public i_trace_filter
{
public:
    virtual bool should_hit_entity(c_base_entity* pEntityHandle, int contentsMask)
    {
        return (pEntityHandle == pEntity);
    }

    virtual TraceType_t GetTraceType() const
    {
        return TraceType_t::TRACE_EVERYTHING;
    }

    void* pEntity = nullptr;
};

class trace_filter_one_entity2 : public i_trace_filter
{
public:
    virtual bool should_hit_entity(c_base_entity* pEntityHandle, int contentsMask)
    {
        return (pEntityHandle == pEntity);
    }

    virtual TraceType_t get_type() const
    {
        return TraceType_t::TRACE_ENTITIES_ONLY;
    }

    void* pEntity = nullptr;
};

class trace_filter_skip_two_entities : public i_trace_filter
{
public:
    trace_filter_skip_two_entities(c_base_entity* pPassEnt1, c_base_entity* pPassEnt2)
    {
        passentity1 = pPassEnt1;
        passentity2 = pPassEnt2;
    }

    virtual bool should_hit_entity(c_base_entity* pEntityHandle, int contentsMask)
    {
        return !(pEntityHandle == passentity1 || pEntityHandle == passentity2);
    }

    virtual TraceType_t get_type() const
    {
        return TraceType_t::TRACE_EVERYTHING;
    }

    void* passentity1 = nullptr;
    void* passentity2 = nullptr;
};

class trace_filter_skip_one_entity : public i_trace_filter
{
public:
    trace_filter_skip_one_entity(void* pPassEnt1)
    {
        passentity1 = pPassEnt1;
    }

    virtual bool should_hit_entity(c_base_entity* pEntityHandle, int contentsMask)
    {
        return !(pEntityHandle == passentity1);
    }

    virtual TraceType_t get_type() const
    {
        return TraceType_t::TRACE_EVERYTHING;
    }

    void* passentity1 = nullptr;
};

class trace_entity : public i_trace_filter
{
public:
    virtual bool should_hit_entity(c_base_entity* pEntityHandle, int contentsMask)
    {
        return !(pEntityHandle == pSkip1);
    }

    virtual TraceType_t get_type() const
    {
        return TraceType_t::TRACE_ENTITIES_ONLY;
    }

    void* pSkip1 = nullptr;
};

class trace_world_only : public i_trace_filter
{
public:
    virtual bool should_hit_entity(c_base_entity* pEntityHandle, int contentsMask)
    {
        return false;
    }

    virtual TraceType_t get_type() const
    {
        return TraceType_t::TRACE_EVERYTHING;
    }

    void* pSkip1 = nullptr;
};
struct cplane_t
{
    vector normal;
    float m_dist;
    BYTE m_type;
    BYTE m_sign_bits;
    BYTE m_pad[2];
};
struct trace_t
{
    vector m_start;
    vector m_end;
    cplane_t m_plane;
    float m_fraction;
    int m_contents;
    unsigned short m_disp_flags;
    bool m_allsolid;
    bool m_start_solid;
    float m_fraction_left_solid;
    csurface_t m_surface;
    int m_hitgroup;
    short m_physics_bone;
    c_base_entity* m_entity;
    int m_hitbox;

    bool did_hit() const
    {
        return m_fraction < 1.f || m_allsolid || m_start_solid;
    }
};
class i_engine_trace
{
public: // no need to use vtables here.
    void trace_ray(const ray_t& ray, unsigned int mask, i_trace_filter* filter, trace_t* trace)
    {
        typedef void(__thiscall * TraceRayFn)(void*, const ray_t&, unsigned int, i_trace_filter*, trace_t*);
        return g_utils.get_virtual_function<TraceRayFn>(this, 4)(this, ray, mask, filter, trace);
    }
};
