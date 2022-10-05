#include "sdk.h"

// combobox, but instead of an index, items are individually toggled.
c_multi_combobox::c_multi_combobox( const char* name, const char* tooltip ) {
	this->m_type = e_control_type::CONTROL_TYPE_MULTI_COMBOBOX;
	this->m_name = name;
	this->m_tooltip = tooltip;
}

void c_multi_combobox::paint( ) {
	// set our dropdown area.
	this->dropdown_area_ = { this->m_area.m_x + ( this->m_area.m_w - ( this->m_area.m_w * 0.5f ) ), this->m_area.m_y + 2, this->m_area.m_w * 0.5f, 14 };

	// set our item area.
	this->item_area_ = { this->m_area.m_x + ( this->m_area.m_w - ( this->m_area.m_w * 0.5f ) ), this->m_area.m_y + 2, this->m_area.m_w * 0.5f, this->m_items.size( ) * 14.f };

	// background.
	g_ui.m_draw.filled_rect( this->dropdown_area_, { 30, 30, 30 } );
	g_ui.m_draw.outlined_rect( this->dropdown_area_, { 65, 65, 65 } );

	// todo, im sure there's a much better implementation i
	// can do, this is just the first thing i thought of
	// off the top of my head.
	int selected_items = 0;

	for ( auto& item : this->m_items ) {
		if ( item.second )
			selected_items++;
	}

	sprintf_s( this->display_buffer_, selected_items == 1 ? "%i item selected" : "%i items selected", selected_items );

	// current value.
	g_ui.m_draw.text( g_render.m_fonts.menu.main, { this->dropdown_area_.m_x + 2, this->dropdown_area_.m_y }, this->display_buffer_, { 255, 255, 255 }, e_text_alignment::text_align_left );

	// title.
	g_ui.m_draw.text( g_render.m_fonts.menu.main, { this->m_area.m_x, this->m_area.m_y + 2 }, this->m_name, { 255, 255, 255 }, e_text_alignment::text_align_left );

	// draw our items.
	if ( !this->open_ )
		return;

	// dropdown background.
	g_ui.m_draw.filled_rect( this->item_area_, { 40, 40, 40 } );
	g_ui.m_draw.outlined_rect( this->item_area_, { 65, 65, 65 } );

	// items.
	for ( auto i = 0; i < this->m_items.size( ); i++ ) {
		auto& item = this->m_items[ i ];

		g_ui.m_draw.text( g_ui.m_draw.fonts.main, { this->item_area_.m_x + 2, this->item_area_.m_y + ( i * 14 ) }, item.first, item.second ? g_ui.m_theme : color( 255, 255, 255 ), e_text_alignment::text_align_left );
	}
}

void c_multi_combobox::update( ) {
	// update our hover state.
	this->hovering_dropdown_ = g_input.hovering( this->dropdown_area_ );
	this->hovering_items_ = g_input.hovering( this->item_area_ );

	// we clicked our dropdown, open it.
	if ( ( hovering_dropdown_ && !this->open_ ) && g_input.key_pressed( VK_LBUTTON ) ) {
		this->open_ = true;
		g_ui.m_focused_control = this;

		// we want to skip it from selecting this frame, will keep
		// it from falsely selecting the first option.
		return;
	}

	// we're not hovering dropdown or items but we clicked, close.
	if ( this->open_ && ( !this->hovering_dropdown_ && !this->hovering_items_ ) && g_input.key_pressed( VK_LBUTTON ) ) {
		g_ui.m_focused_control = nullptr;
		this->open_ = false;
	}

	if ( !this->open_ )
		return;

	for ( auto i = 0; i < this->m_items.size( ); i++ ) {
		auto& item = this->m_items[ i ];

		// we clicked the item, toggle it.
		if ( g_input.hovering( { this->item_area_.m_x + 2, this->item_area_.m_y + 2 + ( i * 14 ), this->item_area_.m_w, 14 } ) && g_input.key_pressed( VK_LBUTTON ) )
			item.second = !item.second;
	}
}

void c_multi_combobox::add_item( const char* item ) {
	this->m_items.emplace_back( item, false );
}

bool c_multi_combobox::get( const char* item ) {
	const auto hashed_name = c_utils::fnv_hash( item );

	for ( auto& item : this->m_items ) {
		if ( c_utils::fnv_hash( item.first ) == hashed_name )
			return item.second;
	}

	// invalid item, return false.
	return false;
}