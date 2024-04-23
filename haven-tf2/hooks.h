#pragma once

class c_hooks
{
    enum e_indexes
    {
        index_paint = 13,
        index_lock_cursor = 62,
    };

public:
    struct
    {
        typedef void(* paint_fn)(uintptr_t, paint_mode_t);
        paint_fn paint;

        typedef void(* override_view_fn)(uintptr_t, c_view_setup*);
        override_view_fn override_view;

        typedef void(* calc_view_model_fn)(uintptr_t, c_base_player*, const vector&, vector& );
        calc_view_model_fn calc_view_model;

        typedef void(* estimate_abs_velocity_fn)(uintptr_t, vector&);
        estimate_abs_velocity_fn estimate_abs_velocity;

        typedef bool(* create_move_fn)(float, usercmd_t*);
        create_move_fn create_move;

        typedef void (*frame_stage_fn)(void*, client_frame_stage_t);
        frame_stage_fn frame_stage;

        typedef void(* lock_cursor_fn)(uintptr_t);
        lock_cursor_fn lock_cursor;

        typedef void(__thiscall* run_command_fn)(c_prediction*, c_base_entity*, usercmd_t*, CMoveHelper*);
        run_command_fn run_command;

        using get_outer_abs_velo_fn = void(__stdcall*)(vector);
        get_outer_abs_velo_fn get_outer_abs_velocity_orig;
        WNDPROC wnd_proc;
    } m_original;

    void init();
    void unload();
};
inline c_hooks g_hooks;
