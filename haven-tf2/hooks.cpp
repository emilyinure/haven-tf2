#include "sdk.h"
#include "projectile aim.h"
#include "stab.h"
#include "movement_simulate.h"
#include "movement.h"
#include "aimbot.h"
#include "player_manager.h"
#include "prediction.h"

void __fastcall paint( uintptr_t ecx, uintptr_t edx, paint_mode_t mode ) {
	if ( g_cl.m_unloading )
		return g_hooks.m_original.paint( ecx, edx, mode );

	g_hooks.m_original.paint( ecx, edx, mode );

	g_cl.m_local = g_interfaces.m_entity_list->get_entity<c_base_player>( g_interfaces.m_engine->get_local_player( ) );

	g_interfaces.m_surface->start_drawing( ); {
		g_input.poll( );
		g_visuals.on_paint( );
		g_proj.draw( );
		g_ui.on_paint( );
	} g_interfaces.m_surface->finish_drawing( );
}

bool __stdcall create_move( float sample, usercmd_t *cmd ) {
	g_hooks.m_original.create_move( sample, cmd );
	
	g_cl.on_move( cmd );
	return false;
}

LRESULT __stdcall wnd_proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam ) {
	if ( g_cl.m_unloading )
		return CallWindowProcA( g_hooks.m_original.wnd_proc, hwnd, msg, wparam, lparam );

	g_input.on_wnd_proc( hwnd, msg, wparam, lparam );

	return CallWindowProcA( g_hooks.m_original.wnd_proc, hwnd, msg, wparam, lparam );
}

void __fastcall run_command( c_prediction* _this, uintptr_t, c_base_entity* player, usercmd_t* cmd, CMoveHelper* move_helper ) {
	g_hooks.m_original.run_command( _this, player, cmd, move_helper );
	if ( move_helper )
		g_interfaces.m_move_helper = move_helper;
}

void __stdcall frame_stage( client_frame_stage_t stage ) {
	g_cl.m_local = g_interfaces.m_entity_list->get_entity<c_base_player>( g_interfaces.m_engine->get_local_player( ) );
	if ( stage == FRAME_RENDER_START ) {
		if ( g_cl.m_local ) {
			static bool thirdperson = false;
			static bool set = false;
			if ( GetAsyncKeyState( 'X' ) ) {
				if ( !set ) {
					thirdperson = !thirdperson;
					g_cl.m_local->force_taunt_cam( thirdperson );
					set = true;
				}
			}
			else
				set = false;
		}
	}
	g_hooks.m_original.frame_stage( stage );
	if ( stage == FRAME_NET_UPDATE_POSTDATAUPDATE_END ) {
		if ( g_cl.m_local ) {
			g_player_manager.update_players( );
		}
	}
}

void __fastcall lock_cursor( uintptr_t ecx, uintptr_t edx ) {
	return g_ui.m_open ? g_interfaces.m_surface->unlock_cursor( ) : g_hooks.m_original.lock_cursor( ecx, edx );
}

void c_hooks::init( ) {
	MH_Initialize( );

	MH_CreateHook( g_modules.get( "client.dll" ).get_sig( "55 8B EC E8 ? ? ? ? 8B C8 85 C9 75 ? B0 ?" ).as<void*>( ), create_move, reinterpret_cast< void** >( &this->m_original.create_move ) );
	MH_CreateHook( c_utils::get_virtual_function<void*>( g_interfaces.m_engine_vgui, 14 ), paint, reinterpret_cast< void** >( &this->m_original.paint ) );
	//MH_CreateHook( c_utils::get_virtual_function<void*>( g_interfaces.m_prediction, 17 ), run_command, reinterpret_cast< void** >( &this->m_original.run_command ) );
	this->m_original.wnd_proc = reinterpret_cast< WNDPROC >( SetWindowLongPtr( g_cl.m_hwnd, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( wnd_proc ) ) );
	MH_CreateHook( c_utils::get_virtual_function<void*>( g_interfaces.m_surface, e_indexes::index_lock_cursor ), lock_cursor, reinterpret_cast< void** >( &this->m_original.lock_cursor ) );
	MH_CreateHook( c_utils::get_virtual_function<void*>( g_interfaces.m_client, 35 ), frame_stage, reinterpret_cast< void** >( &this->m_original.frame_stage ) );
	
	MH_EnableHook( MH_ALL_HOOKS );
}

void c_hooks::unload( ) {
	MH_DisableHook( MH_ALL_HOOKS );
	MH_RemoveHook( MH_ALL_HOOKS );
	MH_Uninitialize( );
	SetWindowLongPtr( g_cl.m_hwnd, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( this->m_original.wnd_proc ) );
}