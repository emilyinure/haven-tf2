#include "sdk.h"

void i_surface::start_drawing( ) {
	static auto start_drawing = reinterpret_cast< start_drawing_fn >( g_offsets.m_sigs.start_drawing );

	if ( !start_drawing )
		return;

	start_drawing( this );
}

void i_surface::finish_drawing( ) {
	static auto finish_drawing = reinterpret_cast< finish_drawing_fn >( g_offsets.m_sigs.finish_drawing );

	if ( !finish_drawing )
		return;

	finish_drawing( this );
}