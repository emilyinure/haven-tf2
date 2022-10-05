#pragma once

struct box_t {
public:
	float m_x, m_y, m_w, m_h;

	bool is_zero( ) const {
		return this->m_x == 0.f && this->m_y == 0.f && this->m_w == 0.f && this->m_h == 0.f;
	}
};