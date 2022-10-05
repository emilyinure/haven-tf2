#include "sdk.h"
#include "projectile aim.h"
#include "stab.h"
#include "movement_simulate.h"
#include "movement.h"
#include "aimbot.h"
#include "player_manager.h"
#include "prediction.h"


void c_client::init( ) {
	while ( !this->m_hwnd )
		this->m_hwnd = FindWindowA( "Valve001", nullptr );

	g_utils.m_debug.open_console( );

	g_modules.init( );
	g_tf2.init( );
	g_ui.init( );
	g_hooks.init( );
	g_tier0.warning( "init\n" );

	c_utils::debug_t::set_console_color(console_color_cyan);
	printf_s("[>] injected successfully\n");
}

void c_client::unload( ) {
	// let the cheat know we're unloading.
	this->m_unloading = true;
	
	g_hooks.unload( );
	g_utils.m_debug.close_console( );
}

void c_client::on_move(usercmd_t* cmd) {
	this->m_cmd = cmd;
	this->m_local = g_interfaces.m_entity_list->get_entity < c_base_player > ( g_interfaces.m_engine->get_local_player( ) );

	vector original_view = cmd->m_viewangles;
	if ( this->m_local ) {
		this->m_shoot_pos = this->m_local->m_vec_origin() + this->m_local->m_view_offset();

		g_local_move.auto_strafe( &original_view.m_y );
		g_local_move.bhop( );

		g_prediction.start( );
		{
			if ( g_cl.m_weapon = g_cl.m_local->get_active_weapon( ) ) {
				if ( g_ui.m_controls.aim.players.enabled->m_value ) {
					g_proj.run( );
					g_backstab.run( );
					//g_aimbot.run( local, cmd );
				}
			}
		}
		g_prediction.finish( );
	}
	while ( cmd->m_viewangles.m_y > 180.f )
		cmd->m_viewangles.m_y -= 360.f;
	while ( cmd->m_viewangles.m_y < -180.f )
		cmd->m_viewangles.m_y += 360.f;
	cmd->m_viewangles.m_x = std::clamp<float>( cmd->m_viewangles.m_x, -89.9f, 89.9f );
	g_local_move.correct_movement( original_view );
}