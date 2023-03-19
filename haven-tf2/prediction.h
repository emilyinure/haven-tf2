#pragma once
class prediction
{
    float m_fOldCurrentTime, m_fOldFrameTime;
    int m_nOldTickCount;
    CMoveData m_MoveData = {};

public:
    int get_tickbase(usercmd_t* pCmd, c_base_player* pLocal);
    void start();
    void finish();
} inline g_prediction;
