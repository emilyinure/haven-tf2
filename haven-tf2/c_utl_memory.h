#pragma once

template< class T, class I = int >
class c_utl_memory {
public:
	T* m_memory;
	int m_allocation_count;
	int m_grow_size;
};