#pragma once

class c_window : public c_base_control {
	bool dragging_ = false, clicked_ = false, hovering_title_bar_ = false;
	vector_2d drag_offset_ = { 0, 0 }, padding_ = { 0, 0 };
	int selected_tab_ = 0;
	box_t item_area_ = { 0, 0, 0 }, tab_area_ = { 0, 0, 0 }, title_area_ = { 0, 0, 0 };

	void paint_children( );
	void update_children( );
public:
	std::deque< std::shared_ptr< c_tab > > m_tabs = { };
	float m_last_click_time = 0.f;

	c_window( const char* name, box_t area );

	void paint( ) override;
	void update( ) override;
	std::shared_ptr<c_tab> add_tab( const char* name );
};