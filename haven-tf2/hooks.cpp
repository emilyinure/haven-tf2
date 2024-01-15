#include "aimbot.h"
#include "movement.h"
#include "movement_simulate.h"
#include "player_manager.h"
#include "prediction.h"
#include "projectile aim.h"
#include "sdk.h"
#include "stab.h"

void __fastcall paint(uintptr_t ecx, uintptr_t edx, paint_mode_t mode)
{
    if (g_cl.m_unloading)
        return g_hooks.m_original.paint(ecx, edx, mode);

    g_hooks.m_original.paint(ecx, edx, mode);

    g_cl.m_local = g_interfaces.m_entity_list->get_entity<c_base_player>(g_interfaces.m_engine->get_local_player());

    if (mode & PAINT_UIPANELS)
    {
        c_view_setup setup;
        if (g_interfaces.m_client->get_player_view(setup))
        {
            view_matrix u0{}, u1{}, u2{};
            g_interfaces.m_render_view->GetMatricesForView(setup, &u0, &u1, &g_cl.m_view_matrix, &u2);
        }

        g_interfaces.m_surface->start_drawing();
        {
            g_input.poll();
            g_visuals.on_paint();
            g_proj.draw();
            g_ui.on_paint();
        }
        g_interfaces.m_surface->finish_drawing();
    }
}

void __fastcall override_view(uintptr_t ecx, uintptr_t edx, c_view_setup* view)
{
    if (g_cl.m_local)
    {
        static bool thirdperson = false;
        static bool set = false;
        if (GetAsyncKeyState('X'))
        {
            if (!set)
            {
                thirdperson = !thirdperson;
                g_cl.m_local->force_taunt_cam(thirdperson);
                set = true;
            }
        }
        else
            set = false;

       if (thirdperson)
        {
            vector ViewAngles;
            g_interfaces.m_engine->get_view_angles(ViewAngles);

            vector Forward;
            vector Right;
            vector Up;
            ViewAngles.angle_vectors(&Forward, &Right, &Up);

            view->m_origin += Right * g_ui.m_controls.misc.ThirdpersonHorizontalOffset->m_value;
            view->m_origin += Up * g_ui.m_controls.misc.ThirdpersonVerticalOffset->m_value;
            view->m_origin += Forward * g_ui.m_controls.misc.ThirdpersonDistanceOffset->m_value;
        }
    }

    g_hooks.m_original.override_view(ecx, edx, view);
}

bool __stdcall create_move(float sample, usercmd_t* cmd)
{
    g_hooks.m_original.create_move(sample, cmd);
    if (cmd->command_number_ != 0)
    {
        g_interfaces.m_engine->set_view_angles(cmd->m_viewangles);
        g_cl.on_move(cmd);
    }
    return false;
}

LRESULT __stdcall wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (g_cl.m_unloading)
        return CallWindowProcA(g_hooks.m_original.wnd_proc, hwnd, msg, wparam, lparam);

    g_input.on_wnd_proc(hwnd, msg, wparam, lparam);

    return CallWindowProcA(g_hooks.m_original.wnd_proc, hwnd, msg, wparam, lparam);
}

void __fastcall run_command(c_prediction* _this, uintptr_t, c_base_entity* player, usercmd_t* cmd,
                            CMoveHelper* move_helper)
{
    g_hooks.m_original.run_command(_this, player, cmd, move_helper);
    if (move_helper)
        g_interfaces.m_move_helper = move_helper;
}

void __stdcall frame_stage(client_frame_stage_t stage)
{
    g_cl.m_local = g_interfaces.m_entity_list->get_entity<c_base_player>(g_interfaces.m_engine->get_local_player());
    g_hooks.m_original.frame_stage(stage);
    if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_END)
    {
        if (g_cl.m_local)
        {
            g_player_manager.update_players();
        }
    }
}

void __fastcall lock_cursor(uintptr_t ecx, uintptr_t edx)
{
    return g_ui.m_open ? g_interfaces.m_surface->unlock_cursor() : g_hooks.m_original.lock_cursor(ecx, edx);
}

void __fastcall calc_view_model(uintptr_t ecx, uintptr_t edx, c_base_player* owner, const vector& eye_position,
                                vector& eye_angles)
{

    vector forward, right, up;
    eye_angles.angle_vectors(&forward, &right, &up);

    vector eye_pos = eye_position + ((right * g_ui.m_controls.visuals.view_model.x_offset->m_value) +
                                    (forward * g_ui.m_controls.visuals.view_model.y_offset->m_value) +
                                    (up * g_ui.m_controls.visuals.view_model.z_offset->m_value));
    
    eye_angles.m_z += g_ui.m_controls.visuals.view_model.roll->m_value; // VM Roll
    g_hooks.m_original.calc_view_model(ecx, edx, owner, eye_pos, eye_angles);
}

void __fastcall estimate_abs_velocity(uintptr_t ecx, uintptr_t edx, vector& vel) noexcept
{
    const auto entity = reinterpret_cast<c_base_player*>(ecx);
    if (g_cl.m_local && entity != g_cl.m_local &&
        entity->get_client_class()->m_class_id == 247 /*CTFPlayer*/) // ur class id is outdated
    {
        vel = entity->calculate_abs_velocity();
        return;
    }

    g_hooks.m_original.estimate_abs_velocity(ecx, edx, vel);
}

void c_hooks::init()
{
    MH_Initialize();

    MH_CreateHook(g_modules.get("client.dll").get_sig("55 8B EC E8 ? ? ? ? 8B C8 85 C9 75 ? B0 ?").as<void*>(),
                  create_move, reinterpret_cast<void**>(&this->m_original.create_move));
    MH_CreateHook(c_utils::get_virtual_function<void*>(g_interfaces.m_engine_vgui, 14), paint,
                  reinterpret_cast<void**>(&this->m_original.paint));
    MH_CreateHook( c_utils::get_virtual_function<void*>(
        g_interfaces.m_prediction, 17 ), run_command, reinterpret_cast< void** >(
        &this->m_original.run_command ) );

    this->m_original.wnd_proc =
        reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_cl.m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wnd_proc)));
    MH_CreateHook(c_utils::get_virtual_function<void*>(g_interfaces.m_surface, e_indexes::index_lock_cursor),
                  lock_cursor, reinterpret_cast<void**>(&this->m_original.lock_cursor));
    MH_CreateHook(c_utils::get_virtual_function<void*>(g_interfaces.m_client, 35), frame_stage,
                  reinterpret_cast<void**>(&this->m_original.frame_stage));
    MH_CreateHook(c_utils::get_virtual_function<void*>(g_interfaces.m_client_mode, 16), override_view,
                  reinterpret_cast<void**>(&this->m_original.override_view));
    MH_CreateHook(g_modules.get("client.dll")
                      .get_sig("55 8B EC 83 EC 70 8B 55 0C 53 8B 5D 08 89 4D FC 8B 02 89 45 E8 8B 42 04 89 45 EC 8B 42 "
                               "08 89 45 F0 56 57")
                      .as<void*>(),
                  calc_view_model, reinterpret_cast<void**>(&this->m_original.calc_view_model));
    
    MH_CreateHook(g_modules.get("client.dll")
                      .get_sig("55 8B EC 83 EC ? 56 8B F1 E8 ? ? ? ? 3B F0 75 ? 8B CE E8 ? ? ? ? 8B 45 ? D9 86 ? ? ? ? D9 18 D9 86 ? ? ? ? D9 58 ? D9 86 ? ? ? ? D9 58 ? 5E 8B E5 5D C2")
                      .as<void*>(),
                  estimate_abs_velocity, reinterpret_cast<void**>(&this->m_original.estimate_abs_velocity));

    MH_EnableHook(MH_ALL_HOOKS);
}

void c_hooks::unload()
{
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    SetWindowLongPtr(g_cl.m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(this->m_original.wnd_proc));
}
