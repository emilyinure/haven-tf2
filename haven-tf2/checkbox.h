#pragma once

class c_checkbox : public c_base_control {
public:
	bool m_value;

	c_checkbox( const char* name, const char* tooltip = "" );

	void paint( ) override;
	void update( ) override;
};