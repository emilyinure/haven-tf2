#pragma once
#include "utils.h"

struct interface_t {
	const char* m_name = "";
	unsigned int m_hash = 0;
	uintptr_t* m_ptr = nullptr;

	template < typename T >
	T* as( ) {
		return reinterpret_cast< T* >( this->m_ptr );
	}
};

struct signature_t {
	uint8_t* m_ptr = nullptr;

	template < typename T >
	T as( uintptr_t offset = 0 ) {
		return reinterpret_cast< T >( this->m_ptr + offset );
	}

	[[nodiscard]] uint8_t* add( const uint8_t offset ) const {
		return this->m_ptr + offset;
	}
};

struct module_t {
private:
	interface_t get_interface_exact( const char* name ) const;
public:
	const char* m_name = "";
	HMODULE m_module = nullptr;
	unsigned int m_hash = 0;
	std::deque< interface_t > m_interfaces = { };

	template < typename T >
	T get_proc_address( const char* proc ) {
		auto proc_addr = reinterpret_cast< T >( GetProcAddress( this->m_module, proc ) );

		if ( !proc_addr ) {
			g_utils.m_debug.set_console_color(e_console_colors::console_color_red);
			printf_s( "[!] failed to get %s (%s)\n", proc, this->m_name );
		}

		return proc_addr;
	}

	void populate_interfaces( uint8_t* reg );
	interface_t get_interface( const char* name, bool exact = false ) const;
	signature_t get_sig( const char* sig ) const;
};

class c_modules {
	std::deque< module_t > modules_;
public:
	void init( );
	module_t get( const char* name );
	void add( const char* name, bool populate_ifaces = true );
}; inline c_modules g_modules;