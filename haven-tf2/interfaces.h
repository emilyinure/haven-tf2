#pragma once

class c_interfaces
{
    void gather();

public:
    void init();
    game_rules* m_game_rules;
    i_base_client_dll* m_client;
    iv_engine_client* m_engine;
    c_prediction* m_prediction;
    CMoveHelper* m_move_helper;
    c_game_movement* m_game_movement;
    i_engine_sound* m_engine_sound;
    i_client_entity_list* m_entity_list;
    i_engine_vgui* m_engine_vgui;
    i_surface* m_surface;
    iv_debug_overlay* m_debug_overlay;
    c_global_vars_base* m_global_vars;
    i_engine_trace* m_engine_trace;
    i_cvar* m_cvar;
    i_model_info* m_model_info;
    mem_alloc_t* m_mem_alloc;
    i_localize* m_localize;
    void* m_client_mode;
    c_render_view* m_render_view;
};
inline c_interfaces g_interfaces;
