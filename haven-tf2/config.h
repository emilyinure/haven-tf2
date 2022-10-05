#pragma once

class c_config {
	struct config_control_t {
		std::shared_ptr< c_base_control > m_control;
		const char* m_window_name;
		const char* m_tab_name;
		const char* m_groupbox_name;
	};

	std::deque< config_control_t > config_controls_;
	std::deque< std::shared_ptr< c_window > > windows_;
public:
	void init( );
	void save( const char* file_name ) const;
	void load( const char* file_name ) const;
}; inline c_config g_config;