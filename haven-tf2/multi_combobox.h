#pragma once

class c_multi_combobox : public c_base_control {
	box_t dropdown_area_ = { 0, 0, 0, 0 }, item_area_ = { 0, 0, 0, 0 };
	bool open_ = false, hovering_dropdown_ = false, hovering_items_ = false;
	char display_buffer_[ 128 ] = { };
public:
	std::deque< std::pair< const char*, bool > > m_items = { };

	c_multi_combobox( const char* name, const char* tooltip = "" );

	void paint( ) override;
	void update( ) override;
	void add_item( const char* item );
	bool get( const char* item );
};