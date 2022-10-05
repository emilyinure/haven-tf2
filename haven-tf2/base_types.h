#pragma once

struct vrect_t {
	int	m_x, m_y, m_width, m_height;
	vrect_t* m_next;
};

typedef struct color32_s {
	byte m_r, m_g, m_b, m_a;
} color32;