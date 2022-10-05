#pragma once

class c_combobox : public c_base_control {
	box_t dropdown_area_ = { 0, 0, 0, 0 }, item_area_ = { 0, 0, 0, 0 };
	std::deque< const char* > items_;
public:
	bool open_ = false, hovering_dropdown_ = false, hovering_items_ = false;
	int m_selected_index = 0;

	c_combobox( const char* name, const char* tooltip = "" );

	void paint( ) override;
	void update( ) override;
	void add_item( const char* item );
};