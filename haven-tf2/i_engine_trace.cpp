#include "i_engine_trace.h"
#include "sdk.h"

bool StandardFilterRules(void* pHandleEntity, int fContentsMask)
{
    c_base_entity* ent = (c_base_entity*)(pHandleEntity);

    // Static prop case...
    if (!ent)
        return true;
    const auto collide = ent->get_collideable();
    if (!collide)
        return true;

    solid_type_t solid = collide->get_solid();
    const model_t* pModel = collide->get_collision_model();

    // if ((modelinfo->GetModelType(pModel) != mod_brush) || (solid != SOLID_BSP && solid != SOLID_VPHYSICS))
    //{
    //	if ((fContentsMask & CONTENTS_MONSTER) == 0)
    //		return false;
    // }

    // This code is used to cull out tests against see-thru entities
    // if (!(fContentsMask & CONTENTS_WINDOW) && collide->IsTransparent())
    //	return false;

    // FIXME: this is to skip BSP models that are entities that can be
    // potentially moved/deleted, similar to a monster but doors don't seem to
    // be flagged as monsters
    // FIXME: the FL_WORLDBRUSH looked promising, but it needs to be set on
    // everything that's actually a worldbrush and it currently isn't
    // if (!(fContentsMask & CONTENTS_MOVEABLE) && (collid() == MOVETYPE_PUSH))// !(touch->flags & FL_WORLDBRUSH) )
    //	return false;
    //
    return true;
}

bool PassServerEntityFilter(const void* pTouch, const void* pPass)
{
    if (!pPass)
        return true;

    if (pTouch == pPass)
        return false;

    const auto* pEntTouch = (c_base_entity*)(pTouch);
    const auto* pEntPass = (c_base_entity*)(pPass);
    if (!pEntTouch || !pEntPass)
        return true;

    // don't clip against own missiles
    // if (pEntTouch->GetOwnerEntity() == pEntPass)
    //	return false;
    //
    //// don't clip against owner
    // if (pEntPass->GetOwnerEntity() == pEntTouch)
    //	return false;

    return true;
}

CTraceFilterSimple::CTraceFilterSimple(c_base_entity* passedict, int collisionGroup,
                                       ShouldHitFunc_t pExtraShouldHitFunc)
{
    m_pPassEnt = passedict;
    m_collisionGroup = collisionGroup;
    m_pExtraShouldHitCheckFunction = pExtraShouldHitFunc;
}

//-----------------------------------------------------------------------------
// The trace filter!
//-----------------------------------------------------------------------------
typedef bool(_cdecl* PassServer)(void* pTouch, void* pPass);
typedef bool(_cdecl* StandardFilter)(void* a1, int a2);
PassServer oPassServerEntityFilter = 0;
typedef bool(__stdcall* ShouldCollide)(int a1, int a2);
ShouldCollide oShouldCollide = 0;
StandardFilter oStandardFilterRules = 0;
bool CTraceFilterSimple::should_hit_entity(c_base_entity* pHandleEntity, int contentsMask)
{
    if (!oPassServerEntityFilter)
        oPassServerEntityFilter =
            g_modules.get("client.dll").get_sig("55 8B EC 56 8B 75 ? 85 F6 75 ? B0 ?").as<PassServer>();
    if (!oShouldCollide)
        oShouldCollide = g_modules.get("client.dll").get_sig("55 8B EC 8B 55 ? 56 8B 75 ? 3B F2").as<ShouldCollide>();
    if (!oStandardFilterRules)
        oStandardFilterRules = g_modules.get("client.dll")
                                   .get_sig("55 8B EC 8B 4D ? 56 8B 01 FF 50 ? 8B F0 85 F6 75 ? B0 ? 5E 5D C3")
                                   .as<StandardFilter>();
    if (!oStandardFilterRules(pHandleEntity, contentsMask))
        return false;

    if (m_pPassEnt)
    {
        if (!oPassServerEntityFilter(pHandleEntity, m_pPassEnt))
        {
            return false;
        }
    }
    "55 8B EC 56 8B 75 ? 85 F6 75 ? B0 ?"; // bool __cdecl PassServerEntityFilter(int pTouch, int pPass)
    "55 8B EC 8B 4D ? 56 8B 01 FF 50 ? 8B F0 85 F6 75 ? B0 ? 5E 5D C3"; // bool __cdecl StandardFilterRules(int a1, int
                                                                        // a2)
    // Don't test if the game code tells us we should ignore this collision...
    c_base_entity* pEntity = (c_base_entity*)(pHandleEntity);
    if (!pEntity)
        return false;
    if (!pEntity->should_collide(m_collisionGroup, contentsMask))
        return false;
    if (pEntity && !oShouldCollide(m_collisionGroup, pEntity->collision_group()))
        return false;
    if (m_pExtraShouldHitCheckFunction && (!(m_pExtraShouldHitCheckFunction(pHandleEntity, contentsMask))))
        return false;

    return true;
}

bool CTraceFilterIgnorePlayers::should_hit_entity(c_base_entity* pServerEntity, int contentsMask)
{
    if (!(pServerEntity->entindex() == 0 || pServerEntity->entindex() >= g_interfaces.m_engine->get_max_clients()))
        return false;

    return CTraceFilterSimple::should_hit_entity(pServerEntity, contentsMask);
}

bool CTraceFilterIgnoreTeammates::should_hit_entity(c_base_entity* pServerEntity, int contentsMask)
{
    if (!(pServerEntity->entindex() == 0 || pServerEntity->entindex() >= g_interfaces.m_engine->get_max_clients()))
    {
        if ((pServerEntity->m_i_team_num() == m_iIgnoreTeam))
        {
            return false;
        }
    }

    return CTraceFilterSimple::should_hit_entity(pServerEntity, contentsMask);
}
