#pragma once

class c_slider : public c_base_control {
	box_t bar_area_ = { 0, 0, 0, 0 };
	float filling_ = 0.f;
public:
	float m_value = 0.f, m_min = 0.f, m_max = 0.f;
	bool m_dragging = false;

	c_slider( const char* name, float min = 0.f, float max = 100.f, const char* tooltip = "" );

	void paint( ) override;
	void update( ) override;
};