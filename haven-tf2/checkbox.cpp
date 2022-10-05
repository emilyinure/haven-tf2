#include "sdk.h"

c_checkbox::c_checkbox( const char* name, const char* tooltip ) {
	this->m_type = e_control_type::CONTROL_TYPE_CHECKBOX;
	this->m_name = name;
	this->m_tooltip = tooltip;
	this->m_value = false;
}

void c_checkbox::paint( ) {
	// background.
	g_ui.m_theme.m_a = 100 * g_ui.m_fade;
	g_ui.m_draw.filled_rect( this->m_area, this->m_value ? g_ui.m_theme : color( 35, 35, 35, 40 * g_ui.m_fade ) );
	g_ui.m_draw.outlined_rect( this->m_area, color( 35, 35, 35, 40 * g_ui.m_fade )  );

	// title.
	g_ui.m_draw.text( g_render.m_fonts.menu.main, { this->m_area.m_x + ( this->m_area.m_w + 5 ), this->m_area.m_y }, this->m_name, g_ui.m_theme, e_text_alignment::text_align_left );
	g_ui.m_theme.m_a = 255;
}

void c_checkbox::update( ) {
	if ( g_input.hovering( this->m_area ) && g_input.key_pressed( VK_LBUTTON ) )
		this->m_value = !this->m_value;
}