#include "sdk.h"

void c_interfaces::gather()
{
    const auto client = g_modules.get("client.dll");
    {
        this->m_client = client.get_interface("VClient0").as<i_base_client_dll>();
        this->m_prediction = client.get_interface("VClientPrediction001", true).as<c_prediction>();
        this->m_entity_list = client.get_interface("VClientEntityList0").as<i_client_entity_list>();
        this->m_game_movement = client.get_interface("GameMovement001", true).as<c_game_movement>();
        //this->m_game_rules = **client.get_sig("8B 0D ? ? ? ? 83 C4 10 C7 45").as<game_rules***>(0x2);
        this->m_client_mode = *reinterpret_cast<void**>(client.get_sig("48 8B 0D ? ? ? ? 45 33 C9 FF 93").rel32(0x3));
        this->m_global_vars =
            *reinterpret_cast<c_global_vars_base**>(client.get_sig("48 8B 05 ?? ?? ?? ?? 48 8D 56 18").rel32(0x3));
        this->steam_friends = *reinterpret_cast<ISteamFriends**>(
            client
                .get_sig("48 8B 0D ?? ?? ?? ?? 48 85 C9 74 1C 48 8B 49 10 48 85 C9 74 13 48 8B 01 48 8D 15 ?? ?? ?? "
                         "?? 45 33 C0 FF 90 ?? ?? ?? ?? 48 8B D3 48 8B CE")
                .rel32(0x3));
    }

    const auto engine = g_modules.get("engine.dll");
    {
        this->m_engine = engine.get_interface("VEngineClient0").as<iv_engine_client>();
        this->m_engine_sound = engine.get_interface("IEngineSoundClient0").as<i_engine_sound>();
        this->m_engine_vgui = engine.get_interface("VEngineVGui0").as<i_engine_vgui>();
        this->m_debug_overlay = engine.get_interface("VDebugOverlay0").as<iv_debug_overlay>();
        this->m_engine_trace = engine.get_interface("EngineTraceClient003", true).as<i_engine_trace>();
        this->m_model_info = engine.get_interface("VModelInfoClient006", true).as<i_model_info>();
        this->m_render_view = engine.get_interface("VEngineRenderView014", true).as<c_render_view>();
        this->m_client_state = reinterpret_cast<c_client_state*>(engine.get_sig("48 8D 0D ? ? ? ? E8 ? ? ? ? F3 0F 5E 05").rel32(0x3));
    }

    const auto steam = g_modules.get("steamclient64.dll");
    {
        steam_client = steam.get_interface("SteamClient020", true).as <ISteamClient>();

        const HSteamPipe hsNewPipe = steam_client->CreateSteamPipe();
        const HSteamPipe hsNewUser = steam_client->ConnectToGlobalUser(hsNewPipe);
        steam_friends = steam_client->GetISteamFriends(hsNewUser, hsNewPipe, STEAMFRIENDS_INTERFACE_VERSION);

        steam_utils = steam_client->GetISteamUtils(hsNewUser, STEAMUTILS_INTERFACE_VERSION);

        steam_apps = steam_client->GetISteamApps(hsNewUser, hsNewPipe, STEAMAPPS_INTERFACE_VERSION);

        steam_user_stats = steam_client->GetISteamUserStats(hsNewUser, hsNewPipe, STEAMUSERSTATS_INTERFACE_VERSION);

        steam_user = steam_client->GetISteamUser(hsNewUser, hsNewPipe, STEAMUSER_INTERFACE_VERSION);
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
