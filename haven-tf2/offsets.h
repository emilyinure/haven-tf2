#pragma once

class c_offsets
{
public:
    struct
    {
        uint8_t* start_drawing;
        uint8_t* finish_drawing;
        uint8_t* global_vars_base;
        uint8_t* compute_hitbox_surrounding_box;
        uint8_t* in_cond;
        uint8_t* invalidate_bone_cache;
    } m_sigs;

    void init();
};
inline c_offsets g_offsets;
