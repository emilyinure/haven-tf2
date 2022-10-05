#pragma once

class overlay_text_t {
public:
	vector m_origin;
	bool m_use_origin;
	int m_line_offset;
	float m_x_pos;
	float m_y_pos;
	char m_text[ 512 ];
	float m_fl_end_time;
	int m_creation_tick;
	int m_server_count;
	int m_r;
	int m_g;
	int m_b;
	int m_a;
	overlay_text_t* next_overlay_text;
};