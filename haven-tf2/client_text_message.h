#pragma once

struct client_text_message_t {
public:
	int m_effect;
	byte m_r1, m_g1, m_b1, m_a1;
	byte m_r2, m_g2, m_b2, m_a2;
	float m_x;
	float m_y;
	float m_fade_in;
	float m_fade_out;
	float m_hold_time;
	float m_fx_time;
	const char* m_vgui_scheme_font_name;
	const char* m_name;
	const char* m_message;
	bool m_rounded_rect_backdrop_box;
	float m_box_size;
	byte m_box_color[ 4 ];
	char const* m_clear_message;
};