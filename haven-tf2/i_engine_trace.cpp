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

typedef bool(* ShouldHitEntity)(CTraceFilterSimple*, c_base_entity*, int);
ShouldHitEntity oShouldHitEntity = 0;
bool CTraceFilterSimple::should_hit_entity(c_base_entity* pHandleEntity, int contentsMask)
{
    if (!oShouldHitEntity)
        oShouldHitEntity =
            g_modules.get("client.dll")
                .get_sig("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 20 48 8B 02 48 8B E9") // first index of CTraceFilterSimple
                .as<ShouldHitEntity>();
    return oShouldHitEntity(this, pHandleEntity, contentsMask);
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
