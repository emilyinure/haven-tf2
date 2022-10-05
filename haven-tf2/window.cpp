#include "sdk.h"

c_window::c_window( const char* name, box_t area ) {
	this->m_type = e_control_type::CONTROL_TYPE_WINDOW;
	this->m_name = name;
	this->m_area = area;
	this->padding_ = { 5, 5 };
}

void c_window::paint( ) {
	// set our areas.
	this->title_area_ = {
		this->m_area.m_x,
		this->m_area.m_y, 
		this->m_area.m_w, 
		17
	};

	// align with left side.
	this->tab_area_ = { 
		this->m_area.m_x, 
		this->title_area_.m_y + ( this->title_area_.m_h ),
		this->m_area.m_w * .2f,
		this->m_area.m_h - ( this->title_area_.m_h  )
	};

	// align with tab area.
	this->item_area_ = { 
		this->tab_area_.m_x + ( this->tab_area_.m_w ), 
		this->tab_area_.m_y,
		this->m_area.m_w - ( this->tab_area_.m_w ), 
		this->tab_area_.m_h 
	};

	// background.
	g_ui.m_draw.filled_rect( this->m_area, color( 168, 240, 225, 20 * g_ui.m_fade ) );
	//g_ui.m_draw.outlined_rect( this->m_area, { 55, 55, 55 } );
	//
	//// inner background.
	//g_ui.m_draw.filled_rect( this->item_area_, { 45, 45, 45 } );
	//g_ui.m_draw.outlined_rect( this->item_area_, { 65, 65, 65 } );
	//
	//// title.
	g_ui.m_draw.filled_rect( this->title_area_, color( 35, 35, 35, 50 * g_ui.m_fade ) );
	//g_ui.m_draw.outlined_rect( this->title_area_, { 65, 65, 65 } );

	// temp. until we support vertical alignment.
	const auto text_size = g_ui.m_draw.get_text_size( this->m_name, g_render.m_fonts.menu.title );

	g_ui.m_theme.m_a = 100 * g_ui.m_fade;
	g_ui.m_draw.text( g_render.m_fonts.menu.title, { this->title_area_.m_x + ( this->title_area_.m_w * 0.5f ), this->title_area_.m_y + ( this->title_area_.m_h * 0.5f ) - ( text_size.m_y * 0.5f ) }, this->m_name, g_ui.m_theme, e_text_alignment::text_align_center );
	g_ui.m_theme.m_a = 255;

	// tabs.
	//if ( !this->m_tabs.empty( ) ) {
	//	g_ui.m_draw.filled_rect( this->tab_area_, { 40, 40, 40 } );
	//	g_ui.m_draw.outlined_rect( this->tab_area_, { 65, 65, 65 } );
	//}

	this->paint_children( );
}

void c_window::update( ) {
	this->hovering_title_bar_ = g_input.hovering( this->title_area_ );

	// dragging.
	if ( this->hovering_title_bar_ && g_input.key_pressed( VK_LBUTTON ) )
		this->clicked_ = true;

	if ( !g_input.key_down( VK_LBUTTON ) )
		this->clicked_ = false;

	if ( this->dragging_ && !this->clicked_ )
		this->dragging_ = false;

	if ( this->dragging_ && this->clicked_ ) {
		this->m_area.m_x = g_input.m_mouse_pos.m_x - this->drag_offset_.m_x;
		this->m_area.m_y = g_input.m_mouse_pos.m_y - this->drag_offset_.m_y;
	}

	if ( this->hovering_title_bar_ ) {
		this->dragging_ = true;
		this->drag_offset_.m_x = g_input.m_mouse_pos.m_x - this->m_area.m_x;
		this->drag_offset_.m_y = g_input.m_mouse_pos.m_y - this->m_area.m_y;
	}

	if ( g_input.hovering( this->m_area ) && g_input.key_pressed( VK_LBUTTON ) )
		this->m_last_click_time = g_ui.m_current_time;

	this->update_children( );
}

void c_window::paint_children( ) {
	if ( this->m_tabs.empty( ) )
		return;

	for ( const auto& child : this->m_tabs )
		child->paint( );
}

void c_window::update_children( ) {
	if ( this->m_tabs.empty( ) )
		return;

	for ( auto i = 0; i < this->m_tabs.size( ); i++ ) {
		const auto& child = this->m_tabs[ i ];

		// pass our item area to the child.
		child->m_item_area = this->item_area_;

		// handle position.
		child->m_area = { 
			this->tab_area_.m_x,
			this->tab_area_.m_y,
			this->tab_area_.m_w - ( this->padding_.m_x ),
			25 
		};

		// we have to do this to account for padding.
		child->m_area.m_y += i * ( child->m_area.m_h );

		// we clicked our child.
		if ( g_input.hovering( child->m_area ) && g_input.key_pressed( VK_LBUTTON ) )
			this->selected_tab_ = i;

		child->m_enabled = this->selected_tab_ == i;
		child->update( );
	}
}

std::shared_ptr< c_tab > c_window::add_tab( const char* name ) {
	auto tab = std::make_shared< c_tab >( name );

	this->m_tabs.emplace_back( tab );

	return tab;
}