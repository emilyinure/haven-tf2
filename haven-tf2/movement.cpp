#include "movement.h"
#include "client.h"


void c_movement::bhop( ) {
	if ( g_cl.m_local->get_ground() && ( g_cl.m_cmd->buttons_ & IN_JUMP ) ) {
		return;
	}

	g_cl.m_cmd->buttons_ &= ~IN_JUMP;
}
void c_movement::auto_strafe( float *view ) {
	if ( g_cl.m_local->get_ground( ) || !( g_cl.m_cmd->buttons_ & IN_JUMP ) ) {
		return;
	}
	const auto velocity = g_cl.m_local->m_velocity( );

	const float speed = velocity.length_2d( );

	// compute the ideal strafe angle for our velocity.
	const float ideal = ( speed > 0.f ) * rad_to_deg( std::asin( 15.f / speed ) ) + ( speed <= 0.f) * 90.f;
	const float ideal2 = ( speed > 0.f ) * rad_to_deg( std::asin( 30.f / speed ) ) + ( speed <= 0.f ) * 90.f;

	m_switch *= -1;

	// get our viewangle change.

	vector direction;
	direction.m_x = 1 * ( fabsf( g_cl.m_cmd->sidemove_ ) < 0 && fabsf( g_cl.m_cmd->forwardmove_ ) < 0); // branchless optimization
	direction.m_x += 1 * ( g_cl.m_cmd->forwardmove_ > 0 ) + -1 * ( g_cl.m_cmd->forwardmove_ < 0 );
	direction.m_y = 1 * ( g_cl.m_cmd->sidemove_ < 0 ) + -1 * ( g_cl.m_cmd->sidemove_ > 0 );

	*view += vector( ).look( direction ).m_y;

	auto delta = *view - m_old_yaw;
	while ( delta > 180 )
		delta -= 360;
	while ( delta < -180 )
		delta += 360;
	m_old_yaw = *view;

	// convert to absolute change.
	const auto abs_delta = std::abs( delta );

	g_cl.m_cmd->sidemove_ = 450.f * ( delta > 0 ) + -450.f * ( delta <= 0 );
	g_cl.m_cmd->forwardmove_ = 0;
	// set strafe direction based on mouse direction change.

	// we can accelerate more, because we strafed less then needed
	// or we got of track and need to be retracked.

	/*
	* data struct
	* 68 74 74 70 73 3a 2f 2f 73 74 65 61 6d 63 6f 6d 6d 75 6e 69 74 79 2e 63 6f 6d 2f 69 64 2f 73 69 6d 70 6c 65 72 65 61 6c 69 73 74 69 63 2f
	*/
	//std::printf( std::to_string( g_cl.m_local->m_velocity(  ).length_2d(  ) ).c_str(  ) );
	//std::printf( "\n" );
	if ( abs_delta <= ideal2 || abs_delta >= 30.f ) {
		// compute angle of the direction we are traveling in.
		const auto velocity_angle = rad_to_deg( atan2( velocity.m_y, velocity.m_x ) );

		// get the delta between our direction and where we are looking at.
		auto velocity_delta = velocity_angle - *view;
		while ( velocity_delta > 180 )
			velocity_delta -= 360;
		while ( velocity_delta < -180 )
			velocity_delta += 360;

		// correct our strafe amongst the path of a circle.
		const auto correct = ideal2;

		if( fabsf(velocity_delta) > correct ) {
			*view = velocity_angle - std::copysignf( correct, velocity_delta );
			g_cl.m_cmd->sidemove_ = std::copysignf( 450.f, velocity_delta );
		} else {
			*view += std::copysignf( ideal, velocity_delta );
			g_cl.m_cmd->sidemove_ = std::copysignf( 450.f, velocity_delta );
		}
	}
}

void c_movement::correct_movement( vector old ) {
	vector wish_forward, wish_right, wish_up, cmd_forward, cmd_right, cmd_up;

	const vector movedata{ g_cl.m_cmd->forwardmove_, g_cl.m_cmd->sidemove_, g_cl.m_cmd->upmove_ };


	old.angle_vectors( &wish_forward, &wish_right, &wish_up );
	g_cl.m_cmd->m_viewangles.angle_vectors( &cmd_forward, &cmd_right, &cmd_up );

	const auto v8 = sqrtf( wish_forward.m_x * wish_forward.m_x + wish_forward.m_y * wish_forward.m_y ), v10 = sqrt(
		wish_right.m_x * wish_right.m_x + wish_right.m_y * wish_right.m_y ), v12 = sqrt( wish_up.m_z * wish_up.m_z );

	const vector wish_forward_norm( 1.0f / v8 * wish_forward.m_x, 1.0f / v8 * wish_forward.m_y, 0.f ),
		wish_right_norm( 1.0f / v10 * wish_right.m_x, 1.0f / v10 * wish_right.m_y, 0.f ),
		wish_up_norm( 0.f, 0.f, 1.0f / v12 * wish_up.m_z );

	const auto v14 = sqrtf( cmd_forward.m_x * cmd_forward.m_x + cmd_forward.m_y * cmd_forward.m_y ), v16 = sqrt(
		cmd_right.m_x * cmd_right.m_x + cmd_right.m_y * cmd_right.m_y ), v18 = sqrt( cmd_up.m_z * cmd_up.m_z );

	const vector cmd_forward_norm( 1.0f / v14 * cmd_forward.m_x, 1.0f / v14 * cmd_forward.m_y, 1.0f / v14 * 0.0f ),
		cmd_right_norm( 1.0f / v16 * cmd_right.m_x, 1.0f / v16 * cmd_right.m_y, 1.0f / v16 * 0.0f ),
		cmd_up_norm( 0.f, 0.f, 1.0f / v18 * cmd_up.m_z );

	const auto v22 = wish_forward_norm.m_x * movedata.m_x, v26 = wish_forward_norm.m_y * movedata.m_x, v28 =
		wish_forward_norm.m_z * movedata.m_x, v24 = wish_right_norm.m_x * movedata.m_y, v23 =
		wish_right_norm.m_y * movedata.m_y, v25 = wish_right_norm.m_z * movedata.m_y, v30 =
		wish_up_norm.m_x * movedata.m_z, v27 = wish_up_norm.m_z * movedata.m_z, v29 =
		wish_up_norm.m_y * movedata.m_z;

	vector correct_movement{ 
		  ( cmd_forward_norm.m_x * v24 + cmd_forward_norm.m_y * v23 + cmd_forward_norm.m_z * v25 )
		+ ( cmd_forward_norm.m_x * v22 + cmd_forward_norm.m_y * v26 + cmd_forward_norm.m_z * v28 )
		+ ( cmd_forward_norm.m_y * v30 + cmd_forward_norm.m_x * v29 + cmd_forward_norm.m_z * v27 ),
		
		  ( cmd_right_norm.m_x * v24 + cmd_right_norm.m_y * v23 + cmd_right_norm.m_z * v25 )
		+ ( cmd_right_norm.m_x * v22 + cmd_right_norm.m_y * v26 + cmd_right_norm.m_z * v28 )
		+ ( cmd_right_norm.m_x * v29 + cmd_right_norm.m_y * v30 + cmd_right_norm.m_z * v27 ),
		
		  (correct_movement.m_z = cmd_up_norm.m_x * v23 + cmd_up_norm.m_y * v24 + cmd_up_norm.m_z * v25 )
		+ ( cmd_up_norm.m_x * v26 + cmd_up_norm.m_y * v22 + cmd_up_norm.m_z * v28 )
		+ ( cmd_up_norm.m_x * v30 + cmd_up_norm.m_y * v29 + cmd_up_norm.m_z * v27 ) 
	};

	g_cl.m_cmd->forwardmove_ = std::clamp<float>( correct_movement.m_x, -450.f, 450.f );
	g_cl.m_cmd->sidemove_ = std::clamp<float>( correct_movement.m_y, -450.f, 450.f );
	g_cl.m_cmd->upmove_ = std::clamp<float>( correct_movement.m_z, -320.f, 320.f );
}
