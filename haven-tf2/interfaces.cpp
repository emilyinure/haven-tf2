#include "sdk.h"

void c_interfaces::gather()
{
    const auto client = g_modules.get("client.dll");
    {
        this->m_client = client.get_interface("VClient0").as<i_base_client_dll>();
        this->m_prediction = client.get_interface("VClientPrediction001", true).as<c_prediction>();
        this->m_entity_list = client.get_interface("VClientEntityList0").as<i_client_entity_list>();
        this->m_game_movement = client.get_interface("GameMovement001", true).as<c_game_movement>();
        this->m_game_rules = **client.get_sig("8B 0D ? ? ? ? 83 C4 10 C7 45").as<game_rules***>(0x2);
        this->m_client_mode = **reinterpret_cast<void***>(client.get_sig("8B 0D ? ? ? ? 8B 02 D9 05").add(2));
    }

    const auto engine = g_modules.get("engine.dll");
    {
        this->m_global_vars = *engine.get_sig("A1 ? ? ? ? 8B 11 68").as<c_global_vars_base**>(0x8);
        this->m_engine = engine.get_interface("VEngineClient0").as<iv_engine_client>();
        this->m_engine_sound = engine.get_interface("IEngineSoundClient0").as<i_engine_sound>();
        this->m_engine_vgui = engine.get_interface("VEngineVGui0").as<i_engine_vgui>();
        this->m_debug_overlay = engine.get_interface("VDebugOverlay0").as<iv_debug_overlay>();
        this->m_engine_trace = engine.get_interface("EngineTraceClient003", true).as<i_engine_trace>();
        this->m_model_info = engine.get_interface("VModelInfoClient006", true).as<i_model_info>();
        this->m_render_view = engine.get_interface("VEngineRenderView014", true).as<c_render_view>();
        this->m_client_state = *engine.get_sig("68 ? ? ? ? E8 ? ? ? ? 83 C4 08 5F 5E 5B 5D C3").as<c_client_state**>(1);
    }

    const auto vgui_mat_surface = g_modules.get("vguimatsurface.dll");
    {
        this->m_surface = vgui_mat_surface.get_interface("VGUI_Surface0").as<i_surface>();
    }

    const auto vstdlib = g_modules.get("vstdlib.dll");
    {
        this->m_cvar = vstdlib.get_interface("VEngineCvar0").as<i_cvar>();
    }

    const auto vgui2 = g_modules.get("vgui2.dll");
    {
        this->m_localize = vgui2.get_interface("VGUI_Localize005").as<i_localize>();
    }

    m_mem_alloc = *reinterpret_cast<mem_alloc_t**>(GetProcAddress(GetModuleHandleA("tier0.dll"), "g_pMemAlloc"));
}

void c_interfaces::init()
{
    this->gather();
}
