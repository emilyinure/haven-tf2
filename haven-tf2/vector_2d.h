#pragma once

class vector_2d {
public:
	float m_x, m_y;
	vector_2d(){}
	vector_2d( float x, float y ) {
		m_x = x;
		m_y = y;
	}
	vector_2d( vector other ) {
		m_x = other.m_x;
		m_y = other.m_y;
	}
};