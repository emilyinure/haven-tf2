
#include "prediction.h"
#include "sdk.h"

#define TIME_TO_TICKS(dt) ((int)(0.5f + (float)(dt) / g_interfaces.m_global_vars->m_interval_per_tick))
#define TICKS_TO_TIME(dt) (g_interfaces.m_global_vars->m_interval_per_tick * (float)(dt))

int prediction::get_tickbase(usercmd_t* pCmd, c_base_player* pLocal)
{
    static int nTick = 0;
    static usercmd_t* pLastCmd = nullptr;

    if (pCmd)
    {
        if (!pLastCmd || pLastCmd->predicted_)
            nTick = pLocal->m_tick_base();

        else
            nTick++;

        pLastCmd = pCmd;
    }

    return nTick;
}

void prediction::start()
{
    if (!g_interfaces.m_move_helper || !g_cl.m_local)
        return;

    g_interfaces.m_move_helper->SetHost(g_cl.m_local);

    int backup_tickbase = g_cl.m_local->m_tick_base();
    g_cl.m_local->SetCurrentCmd(g_cl.m_cmd);
    m_fOldCurrentTime = g_interfaces.m_global_vars->m_cur_time;
    m_fOldFrameTime = g_interfaces.m_global_vars->m_frame_time;
    m_nOldTickCount = g_interfaces.m_global_vars->m_tick_count;

    static auto update_buttons =
        g_modules.get("client.dll").get_sig("55 8B EC 8B 81 ? ? ? ? 8B D0").as<int(__thiscall*)(c_base_entity*, int)>();
    update_buttons(g_cl.m_local, g_cl.m_cmd->buttons_);
    const bool bOldIsFirstPrediction = g_interfaces.m_prediction->m_bFirstTimePredicted;
    const bool bOldInPrediction = g_interfaces.m_prediction->m_bInPrediction;

    g_interfaces.m_global_vars->m_cur_time =
        g_cl.m_local->m_tick_base() * g_interfaces.m_global_vars->m_interval_per_tick;
    g_interfaces.m_global_vars->m_tick_count = g_cl.m_local->m_tick_base();
    g_interfaces.m_global_vars->m_frame_time = g_interfaces.m_global_vars->m_interval_per_tick;

    g_interfaces.m_prediction->m_bFirstTimePredicted = true;
    g_interfaces.m_prediction->m_bInPrediction = true;

    g_interfaces.m_prediction->SetLocalViewAngles(g_cl.m_cmd->m_viewangles);

    g_interfaces.m_game_movement->StartTrackPredictionErrors(g_cl.m_local);

    memset(&m_MoveData, 0, sizeof(m_MoveData));

    g_interfaces.m_prediction->SetupMove(g_cl.m_local, g_cl.m_cmd, g_interfaces.m_move_helper, &m_MoveData);
    g_interfaces.m_game_movement->ProcessMovement(g_cl.m_local, &m_MoveData);
    g_interfaces.m_prediction->FinishMove(g_cl.m_local, g_cl.m_cmd, &m_MoveData);

    g_interfaces.m_game_movement->FinishTrackPredictionErrors(g_cl.m_local);

    g_interfaces.m_prediction->m_bInPrediction = bOldInPrediction;
    g_interfaces.m_prediction->m_bFirstTimePredicted = bOldIsFirstPrediction;
    // player->m_tick_base( )++;
    // cmd->predicted_ = true;
    // player->m_tick_base( ) = backup_tickbase;
}
void prediction::finish()
{
    if (!g_interfaces.m_move_helper)
        return;
    g_interfaces.m_move_helper->SetHost(nullptr);

    g_interfaces.m_global_vars->m_cur_time = m_fOldCurrentTime;
    g_interfaces.m_global_vars->m_frame_time = m_fOldFrameTime;
    g_interfaces.m_global_vars->m_tick_count = m_nOldTickCount;

    g_cl.m_local->SetCurrentCmd(nullptr);
}
