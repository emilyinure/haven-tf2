#pragma once

template< class T, class A = c_utl_memory<T> >
class c_utl_vector {
	typedef A CAllocator;
public:
	CAllocator m_memory;
	int m_size;
	T* m_elements;
};