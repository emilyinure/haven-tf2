#include "movement.h"
#include "client.h"
#include "interfaces.h"
#include <numbers>

void c_movement::bhop()
{
    const auto MoveType = g_cl.m_local->move_type();
    if (MoveType == MOVETYPE_NOCLIP || MoveType == MOVETYPE_LADDER || MoveType == MOVETYPE_OBSERVER)
        return;

    const bool jump = g_cl.m_cmd->buttons_ & IN_JUMP;
    static bool was_jump_held = false;
    const auto ground = g_cl.m_local->get_ground();

    if (ground && jump)
    {
        was_jump_held = true;
        return;
    }

    if (was_jump_held && !ground && jump)
    {
        g_cl.m_cmd->buttons_ &= ~IN_JUMP;
        return;
    }

    was_jump_held = jump;
}

double normalize_rad(double a)
{
    a = std::fmod(a, std::numbers::pi_v<double> * 2);

    if (a >= std::numbers::pi_v<double>)
    {
        a -= 2 * std::numbers::pi_v<double>;
    }
    else if (a < -std::numbers::pi_v<double>)
    {
        a += 2 * std::numbers::pi_v<double>;
    }

    return a;
}

#define M_PI 3.141592653589793238463
double MaxAccelTheta(double wishspeed)
{    
    static auto air_accel = g_interfaces.m_cvar->find_var("sv_airaccelerate");
    double accel = air_accel->m_value.m_float_value;
    double accelspeed = accel * 450.f * g_interfaces.m_global_vars->m_interval_per_tick;
    if (accelspeed <= 0.0)
        return M_PI;
    auto vel = g_cl.m_local->m_velocity();
    if (vel.length_2d() < 1.f)
        return 0.0;

    double wishspeed_capped = 30;
    double tmp = wishspeed_capped - accelspeed;
    if (tmp <= 0.0)
        return M_PI / 2;

    double speed = vel.length_2d();
    if (tmp < speed)
        return std::acos(tmp / speed);

    return 0.0;
}
double max_accel_into_yaw_theta(const double& vel_yaw, const double& yaw, const double& wish_speed)
{
    const double theta = MaxAccelTheta(wish_speed);

    if (theta == 0.0 || theta == std::numbers::pi_v<double>)
    {
        return normalize_rad(yaw - vel_yaw + theta);
    }

    return std::copysign(theta, normalize_rad(yaw - vel_yaw));
}


static inline double ButtonsPhi(float forward, float side)
{
    return -std::atan2f(side, forward);
}

void c_movement::auto_strafe(float* view)
{
    if (g_cl.m_local->get_ground() || !(g_cl.m_cmd->buttons_ & IN_JUMP))
    {
        return;
    }

    const auto velocity = g_cl.m_local->m_velocity();

    const float speed = velocity.length_2d();

    // compute the ideal strafe angle for our velocity.
    const float ideal = (speed > 0.f) * rad_to_deg(std::asin(15.f / speed)) + (speed <= 0.f) * 90.f;
    const float ideal2 = (speed > 0.f) * rad_to_deg(std::asin(30.f / speed)) + (speed <= 0.f) * 90.f;

    m_switch *= -1;

    // get our viewangle change.

    vector direction;
    direction.m_x =
        1 * (fabsf(g_cl.m_cmd->sidemove_) < 0 && fabsf(g_cl.m_cmd->forwardmove_) < 0); // branchless optimization
    direction.m_x += 1 * (g_cl.m_cmd->forwardmove_ > 0) + -1 * (g_cl.m_cmd->forwardmove_ < 0);
    direction.m_y = 1 * (g_cl.m_cmd->sidemove_ < 0) + -1 * (g_cl.m_cmd->sidemove_ > 0);

    *view += vector().look(direction).m_y;

    auto delta = *view - m_old_yaw;
    while (delta > 180)
        delta -= 360;
    while (delta < -180)
        delta += 360;
    m_old_yaw = *view;

    // convert to absolute change.
    const auto abs_delta = std::abs(delta);

    // set strafe direction based on mouse direction change.
    double vel_yaw_rad = std::atan2(velocity.m_y, velocity.m_x);
    double yaw_rad = deg_to_rad(*view);
    double theta = max_accel_into_yaw_theta(vel_yaw_rad, yaw_rad, 450.f);

    g_cl.m_cmd->sidemove_ = (theta > 0) * 450.f + (theta < 0) * -450.f;
    g_cl.m_cmd->forwardmove_ = 0;

    double phi = ButtonsPhi(g_cl.m_cmd->forwardmove_, g_cl.m_cmd->sidemove_);
    float yaw = vel_yaw_rad - phi + theta;

    *view = rad_to_deg(normalize_rad(yaw));
}

void c_movement::correct_movement(vector old)
{
    vector wish_forward, wish_right, wish_up, cmd_forward, cmd_right, cmd_up;

    const vector movedata{g_cl.m_cmd->forwardmove_, g_cl.m_cmd->sidemove_, g_cl.m_cmd->upmove_};

    old.angle_vectors(&wish_forward, &wish_right, &wish_up);
    g_cl.m_cmd->m_viewangles.angle_vectors(&cmd_forward, &cmd_right, &cmd_up);

    const auto v8 = sqrtf(wish_forward.m_x * wish_forward.m_x + wish_forward.m_y * wish_forward.m_y),
               v10 = sqrt(wish_right.m_x * wish_right.m_x + wish_right.m_y * wish_right.m_y),
               v12 = sqrt(wish_up.m_z * wish_up.m_z);

    const vector wish_forward_norm(1.0f / v8 * wish_forward.m_x, 1.0f / v8 * wish_forward.m_y, 0.f),
        wish_right_norm(1.0f / v10 * wish_right.m_x, 1.0f / v10 * wish_right.m_y, 0.f),
        wish_up_norm(0.f, 0.f, 1.0f / v12 * wish_up.m_z);

    const auto v14 = sqrtf(cmd_forward.m_x * cmd_forward.m_x + cmd_forward.m_y * cmd_forward.m_y),
               v16 = sqrt(cmd_right.m_x * cmd_right.m_x + cmd_right.m_y * cmd_right.m_y),
               v18 = sqrt(cmd_up.m_z * cmd_up.m_z);

    const vector cmd_forward_norm(1.0f / v14 * cmd_forward.m_x, 1.0f / v14 * cmd_forward.m_y, 1.0f / v14 * 0.0f),
        cmd_right_norm(1.0f / v16 * cmd_right.m_x, 1.0f / v16 * cmd_right.m_y, 1.0f / v16 * 0.0f),
        cmd_up_norm(0.f, 0.f, 1.0f / v18 * cmd_up.m_z);

    const auto v22 = wish_forward_norm.m_x * movedata.m_x, v26 = wish_forward_norm.m_y * movedata.m_x,
               v28 = wish_forward_norm.m_z * movedata.m_x, v24 = wish_right_norm.m_x * movedata.m_y,
               v23 = wish_right_norm.m_y * movedata.m_y, v25 = wish_right_norm.m_z * movedata.m_y,
               v30 = wish_up_norm.m_x * movedata.m_z, v27 = wish_up_norm.m_z * movedata.m_z,
               v29 = wish_up_norm.m_y * movedata.m_z;

    vector correct_movement{
        (cmd_forward_norm.m_x * v24 + cmd_forward_norm.m_y * v23 + cmd_forward_norm.m_z * v25) +
            (cmd_forward_norm.m_x * v22 + cmd_forward_norm.m_y * v26 + cmd_forward_norm.m_z * v28) +
            (cmd_forward_norm.m_y * v30 + cmd_forward_norm.m_x * v29 + cmd_forward_norm.m_z * v27),

        (cmd_right_norm.m_x * v24 + cmd_right_norm.m_y * v23 + cmd_right_norm.m_z * v25) +
            (cmd_right_norm.m_x * v22 + cmd_right_norm.m_y * v26 + cmd_right_norm.m_z * v28) +
            (cmd_right_norm.m_x * v29 + cmd_right_norm.m_y * v30 + cmd_right_norm.m_z * v27),

        (correct_movement.m_z = cmd_up_norm.m_x * v23 + cmd_up_norm.m_y * v24 + cmd_up_norm.m_z * v25) +
            (cmd_up_norm.m_x * v26 + cmd_up_norm.m_y * v22 + cmd_up_norm.m_z * v28) +
            (cmd_up_norm.m_x * v30 + cmd_up_norm.m_y * v29 + cmd_up_norm.m_z * v27)};

    g_cl.m_cmd->forwardmove_ = std::clamp<float>(correct_movement.m_x, -450.f, 450.f);
    g_cl.m_cmd->sidemove_ = std::clamp<float>(correct_movement.m_y, -450.f, 450.f);
    g_cl.m_cmd->upmove_ = std::clamp<float>(correct_movement.m_z, -320.f, 320.f);
}
