#include "player_manager.h"
#include "sdk.h"
#include "movement_simulate.h"
#include "c_user_cmd.h"

#define DIST_EPSILON (0.03125f)
#define TIME_TO_TICKS(dt)                                                                      \
  ((int)(0.5f + (float)(dt) / g_interfaces.m_global_vars->m_interval_per_tick))
#define TICKS_TO_TIME(dt) (g_interfaces.m_global_vars->m_interval_per_tick * (float)(dt))

bool player_record_t::valid() const {
  const auto* nci = g_interfaces.m_engine->get_net_channel_info();
  if (!nci || !g_interfaces.m_client_state)
    return false;

  const int curtick = g_interfaces.m_client_state->clock_drift.server_tick + 1;

  // correct is the amount of time we have to correct game time,
  static auto  sv_maxunlag     = g_interfaces.m_cvar->find_var("sv_maxunlag");
  static auto* cl_interp       = g_interfaces.m_cvar->find_var("cl_interp");
  static auto* cl_interp_ratio = g_interfaces.m_cvar->find_var("cl_interp_ratio");
  static auto* cl_updaterate   = g_interfaces.m_cvar->find_var("cl_updaterate");
  const int    lerp_ticks      = TIME_TO_TICKS(
      std::fmaxf(cl_interp->m_value.m_float_value, cl_interp_ratio->m_value.m_float_value /
                                                               cl_updaterate->m_value.m_float_value));
  float correct = TICKS_TO_TIME(lerp_ticks);

  // https://www.unknowncheats.me/forum/counterstrike-global-offensive/359885-fldeadtime-int.html
  // https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/server/player_lagcompensation.cpp#L716
  // does this ever come into use? no, is it needed maybe
  // yes it does, thank you i forgot about this
  const auto dead_time =
      static_cast<int>(TICKS_TO_TIME(curtick) - sv_maxunlag->m_value.m_float_value);
  if (sim_time < dead_time)
    return false;

  // stupid fake latency goes into the incoming latency.
  correct += nci->GetLatency(1);

  // check bounds [ 0, sv_maxunlag ]
  correct = std::clamp<float>(correct, 0.f, sv_maxunlag->m_value.m_float_value);
  return std::fabs(correct - TICKS_TO_TIME(curtick - TIME_TO_TICKS(sim_time))) <= 0.2f;
}

vector direction_pressed(player_record_t* record, vector last_vel) {
  const vector vel = record->vel.normalized();
  last_vel.normalize_in_place();
  if ((vel - last_vel).length_2d() > 0.1f) {
    vector vel_difference = vel - last_vel;
    vel_difference.m_z    = 0;
    vector dir            = vel_difference.angle_to();
    dir.m_y -= record->eye_angle.m_y;
    return dir.angle_vector() * 450.f;
  }
  return vector();
}

inline float anglemod(float a) {
  a = (360.f / 65536) * ((int)(a * (65536.f / 360.0f)) & 65535);
  return a;
}

float ApproachAngle(float target, float value, float speed) {
  float delta = target - value;

  // Speed is assumed to be positive
  if (speed < 0)
    speed = -speed;

  if (delta < -180)
    delta += 360;
  else if (delta > 180)
    delta -= 360;

  if (delta > speed)
    value += speed;
  else if (delta < -speed)
    value -= speed;
  else
    value = target;

  return value;
}

float turn_epsilon(vector orig_vel) {
  float max = 0.f;
  for (auto i = -1; i < 3; i++) {
    for (auto k = -1; k < 3; k++) {
      vector vel = orig_vel;
      vel.m_x += i * DIST_EPSILON;
      vel.m_y += k * DIST_EPSILON;
      float turn =
          rad_to_deg(std::atan2(vel.m_y, vel.m_x) - std::atan2(orig_vel.m_y, orig_vel.m_x));
      if (turn > max)
        max = turn;
    }
  }
  return max;
}

float dir_turning(vector vel, vector last_vel) {
  if (vel.length_sqr_2d() < 1.f)
    return 0;
  if (last_vel.length_sqr_2d() < 1.f)
    return 0;

  float dir = rad_to_deg(std::atan2(vel.m_y, vel.m_x) - std::atan2(last_vel.m_y, last_vel.m_x));
  while (dir > 180.f)
    dir -= 360.f;
  while (dir < -180.f)
    dir += 360.f;
  if (fabsf(dir) > 175.f || fabsf(dir) < 1.f)
    return 0;
  if (fabs(dir) < 0.5f)
    return 0;
  return dir;
}

void player_t::SetGroundEntity(trace_t* pm) {
  auto* oldGround = m_ground;
  auto* newGround = pm ? pm->m_entity : NULL;
  if (oldGround && newGround != oldGround) {
    if (oldGround->get_client_class()->m_class_id == CFuncConveyor) {
      vector right;
      oldGround->m_ang_rot().angle_vectors(nullptr, &right, nullptr);
      right *= ((c_func_conveyor*)oldGround)->conveyor_speed();
      m_base_velocity -= right;
      m_base_velocity.m_z = right.m_z;
    }
  }
  if (newGround && oldGround != newGround) {
    if (newGround->get_client_class()->m_class_id == CFuncConveyor) {
      vector right;
      newGround->m_ang_rot().angle_vectors(nullptr, &right, nullptr);
      right *= ((c_func_conveyor*)newGround)->conveyor_speed();
      m_base_velocity += right;
      m_base_velocity.m_z = right.m_z;
    }
  }
  m_ground = newGround;
}

#define GAMEMOVEMENT_JUMP_TIME 510.f

vector get_velocity(vector origin_diff, int lag, int flags, int last_flags,
                    float jump_time_delta) // TODO: implement the rest of these cases
{
  if (!(flags & FL_ONGROUND)) {
    if ((flags & FL_DUCKING) && !(last_flags & FL_DUCKING)) {
      origin_diff.m_z -= 37;
    } else if (!(flags & FL_DUCKING) && (last_flags & FL_DUCKING)) {
      origin_diff.m_z += 37;
    }
  }
  return (origin_diff) * (1.f / TICKS_TO_TIME(lag));
}

void c_player_manager::update_players() {
  for (auto i = 1; i <= g_interfaces.m_entity_list->get_highest_entity_index(); i++) {
    const auto target = g_interfaces.m_entity_list->get_entity<c_base_player>(i);
    auto*      player = &players[i - 1];
    if (player->player != target) {
      player->player     = target;
      player->m_sim_time = -1.f;
    }

    if (!player->player)
      continue;

    if (!target->is_valid(g_cl.m_local, false)) {
      player->m_records.clear();
      continue;
    }

    const float new_time = target->sim_time();
    if (player->m_sim_time < target->sim_time()) {
      player_record_t new_record;
      new_record.flags              = target->flags();
      new_record.eye_angle          = target->m_eye_angles();
      new_record.world_space_center = target->world_space_center();
      new_record.m_lag =
          (player->m_sim_time == -1.f)
              ? 1
              : static_cast<int>(0.5f +
                                 (float)(new_time - player->m_sim_time) /
                                     g_interfaces.m_global_vars
                                         ->m_interval_per_tick); // TIME_TO_TICKS( ( new_time -
                                                                 // player->sim_time ) );
      player->m_sim_time = new_time;
      new_record.origin  = target->m_vec_origin();
      const vector dif   = (new_record.origin - player->pred_origin);
      if (fabs(dif.m_x) <= 0.03125f)
        new_record.origin.m_x = player->pred_origin.m_x;
      if (fabs(dif.m_y) <= 0.03125f)
        new_record.origin.m_y = player->pred_origin.m_y;
      if (fabs(dif.m_z) <= 0.03125f)
        new_record.origin.m_z = player->pred_origin.m_z;
      new_record.vel      = target->m_velocity();
      // new_record.dir = 0;
      new_record.player   = player;
      new_record.sim_time = new_time;

      if (new_record.flags & FL_ONGROUND) {
        trace_t pm;

        vector vecStartPos = new_record.origin;
        vector vecEndPos(new_record.origin.m_x, new_record.origin.m_y,
                         (new_record.origin.m_z - 2.0f));
        bool   bMoveToEndPos = false;
        vecEndPos.m_z -= 16.f + DIST_EPSILON;
        bMoveToEndPos = true;
        c_movement_simulate::try_touch_ground(target, vecStartPos, vecEndPos, target->mins(),
                                              target->maxs(), MASK_PLAYERSOLID,
                                              COLLISION_GROUP_PLAYER_MOVEMENT, pm);
        if (pm.m_plane.normal[2] < 0.7f) {
          // Test four sub-boxes, to see if any of them would have found shallower slope we
          // could actually stand on
          c_movement_simulate::try_touch_ground_in_quadrants(
              target, vecStartPos, vecEndPos, target->mins(), target->maxs(), MASK_PLAYERSOLID,
              COLLISION_GROUP_PLAYER_MOVEMENT, pm);
          player->SetGroundEntity(&pm);
        } else {
          player->SetGroundEntity(&pm);
        }
      } else
        player->SetGroundEntity(nullptr);

      new_record.on_ground      = player->m_ground != nullptr;
      new_record.mins_prescaled = target->get_collideable()->obb_mins_pre_scaled();
      new_record.maxs_prescaled = target->get_collideable()->obb_maxs_pre_scaled();
      new_record.mins           = target->mins();
      new_record.maxs           = target->maxs();
      if (new_record.on_ground)
        new_record.vel.m_z = 0;

      const auto backup_abs_origin = target->get_abs_origin();

      target->set_abs_origin(target->m_vec_origin());

      target->invalidate_bone_cache();
      new_record.built = target->setup_bones(new_record.bones, 128, 0x100, target->sim_time());

      target->set_abs_origin(backup_abs_origin);
      player_info_t info;
      if (g_interfaces.m_engine->get_player_info(target->entindex(), &info))
        if (!player->m_records.empty()) {
          new_record.vel =
              get_velocity(new_record.origin - player->m_records[0]->origin, new_record.m_lag,
                           new_record.flags, player->m_records[0]->flags,
                           new_record.sim_time - player->m_last_time_jumped);
          g_movement.mv.m_player = target;
          new_record.move_data =
              g_movement.estimate_walking_dir(new_record.vel, player->m_records[0]->vel,
                                              new_record.eye_angle, new_record.origin);
          new_record.dir_change =
              (new_record.eye_angle.m_y - player->m_records[0]->eye_angle.m_y);
          if (!new_record.on_ground) {
          }
          while (new_record.dir_change > 180.f)
            new_record.dir_change -= 360.f;
          while (new_record.dir_change < -180.f)
            new_record.dir_change += 360.f;
          new_record.dir_change /= new_record.m_lag;

          float accumulated_change = 0.f;
          int   count              = 0;
          for (auto i = 0; i < 100; i++) {
            if (i + 2 >= player->m_records.size())
              break;
            auto record_far  = player->m_records[i + 1];
            auto record_last = player->m_records[i];

            float velocity_ang = rad_to_deg(atan2(new_record.vel.m_y, new_record.vel.m_x));

            if ((new_record.flags & FL_ONGROUND) || (record_last->flags & FL_ONGROUND)) {
              break;
            }

            float last_velocity_ang =
                rad_to_deg(atan2(record_last->vel.m_y, record_last->vel.m_x));
            float far_velocity_ang =
                rad_to_deg(atan2(record_far->vel.m_y, record_far->vel.m_x));

            float far_delta_theta = (last_velocity_ang - far_velocity_ang);
            float delta_theta     = (velocity_ang - last_velocity_ang);

            while (far_delta_theta > 180.f)
              far_delta_theta -= 360.f;
            while (far_delta_theta < -180.f)
              far_delta_theta += 360.f;

            while (delta_theta > 180.f)
              delta_theta -= 360.f;
            while (delta_theta < -180.f)
              delta_theta += 360.f;

            float last_speed = record_last->vel.length_2d();
            float far_speed  = record_far->vel.length_2d();

            if (last_speed < 30.f || far_speed < 30.f)
              break;

            float counter_strafe_far = rad_to_deg(asin(30.f / far_speed));
            if (far_delta_theta < 0)
              counter_strafe_far *= -1.f;

            float counter_strafe = rad_to_deg(asin(30.f / last_speed));
            if (delta_theta < 0)
              counter_strafe *= -1.f;

            float delta_time = new_record.sim_time - record_last->sim_time;
            if (!(delta_time > 0))
              continue;

            delta_theta = ((velocity_ang - counter_strafe) -
                                                (last_velocity_ang - counter_strafe_far));
            while (delta_theta > 180.f)
              delta_theta -= 360.f;
            while (delta_theta < -180.f)
              delta_theta += 360.f;
            if (delta_theta > 45.f)
              continue;

            delta_theta /= delta_time;

            delta_theta *= g_interfaces.m_global_vars->m_interval_per_tick;

            if (count == 0)
              accumulated_change = delta_theta;
            else
              accumulated_change = 0.54f * delta_theta + (1.f - 0.54f) * accumulated_change;
            count++;
          }
          new_record.dir = 0.f;
          if (count != 0) {
            const float angle_limit = 360.f * g_interfaces.m_global_vars->m_interval_per_tick;
            new_record.dir          = std::clamp(
                accumulated_change, -angle_limit,
                angle_limit); // ApproachAngle(turn, player->m_records[0]->dir, max_turn);
            float velocity_ang   = rad_to_deg(atan2(new_record.vel.m_y, new_record.vel.m_x));
            float counter_strafe =
                rad_to_deg(asin(30.f / player->m_records[0]->vel.length_2d()));
            new_record.air_dir = velocity_ang - counter_strafe;
            while (new_record.air_dir > 180.f)
              new_record.air_dir -= 360.f;
            while (new_record.air_dir < -180.f)
              new_record.air_dir += 360.f;
          }
          new_record.ground_dir = 0.f;

          float max_turn        = 10.f * g_interfaces.m_global_vars->m_interval_per_tick;
          if (new_record.move_data.length_sqr_2d() > 1.f) {

            const int count = fmin(player->m_records.size(), TIME_TO_TICKS(0.2));
            vector    last_move_dir;
            bool      found = false;
            for (auto i = 0; i < count; i++) {
              if (i >= player->m_records.size())
                break;
              if (player->m_records[i]->move_data.length_sqr_2d() > 1.f) {
                last_move_dir = player->m_records[i]->move_data;
                found         = true;
                break;
              }
            }
            if (found) {
              float turn = dir_turning(new_record.move_data, last_move_dir);
              while (turn > 180.f)
                turn -= 360.f;
              while (turn < -180.f)
                turn += 360.f;
              max_turn              = 315 * g_interfaces.m_global_vars->m_interval_per_tick;
              new_record.ground_dir = turn;
            }
          }

          vector predicted_origin;
          if (g_movement.setup_mv(new_record.vel, player->player) &&
              g_movement.run(player->pred_origin)) {
            player->pred_origin = predicted_origin;
          }

          if (!(new_record.flags & FL_ONGROUND) &&
              (player->m_records[0]->flags & FL_ONGROUND) && new_record.vel.m_z > 100.f)
            player->m_last_time_jumped = new_record.sim_time;
        }

      player->m_records.push_front(std::make_shared<player_record_t>(new_record));
    }
    while (player->m_records.size() > 256)
      player->m_records.pop_back();
  }
}

player_record_t::~player_record_t() {
  // g_interfaces.m_mem_alloc->free( bones );
}

bool player_record_t::cache() {
  if (!built)
    return false;
  return false;
}

void player_record_t::restore() { player->player->invalidate_bone_cache(); }
