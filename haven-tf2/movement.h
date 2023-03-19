#pragma once
class c_movement
{
public:
    void bhop();
    void auto_strafe(float* view);
    void correct_movement(vector old);
    int m_switch = 1;
    float m_old_yaw = 0;
} inline g_local_move;
