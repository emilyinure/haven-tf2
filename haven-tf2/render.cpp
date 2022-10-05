#include "sdk.h"

void c_render::init( ) {
	this->m_fonts.main = create_font( "Tahoma", 12, 300, e_font_flags::font_flag_dropshadow );

	// menu.
	this->m_fonts.menu.main = create_font( "Tahoma", 12, 200, e_font_flags::font_flag_antialias );
	this->m_fonts.menu.title = create_font( "Segoe UI", 13, 200, e_font_flags::font_flag_antialias );
	this->m_fonts.menu.tabs = create_font( "Segoe UI", 18, 200, e_font_flags::font_flag_antialias | e_font_flags::font_flag_dropshadow );
	this->m_fonts.menu.tooltip = create_font( "Tahoma", 13, 300, e_font_flags::font_flag_dropshadow );
}

void c_render::filled_rect( const box_t box, const color col ) {
	g_interfaces.m_surface->draw_set_color( col );
	g_interfaces.m_surface->draw_filled_rect( box.m_x, box.m_y, box.m_x + box.m_w, box.m_y + box.m_h );
}

void c_render::filled_rect( const vector_2d pos, const vector_2d size, const color col ) {
	c_render::filled_rect( { pos.m_x, pos.m_y, size.m_x, size.m_y }, col );
}

void c_render::outlined_rect( const box_t box, const color col ) {
	g_interfaces.m_surface->draw_set_color( col );
	g_interfaces.m_surface->draw_outlined_rect( box.m_x, box.m_y, static_cast< int >( box.m_x + box.m_w ), static_cast< int >( box.m_y + box.m_h ) );
}

void c_render::outlined_rect( const vector_2d pos, const vector_2d size, const color col ) {
	outlined_rect( { pos.m_x, pos.m_y, size.m_x, size.m_y }, col );
}

void c_render::line( const vector_2d from, const vector_2d to, const color col ) {
	g_interfaces.m_surface->draw_set_color( col );
	g_interfaces.m_surface->draw_line( from.m_x, from.m_y, to.m_x, to.m_y );
}

void c_render::text( const unsigned long font, vector_2d pos, const char* text, const color col, const e_text_alignment alignment ) {
	if ( !font )
		return;

	wchar_t buf[ 2056 ] = { };

	const auto text_size = get_text_size( text, font );

	// todo, maybe vertical alignment?
	switch ( alignment ) {
	case e_text_alignment::text_align_center: pos.m_x -= text_size.m_x * 0.5; break;
	case e_text_alignment::text_align_right: pos.m_x -= text_size.m_x; break;
	default: break;
	}

	if ( MultiByteToWideChar( CP_UTF8, 0, text, -1, buf, 128 ) > 0 ) {
		g_interfaces.m_surface->draw_set_text_font( font );
		g_interfaces.m_surface->draw_set_text_color( col );
		g_interfaces.m_surface->draw_set_text_pos( pos.m_x, pos.m_y );
		g_interfaces.m_surface->draw_print_text( buf, wcslen( buf ) );
	}
}

vector_2d c_render::get_text_size( const char* text, const unsigned long font ) {
	if ( !font )
		return { };

	wchar_t buf[ 2056 ] = { };

	if ( MultiByteToWideChar( CP_UTF8, 0, text, -1, buf, 128 ) > 0 ) {
		auto width = 0, height = 0;

		g_interfaces.m_surface->get_text_size( font, buf, width, height );

		return { static_cast< float >( width ), static_cast< float >( height ) };
	}

	return { };
}

int c_render::create_font( const char* name, const int size, const int weight, const int flags ) {
	const auto font = g_interfaces.m_surface->create_font( );
	g_interfaces.m_surface->set_font_glyph_set( font, name, size, weight, flags );

	return font;
}