#pragma once
class game_rules {
public:
  bool should_collide(int group, int collision_group) {
    return g_utils.get_virtual_function<bool(__thiscall*)(void*, int, int)>(this, 29)(
        this, group, collision_group);
  }
};
