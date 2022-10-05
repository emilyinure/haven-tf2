#pragma once

// we won't really need this from what i can see.
class c_save_restore_data;

class c_global_vars_base {
public:
	float m_real_time;
	int	m_frame_count;
	float m_absolute_frame_time;
	float m_cur_time;
	float m_frame_time;	
	int	m_max_clients;
	int	m_tick_count;
	float m_interval_per_tick;
	float m_interpolation_amount;
	int m_sim_ticks_this_frame;
	int	m_network_protocol;
	c_save_restore_data* m_save_data;
private:
	bool m_client;
	int m_time_stamp_network_base;
	int m_time_stamp_randomize_window;
};