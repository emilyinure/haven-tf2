#pragma once

typedef void* ( *instantiate_interface_fn )( );
typedef void* ( *create_interface_fn )( const char* name, int return_code );

class interface_reg {
public:
	instantiate_interface_fn m_create_fn;
	const char* m_name;
	interface_reg* m_next;
};