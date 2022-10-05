#pragma once

#define ASSERT( _exp ) ( (void ) 0 )
class matrix_3x4 {
public:
	float m_mat[ 3 ][ 4 ];
	const float* operator[]( int i ) const { ASSERT( ( i >= 0 ) && ( i < 3 ) ); return m_mat[ i ]; }
};